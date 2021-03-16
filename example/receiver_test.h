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
#include "../src/call_in_definitions.h"

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
