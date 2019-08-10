#include "Slim.h"         // for Slim_Create, Slim_Destroy, Slim_HandleConn...
#include "SocketServer.h" // for SocketServer_Create, SocketServer_Destroy
#include "TcpComLink.h"   // for TcpComLink_Create, TcpComLink_Destroy, Tcp...

Slim* slim;

int connection_handler(int socket) {
    int result = 0;
    TcpComLink* comLink = TcpComLink_Create(socket);

    result = Slim_HandleConnection(slim, (void*)comLink, &TcpComLink_send, &TcpComLink_recv);

    TcpComLink_Destroy(comLink);

    return result;
}

int main(int ac, char** av) {
    slim = Slim_Create();
    SocketServer* server = SocketServer_Create();
    SocketServer_register_handler(server, &connection_handler);

    int result = SocketServer_Run(server, av[1]);

    SocketServer_Destroy(server);
    Slim_Destroy(slim);
    return result;
}
