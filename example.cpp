#include <iostream>
#include "rpcbuf.h"

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
