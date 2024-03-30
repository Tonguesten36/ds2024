#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 8080

int main() {
    int client_fd;
    struct sockaddr_in serv_addr;
    char* hello = "Hello from client";
    char buffer[1024] = {0};

    // Create client socket
    if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("[-] Error creating socket");
        exit(EXIT_FAILURE);
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Connect to server
    if (connect(client_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("[-] Error connecting to server");
        exit(EXIT_FAILURE);
    }

    // Send local file (file.txt) to server
    FILE* fp = fopen("hello_from_client.txt", "rb");
    if (fp == NULL) {
        perror("[-] Error opening file");
        exit(EXIT_FAILURE);
    }

    size_t bytes_read;
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), fp)) > 0) {
        send(client_fd, buffer, bytes_read, 0);
    }
    printf("Sent hello_from_client.txt to server\n");
    fclose(fp);

    // Receive greeting from server
    ssize_t greeting_bytes_received = recv(client_fd, buffer, sizeof(buffer), 0);
    if (greeting_bytes_received > 0) {
        FILE* greeting_fp = fopen("received_greeting.txt", "wb");
        if (greeting_fp != NULL) {
            fwrite(buffer, 1, greeting_bytes_received, greeting_fp);
            fclose(greeting_fp);
            printf("Received greeting saved as received_greeting.txt\n");
        } else {
            perror("[-] Error creating received greeting file");
        }
    } else {
        perror("[-] Error receiving greeting from server");
    }

    close(client_fd);
    return 0;
}
