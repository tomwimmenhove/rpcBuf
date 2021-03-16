# rpcBuf
A C++ template library for calling remote methods transparently

This template library allows you to easily define a set of methods, which can be called just like a regular method. Calling one of these methods will trigger a call to the `exec()` method in a class derived from the `call_dispatcher` interface, which can then move or transmit (be it over TCP/IP, or any data transportation method).

On the 'receiving side', this data can be fed into the `exec()` method of the `call_receiver` class, which will finally call the previously defined method. Any data returned by this method will be sent back to the 'calling side', to be used as the return value of the original call.

## Example
The `call_receiver` class implements `call_dispatcher` interface, so the example directly 'connects' a `call_sender` derived class to a `call_receiver` derived class and calls 2 methods. One of the methods does a simple calculation and returns a result, the other prints "Hello world". In any real-world scenario, there will be some form of communication protocol separating the sender and receiver.

## Compiling the example
```
git clone https://github.com/tomwimmenhove/rpcbuf.git
cd rpcbuf/example
g++ -o example example.cpp -std=c++17 -Wall -O2
```
