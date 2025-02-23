/*
 * rpc_sync_server.c
 * A synchronous RPC server that supports three operations:
 *   - foo()  : Performs heavy computation.
 *   - add(i, j)  : Returns the sum of two integers.
 *   - sort(array)  : Sorts an array of integers.
 * The server listens on port 8081.
 */

 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <unistd.h>
 #include <arpa/inet.h>
 
 #define PORT 8081
 #define BUFFER_SIZE 1024
 
 // RPC function: foo() – simulate heavy computation
 void rpc_foo(int client_sock) {
     long sum = 0;
     for (long i = 0; i < 1000000; i++) {
         sum += i;
     }
     char result[BUFFER_SIZE];
     snprintf(result, sizeof(result), "foo result: %ld", sum);
     write(client_sock, result, strlen(result) + 1);
 }
 
 // RPC function: add(i, j)
 void rpc_add(int client_sock, int a, int b) {
     int sum = a + b;
     char result[BUFFER_SIZE];
     snprintf(result, sizeof(result), "add result: %d", sum);
     write(client_sock, result, strlen(result) + 1);
 }
 
 // RPC function: sort(array)
 void rpc_sort(int client_sock, char *numbers_str) {
     int nums[100], count = 0;
     char *token = strtok(numbers_str, ",");
     while (token != NULL && count < 100) {
         nums[count++] = atoi(token);
         token = strtok(NULL, ",");
     }
     // Simple bubble sort
     for (int i = 0; i < count - 1; i++) {
         for (int j = 0; j < count - i - 1; j++) {
             if (nums[j] > nums[j + 1]) {
                 int temp = nums[j];
                 nums[j] = nums[j + 1];
                 nums[j + 1] = temp;
             }
         }
     }
     char result[BUFFER_SIZE] = "sort result:";
     char temp[16];
     for (int i = 0; i < count; i++) {
         snprintf(temp, sizeof(temp), " %d", nums[i]);
         strcat(result, temp);
     }
     write(client_sock, result, strlen(result) + 1);
 }
 
 int main() {
     int sockfd, client_sock;
     struct sockaddr_in server_addr, client_addr;
     socklen_t addr_len = sizeof(client_addr);
     char buffer[BUFFER_SIZE];
     
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0) {
         perror("Socket creation failed");
         exit(EXIT_FAILURE);
     }
     
     server_addr.sin_family = AF_INET;
     server_addr.sin_addr.s_addr = INADDR_ANY;
     server_addr.sin_port = htons(PORT);
     
     if (bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
         perror("Bind failed");
         close(sockfd);
         exit(EXIT_FAILURE);
     }
     
     if (listen(sockfd, 5) < 0) {
         perror("Listen failed");
         close(sockfd);
         exit(EXIT_FAILURE);
     }
     printf("Synchronous RPC Server listening on port %d\n", PORT);
     
     while (1) {
         client_sock = accept(sockfd, (struct sockaddr*)&client_addr, &addr_len);
         if (client_sock < 0) {
             perror("Accept failed");
             continue;
         }
         int n = read(client_sock, buffer, BUFFER_SIZE);
         if (n <= 0) {
             close(client_sock);
             continue;
         }
         buffer[n] = '\0';
         
         // Command format: "command param1 param2 ..." or "sort <comma_separated_numbers>"
         char *command = strtok(buffer, " ");
         if (strcmp(command, "foo") == 0) {
             rpc_foo(client_sock);
         } else if (strcmp(command, "add") == 0) {
             char *param1 = strtok(NULL, " ");
             char *param2 = strtok(NULL, " ");
             if (param1 && param2)
                 rpc_add(client_sock, atoi(param1), atoi(param2));
         } else if (strcmp(command, "sort") == 0) {
             char *numbers = strtok(NULL, "\n");
             if (numbers)
                 rpc_sort(client_sock, numbers);
         } else {
             char msg[] = "Unknown RPC command";
             write(client_sock, msg, strlen(msg) + 1);
         }
         close(client_sock);
     }
     close(sockfd);
     return 0;
 }
 