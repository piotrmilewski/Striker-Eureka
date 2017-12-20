#include "pipe_networking.h"
#include <signal.h>

void process(char *s);
void subserver(int from_client);

static void sighandler(int signo) {
  if (signo == SIGINT) {
    remove("luigi");
    exit(0);
  }
}

int main() {
  int to_client;
  int from_client;
  while (1){
    from_client = server_setup(&to_client);

    int pid = fork();
    if (pid){ //child
      printf("i happened\n");
      subserver(from_client);
      exit(0);
    }
  }
  return 0;
}

void subserver(int from_client) {
  int to_client = server_connect(from_client);
  
  char buf[BUFFER_SIZE];
  while( read(from_client, buf, sizeof(buf))){
    process(buf);
    //printf("%s",buf);
    write(to_client, buf, BUFFER_SIZE);
  }
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
