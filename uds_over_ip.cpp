#include <iostream>
#include <thread>
#include <cstring>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <unistd.h>

#define TCP_PORT 8080
#define UDS_PATH "/tmp/uds_socket"

void handleUDSConnection(int tcpClientSocket) {
    int udsSocket = socket(AF_UNIX, SOCK_STREAM, 0);
    if (udsSocket < 0) {
        perror("UDS socket creation failed");
        return;
    }

    sockaddr_un udsAddr = {};
    udsAddr.sun_family = AF_UNIX;
    strncpy(udsAddr.sun_path, UDS_PATH, sizeof(udsAddr.sun_path) - 1);

    if (connect(udsSocket, (sockaddr*)&udsAddr, sizeof(udsAddr)) < 0) {
        perror("UDS connection failed");
        close(udsSocket);
        return;
    }

    char buffer[1024];
    while (true) {
        ssize_t bytesRead = recv(tcpClientSocket, buffer, sizeof(buffer), 0);
        if (bytesRead <= 0) break;

        send(udsSocket, buffer, bytesRead, 0);

        bytesRead = recv(udsSocket, buffer, sizeof(buffer), 0);
        if (bytesRead <= 0) break;

        send(tcpClientSocket, buffer, bytesRead, 0);
    }

    close(udsSocket);
    close(tcpClientSocket);
}

int main() {
    // Create TCP socket
    int tcpSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (tcpSocket < 0) {
        perror("TCP socket creation failed");
        return 1;
    }

    sockaddr_in tcpAddr = {};
    tcpAddr.sin_family = AF_INET;
    tcpAddr.sin_addr.s_addr = INADDR_ANY;
    tcpAddr.sin_port = htons(TCP_PORT);

    if (bind(tcpSocket, (sockaddr*)&tcpAddr, sizeof(tcpAddr)) < 0) {
        perror("TCP bind failed");
        close(tcpSocket);
        return 1;
    }

    if (listen(tcpSocket, 5) < 0) {
        perror("TCP listen failed");
        close(tcpSocket);
        return 1;
    }

    std::cout << "Listening for TCP connections on port " << TCP_PORT << "...\n";

    while (true) {
        int tcpClientSocket = accept(tcpSocket, nullptr, nullptr);
        if (tcpClientSocket < 0) {
            perror("TCP accept failed");
            continue;
        }

        std::thread(handleUDSConnection, std::ref(tcpClientSocket)).detach();
    }

    close(tcpSocket);
    return 0;
}