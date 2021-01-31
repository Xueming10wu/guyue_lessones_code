#include "Server.h"

using namespace std;

int main(int argc, char *argv[])
{
    signal(SIGINT, MyFunctions::stop);
    Server *server = new Server();
    server->listening();
    return 0;
}