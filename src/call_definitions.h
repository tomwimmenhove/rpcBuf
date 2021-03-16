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
