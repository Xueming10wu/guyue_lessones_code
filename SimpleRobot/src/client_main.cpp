#include "Client.h"

using namespace std;


int main(int argc, char *argv[])
{
    signal(SIGINT,MyFunctions::stop);
    Client *client = new Client("127.0.0.1", 8000, 6 ,8192);
    client->timerRegister(100000);
    client->listening();
    return 0;
}