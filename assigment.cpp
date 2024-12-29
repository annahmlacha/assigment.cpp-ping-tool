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

    cout<< "thank you"<<
    #include <iostream>
#include <cstring>
#include <chrono>
#include <thread>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/ip_icmp.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <netdb.h>

// ICMP Header Structure
struct icmp_packet {
    struct icmphdr hdr;
    char data[64]; // You can increase the size if needed
};

// Function to calculate the checksum
unsigned short checksum(void *b, int len) {
    unsigned short *buf = (unsigned short*)b;
    unsigned int sum = 0;
    unsigned short result;

    for (sum = 0; len > 1; len -= 2)
        sum += *buf++;
    if (len == 1)
        sum += *(unsigned char*)buf;

    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    result = ~sum;
    return result;
}

// Function to create an ICMP Echo Request packet
void create_icmp_packet(struct icmp_packet *pkt, int seq_num) {
    pkt->hdr.type = ICMP_ECHO;
    pkt->hdr.code = 0;
    pkt->hdr.un.echo.id = getpid();
    pkt->hdr.un.echo.sequence = seq_num;
    pkt->hdr.checksum = 0;
    std::memset(pkt->data, 0x42, sizeof(pkt->data));  // Fill the packet with some data (e.g., 'B')

    pkt->hdr.checksum = checksum(&pkt->hdr, sizeof(struct icmphdr) + sizeof(pkt->data));
}

// Function to send a ping request and receive the response
void ping(const char* ip_address) {
    int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sockfd < 0) {
        std::cerr << "Error: Unable to open socket" << std::endl;
        return;
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = 0;
    addr.sin_addr.s_addr = inet_addr(ip_address);

    struct icmp_packet pkt;
    int seq_num = 1;

    // Send 4 ping requests
    for (int i = 0; i < 4; ++i) {
        create_icmp_packet(&pkt, seq_num);

        auto start = std::chrono::high_resolution_clock::now();
        if (sendto(sockfd, &pkt, sizeof(pkt), 0, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
            std::cerr << "Error: Unable to send packet" << std::endl;
        } else {
            char buffer[1024];
            socklen_t addr_len = sizeof(addr);

            if (recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr*)&addr, &addr_len) < 0) {
                std::cerr << "Error: No response received" << std::endl;
            } else {
                auto end = std::chrono::high_resolution_clock::now();
                std::chrono::duration<double> duration = end - start;
                std::cout << "Reply from " << ip_address << ": time=" << duration.count() * 1000 << " ms" << std::endl;
            }
        }

        // Wait for a second before sending the next ping
        std::this_thread::sleep_for(std::chrono::seconds(1));
        seq_num++;
    }

    close(sockfd);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <IP Address>" << std::endl;
        return 1;
    }

    ping(argv[1]);

    return 0;
}


    return 0;
}
