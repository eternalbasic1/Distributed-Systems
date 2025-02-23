/*
 * client_tcp.c
 * A simple TCP client that requests an image file and saves it locally.
 */

 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <unistd.h>
 #include <arpa/inet.h>
 #include <sys/socket.h>
 
 #define SERVER_IP "127.0.0.1"  // Change to server IP if needed
 #define PORT 8080
 #define BUFFER_SIZE 1024
 
 int main() {
     int sockfd;
     struct sockaddr_in server_addr;
     char buffer[BUFFER_SIZE];
     
     // Create TCP socket
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0) {
         perror("Socket creation error");
         exit(EXIT_FAILURE);
     }
     
     // Set server address
     server_addr.sin_family = AF_INET;
     server_addr.sin_port = htons(PORT);
     if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0) {
         perror("Invalid address");
         close(sockfd);
         exit(EXIT_FAILURE);
     }
     
     // Connect to server
     if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
         perror("Connection failed");
         close(sockfd);
         exit(EXIT_FAILURE);
     }
     
     // Send a request (e.g., "GET sample.jpg")
     char *request = "GET sample.jpg";
     write(sockfd, request, strlen(request) + 1);
     
     // Receive new file name from server
     int n = read(sockfd, buffer, BUFFER_SIZE);
     if (n <= 0) {
         perror("Read new filename failed");
         close(sockfd);
         exit(EXIT_FAILURE);
     }
     buffer[n] = '\0';
     printf("New filename: %s\n", buffer);
     
     // Receive file size
     long file_size;
     read(sockfd, &file_size, sizeof(file_size));
     printf("File size: %ld bytes\n", file_size);
     
     // Allocate memory and receive file content
     char *file_buffer = malloc(file_size);
     if (!file_buffer) {
         perror("Memory allocation failed");
         close(sockfd);
         exit(EXIT_FAILURE);
     }
     read(sockfd, file_buffer, file_size);
     
     // Save the received file using the new file name
     FILE *fp = fopen(buffer, "wb");
     if (!fp) {
         perror("Error opening file for writing");
         free(file_buffer);
         close(sockfd);
         exit(EXIT_FAILURE);
     }
     fwrite(file_buffer, 1, file_size, fp);
     fclose(fp);
     printf("File received and saved as %s\n", buffer);
     
     free(file_buffer);
     close(sockfd);
     return 0;
 }
 