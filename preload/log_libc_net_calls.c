#define _GNU_SOURCE
#include <stdio.h>
#include <dlfcn.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>

typedef int (*socket_t)(int, int, int);
typedef int (*bind_t)(int, const struct sockaddr *, socklen_t);
typedef int (*listen_t)(int, int);
typedef int (*connect_t)(int, const struct sockaddr *, socklen_t);
typedef ssize_t (*send_t)(int, const void *, size_t, int);
typedef ssize_t (*recv_t)(int, void *, size_t, int);

void get_ip_port(const struct sockaddr *addr, char *ip_str, int *port) {
    if (addr->sa_family == AF_INET) {
        struct sockaddr_in *addr_in = (struct sockaddr_in *)addr;
        inet_ntop(AF_INET, &(addr_in->sin_addr), ip_str, INET_ADDRSTRLEN);
        *port = ntohs(addr_in->sin_port);
    } else if (addr->sa_family == AF_INET6) {
        struct sockaddr_in6 *addr_in6 = (struct sockaddr_in6 *)addr;
        inet_ntop(AF_INET6, &(addr_in6->sin6_addr), ip_str, INET6_ADDRSTRLEN);
        *port = ntohs(addr_in6->sin6_port);
    }
}

int socket(int domain, int type, int protocol) {
    static socket_t real_socket = NULL;
    if (!real_socket) {
        real_socket = (socket_t)dlsym(RTLD_NEXT, "socket");
    }

    int sockfd = real_socket(domain, type, protocol);
    printf("[LOG] socket() called: domain=%d, type=%d, protocol=%d → sockfd=%d\n", domain, type, protocol, sockfd);
    fflush(stdout);

    return sockfd;
}

int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
    static bind_t real_bind = NULL;
    if (!real_bind) {
        real_bind = (bind_t)dlsym(RTLD_NEXT, "bind");
    }

    char ip_str[INET6_ADDRSTRLEN] = {0};
    int port = 0;
    get_ip_port(addr, ip_str, &port);

    int result = real_bind(sockfd, addr, addrlen);
    printf("[LOG] bind() called: sockfd=%d, addr=%s, port=%d, family=%d → result=%d\n",
           sockfd, ip_str, port, addr->sa_family, result);
    fflush(stdout);

    return result;
}

int listen(int sockfd, int backlog) {
    static listen_t real_listen = NULL;
    if (!real_listen) {
        real_listen = (listen_t)dlsym(RTLD_NEXT, "listen");
    }

    int result = real_listen(sockfd, backlog);
    printf("[LOG] listen() called: sockfd=%d, backlog=%d → result=%d\n", sockfd, backlog, result);
    fflush(stdout);

    return result;
}

int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
    static connect_t real_connect = NULL;
    if (!real_connect) {
        real_connect = (connect_t)dlsym(RTLD_NEXT, "connect");
    }

    char ip_str[INET6_ADDRSTRLEN] = {0};
    int port = 0;
    get_ip_port(addr, ip_str, &port);

    int result = real_connect(sockfd, addr, addrlen);
    printf("[LOG] connect() called: sockfd=%d, addr=%s, port=%d, family=%d → result=%d\n",
           sockfd, ip_str, port, addr->sa_family, result);
    fflush(stdout);

    return result;
}

ssize_t send(int sockfd, const void *buf, size_t len, int flags) {
    static send_t real_send = NULL;
    if (!real_send) {
        real_send = (send_t)dlsym(RTLD_NEXT, "send");
    }

    printf("[LOG] send() called: sockfd=%d, len=%zu\n", sockfd, len);
    if (buf && len > 0) {
        printf("[LOG] Data Sent: ");
        fwrite(buf, 1, len < 64 ? len : 64, stdout); // Limit to 64 bytes for readability
        printf("\n");
    }
    fflush(stdout);

    return real_send(sockfd, buf, len, flags);
}

ssize_t recv(int sockfd, void *buf, size_t len, int flags) {
    static recv_t real_recv = NULL;
    if (!real_recv) {
        real_recv = (recv_t)dlsym(RTLD_NEXT, "recv");
    }

    ssize_t ret = real_recv(sockfd, buf, len, flags);

    if (ret > 0) {
        printf("[LOG] recv() called: sockfd=%d, len=%zd\n", sockfd, ret);
        printf("[LOG] Data Received: ");
        fwrite(buf, 1, ret < 64 ? ret : 64, stdout); // Limit to 64 bytes for readability
        printf("\n");
    }
    fflush(stdout);

    return ret;
}

