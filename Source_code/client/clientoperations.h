/*
Vipul Garg (201001049) and Vidhan Agarwal (201001034)

Quiz client process header file 
*/

#include <stdio.h>						//Libraries
#include <stdlib.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>

#define SIZE sizeof(struct sockaddr_in)				//Constants
#define MSGSZ 100

typedef struct msgbuf						//Global Variables
{
  long mtype;
  char mtext[100];
} message_buf;
int msgid;
int n = 0;
key_t key;

void info (int);						//Functions
void msg (int);
void messg (int, int);
void add (int);
void read (int, int);
void printque(int);
void topscore(int);


/*-------------------------Function to start the Quiz---------------------------*/

void start(int sockfd, int uid){
	char answers[6];
	char score[2];
	char c[3];
	int i;

	sprintf(c,"s%d",uid);
	c[2] = '\0';
	send(sockfd,c,sizeof(c),0);

	printf("\nThe quiz has five multiple choice questions with 4 options in each, All questions are compulsory");

	for(i=0;i<5;i++){
		printque(i);
		while(answers[i]!='a'&&answers[i]!='b'&&answers[i]!='c'&&answers[i]!='d'){
		scanf("%c", &answers[i]);
	}

	}
	answers[5] = '\0';
	
	if(send(sockfd,answers,strlen(answers),0)<0)
		printf("error in sending results to server");
	else{
		if (recv (sockfd, score, strlen(score), 0)<0)
			printf("error in recieving the scores from the server");

		else{		
			printf("\n\n\t\tThe quiz is completed\n\t\tThe results are:%s\n", score);
		}
	}

}

/*-------------------------Function to print the questions of Quiz---------------------------*/

void printque(int num){
	switch(num){
	case 0:
		printf("\n\nQue.1\n\tThe day before the day before yesterday is three days after Saturday, What day is today?\n");
		printf("\ta) Mon\tb) Tues\n\tc) Wed\td) Fri\n\tAnswer: ");		//Answer: d) Fri
	break;

	case 1:
		printf("\n\nQue.2\n\tWhich should be the next number?\n\t1,3,6,10,15\n");
		printf("\ta) 8\tb) 11\n\tc) 24\td) 21\n\tAnswer: ");			//Answer: d) 21
	break;

	case 2:
		printf("\n\nQue.3\n\tLibrary is to Book as Book is to\n");
		printf("\ta) Page\tb) Copy\n\tc) Binding\td) Cover\n\tAnswer: ");	//Answer: a) page
	break;

	case 3:
		printf("\n\nQue.4\n\tIf 10 people shake hands with each other, how many handshakes will be there in total?\n");
		printf("\ta) 100\tb) 20\n\tc) 45\td) 50\n\tAnswer: ");			//Answer: c) 45
	break;

	case 4:
		printf("\n\nQue.5\n\tRearange the following letters to make a word and choose the category in which it fits\n\tRAPETEKA\n"); 
		printf("\ta) City\tb) Fruit\n\tc) Bird\td) Vegetable\n\tAnswer: ");	//Answer: PARAKEET c)bird
	break;



	}
}

/*-------------------------Function to get the score of a user---------------------------*/

void
info (int sockfd)
{
  int key;
  char a = 'i';
  char c[3], rc[30] = "\0", b[60];
  printf ("\n\nEnter the ID :");
  scanf ("%d", &key);

  sprintf (c, "%c%d", a,key);
  send (sockfd, c, 3, 0);


  if (recv (sockfd, rc, sizeof (rc), 0) > 0)
    {
      char *p;
      p = strtok (rc, ",");
      printf ("Id\tScore\n");
      while (p != NULL)
	{
	  printf ("%s\t", p);
	  p = strtok (NULL, ",");
	}
      printf ("\n");
    }
  else
    {
      printf ("socket closed.\n");
      exit (1);
    }
}

/*-------------------------Function to send a message to other user---------------------------*/

void
messg (int sockfd, int uid)
{
  char a[] = "w", c[3], x[3];
  char status[50];
  int xid;
  printf ("Enter the id of your friend : ");
  scanf ("%d", &xid);
  sprintf (x, "%d", xid);
  sprintf (c, "%d", uid);
  strcat (a, c);
  strcat (a, x);
  strcat (a, "\0");
  send (sockfd, a, 10, 0);
  char dump[2];
  gets (dump);
  printf ("Enter what's on ur mind : \n");
  gets (status);
  send (sockfd, status, sizeof(status), 0);
}

/*-------------------------Function to read the messages recieved---------------------------*/

void
read (int sockfd, int uid)
{
  int size;
  message_buf rbuf;
  size = sizeof (rbuf);
  key = uid;
  if ((msgrcv (msgid, &rbuf, size, 1, 0)) < 0)
    {
      perror ("msgrcv");
    }
  else
      printf ("%s\n", rbuf.mtext);
}

/*-------------------------Function to add a new user to the Quiz portal ---------------------------*/

void
add (int sockfd)
{
  char a = 'a';
  send (sockfd, &a, 10, 0);
  char c[3], sage[3], name[10], city[10], college[10], tosend[50];
  int age;
  printf ("\n\nEnter your name : ");
  scanf ("%s", name);
  printf ("\nEnter the age : ");
  scanf ("%d", &age);
  printf ("\nEnter the college : ");
  scanf ("%s", college);
  printf ("\nEnter the city :");
  scanf ("%s", city);
  strcat (tosend, name);
  strcat (tosend, "\t");
  sprintf (sage, "%d", age);
  strcat (tosend, sage);
  strcat (tosend, "\t");
  strcat (tosend, college);
  strcat (tosend, "\t");
  strcat (tosend, city);
  strcat (tosend, "\n");
  printf ("%s", tosend);

printf("\n\nName of the user: %s\n\n",name);

  if(send (sockfd, tosend, sizeof (tosend), 0)<0)
	printf("error in sending to server");
  if(send (sockfd, name, sizeof (name), 0)<0)
	printf("error in sending to server");
}

/*-------------------------Function to find the top score---------------------------*/

void topscore(int sockfd){
	char data[2];
	data[0] = 't';
	data[1] = '\0';
	char top[3];
	send (sockfd, data, 2, 0);
	recv(sockfd, top, 3, 0);

	printf("\n\nTop Scorer's ID: %c\tTop Score: %c\n\n", top[0], top[1]);

	
	
}

/*-------------------END--------------------*/
