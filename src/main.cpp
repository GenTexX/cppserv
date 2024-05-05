
#include <iostream>
#include <csignal>
#include <cstring>

#include "socket/Socket.h"
#include "logger/Logger.h"
#include "threadpool/Threadpool.h"

#include "core/cppservcore.h"
#include "http/httprequest.h"

#define SERVER_RUNNING 1
#define SERVER_STOP 0


/**
 * @brief A static int, that indicates, if the server is running or not (!= 0 => running, == 0 => not running)
 */
static volatile int s_Running = SERVER_STOP;


/**
 * @brief Prints the usage of the server program and exits with status code EXIT_FAILURE.
 *
 * @param signal the signal
 */
void handle_signal(int signal) {
    if (signal == SIGINT || signal == SIGTERM) {
        std::cout << "Received signal " << signal << ". Stopping server..." << std::endl;
        s_Running = SERVER_STOP;
    }
}


int main(int argc, char* argv[]) {

    cppserv::Logger::init();

    struct sigaction signal_handler;
    memset(&signal_handler, 0, sizeof(signal_handler));
    signal_handler.sa_handler = handle_signal;
    sigaction(SIGINT, &signal_handler, nullptr);
    sigaction(SIGTERM, &signal_handler, nullptr);

    cppserv::Socket socket(AF_INET, SOCK_STREAM, 0);

    socket.Bind("0.0.0.0", "8080");
    socket.Listen(20);

    s_Running = SERVER_RUNNING;

    cppserv::ThreadPool threadpool;
    threadpool.Init();



    while (s_Running == SERVER_RUNNING) {
        cppserv::Ref<cppserv::Socket> clientSocket = socket.Accept();

        threadpool.Submit([clientSocket] {
            CPPSERV_INFO("Accepted connection");
            std::string request;
            std::string buffer;
            int received = 0;
            while ((received = clientSocket->SocketSafeRead(buffer, 1024, 10)) > 0) {
                CPPSERV_INFO("Received {0} bytes", received);
                request += buffer;
                if (received < 1024) {
                    break;
                }
            }
            
            CPPSERV_INFO("Received request: {0}", request);

            std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: 12\r\n\r\nHello World!";
            clientSocket->SocketWrite(response);
            clientSocket->Close();
        });
    }

    threadpool.Terminate();
    threadpool.Shutdown();
    socket.Close();

    return EXIT_SUCCESS;
}