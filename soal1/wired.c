#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <time.h>

#define PORT 8080
#define MAX_CLIENTS 100

int clients[MAX_CLIENTS];
char client_names[MAX_CLIENTS][50];
int client_count = 0;

pthread_mutex_t lock;
time_t start_time;

// ================= LOG =================
void write_log(const char *type, const char *msg) {
    FILE *f = fopen("history.log", "a");
    if (!f) return;

    time_t now = time(NULL);
    struct tm *t = localtime(&now);

    fprintf(f, "[%04d-%02d-%02d %02d:%02d:%02d] [%s] %s\n",
        t->tm_year+1900, t->tm_mon+1, t->tm_mday,
        t->tm_hour, t->tm_min, t->tm_sec,
        type, msg);

    fclose(f);
}

// ================= ADD CLIENT =================
void add_client(int sock, char *name) {
    pthread_mutex_lock(&lock);
    clients[client_count] = sock;
    strcpy(client_names[client_count], name);
    client_count++;
    pthread_mutex_unlock(&lock);
}

// ================= REMOVE CLIENT =================
void remove_client(int sock) {
    pthread_mutex_lock(&lock);
    for (int i = 0; i < client_count; i++) {
        if (clients[i] == sock) {
            for (int j = i; j < client_count - 1; j++) {
                clients[j] = clients[j+1];
                strcpy(client_names[j], client_names[j+1]);
            }
            client_count--;
            break;
        }
    }
    pthread_mutex_unlock(&lock);
}

// ================= BROADCAST =================
void broadcast(char *msg, int sender) {
    pthread_mutex_lock(&lock);
    for (int i = 0; i < client_count; i++) {
        if (clients[i] != sender) {
            send(clients[i], msg, strlen(msg), 0);
        }
    }
    pthread_mutex_unlock(&lock);
}

// ================= HANDLE CLIENT =================
void* handle_client(void *arg) {
    int sock = *(int*)arg;
    free(arg);

    char name[50], buffer[1024];

    read(sock, name, sizeof(name));
    name[strcspn(name, "\n")] = 0;

    // ===== ADMIN =====
    if (strcmp(name, "The Knights") == 0) {
        char password[50];
        read(sock, password, sizeof(password));
        password[strcspn(password, "\n")] = 0;

        if (strcmp(password, "protocol7") == 0) {
            send(sock, "[System] Authentication Successful\n", 36, 0);
            write_log("System", "[User 'The Knights' connected]");

            while (1) {
                char cmd[10];
                read(sock, cmd, sizeof(cmd));

                if (cmd[0] == '1') {
                    write_log("Admin", "[RPC_GET_USERS]");

                    char msg[1024] = "Active Users:\n";
                    pthread_mutex_lock(&lock);
                    for (int i = 0; i < client_count; i++) {
                        strcat(msg, client_names[i]);
                        strcat(msg, "\n");
                    }
                    pthread_mutex_unlock(&lock);

                    send(sock, msg, strlen(msg), 0);
                }
                else if (cmd[0] == '2') {
                    write_log("Admin", "[RPC_GET_UPTIME]");

                    time_t now = time(NULL);
                    char msg[100];
                    sprintf(msg, "Server uptime: %ld seconds\n", now - start_time);

                    send(sock, msg, strlen(msg), 0);
                }
                else if (cmd[0] == '3') {
                    write_log("Admin", "[RPC_SHUTDOWN]");
                    write_log("System", "[EMERGENCY SHUTDOWN INITIATED]");

                    char msg[] = "[System] Server is shutting down...\n";

                    pthread_mutex_lock(&lock);
                    for (int i = 0; i < client_count; i++) {
                        send(clients[i], msg, strlen(msg), 0);
                        close(clients[i]);
                    }
                    pthread_mutex_unlock(&lock);

                    printf("Server shutting down...\n");
                    sleep(1);
                    exit(0);
                }
                else if (cmd[0] == '4') {
                    break;
                }
            }
        }

        close(sock);
        return NULL;
    }

    // ===== USER BIASA =====
    add_client(sock, name);

    printf("[System] %s connected\n", name);

    char logmsg[100];
    sprintf(logmsg, "[User '%s' connected]", name);
    write_log("System", logmsg);

    while (1) {
        memset(buffer, 0, sizeof(buffer));
        int val = read(sock, buffer, sizeof(buffer));

        if (val <= 0) {
            printf("[System] %s disconnected\n", name);

            sprintf(logmsg, "[User '%s' disconnected]", name);
            write_log("System", logmsg);

            remove_client(sock);
            close(sock);
            break;
        }

        buffer[strcspn(buffer, "\n")] = 0;

        char msg[1200];
        sprintf(msg, "[%s]: %s\n", name, buffer);

        printf("%s", msg);
        broadcast(msg, sock);

        char chatlog[1200];
        sprintf(chatlog, "[%s]: %s", name, buffer);
        write_log("User", chatlog);
    }

    return NULL;
}

// ================= MAIN =================
int main() {
    int server_fd;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    pthread_mutex_init(&lock, NULL);
    start_time = time(NULL);

    write_log("System", "[SERVER ONLINE]");

    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    bind(server_fd, (struct sockaddr*)&address, sizeof(address));
    listen(server_fd, 10);

    printf("Server running on port %d...\n", PORT);

    while (1) {
        int new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen);

        int *p = malloc(sizeof(int));
        *p = new_socket;

        pthread_t tid;
        pthread_create(&tid, NULL, handle_client, p);
        pthread_detach(tid);
    }
}
