#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <vector>

#include "config.h"
#include "http_def.h"
#include "res_man.h"
#include "setup.h"
#include "logger.h"
#include "worker_man.h"

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
    // LOAD USER CONFIG
    SERVERCONFIG server_config = parse_configfile();
    Ressource_Manager_Config config;
    config.serve_path = server_config.site_dir;

    Ressource_Manager res_man(config);
    Worker_Manager worker_man(res_man, true);

    int SOCKTCPIP4 = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in BIND_SOCK{};
    BIND_SOCK.sin_family = AF_INET;
    BIND_SOCK.sin_port = htons(server_config.port);
    BIND_SOCK.sin_addr.s_addr = htonl(INADDR_ANY);
    int opt = 1;
    // schneller.
    setsockopt(SOCKTCPIP4, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

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
        if (new_confd > 0)
        {
            char buffer[4096];
            int len = read(new_confd, buffer, sizeof(buffer) - 1);
            buffer[len] = '\0';
            HttpRequest req;
            req = HttpRequest::from(buffer);
            std::string msg = "New Request: " + req.path;
            worker_man.push_job(req, new_confd);
        }
    }
}
