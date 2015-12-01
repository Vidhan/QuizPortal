/*
Vipul Garg (201001049) and Vidhan Agarwal (201001034)

Quiz server process header file  
*/


#include <stdio.h>					//Libraries
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <netinet/in.h>
#include <signal.h>
#include <pthread.h>
#include <err.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <time.h>

#define SIZE sizeof(struct sockaddr_in)			//Constants
#define MSGSZ 100

int newsockfd;						//Variables
FILE *fp;
static int id = 1;
int uid, upwd;
typedef struct msgbuf
{
  long mtype;
  char mtext[MSGSZ];
} message_buf;
int msgid;
int msgflg = IPC_CREAT | 0666;
key_t key;
size_t buf_length;

void messg (char, char);				//Functions
void add ();
void info (char);
void topscore();
void catcher (int);
void results(char);
void createlog(int, int);
void updatescore(int, int);



void
catcher (int sig)
{
  close (newsockfd);
  exit (0);
}

/*-------------------Function to get the score of a user from info.txt--------------------*/

void
info (char c)
{
  FILE *f1;
  int id, age, key;
  char tosend[30];
  key = (int) c;
  key = key - 48;
  char name[20];
  int score = 0;

  f1 = fopen ("score.txt", "r");
  fseek (f1, 0L, 0);
  while (feof (f1) == NULL)
    {
      fscanf (f1, "%d\t%d\n", &id, &score);
      if (id == key)
	{
	  sprintf (tosend, "%d,%d", id, score);	//To use delimiter to split to client
	  send (newsockfd, tosend, sizeof (tosend), 0);
	  break;
	}
    }
  fclose (f1);
}

/*-------------------Function to send message to another client--------------------*/

void
messg (char id, char xid)					
{
  message_buf sbuf;
  char ids[3];
  id = (int) id;
  id = id - 48;
  xid = (int) xid;
  xid = xid - 48;
  sprintf (ids, "%d", id);
  char status[10] = "\0", rc[100];
  recv (newsockfd, rc, sizeof (rc), 0);
  strcat (status, ids);
  strcat (status, " : ");
  strcat (status, rc);
  char pids[6];
  int i;
  key = xid;
  if ((msgid = msgget (key, msgflg)) < 0)
    {
      perror ("msgget");
    }
  sbuf.mtype = 1;
  strcpy (sbuf.mtext, status);
  buf_length = strlen (sbuf.mtext) + 1;
  if (msgsnd (msgid, &sbuf, buf_length, 0) < 0)
    {
      perror ("msgsnd");
    }
}

/*-------------------Function to add a new user and its info to user.txt and info.txt resp.-------------------*/

void
add ()
{
  int nl = 1;
  int score = 0;		
  FILE *f1;
  f1 = fopen ("info.txt", "r");
  fseek (f1, 0l, 0);
  char rc[30];
  FILE *f0;
  recv (newsockfd, rc, sizeof (rc), 0);
//-----------------------semaphore to ensure no two same ids--------------------------
  sem_t sem1;
  sem_init (&sem1, 0, 1);
  sem_wait (&sem1);
//----------------------------counting current id--------------------------------------
  for (;;)
    {
      if (getc (f1) == EOF)
	{
	  break;
	}
      else
	{
	  if (getc (f1) == '\n')
	    nl++;
	}
    }
  fclose (f1);
  id = nl;
//--------------------------adding to file------------------------------------
  f0 = fopen ("info.txt", "a");
  fprintf (f0, "%d\t%s\n", id, rc);
  fclose (f0);

  sem_post (&sem1);
  sem_destroy (&sem1);
  char name[10];
  recv (newsockfd, name, sizeof (name), 0);

  FILE *f2;
  f2 = fopen ("score.txt", "a");
  fprintf (f2, "%d\t%s\t%d\n", id, name, score);
  fclose (f2);
  


}

/*-------------------Function to get the results of the quiz--------------------*/

void results(char uid){
	char results[6];
	char answer[] = "ddacc";
	int k;
	int value = 0;
	char score[2];

	if (recv (newsockfd,results , 5, 0)){
		
		for(k=0;k<5;k++){
			if(results[k]==answer[k])
				value++;
		}

		sprintf(score, "%d", value);
		score[1] = '\0';
		send(newsockfd,score , sizeof(score), 0);
		updatescore(value, atoi(&uid));
	}

}

/*-------------------Function to create log file of all the major events--------------------*/

void createlog(int choice, int id){
	FILE* flog;
	char logdata[50];

	flog = fopen ("log.txt", "a");
	switch(choice){
	case 0:
	sprintf(logdata, "User with id %d logged in",id );
	break;
	case 1:
	sprintf(logdata, "User with id %d took the quiz",id );
	break;
	case 2:
	sprintf(logdata, "User with id %d logged out",id );
	break;
	case 3:
	sprintf(logdata, "New user with id %d registered",id );
	break;
	}

	time_t rawtime;
	struct tm * timeinfo;

	time ( &rawtime );
	timeinfo = localtime ( &rawtime );
	fprintf (flog, "%s\t%s\n\n",asctime (timeinfo), logdata);
	fclose (flog);
}

/*-------------------Function to update a user's score after he/she has taken the quiz--------------------*/

void updatescore(int score, int id){
int xid;
  int xscore;
  FILE *f1, *f2;

//---------------------------------copying to temp file-------------------------------- 
  f1 = fopen ("score.txt", "r");
  f2 = fopen ("temp.txt", "w");
  fseek (f1, 0L, 0);
  while (!feof (f1))
    {
      fscanf (f1, "%d\t%d\n", &xid, &xscore);
      fprintf (f2, "%d\t%d\n", xid, xscore);
    }
  fclose (f1);
  fclose (f2);

//------------------------------------updating to main file---------------------------------
  f1 = fopen ("temp.txt", "r");
  f2 = fopen ("score.txt", "w");
  fseek (f1, 0L, 0);
  while (feof (f1) == NULL)
    {
      fscanf (f1, "%d\t%d\n", &xid, &xscore);
      if (xid == id)
	{
	  fprintf (f2, "%d\t%d\n", id, score);
	}
      else
	{
	  fprintf (f2, "%d\t%d\n", xid, xscore);
	}
    }
  fclose (f1);
  fclose (f2);

}

/*-------------------Function to find the topscorer and his/her score--------------------*/

void topscore(){
  int score, id;
  FILE *f1;
  int top = 0;
  int topid = 0;
  char data[3];

  f1 = fopen ("score.txt", "r");
  fseek (f1, 0L, 0);
  while (!feof (f1))
    {
	fscanf (f1, "%d\t%d\n", &id, &score);
	if(score>top){
		top = score;
		topid = id;
	}
    }
  fclose (f1);
  sprintf(data,"%d%d",id,score);
  send(newsockfd, data,3,0);

}


/*-------------------END--------------------*/
