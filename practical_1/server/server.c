#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <asm-generic/socket.h>

#define PORT 8080

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    socklen_t addrlen = sizeof(address);
    char* greeting_message = "Welcome to the server!";

    // Create server socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("[-] Error creating socket");
        exit(EXIT_FAILURE);
    }

    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("[-] Error binding");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("[-] Error listening");
        exit(EXIT_FAILURE);
    }

    if ((new_socket = accept(server_fd, (struct sockaddr*)&address, &addrlen)) < 0) {
        perror("[-] Error accepting connection");
        exit(EXIT_FAILURE);
    }

    // Receive client's file
    FILE* received_fp = fopen("received_file.txt", "wb");
    if (received_fp == NULL) {
        perror("[-] Error creating received file");
        exit(EXIT_FAILURE);
    }
    printf("received-file.txt created, writing data from client to this file now.\n");

    ssize_t bytes_received;
    char buffer[1024];
    bytes_received = recv(new_socket, buffer, sizeof(buffer), 0);
    fwrite(buffer, 1, bytes_received, received_fp);
    printf("Received file saved as received_file.txt\n");

    // Read the greeting message from a local file (greeting.txt)
    FILE* greeting_file = fopen("hello_from_server.txt", "rb");
    if (greeting_file != NULL) {
        fseek(greeting_file, 0, SEEK_END);
        long greeting_size = ftell(greeting_file);
        fseek(greeting_file, 0, SEEK_SET);

        fread(buffer, 1, greeting_size, greeting_file);
        fclose(greeting_file);

        // Send greeting message back to the client
        send(new_socket, buffer, greeting_size, 0);
        printf("Greeting sent to client.\n");
    } else {
        perror("[-] Error opening greeting file");
    }

    fclose(received_fp);
    close(new_socket);
    close(server_fd);
    return 0;
}
