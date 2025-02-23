/*
 * rpc_async_client.c
 * A client for asynchronous RPC.
 * It first sends "foo" to request computation and prints the ACK.
 * Then, after a delay, it sends a query ("result <id>") to retrieve the computation result.
 *
 * (For simplicity, this demo assumes the ACK contains request ID 1.)
 */

 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <unistd.h>
 #include <arpa/inet.h>
 #include <sys/socket.h>
 
 #define SERVER_IP "127.0.0.1"
 #define PORT 8082
 #define BUFFER_SIZE 1024
 
 // Helper function to connect to the server and send a command
 void async_rpc_call(const char *command) {
     int sockfd;
     struct sockaddr_in server_addr;
     char buffer[BUFFER_SIZE];
     
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0) {
         perror("Socket creation error");
         exit(EXIT_FAILURE);
     }
     server_addr.sin_family = AF_INET;
     server_addr.sin_port = htons(PORT);
     if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0) {
         perror("Invalid address");
         close(sockfd);
         exit(EXIT_FAILURE);
     }
     if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
         perror("Connection failed");
         close(sockfd);
         exit(EXIT_FAILURE);
     }
     write(sockfd, command, strlen(command) + 1);
     int n = read(sockfd, buffer, BUFFER_SIZE);
     if (n > 0) {
         buffer[n] = '\0';
         printf("Server response: %s\n", buffer);
     }
     close(sockfd);
 }
 
 // Helper function to query the result by request ID
 void query_result(int req_id) {
     int sockfd;
     struct sockaddr_in server_addr;
     char buffer[BUFFER_SIZE];
     
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0) {
         perror("Socket creation error");
         exit(EXIT_FAILURE);
     }
     server_addr.sin_family = AF_INET;
     server_addr.sin_port = htons(PORT);
     if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0) {
         perror("Invalid address");
         close(sockfd);
         exit(EXIT_FAILURE);
     }
     if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
         perror("Connection failed");
         close(sockfd);
         exit(EXIT_FAILURE);
     }
     char query[BUFFER_SIZE];
     snprintf(query, sizeof(query), "result %d", req_id);
     write(sockfd, query, strlen(query) + 1);
     int n = read(sockfd, buffer, BUFFER_SIZE);
     if (n > 0) {
         buffer[n] = '\0';
         printf("RPC result: %s\n", buffer);
     }
     close(sockfd);
 }
 
 int main() {
     // Initiate asynchronous RPC call ("foo")
     async_rpc_call("foo");
     
     // In a real implementation, the ACK would be parsed to get the request ID.
     // For demonstration, we assume the request ID is 1.
     sleep(2); // Wait for async computation to finish
     query_result(1);
     
     return 0;
 }
 