//
//  Socket.cpp
//  SocketServer
//
//  Created by Kay Makowsky on 15.06.16.
//  Copyright © 2016 Kay Makowsky. All rights reserved.
//

#include "Socket.h"

#include "../logger/Logger.h"

namespace cppserv {


    Socket::Socket() {

    }
    Socket::Socket(int domain, int type, int protocol) {
        memset(&m_AddressInfo, 0, sizeof m_AddressInfo);
        m_Socket = socket(domain, type, protocol);
        if (m_Socket < 0) {
            CPPSERV_ERROR("opening socket error: {}", gai_strerror(errno));
        }
        m_AddressInfo.ai_family = domain;
        m_AddressInfo.ai_socktype = type;
        m_AddressInfo.ai_protocol = protocol;

        m_Port = "";
        m_Address = "";
    }

    int Socket::Bind(std::string ip, std::string port) {
        if (m_AddressInfo.ai_family == AF_UNIX) {

            struct sockaddr_un addr;
            memset(&addr, 0, sizeof(addr));
            addr.sun_family = AF_UNIX;
            strncpy(addr.sun_path, ip.c_str(), sizeof(addr.sun_path) - 1);
            int status = ::bind(m_Socket, (struct sockaddr*)&addr, sizeof(addr));
            if (status < 0) {
                CPPSERV_ERROR("bind error: {}", gai_strerror(errno));
            }
            return status;
        }

        m_Address = ip;
        this->m_Port = port;
        int status;
        struct addrinfo* res;
        m_AddressInfo.ai_flags = AI_PASSIVE;
        if ((status = getaddrinfo(ip.c_str(), port.c_str(), &m_AddressInfo, &res)) != 0) {
            CPPSERV_ERROR("getaddrinfo error: {}", gai_strerror(errno));
            return status;
        }
        m_AddressInfo.ai_addrlen = res->ai_addrlen;
        m_AddressInfo.ai_addr = res->ai_addr;
        freeaddrinfo(res);
        status = ::bind(m_Socket, m_AddressInfo.ai_addr, m_AddressInfo.ai_addrlen);
        if (status < 0) {
            CPPSERV_ERROR("bind error: {}", gai_strerror(errno));
        }
        CPPSERV_INFO("Binding to: {}:{}", ip, port);
        return status;
    }

    int Socket::Connect(std::string ip, std::string port) {
        if (m_AddressInfo.ai_family == AF_UNIX) {

            struct sockaddr_un addr;
            memset(&addr, 0, sizeof(addr));
            addr.sun_family = AF_UNIX;
            strncpy(addr.sun_path, ip.c_str(), sizeof(addr.sun_path) - 1);
            int status = ::connect(m_Socket, (struct sockaddr*)&addr, sizeof(addr));
            if (status < 0) {
                CPPSERV_ERROR("connect error: {}", gai_strerror(errno));
            }
            return status;
        }


        m_Address = ip;
        this->m_Port = port;
        struct addrinfo* res;
        int status;
        if ((status = getaddrinfo(ip.c_str(), port.c_str(), &m_AddressInfo, &res)) != 0) {
            CPPSERV_ERROR("getaddrinfo error: {}", gai_strerror(errno));
            return status;
        }
        m_AddressInfo.ai_addrlen = res->ai_addrlen;
        m_AddressInfo.ai_addr = res->ai_addr;
        freeaddrinfo(res);
        status = ::connect(m_Socket, m_AddressInfo.ai_addr, m_AddressInfo.ai_addrlen);
        if (status < 0) {
            CPPSERV_ERROR("connect error: {}", gai_strerror(errno));
        }
        return status;
    }

    int Socket::Listen(int max_queue) {
        int status;
        status = ::listen(m_Socket, max_queue);
        if (status < 0) {
            CPPSERV_ERROR("listen error: {}", gai_strerror(errno));
        }
        CPPSERV_INFO("Listening on: {}:{}", m_Address, m_Port);
        return status;
    }

