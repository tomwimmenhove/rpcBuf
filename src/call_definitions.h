#ifndef CALL_DEFINITIONS_H
#define CALL_DEFINITIONS_H

#define DEFINE_CALL_OUT(func, proto) sender_functor<proto> func = get_functor<proto>(__COUNTER__ - 1 - first_counter)
#define DEFINE_CALL_OUT_START inline const static size_t first_counter = __COUNTER__
#define DEFINE_CALL_OUT_END
#define CALL_OUT_INIT call_receiver(num_callers)

#define DEFINE_CALL_IN(func, proto) set_caller(__COUNTER__ - 1 - first_counter, std::function<proto>(bind_to(&receiver_type::func, this)));

#define DEFINE_CALL_IN_START \
private: \
    using receiver_type = receiver_test; \
        inline const static size_t first_counter = __COUNTER__; \
        void setup() \
        {

#define DEFINE_CALL_IN_END \
    } \
    inline const static size_t num_callers = (__COUNTER__ - 1 - first_counter)

#endif /* CALL_DEFINITIONS_H */
