/*
 * rpc_async_server.c
 * An asynchronous RPC server that supports the "foo" operation.
 * Upon receiving a "foo" command, the server immediately sends an ACK with a unique request ID.
 * The heavy computation (foo) is then performed in a separate thread.
 * Clients can later query the result using the "result <id>" command.
 */

 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <unistd.h>
 #include <arpa/inet.h>
 #include <pthread.h>
 
 #define PORT 8082
 #define BUFFER_SIZE 1024
 #define MAX_RESULTS 100
 
 typedef struct {
     int id;
     char result[BUFFER_SIZE];
     int ready; // 0: not ready, 1: ready
 } rpc_result;
 
 rpc_result results[MAX_RESULTS];
 int next_id = 1;
 pthread_mutex_t result_mutex = PTHREAD_MUTEX_INITIALIZER;
 
 int generate_request_id() {
     pthread_mutex_lock(&result_mutex);
     int id = next_id++;
     pthread_mutex_unlock(&result_mutex);
     return id;
 }
 
 void store_result(int id, const char *result_str) {
     pthread_mutex_lock(&result_mutex);
     for (int i = 0; i < MAX_RESULTS; i++) {
         if (results[i].id == 0) {
             results[i].id = id;
             strncpy(results[i].result, result_str, BUFFER_SIZE);
             results[i].ready = 1;
             break;
         }
     }
     pthread_mutex_unlock(&result_mutex);
 }
 
 char *get_result(int id) {
     for (int i = 0; i < MAX_RESULTS; i++) {
         if (results[i].id == id && results[i].ready)
             return results[i].result;
     }
     return NULL;
 }
 
 // Background thread to process "foo" operation
 void *async_foo_handler(void *arg) {
     int request_id = *(int *)arg;
     free(arg);
     long sum = 0;
     for (long i = 0; i < 1000000; i++)
         sum += i;
     char result_str[BUFFER_SIZE];
     snprintf(result_str, sizeof(result_str), "foo result: %ld", sum);
     store_result(request_id, result_str);
     pthread_exit(NULL);
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
     
     if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
         perror("Bind failed");
         close(sockfd);
         exit(EXIT_FAILURE);
     }
     
     if (listen(sockfd, 5) < 0) {
         perror("Listen failed");
         close(sockfd);
         exit(EXIT_FAILURE);
     }
     printf("Asynchronous RPC Server listening on port %d\n", PORT);
     
     while (1) {
         client_sock = accept(sockfd, (struct sockaddr *)&client_addr, &addr_len);
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
         
         // Expected commands:
         // "foo" -> initiate async computation and return ACK with request ID.
         // "result <id>" -> query the result.
         char *command = strtok(buffer, " ");
         if (strcmp(command, "foo") == 0) {
             int req_id = generate_request_id();
             char ack[BUFFER_SIZE];
             snprintf(ack, sizeof(ack), "ACK %d", req_id);
             write(client_sock, ack, strlen(ack) + 1);
             
             int *p_id = malloc(sizeof(int));
             *p_id = req_id;
             pthread_t tid;
             pthread_create(&tid, NULL, async_foo_handler, p_id);
             pthread_detach(tid);
         } else if (strcmp(command, "result") == 0) {
             char *id_str = strtok(NULL, " ");
             if (id_str) {
                 int req_id = atoi(id_str);
                 char *res = get_result(req_id);
                 if (res)
                     write(client_sock, res, strlen(res) + 1);
                 else {
                     char msg[] = "Result not ready";
                     write(client_sock, msg, strlen(msg) + 1);
                 }
             }
         } else {
             char msg[] = "Unknown command";
             write(client_sock, msg, strlen(msg) + 1);
         }
         close(client_sock);
     }
     close(sockfd);
     return 0;
 }
 