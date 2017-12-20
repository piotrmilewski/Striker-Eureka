#include "pipe_networking.h"

/*=========================
  server_setup
  args:
  creates the WKP (upstream) and opens it, waiting for a
  connection.
  removes the WKP once a connection has been made
  returns the file descriptor for the upstream pipe.
  =========================*/
int server_setup() {
  int from_client;

  mkfifo(SERVERNAME, 0600);

  //block on open, receive message
  printf("[server] handshake: making WKP\n");
  from_client = open(SERVERNAME, O_RDONLY, 0);
  printf("[server] handshake: opened WKP & opened connection\n");

  remove(SERVERNAME);
  printf("[server] handshake: removed WKP\n");

  int pid = fork();
  
  if (!pid)
    return from_client;

  return from_client;
}


/*=========================
  server_connect
  args: int from_client
  handles the subserver portion of the 3 way handshake
  returns the file descriptor for the downstream pipe.
  =========================*/
int server_connect(int from_client){
  char buffer[HANDSHAKE_BUFFER_SIZE];
  printf("[subserver] handshake: reading from WKP\n");
  read(from_client, buffer, sizeof(buffer));
  printf("[subserver] handshake: received [%s]\n", buffer);

  int to_client = open(buffer, O_WRONLY, 0);
  write(to_client, buffer, sizeof(buffer));

  //read from client
  read(from_client, buffer, sizeof(buffer));
  printf("[subserver] handshake received: %s\n", buffer);

  return to_client;
}

/*=========================
  server_handshake
  args: int * to_client
  Performs the server side pipe 3 way handshake.
  Sets *to_client to the file descriptor to the downstream pipe.
  returns the file descriptor for the upstream pipe.
  =========================*/

int server_handshake(int *to_client) {

  int from_client;

  char buffer[HANDSHAKE_BUFFER_SIZE];

  mkfifo(SERVERNAME, 0600);

  //block on open, recieve mesage
  printf("[server] handshake: making wkp\n");
  from_client = open( SERVERNAME, O_RDONLY, 0);
  read(from_client, buffer, sizeof(buffer));
  printf("[server] handshake: received [%s]\n", buffer);

  remove(SERVERNAME);
  printf("[server] handshake: removed wkp\n");

  //connect to client, send message
  *to_client = open(buffer, O_WRONLY, 0);
  write(*to_client, buffer, sizeof(buffer));

  //read for client
  read(from_client, buffer, sizeof(buffer));
  printf("[server] handshake received: %s\n", buffer);

  return from_client;
}


/*=========================
  client_handshake
  args: int * to_server
  Performs the client side pipe 3 way handshake.
  Sets *to_server to the file descriptor for the upstream pipe.
  returns the file descriptor for the downstream pipe.
  =========================*/
int client_handshake(int *to_server) {
  printf("start\n");

  int from_server;
  char buffer[HANDSHAKE_BUFFER_SIZE];

  //send pp name to server
  printf("[client] handshake: connecting to wkp\n");
  *to_server = open(SERVERNAME, O_WRONLY, 0);
  if ( *to_server == -1 ){
    printf("Server Pipe doesn't exist\n");
    exit(1);
  }
  //make private pipe
  printf("[client] handshake: making private pipe\n");
  sprintf(buffer, "%d", getpid() );
  mkfifo(buffer, 0600);

  printf("[client] handshake: writing to WKP\n");
  write(*to_server, buffer, sizeof(buffer));

  //open and wait for connection
  from_server = open(buffer, O_RDONLY, 0);
  read(from_server, buffer, sizeof(buffer));
  /*validate buffer code goes here */
  printf("[client] handshake: received [%s]\n", buffer);

  //remove pp
  remove(buffer);
  printf("[client] handshake: removed private pipe\n");

  //send ACK to server
  write(*to_server, ACK, sizeof(buffer));

  return from_server;
}
