# Multithreaded_TCPServer
A basic TCP multithreaded server class implemented in c++ using winsock
___________________

INSTRUCTION FOR USE

1. Create a new TCPListener object (Ex.: TCPListener server(<ip addr(std::string)>, <port(int)> <handler(MessageReceivedHandler)>,); )
2. Create the loop (Ex.: if(server.Init()){ server.Run(); })
3. Open the client (Client source code available here: <link>)

P.S. MessageReceivedHandler is a void function pointer that gets these parameter: (TCPListener*, int, std::string). You have to give the server a reference of a void function with these parameters.  (Ex.: void Handler(TCPListener* listener, int socketId, std::string msg) { listen->Send(socketId, msg); } )

___________________
