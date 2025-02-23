# Distributed Systems Project in C

## Overview

This project implements client-server communication and RPC in C. It covers:

- **Assignment 0:** Environment setup on a multi-machine cluster.
- **Assignment 1:** A single-threaded web server (TCP version) that delivers an image file.
- **Assignment 2:** A multi-threaded web server (TCP version) using pthreads.
- **Assignment 3:** A synchronous RPC server supporting operations:
  - `foo()` – heavy computation,
  - `add(i, j)` – summing two integers,
  - `sort(array)` – sorting a list of integers.
- **Assignment 4:** An asynchronous RPC server that immediately acknowledges a "foo" request and later returns the result upon query.

## Directory Structure

project/
├── assignment1/
│ ├── server_tcp.c
│ └── client_tcp.c
├── assignment2/
│ └── server_mt_tcp.c
├── assignment3/
│ ├── rpc_sync_server.c
│ └── rpc_sync_client.c
├── assignment4/
│ ├── rpc_async_server.c
│ └── rpc_async_client.c
├── sample.jpg # (Place a small image here for testing assignment 1)
├── Dockerfile
└── README.md

## Pre-requisites

- **Environment:** Ubuntu (or any Linux distribution)
- **Compiler:** GCC (installed by default on many Linux distributions)
- **Docker:** Optional, for containerized builds and testing
- **Note:** Make sure the file `sample.jpg` exists in the project root (used by Assignment 1).

## Compilation and Execution

### Using Docker

1. **Build the Docker image:**
   ```bash
   docker build -t distsys_project:latest .
   ```

# Run the container

```docker run -it --rm -p 8080:8080 -p 8081:8081 -p 8082:8082 distsys_project:latest

## Inside the container
Open multiple terminal sessions (or use tmux/screen) to run the different executables:
Assignment 1 (TCP):
Start the server: ./server_tcp
In another session, run the client: ./client_tcp
Assignment 2 (Multi-threaded TCP):
Run: ./server_mt_tcp
Assignment 3 (Synchronous RPC):
Start the RPC server: ./rpc_sync_server
In another session, run the client: ./rpc_sync_client
Assignment 4 (Asynchronous RPC):
Start the async RPC server: ./rpc_async_server
In another session, run the client: ./rpc_async_client

Compile Manually (from project root):

gcc -o server_tcp assignment1/server_tcp.c
gcc -o client_tcp assignment1/client_tcp.c
gcc -o server_mt_tcp assignment2/server_mt_tcp.c -lpthread
gcc -o rpc_sync_server assignment3/rpc_sync_server.c
gcc -o rpc_sync_client assignment3/rpc_sync_client.c
gcc -o rpc_async_server assignment4/rpc_async_server.c -lpthread
gcc -o rpc_async_client assignment4/rpc_async_client.c -lpthread





---

## Final Notes

This project provides a simplified but comprehensive example in C that demonstrates:
- Basic client–server communication (single-threaded and multi-threaded).
- Implementation of synchronous and asynchronous RPC calls.
- Building and testing using Docker.

You can expand each part with additional error handling, logging, and features as needed for your course. Feel free to ask if you have further questions or need additional clarifications!
```
