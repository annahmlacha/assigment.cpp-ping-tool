#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

int main() {
    WSADATA wsaData;
    SOCKET sock;
    struct sockaddr_in dest;
    const char* host = "8.8.8.8"; // Google DNS server for example
    int timeout = 1000; // Timeout in milliseconds

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed." << std::endl;
        return 1;
    }

    // Create socket
    if ((sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) == INVALID_SOCKET) {
        std::cerr << "Socket creation failed." << std::endl;
        WSACleanup();
        return 1;
    }

    // Set timeout option
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));

    // Set up destination address
    dest.sin_family = AF_INET;
    dest.sin_addr.s_addr = inet_addr(host);

    // ICMP header
    struct icmp {
        unsigned char type; // ICMP type
        unsigned char code; // ICMP code
        unsigned short checksum; // Checksum
        unsigned short id; // Identification
        unsigned short seq; // Sequence number
    } icmp_hdr;

    icmp_hdr.type = 8; // Echo request
    icmp_hdr.code = 0;
    icmp_hdr.id = (unsigned short)GetCurrentProcessId();
    icmp_hdr.seq = 0;
    icmp_hdr.checksum = 0;

    // Calculate checksum
    icmp_hdr.checksum = 0;
    unsigned short* ptr = (unsigned short*)&icmp_hdr;
    unsigned int sum = 0;
    for (int i = 0; i < sizeof(icmp_hdr) / 2; ++i) {
        sum += *ptr++;
    }
    icmp_hdr.checksum = (unsigned short)~(sum + (sum >> 16));

    // Send ICMP echo request
    if (sendto(sock, (char*)&icmp_hdr, sizeof(icmp_hdr), 0, (struct sockaddr*)&dest, sizeof(dest)) == SOCKET_ERROR) {
        std::cerr << "Send failed." << std::endl;
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    // Receive ICMP echo reply
    char recvbuf[1024];
    int recvsize = sizeof(dest);
    if (recvfrom(sock, recvbuf, sizeof(recvbuf), 0, (struct sockaddr*)&dest, &recvsize) == SOCKET_ERROR) {
        std::cerr << "Receive failed." << std::endl;
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    std::cout << "Ping to " << host << " successful." << std::endl;

    // Clean up
    closesocket(sock);
    WSACleanup();

    return 0;
}
