#include <iostream>
#include "rpcbuf.h"
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