    Ref<Socket> Socket::Accept() {
        struct sockaddr_storage their_addr;
        socklen_t addr_size;
        addr_size = sizeof their_addr;
        int newsock = ::accept(m_Socket, (struct sockaddr*)&their_addr, &addr_size);
        if (newsock < 0) {
            CPPSERV_ERROR("accept error: {}", gai_strerror(errno));
        }
        Ref<Socket> newSocket = createRef<Socket>(m_AddressInfo.ai_family, m_AddressInfo.ai_socktype, m_AddressInfo.ai_protocol);
        newSocket->m_Socket = newsock;
        newSocket->m_Port = m_Port;

        char host[NI_MAXHOST];
        int status = getnameinfo((struct sockaddr*)&their_addr, sizeof(their_addr), host, sizeof(host), NULL, 0, NI_NUMERICHOST);
        if (status < 0) {
            CPPSERV_ERROR("getnameinfo error: {}", gai_strerror(errno));
        }
        newSocket->m_Address = host;
        newSocket->m_AddressInfo.ai_family = their_addr.ss_family;
        newSocket->m_AddressInfo.ai_addr = (struct sockaddr*)&their_addr;
        std::cout << "Connection from: " << host << std::endl;
        return newSocket;
    }
    int Socket::SocketWrite(std::string msg) {
        const char* buf = msg.c_str();
        int len = (int)strlen(buf);
        int status = (int)send(m_Socket, buf, len, 0);
        if (status < 0) {
            CPPSERV_ERROR("write error: {}", gai_strerror(errno));
        }
        return status;
    }
    int Socket::SocketSafeRead(std::string& buf, int len, int seconds) {
        std::vector<Socket> reads;
        reads.push_back(*this);
        int count = Socket::Select(&reads, NULL, NULL, seconds);
        if (count < 1) {
            buf = "";
            return -1;
        }
        char buffer[len];
        bzero(buffer, len);
        int status = (int)recv(m_Socket, buffer, len - 1, 0);
        if (status < 0) {
            CPPSERV_ERROR("read error: {}", gai_strerror(errno));
        }
        buf = std::string(buffer);
        return status;
    }
    int Socket::SocketRead(std::string& buf, int len) {
        char buffer[len];
        bzero(buffer, len);
        int status = (int)recv(m_Socket, buffer, len - 1, 0);
        if (status < 0) {
            CPPSERV_ERROR("read error: {}", gai_strerror(errno));
        }
        buf = std::string(buffer);
        return status;
    }
    int Socket::SocketWriteTo(std::string msg, std::string ip, std::string port) {
        const char* buf = msg.c_str();
        int len = (int)strlen(buf);
        m_Address = ip;
        this->m_Port = port;
        struct addrinfo* res;
        int status;
        if ((status = getaddrinfo(ip.c_str(), port.c_str(), &m_AddressInfo, &res)) != 0) {
            CPPSERV_ERROR("getaddrinfo error: {}", gai_strerror(errno));
            return status;
        }
        m_AddressInfo.ai_addrlen = res->ai_addrlen;
        m_AddressInfo.ai_addr = res->ai_addr;
        freeaddrinfo(res);
        status = (int)sendto(m_Socket, buf, len, 0, m_AddressInfo.ai_addr, m_AddressInfo.ai_addrlen);
        if (status < 0) {
            CPPSERV_ERROR("writeTo error: {}", gai_strerror(errno));
        }
        return status;
    }
    int Socket::SocketReadFrom(std::string& buf, int len, std::string ip, std::string port) {
        char buffer[len];
        bzero(buffer, len);
        struct addrinfo* res;
        int status;
        if ((status = getaddrinfo(ip.c_str(), port.c_str(), &m_AddressInfo, &res)) != 0) {
            CPPSERV_ERROR("getaddrinfo error: {}", gai_strerror(errno));
            return status;
        }
        m_AddressInfo.ai_addrlen = res->ai_addrlen;
        m_AddressInfo.ai_addr = res->ai_addr;
        freeaddrinfo(res);
        status = (int)recvfrom(m_Socket, buffer, len - 1, 0, m_AddressInfo.ai_addr, &m_AddressInfo.ai_addrlen);
        if (status < 0) {
            CPPSERV_ERROR("readFrom error: {}", gai_strerror(errno));
        }
        buf = std::string(buffer);
        return status;
    }

    int Socket::SocketSetOpt(int level, int optname, void* optval) {
        unsigned int len = sizeof(optval);
        int status = ::setsockopt(m_Socket, level, optname, optval, len);
        if (status < 0) {
            CPPSERV_ERROR("socket_set_opt error: {}", gai_strerror(errno));
        }
        return status;
    }

