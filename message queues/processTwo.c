#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <string.h>
#include<unistd.h>
#include<signal.h>
#define MSGSZ     128




/*
* Declare the message structure.
*/

typedef struct msgbuf {
	long    mtype;
	char    mtext[MSGSZ];
} message_buf;

void handleSignals(int signum);
int main(void)
{
        
	int msqid;
	key_t key;
	message_buf  rbuf,sbuf;
	size_t buf_length;
	/*
	* Get the message queue id for the
	* "name" 1234, which was created by
	* the server.
	*/
	key = 1234;

	


	while(1){
		if ((msqid = msgget(key, 0666)) < 0) {
		perror("msgget");
		exit(1);
		}
		void (*sigHandlerInterrupt)(int);
	   	void (*sigHandlerQuit)(int);
	   	void (*sigHandlerReturn)(int);
	   	sigHandlerInterrupt = sigHandlerQuit = handleSignals;
	   	sigHandlerReturn = signal(SIGINT, sigHandlerInterrupt);
	   	if (sigHandlerReturn == SIG_ERR) {
	      		perror("signal error: ");
	      		return 1;
	   	}
	   	sigHandlerReturn = signal(SIGQUIT, sigHandlerQuit);
	   
	   	if (sigHandlerReturn == SIG_ERR) {
	      		perror("signal error: ");
	      		return 1;
	   	}


		if (msgrcv(msqid, &rbuf, MSGSZ, 1, 0) < 0) {
			perror("msgrcv");
			//msgctl(msqid, IPC_RMID, NULL);
			exit(1);
		}else
			printf("\nProcess 1:%s\n", rbuf.mtext);	
	}
	
	exit(0);
}
void handleSignals(int signum) {
   int msqid;
   message_buf sbuf;
   size_t buf_length;
   sbuf.mtype = 2;
   key_t key;
   key = 1234;
   if ((msqid = msgget(key, 0666 )) < 0) {
		perror("msgget");
		exit(1);
   }
   switch(signum) {
      case SIGINT:
      strcpy(sbuf.mtext,"Received signal CTRL+C from process 2");
      buf_length = strlen(sbuf.mtext) + 1 ;
      if (msgsnd(msqid, &sbuf, buf_length, IPC_NOWAIT) < 0) {
      		printf ("%d, %d, %s, %d\n", msqid, sbuf.mtype, sbuf.mtext, buf_length);
		perror("msgsnd");
		exit(1);
      }else 
		printf("\nProcess 2: CTRL+C \n");
      signal(SIGINT, SIG_DFL);
      
      break;
      case SIGQUIT:
      
      strcpy(sbuf.mtext,"Received signal CTRL+\\ from process 2");
      buf_length = strlen(sbuf.mtext) + 1 ;
      if (msgsnd(msqid, &sbuf, buf_length, IPC_NOWAIT) < 0) {
      		printf ("%d, %d, %s, %d\n", msqid, sbuf.mtype, sbuf.mtext, buf_length);
		perror("msgsnd");
		exit(1);
      }else
		printf("\nProcess 2: CTRL+\\ \n");
      break;
      default:
      printf("\nReceived signal number %d\n", signum);
      break;
   }
   return;
}
