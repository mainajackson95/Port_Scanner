#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>

void port_scanner(const char *server_ip, int start_port, int end_port) {
    struct addrinfo hints, *serv_addr, *addr_info;
    int sock, status;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    for (int port = start_port; port <= end_port; ++port) {
        char port_str[6];  // Max port number is 65535
        snprintf(port_str, sizeof(port_str), "%d", port);

        int result = getaddrinfo(server_ip, port_str, &hints, &serv_addr);
        if (result != 0) {
            fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(result));
            continue;
        }

        // Try all addresses in the list
        for (addr_info = serv_addr; addr_info != NULL; addr_info = addr_info->ai_next) {
            sock = socket(addr_info->ai_family, addr_info->ai_socktype, addr_info->ai_protocol);
            if (sock == -1) {
                perror("Socket creation failed");
                continue;
            }

            // Attempt to connect
            status = connect(sock, addr_info->ai_addr, addr_info->ai_addrlen);
            if (status < 0) {
                printf("Port %d is NOT open\n", port);
                close(sock);
                continue;
            }

            printf("Port %d is open.\n", port);
            close(sock);
            break;  // Break out of the loop if connection is successful
        }

        freeaddrinfo(serv_addr);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("Usage: %s <server_ip> <start_port> <end_port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Extract arguments
    const char *server_ip = argv[1];
    int start_port = atoi(argv[2]);
    int end_port = atoi(argv[3]);

    // Validate IPv4 address
    struct sockaddr_in sa;
    int result = inet_pton(AF_INET, server_ip, &(sa.sin_addr));
    if (result <= 0) {
        printf("Invalid IPv4 address\n");
        exit(EXIT_FAILURE);
    }

    // Validate port range
    if (start_port < 1 || start_port > 65535 || end_port < 1 || end_port > 65535 || start_port > end_port) {
        printf("Invalid port range\n");
        exit(EXIT_FAILURE);
    }

    // Perform port scanning
    scan_ports(server_ip, start_port, end_port);

    return 0;
}
