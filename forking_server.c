#include "pipe_networking.h"
#include <signal.h>

void process(char *s);
void subserver(int from_client);

static void sighandler(int signo) {
  if (signo == SIGINT) {
    printf("\nI take pipe away from you now\n");
    remove(SERVERNAME);
    exit(0);
  }
}

int main() {

  signal( SIGINT, sighandler);

  int from_client;
  int serverPID = getpid();
  while (1){
    from_client = server_setup();

    if (serverPID != getpid()){ //child
      subserver(from_client);
    }
  }
}

void subserver(int from_client) {
  int to_client = server_connect(from_client);
  
  char buf[BUFFER_SIZE];
  while( read(from_client, buf, sizeof(buf))){
    printf("[subserver] received: %s\n", buf);
    process(buf);
    printf("[subserver] sending: %s\n", buf);
    write(to_client, buf, sizeof(buf));
  }
  exit(0);
}

void process(char * s) {
  while(*s){
    if((*s >= 'a' && *s <= 'm') || (*s >= 'A' && *s <= 'M')){
      *s += 13;
    } 
    else if((*s >= 'n' && *s <= 'z')||(*s >= 'N' && *s <= 'Z')) {
      *s -= 13;
    }
    s ++;
  }
}
