/*
 * server_mt_tcp.c
 * A multi-threaded TCP server that handles each client connection in a separate thread.
 * It performs the same file transfer as in assignment 1.
 */

 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <unistd.h>
 #include <arpa/inet.h>
 #include <pthread.h>
 #include <sys/socket.h>
 #include <netinet/in.h>
 
 #define PORT 8080
 #define BUFFER_SIZE 1024
 
 // Client thread function
 void *client_handler(void *arg) {
     int client_sock = *(int *)arg;
     free(arg);
     char buffer[BUFFER_SIZE] = {0};
 
     // Receive client request
     int n = read(client_sock, buffer, BUFFER_SIZE);
     if (n <= 0) {
         perror("Read failed");
         close(client_sock);
         pthread_exit(NULL);
     }
     buffer[n] = '\0';
     printf("Received request: %s\n", buffer);
 
     // Open the image file
     FILE *fp = fopen("sample.jpg", "rb");
     if (!fp) {
         perror("File open failed");
         close(client_sock);
         pthread_exit(NULL);
     }
     
     // Get file size and read file into buffer
     fseek(fp, 0, SEEK_END);
     long file_size = ftell(fp);
     rewind(fp);
     char *file_buffer = malloc(file_size);
     if (!file_buffer) {
         perror("Memory allocation failed");
         fclose(fp);
         close(client_sock);
         pthread_exit(NULL);
     }
     fread(file_buffer, 1, file_size, fp);
     fclose(fp);
 
     // Rename file using client socket descriptor and send data
     char new_filename[256];
     snprintf(new_filename, sizeof(new_filename), "image_%d.jpg", client_sock);
     write(client_sock, new_filename, strlen(new_filename) + 1);
     write(client_sock, &file_size, sizeof(file_size));
     write(client_sock, file_buffer, file_size);
     printf("File sent to client as %s\n", new_filename);
 
     free(file_buffer);
     close(client_sock);
     pthread_exit(NULL);
 }
 
 int main() {
     int sockfd;
     struct sockaddr_in server_addr, client_addr;
     socklen_t addr_len = sizeof(client_addr);
     
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
     
     if (listen(sockfd, 10) < 0) {
         perror("Listen failed");
         close(sockfd);
         exit(EXIT_FAILURE);
     }
     printf("Multi-threaded TCP Server listening on port %d\n", PORT);
     
     while (1) {
         int *client_sock = malloc(sizeof(int));
         *client_sock = accept(sockfd, (struct sockaddr*)&client_addr, &addr_len);
         if (*client_sock < 0) {
             perror("Accept failed");
             free(client_sock);
             continue;
         }
         pthread_t tid;
         pthread_create(&tid, NULL, client_handler, client_sock);
         pthread_detach(tid);
     }
     
     close(sockfd);
     return 0;
 }
 