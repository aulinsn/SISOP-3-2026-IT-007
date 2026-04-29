#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 8080

int sock;

void* receive_msg(void* arg) {
    char buffer[1024];

    while (1) {
        memset(buffer, 0, sizeof(buffer));
        int valread = read(sock, buffer, sizeof(buffer));
        if (valread <= 0) break;
        printf("%s", buffer);
    }

    return NULL;
}

int main() {
    struct sockaddr_in serv_addr;

    sock = socket(AF_INET, SOCK_STREAM, 0);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);

    connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

    char name[50];
    printf("Enter your name: ");
    fgets(name, sizeof(name), stdin);
    name[strcspn(name, "\n")] = 0;

    send(sock, name, strlen(name), 0);

    // ===== ADMIN =====
    if (strcmp(name, "The Knights") == 0) {
        char password[50];
        printf("Enter Password: ");
        fgets(password, sizeof(password), stdin);
        password[strcspn(password, "\n")] = 0;

        send(sock, password, strlen(password), 0);

        char response[1024];
        read(sock, response, sizeof(response));
        printf("%s\n", response);

        while (1) {
            printf("\n=== THE KNIGHTS CONSOLE ===\n");
            printf("1. Active Users\n2. Uptime\n3. Shutdown\n4. Exit\n> ");

            char cmd[10];
            fgets(cmd, sizeof(cmd), stdin);

            send(sock, cmd, strlen(cmd), 0);

            if (cmd[0] == '4') break;

            memset(response, 0, sizeof(response));
            read(sock, response, sizeof(response));
            printf("%s\n", response);

            if (cmd[0] == '3') break;
        }

        close(sock);
        return 0;
    }

    // ===== USER =====
    pthread_t recv_thread;
    pthread_create(&recv_thread, NULL, receive_msg, NULL);

    char message[1024];

    while (1) {
        fgets(message, sizeof(message), stdin);
        message[strcspn(message, "\n")] = 0;

        if (strcmp(message, "exit") == 0) break;

        send(sock, message, strlen(message), 0);
    }

    close(sock);
    return 0;
}