    int Socket::SocketGetOpt(int level, int optname, void* optval) {
        unsigned int len = sizeof(optval);
        int status = ::getsockopt(m_Socket, level, optname, optval, &len);
        if (status < 0) {
            CPPSERV_ERROR("socket_get_opt error: {}", gai_strerror(errno));
        }
        return status;
    }

    int Socket::SetBlocking() {
        long status = fcntl(m_Socket, F_GETFL, NULL);
        if (status < 0) {
            CPPSERV_ERROR("set_blocking(get) error: {}", gai_strerror(errno));
        }
        status &= (~O_NONBLOCK);
        status = fcntl(m_Socket, F_SETFL, status);
        if (status < 0) {
            CPPSERV_ERROR("set_blocking(set) error: {}", gai_strerror(errno));
        }
        return (int)status;
    }

    int Socket::SetNonBlocking() {
        long status = fcntl(m_Socket, F_GETFL, NULL);
        if (status < 0) {
            CPPSERV_ERROR("set_non_blocking(get) error: {}", gai_strerror(errno));
        }
        status |= O_NONBLOCK;
        status = fcntl(m_Socket, F_SETFL, status);
        if (status < 0) {
            CPPSERV_ERROR("set_non_blocking(set) error: {}", gai_strerror(errno));
        }
        return (int)status;
    }

    int Socket::SocketShutdown(int how) {
        int status = ::shutdown(m_Socket, how);
        if (status < 0) {
            CPPSERV_ERROR("shutdown error: {}", gai_strerror(errno));
        }
        return status;
    }

    void Socket::Close() {
        ::close(m_Socket);
    }

    int Socket::Select(std::vector<Socket>* reads, std::vector<Socket>* writes, std::vector<Socket>* exceptions, int seconds) {
        int id = reads->at(0).m_Socket;
        struct timeval tv;
        fd_set readfds;
        fd_set writefds;
        fd_set exceptfds;

        tv.tv_sec = seconds;
        tv.tv_usec = 0;


        FD_ZERO(&readfds);

        FD_ZERO(&writefds);

        FD_ZERO(&exceptfds);

        int maxSock = 0;
        if (reads != NULL) {
            for (int i = 0; i < reads->size(); i++) {
                int sockInt = reads->at(i).m_Socket;
                if (sockInt > maxSock) {
                    maxSock = sockInt;
                }
                FD_SET(sockInt, &readfds);
            }
        }
        if (writes != NULL) {
            for (int i = 0; i < writes->size(); i++) {
                int sockInt = writes->at(i).m_Socket;
                if (sockInt > maxSock) {
                    maxSock = sockInt;
                }
                FD_SET(sockInt, &writefds);
            }
        }
        if (exceptions != NULL) {
            for (int i = 0; i < exceptions->size(); i++) {
                int sockInt = exceptions->at(i).m_Socket;
                if (sockInt > maxSock) {
                    maxSock = sockInt;
                }
                FD_SET(sockInt, &exceptfds);
            }
        }

        // Um writefds und exceptfds kümmern wir uns nicht:
        int result = ::select(maxSock + 1, &readfds, &writefds, &exceptfds, &tv);

        if (result < 0) {
            CPPSERV_ERROR("select error: {}", gai_strerror(errno));
        }
        if (reads != NULL) {
            for (int i = (int)reads->size() - 1; i >= 0; i--) {
                if (!FD_ISSET(reads->at(i).m_Socket, &readfds)) {
                    reads->erase(reads->begin() + i);
                }
            }
        }
        if (writes != NULL) {
            for (int i = (int)writes->size() - 1; i >= 0; i--) {
                if (!FD_ISSET(writes->at(i).m_Socket, &writefds)) {
                    writes->erase(reads->begin() + i);
                }
            }
        }
        if (exceptions != NULL) {
            for (int i = (int)exceptions->size() - 1; i >= 0; i--) {
                if (!FD_ISSET(exceptions->at(i).m_Socket, &exceptfds)) {
                    exceptions->erase(exceptions->begin() + i);
                }
            }
        }
        return result;
    }

    std::string Socket::IpFromHostName(std::string hostname) {
        hostent* record = gethostbyname(hostname.c_str());
        if (record == NULL) {
            CPPSERV_ERROR("Is unavailable: {}", hostname);
        }
        in_addr* address = (in_addr*)record->h_addr;
        std::string ip_address = inet_ntoa(*address);
        return ip_address;
    }

}