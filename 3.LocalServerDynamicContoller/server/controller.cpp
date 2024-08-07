#include <iostream>
#include <fstream>
#include <csignal>
#include <unistd.h>
#include <map>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <arpa/inet.h>

using namespace std;

// Read PID mappings from a file
map<string, pid_t> readPidConfig() {
    ifstream pids("pid.txt");
    map<string, pid_t> pidConfig;
    string name;
    pid_t pid;
    while (pids >> name >> pid) {
        pidConfig[name] = pid;
    }
    return pidConfig;
}

// Send a signal to a specific target
void sendSignal(pid_t pid, int sig) {
    if (kill(pid, sig) == -1) {
        cerr << "Failed to send signal " << sig << " to PID " << pid << endl;
    } else {
        cout << "Successfully sent signal " << sig << " to PID " << pid << endl;
    }
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    string command;

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port 8080
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080);

    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    auto pidConfig = readPidConfig();

    while(true) {
        memset(buffer, 0, sizeof(buffer));
        read(new_socket, buffer, 1024);
        command = string(buffer);
        cout << "Received command: " << command << endl;

        for (const auto& entry : pidConfig) {
            if (command.find(entry.first) != string::npos) {
                if (command.find("SIGUSR1") != string::npos) {
                    sendSignal(entry.second, SIGUSR1);
                } else if (command.find("SIGUSR2") != string::npos) {
                    sendSignal(entry.second, SIGUSR2);
                }
                break;
            }
        }
    }

    return 0;
}
