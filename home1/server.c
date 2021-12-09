#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>

enum errors {
    OK,
    ERR_INCORRECT_ARGS,
    ERR_SOCKET,
    ERR_SETSOCKETOPT,
    ERR_BIND,
    ERR_LISTEN
};

int init_socket(int port) {
    int server_socket, socket_option = 1;
    struct sockaddr_in server_address;

    //open socket, return socket descriptor
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("Fail: open socket");
        exit(ERR_SOCKET);
    }

    //set socket option
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &socket_option, (socklen_t) sizeof socket_option);
    if (server_socket < 0) {
        perror("Fail: set socket options");
        exit(ERR_SETSOCKETOPT);
    }

    //set socket address
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = INADDR_ANY;
    if (bind(server_socket, (struct sockaddr *) &server_address, (socklen_t) sizeof server_address) < 0) {
        perror("Fail: bind socket address");
        exit(ERR_BIND);
    }

    //listen mode start
    if (listen(server_socket, 5) < 0) {
        perror("Fail: bind socket address");
        exit(ERR_LISTEN);
    }
    return server_socket;
}


int main(int argc, char** argv) {
    int port, server_socket[5], client_socket[5];
    struct sockaddr_in client_address;
    socklen_t size = sizeof client_address;
    const char data[] = "VMK";
    int client_number = atoi(argv[2]);
    char ch;


    if (argc != 3) {
        puts("Incorrect args.");
        puts("./server <port>");
        puts("Example:");
        puts("./server 5000");
        return ERR_INCORRECT_ARGS;
    }
    port = atoi(argv[1]);
    for (int i = 0; i < client_number; i++) {
        server_socket[i] = init_socket(port++);
    }
    for (;;) {
        for (int i = 0; i < client_number; i++) {
            puts("Wait for connection");
            client_socket[i] = accept(server_socket[i],
                              (struct sockaddr *) &client_address,
                              &size);
            printf("connected: %s %d\n", inet_ntoa(client_address.sin_addr),
                                       ntohs(client_address.sin_port));
            puts("Sent data:");

        }
        int j = 0;
        for (;;j++) {
            if (j == client_number) {
                j = 0;
            }
            if (read(client_socket[j], &ch, 1) < 0) {
                break;
            }
            sleep(2);
            if (ch == '\n') {
                while(1){}
            }
            putchar(ch);
            putchar('\n');
        }
        //write(client_socket, data, sizeof data);
        //for (int i = 0; i < sizeof data; i++) {
          //  printf("%d ", data[i]);
        //}
        //puts(data);  close(client_socket);
        for (int i = 0; i < client_number; i++)
            close(client_socket[i]);
    }
    return OK;
}
