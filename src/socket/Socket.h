#ifndef __SOCKET_H__
#define __SOCKET_H__
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <vector>
#include <sys/un.h>
#include <errno.h>

#include "../core/cppservcore.h"

namespace cppserv {

    class Socket {
    public:
        Socket();
        Socket(int domain, int type, int protocol);

        /**
         * \brief Binds the socket to the specified IP address and port.
         *
         * This function binds the socket to the specified IP address and port. If the address family
         * is AF_UNIX, it binds to a Unix domain socket; otherwise, it binds to an IPv4 or IPv6 address.
         *
         * \param ip The IP address to bind to.
         * \param port The port number or service name to bind to.
         * \return Returns 0 on success, or a negative value indicating an error.
         */
        int Bind(std::string ip, std::string port);

        /**
         * \brief Connects the socket to the specified IP address and port.
         *
         * This function establishes a connection to the specified IP address and port. If the address family
         * is AF_UNIX, it connects to a Unix domain socket; otherwise, it connects to an IPv4 or IPv6 address.
         *
         * \param ip The IP address to connect to.
         * \param port The port number or service name to connect to.
         * \return Returns 0 on success, or a negative value indicating an error.
         */
        int Connect(std::string ip, std::string port);

        /**
         * \brief Starts listening for incoming connections on the socket.
         *
         * This function starts listening for incoming connections on the socket with a maximum
         * queue length specified by `max_queue`.
         *
         * \param max_queue The maximum length for the pending connections queue.
         * \return Returns 0 on success, or a negative value indicating an error.
         */
        int Listen(int max_queue);

        /**
         * \brief Accepts an incoming connection on the socket.
         *
         * This function accepts an incoming connection on the socket, creating a new socket object
         * for the accepted connection.
         *
         * \return Returns a pointer to the newly created Socket object representing the accepted connection.
         */
        Ref<Socket> Accept();

        /**
         * \brief Writes data to the socket.
         *
         * This function writes the specified message to the socket.
         *
         * \param msg The message to write to the socket.
         * \return Returns the number of bytes written on success, or a negative value indicating an error.
         */
        int SocketWrite(std::string msg);

        /**
         * \brief Reads data from the socket.
         *
         * This function reads data from the socket into the specified buffer.
         *
         * \param buf Reference to a string where the received data will be stored.
         * \param len The maximum number of bytes to read into the buffer.
         * \return Returns the number of bytes received on success, or a negative value indicating an error.
         */
        int SocketRead(std::string& buf, int len);

        /**
         * \brief Reads data from the socket with a timeout.
         *
         * This function reads data from the socket into the specified buffer with a timeout.
         *
         * \param buf Reference to a string where the received data will be stored.
         * \param len The maximum number of bytes to read into the buffer.
         * \param seconds The timeout duration in seconds.
         * \return Returns the number of bytes received on success, 0 if the timeout expires, or a negative value indicating an error.
         */
        int SocketSafeRead(std::string& buf, int len, int seconds);

        /**
         * \brief Writes data to a specified IP address and port.
         *
         * This function writes the specified message to the specified IP address and port.
         *
         * \param msg The message to write to the socket.
         * \param ip The IP address to write to.
         * \param port The port number or service name to write to.
         * \return Returns the number of bytes written on success, or a negative value indicating an error.
         */
        int SocketWriteTo(std::string msg, std::string ip, std::string port);

        /**
         * \brief Reads data from a specified IP address and port.
         *
         * This function reads data from the specified IP address and port into the specified buffer.
         *
         * \param buf Reference to a string where the received data will be stored.
         * \param len The maximum number of bytes to read into the buffer.
         * \param ip The IP address to read from.
         * \param port The port number or service name to read from.
         * \return Returns the number of bytes received on success, or a negative value indicating an error.
         */
        int SocketReadFrom(std::string& buf, int len, std::string ip, std::string port);

        /**
         * \brief Sets options on the socket.
         *
         * This function sets options on the socket specified by `level` and `optname`.
         *
         * \param level The protocol level at which the option resides.
         * \param optname The socket option to set.
         * \param optval A pointer to the value for the option being set.
         * \return Returns 0 on success, or a negative value indicating an error.
         */
        int SocketSetOpt(int level, int optname, void* optval);

        /**
         * \brief Gets options from the socket.
         *
         * This function gets options from the socket specified by `level` and `optname`.
         *
         * \param level The protocol level at which the option resides.
         * \param optname The socket option to retrieve.
         * \param optval A pointer to the buffer where the option value will be stored.
         * \return Returns 0 on success, or a negative value indicating an error.
         */
        int SocketGetOpt(int level, int optname, void* optval);

        /**
         * \brief Sets the socket to blocking mode.
         *
         * This function sets the socket to blocking mode, meaning that operations on the socket will block
         * until they can be completed.
         *
         * \return Returns 0 on success, or a negative value indicating an error.
         */
        int SetBlocking();

        /**
         * \brief Sets the socket to non-blocking mode.
         *
         * This function sets the socket to non-blocking mode, meaning that operations on the socket will
         * return immediately with an error if they cannot be completed immediately.
         *
         * \return Returns 0 on success, or a negative value indicating an error.
         */
        int SetNonBlocking();

        /**
         * \brief Shuts down part of a full-duplex connection on the socket.
         *
         * This function shuts down part of a full-duplex connection on the socket.
         *
         * \param how Specifies the type of shutdown. It can be one of the following:
         *            - SHUT_RD to shut down the reading side of the connection.
         *            - SHUT_WR to shut down the writing side of the connection.
         *            - SHUT_RDWR to shut down both the reading and writing sides of the connection.
         * \return Returns 0 on success, or a negative value indicating an error.
         */
        int SocketShutdown(int how);

        /**
         * \brief Closes the socket.
         *
         * This function closes the socket.
         */
        void Close();

        /**
         * \brief Performs a synchronous I/O multiplexing operation.
         *
         * This function performs a synchronous I/O multiplexing operation using the select system call,
         * allowing the caller to monitor multiple file descriptors for various types of readiness events.
         *
         * \param reads A pointer to a vector of Socket objects to monitor for readability.
         * \param writes A pointer to a vector of Socket objects to monitor for writability.
         * \param exceptions A pointer to a vector of Socket objects to monitor for exceptional conditions.
         * \param seconds The timeout duration in seconds. If set to 0, select will return immediately.
         * \return Returns the total number of file descriptors ready for the requested operation, or a negative value indicating an error.
         */
        static int Select(std::vector<Socket>* reads, std::vector<Socket>* writes, std::vector<Socket>* exceptions, int seconds);

        /**
         * \brief Resolves an IP address from a hostname.
         *
         * This function resolves the IP address associated with the given hostname.
         *
         * \param hostname The hostname to resolve.
         * \return Returns the IP address associated with the hostname, or an empty string if the hostname resolution fails.
         */
        static std::string IpFromHostName(std::string hostname);

    private:
        int m_Socket;
        std::string m_Address;
        std::string m_Port;
        struct addrinfo m_AddressInfo;
    };
}
#endif // __SOCKET_H__