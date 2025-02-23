/*
 * server_tcp.c
 * A simple single-threaded TCP server that sends an image file.
 * It renames the image (using the client socket descriptor) before sending.
 */

 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <unistd.h>
 #include <arpa/inet.h>
 #include <sys/socket.h>
 #include <netinet/in.h>
 
 #define PORT 8080
 #define BUFFER_SIZE 1024
 
 int main() {
     int sockfd, client_sock;
     struct sockaddr_in server_addr, client_addr;
     socklen_t addr_len = sizeof(client_addr);
     char buffer[BUFFER_SIZE] = {0};
 
     // Create a TCP socket
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0) {
         perror("Socket creation error");
         exit(EXIT_FAILURE);
     }
     
     // Configure server address (bind to any interface)
     server_addr.sin_family = AF_INET;
     server_addr.sin_addr.s_addr = INADDR_ANY;
     server_addr.sin_port = htons(PORT);
 
     // Bind the socket to port 8080
     if (bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
         perror("Bind failed");
         close(sockfd);
         exit(EXIT_FAILURE);
     }
     
     // Listen for incoming connections
     if (listen(sockfd, 3) < 0) {
         perror("Listen failed");
         close(sockfd);
         exit(EXIT_FAILURE);
     }
     printf("TCP Server listening on port %d\n", PORT);
     
     // Accept one client connection (blocking call)
     client_sock = accept(sockfd, (struct sockaddr*)&client_addr, &addr_len);
     if (client_sock < 0) {
         perror("Accept failed");
         close(sockfd);
         exit(EXIT_FAILURE);
     }
     printf("Client connected\n");
 
     // Receive client request (e.g., file name request)
     int n = read(client_sock, buffer, BUFFER_SIZE);
     if (n <= 0) {
         perror("Read failed");
         close(client_sock);
         close(sockfd);
         exit(EXIT_FAILURE);
     }
     buffer[n] = '\0';
     printf("Received request: %s\n", buffer);
 
     // Open the image file (assumes "sample.jpg" exists in project root)
     FILE *fp = fopen("sample.jpg", "rb");
     if (!fp) {
         perror("File open failed");
         close(client_sock);
         close(sockfd);
         exit(EXIT_FAILURE);
     }
     
     // Get file size
     fseek(fp, 0, SEEK_END);
     long file_size = ftell(fp);
     rewind(fp);
     
     // Allocate memory and read file content
     char *file_buffer = malloc(file_size);
     if (!file_buffer) {
         perror("Memory allocation failed");
         fclose(fp);
         close(client_sock);
         close(sockfd);
         exit(EXIT_FAILURE);
     }
     fread(file_buffer, 1, file_size, fp);
     fclose(fp);
 
     // Rename the file by appending the client socket descriptor
     char new_filename[256];
     snprintf(new_filename, sizeof(new_filename), "image_%d.jpg", client_sock);
     write(client_sock, new_filename, strlen(new_filename) + 1);
     
     // Send file size and file content
     write(client_sock, &file_size, sizeof(file_size));
     write(client_sock, file_buffer, file_size);
     printf("File sent to client as %s\n", new_filename);
 
     free(file_buffer);
     close(client_sock);
     close(sockfd);
     return 0;
 }
 