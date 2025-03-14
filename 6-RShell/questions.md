1. How does the remote client determine when a command's output is fully received from the server, and what techniques can be used to handle partial reads or ensure complete message transmission?

_answer here_ A way remote client determin when a command's output is fully received from the server is when it received a EOF either in the form of static const char RDSH_EOF_CHAR = 0x04;   like in the homework or just a end of message indicator. Furthermore, stateful protocols and chunked transfer encoding can be used to handlle partial reads or ensure complete message transmission.

2. This week's lecture on TCP explains that it is a reliable stream protocol rather than a message-oriented one. Since TCP does not preserve message boundaries, how should a networked shell protocol define and detect the beginning and end of a command sent over a TCP connection? What challenges arise if this is not handled correctly?

_answer here_ In this homework, we used EOF as a way for a networked shell protocol to define and detect the beginning and end of a command sent over a TCP connect, splitted by pipes then by space on the inside. If this was not handled correctly, we might face data issues alongside error handling issues.

Furthermore:
TCP is STREAM protocol: a stream of bytes flow; NOT individual messages
MEANING:  You need to build into your application where messages begin and end
PLUS:  Its reliable, TCP acknowledges all bytes in the stream

3. Describe the general differences between stateful and stateless protocols.

_answer here_ Drawn from the lecture:
STATELESS PROTOCOLS:   Will be faster, will have less overhead, will not be fully reliable
STATEFULL PROTOCOLS:  Will have more robust capabilities such as reliability, but will generally be slower with more overhead

4. Our lecture this week stated that UDP is "unreliable". If that is the case, why would we ever use it?

_answer here_ We use UDP for its simplicity, low latency, and quote on quote " Individual Messages are Packaged and Delivered
 With UDP messages are delivered as complete units up to 64K max".

5. What interface/abstraction is provided by the operating system to enable applications to use network communications?

_answer here_ "Sockets are the programming interface provided by the operating system enabling applications to write network programs"