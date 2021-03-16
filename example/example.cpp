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

#include <iostream>
#include "../src/rpcbuf.h"

#include "receiver_test.h"
#include "sender_test.h"

int main(void)
{
	receiver_test rt;

	sender_test st(rt);

	auto d = st.foo(1, 1.2f, 5);
	std::cout << d << '\n';

	st.bar();

	return 0;
}
