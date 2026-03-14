#include <iostream>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <vector>

#include "setup.h"

int accept_loop();

int PORT = 4200;


/**
 * Arg1 Path
 *
 * -setup
 * -run
 *
 */

void match_args(int argc, char** argv)
{
    std::vector<std::string> arguments;
    for(int i = 0; i < argc; i++)
    {
        arguments.push_back(std::string(argv[i]));
    }
    if(arguments[1] == "-setup") {
        setup_dir();
        setup_config();
    }
    if(arguments[1] == "-run") {
        accept_loop();
    }
}


int main(int argc, char** argv)
{
    match_args(argc, argv);
    return 0;
}






int accept_loop()
{
    int SOCKTCPIP4 = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in BIND_SOCK{};
    BIND_SOCK.sin_family = AF_INET;
    BIND_SOCK.sin_port = htons(PORT);
    BIND_SOCK.sin_addr.s_addr = htonl(INADDR_ANY);

    if(bind(SOCKTCPIP4, (sockaddr*)&BIND_SOCK, sizeof(BIND_SOCK)) != 0)
    {
        perror("bind");
        exit(1);
    }

    listen(SOCKTCPIP4, 10);

    sockaddr_in CLIENT_NEXT{};
    socklen_t CN_LEN = sizeof(CLIENT_NEXT);

    while(1)
    {
        int new_confd = accept(SOCKTCPIP4, (sockaddr*)&CLIENT_NEXT, &CN_LEN);
        if (new_confd < 0)
        {
            perror("accept");
            exit(1);
        }

        std::cout << "Client connected\n";
    }
}
