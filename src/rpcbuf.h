/* 
 * This file is part of the rpcbuf distribution (https://github.com/tomwimmenhove/rpcbuf).
 * Copyright (c) 2021 Tom Wimmenhove.
 * 
 * This program is free software: you can redistribute it and/or modify  
 * it under the terms of the GNU General Public License as published by  
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License 
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef RPCBUF_H
#define RPCBUF_H

#include <utility>
#include <type_traits>
#include <tuple>
#include <utility>
#include <memory>
#include <type_traits>
#include <vector>
#include <functional>

struct caller_base
{
	virtual void exec(void* mem) { return; }

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

	constexpr size_t get_param_size() override
	{
		return sizeof...(Args) ? sizeof(std::tuple<Args...>) : 0;
	}

	constexpr size_t get_return_size() override
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
		return call_fn(params, std::make_index_sequence<sizeof...(Args)>{});
	}
	
	/* Execute func using a tuple pointed to by void pointer 'mem' as arguments
	 * and return the result */
	T operator()(const void* mem)
	{
		return operator()(*(std::tuple<Args...>*) mem);
	}

	/* Execute func using a tuple pointed to by void pointer 'mem' as arguments
	 * and copy the result back into the same memory */
	void exec(void* mem) override
	{
		if constexpr (std::is_same<void, T>::value)
		{
			operator()(mem);
		}
		else
		{
			*((T*) mem) = operator()(mem);
		}
	}

private:
	template<typename U, U... ints>
	T call_fn(const std::tuple<Args...>& params, std::integer_sequence<U, ints...>)
	{
		return func(std::get<ints>(params) ...);
	}

	std::function<T(Args...)> func;
};

struct call_dispatcher
{
	virtual void exec(size_t id, void* mem, size_t dest_size, size_t param_size) = 0;

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

	void exec(size_t id, void* mem, size_t dest_size, size_t param_size) override
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

		caller.exec(mem);
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

		/* Allocate data for parameters and return value */
		char data[return_len > sizeof(std::tuple<Args...>) ? return_len : sizeof(std::tuple<Args...>)];

		/* Create a tuple with parameters in it */
		//*(std::tuple<Args...>*) data = std::tuple<Args...>(std::forward<Args>(args)...);

		/* Create a tuple with parameters in it using placement new() */
		new(data) std::tuple<Args...>(std::forward<Args>(args)...);

		dispatcher.exec(id, data, return_len, sizeof...(Args) ? sizeof(std::tuple<Args...>) : 0);

		if constexpr (!std::is_same<T, void>::value)
		{
			return *(T*)data;
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

#endif /* RPCBUF_H */
