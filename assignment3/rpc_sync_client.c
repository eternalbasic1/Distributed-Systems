/*
 * rpc_sync_client.c
 * A simple client to test synchronous RPC calls.
 * It connects to the RPC server (port 8081) and sends commands.
 */

 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <unistd.h>
 #include <arpa/inet.h>
 #include <sys/socket.h>
 
 #define PORT 8081
 #define BUFFER_SIZE 1024
 #define SERVER_IP "127.0.0.1" // Change if needed
 
 void call_rpc(const char *command) {
     int sockfd;
     struct sockaddr_in server_addr;
     char buffer[BUFFER_SIZE];
     
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0) {
         perror("Socket creation error");
         return;
     }
     server_addr.sin_family = AF_INET;
     server_addr.sin_port = htons(PORT);
     if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0) {
         perror("Invalid address");
         close(sockfd);
         return;
     }
     if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
         perror("Connection failed");
         close(sockfd);
         return;
     }
     write(sockfd, command, strlen(command) + 1);
     int n = read(sockfd, buffer, BUFFER_SIZE);
     if (n > 0) {
         buffer[n] = '\0';
         printf("RPC result: %s\n", buffer);
     }
     close(sockfd);
 }
 
 int main() {
     // Example RPC calls:
     call_rpc("foo");
     call_rpc("add 10 20");
     call_rpc("sort 9,4,8,3,1");
     return 0;
 }
 