#include <iostream>
#include "../src/rpcbuf.h"
#include "../src/call_out_definitions.h"

class sender_test : call_sender
{
public:
	sender_test(call_dispatcher& dispatcher)
		: call_sender(dispatcher)
	{ }

#include "call_declarations.h"
};
