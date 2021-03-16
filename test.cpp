#include <utility>
#include <type_traits>
#include <iostream>
#include <tuple>
#include <iostream>
#include <utility>
#include <memory>
#include <type_traits>
#include <vector>
#include <functional>

struct caller_base
{
	virtual void exec(void* dest_mem, const void* param_mem) { return; }

	virtual size_t get_param_size() = 0;
	virtual size_t get_return_size() = 0;

	virtual ~caller_base()
	{ }
};

template <typename T, typename ...Args>
struct call_tuple_args : caller_base
{
	call_tuple_args(std::function<T(Args...)>& func)
		: func(func)
	{ }

	size_t get_param_size() override
	{
		return sizeof...(Args) ? sizeof(std::tuple<Args...>) : 0;
	}

	size_t get_return_size() override
	{		
		if constexpr (std::is_same<void, T>::value)
		{
			return 0;
		}
		else
		{
			return sizeof(T);
		}
	}

	/* Execute func using the tuple as arguments and return the result */
	T operator()(const std::tuple<Args...>& params)
	{
		constexpr auto indices = std::make_index_sequence<sizeof...(Args)>{};
		if constexpr (std::is_same<void, T>::value)
		{
			call_fn(params, indices);
		}
		else
		{
			return call_fn(params, indices);
		}
	}
	
	/* Execute func using a tuple pointed to by void pointer 'param_mem' as arguments
	 * and return the result */
	T operator()(const void* param_mem)
	{
		const auto& params = *(std::tuple<Args...>*) param_mem;
		if constexpr (std::is_same<void, T>::value)
		{
			operator()(params);
		}
		else
		{
			return operator()(params);
		}
	}

	/* Execute func using a tuple pointed to by void pointer 'param_mem' as arguments
	 * and copy the result into memory pointed to by void pointer 'dest_mem' */
	void exec(void* dest_mem, const void* param_mem) override
	{
		if constexpr (std::is_same<void, T>::value)
		{
			operator()(param_mem);
		}
		else
		{
			*((T*) dest_mem) = operator()(param_mem);
		}
	}

private:
	template<typename U, U... ints>
	T call_fn(const std::tuple<Args...>& params, std::integer_sequence<U, ints...>)
	{
		if constexpr (std::is_same<void, T>::value)
		{
			func(std::get<ints>(params) ...);
		}
		else
		{
			return func(std::get<ints>(params) ...);
		}
	}

	std::function<T(Args...)> func;
};

struct call_dispatcher
{
	virtual void exec(size_t id, void* dest_mem, size_t dest_size, const void* param_mem, size_t param_size) = 0;

	virtual ~call_dispatcher()  { }
};

/* Thanks to https://stackoverflow.com/questions/64712892/mimic-stdbind-front-in-c17-for-calling-member-functions */
template<typename F, typename ...FrontArgs>
struct bindfronthelper
{
	bindfronthelper(F f, FrontArgs&&...args)
		: mF{std::move(f)}
	, mFrontArg{std::forward<FrontArgs>(args)...}
	{}

	template<typename ...BackArgs>
	auto operator()(BackArgs&&...args) const
	{
		return std::apply(mF, std::tuple_cat(mFrontArg, std::forward_as_tuple(args...)));
	}

private:
	F mF;
	std::tuple<std::decay_t<FrontArgs>...> mFrontArg;
};

class call_receiver : public call_dispatcher
{
public:
	call_receiver(size_t num_callers = 0)
		: callers(std::vector<std::unique_ptr<caller_base>>(num_callers))
	{ }

	template <typename T, typename... Args>
	void set_caller(size_t id, std::function<T(Args...)> func)
	{
		std::cout << "id: " << id << '\n';
		if (id < 0 || id >= callers.size())
		{
			throw std::overflow_error("Caller ID out of range\n");
		}

		callers[id] = std::move(std::make_unique<call_tuple_args<T, Args...>>(func));
	}

	template <typename T, typename... Args>
	void add_caller(std::function<T(Args...)> func)
	{
		callers.push_back(std::move(std::make_unique<call_tuple_args<T, Args...>>(func)));
	}

	void exec(size_t id, void* dest_mem, size_t dest_size, const void* param_mem, size_t param_size) override
	{
		auto& caller = get_caller(id);

		if (caller.get_return_size() > dest_size)
		{
			throw std::overflow_error("Called return memory overflow");
		}

		if (caller.get_param_size() > param_size)
		{
			throw std::overflow_error("Called parameter memory overflow");
		}

		caller.exec(dest_mem, param_mem);
	}

	virtual ~call_receiver() { }

protected:
	template<typename F, typename ...FrontArgs>
	auto bind_to(F f, FrontArgs&&...args)
	{
		return bindfronthelper<F, FrontArgs...>{std::move(f), std::forward<FrontArgs>(args)...};
	}

private:
	inline caller_base& get_caller(size_t id)
	{
		if (id < 0 || id >= callers.size())
		{   
			throw std::overflow_error("Caller ID out of range\n");
		}
	
		std::unique_ptr<caller_base>& p = callers[id];
		if (!p)
		{
			throw std::overflow_error("Caller not initialized\n");
		}

		return *p;
	}

private:
	std::vector<std::unique_ptr<caller_base>> callers;
};

template<typename T> class sender_functor;
template<typename T, typename... Args>
class sender_functor<T(Args...)>
{
public:
	sender_functor(call_dispatcher& dispatcher, size_t id)
		: dispatcher(dispatcher), id(id)
	{ }

	T operator()(Args&&... args)
	{
		size_t return_len;

		if constexpr (!std::is_same<T, void>::value)
		{
			return_len = sizeof(T);
		}
		else
		{
			return_len = 0;
		}

		char ret[return_len];
		std::tuple<Args...> params(std::forward<Args>(args)...);

		dispatcher.exec(id, ret, return_len, &params, sizeof...(Args) ? sizeof(std::tuple<Args...>) : 0);

		if constexpr (!std::is_same<T, void>::value)
		{
			return *(T*)ret;
		}
	}

private:
	call_dispatcher& dispatcher;
	const size_t id;
};

class call_sender
{
public:
	call_sender(call_dispatcher& dispatcher)
		: dispatcher(dispatcher)
	{ }

	template<typename T>
	sender_functor<T> get_functor(size_t id)
	{
		return sender_functor<T>(dispatcher, id);
	}

private:
	call_dispatcher& dispatcher;
};

double foo(int x, float y, double z) { return x + y + z; }
void bar() { std::cout << "Hello world\n"; }
int square(int a) { return a * a; }

#include "call_out_definitions.h"
class sender_test : call_sender
{
public:
	sender_test(call_dispatcher& dispatcher)
		: call_sender(dispatcher)
	{ }

#include "call_declarations.h"
};

#include "call_in_definitions.h"

class receiver_test : public call_receiver
{
#include "call_declarations.h"

public:
	receiver_test() : CALL_OUT_INIT
	{
		setup();
	}


private:
	double foo(int x, float y, double z) { return x + y + z; }
	void bar() { std::cout << "Hello world\n"; }
	int square(int a) { return a * a; }
};

int main(void)
{
	receiver_test rt;

	sender_test st(rt);

	auto d = st.foo(1, 1.2f, 5);
	std::cout << d << '\n';

	st.bar();

	return 0;
}
