/*
Vipul Garg (201001049) and Vidhan Agarwal (201001034)

Quiz client process 
*/

#include "clientoperations.h"					//Header file including all libraries and functions used in client process

int
main (void)
{
  int sockfd;
  char c, rc, exitdata[3];;
  struct sockaddr_in server = { AF_INET, 7001 };

  /* convert and store the server's IP address */
  server.sin_addr.s_addr = htonl (INADDR_ANY);

  /* set up the transport end point */
  if ((sockfd = socket (AF_INET, SOCK_STREAM, 0)) == -1)
    {
      perror ("socket call failed");
      exit (1);
    }

  /* connect the socket to the server's address */
  if (connect (sockfd, (struct sockaddr *) &server, SIZE) == -1)
    {
      perror ("connect call failed");
      exit (1);
    }
  /* send and receive information with the server */

/*----------------------------------check the password--------------------------------*/
  char p, r, o[3];
  char a[10] = "\0", b[10];
  int uid[2], upwd[2];
  int utflag = 0, addflag = 0, matchflag = 0;

  while (1)
    { system("clear");
      printf ("Press 'e' if you are existing user.......\n");	//Asking User if he/she is already signed up to the portal
      printf ("Press 'n' if you are a new user..........\n");
      scanf ("%c", &p);
      switch (p)
	{
	a:;
	case 'e':
	  o[0] = 'e';
	  o[1] = '\0';
	  uid[2];
	  upwd[4];
	  printf ("\nEnter your id : ");
	  scanf ("%d", uid);
	  printf ("\nEnter your password : ");
	  scanf ("%d", upwd);
	  sprintf (a, "%d", uid[0]);
	  strcat (a, o);
	  sprintf (b, "%d", upwd[0]);
	  strcat (b, a);
	  send (sockfd, b, 8, 0);				//sending the id and password of user to server for checking
	  if (recv (sockfd, &r, 1, 0))
	    {
	      if (r == 'c')
		{
 		 printf("password matched\n");
       		 printf ("\n\n\n\n\t\t\t\tWelcome To The Quiz Portal\n\n\n\n\n");
		  goto b;
		}
	      else if (r == 'i')
		{
		  printf ("Incorrect password.....:(\n");
		  printf ("Please try again...\n");
		  r = '\0';
		  goto a;
		}
	    }
	  break;
	  case 'n':
		  o[0] = 'n';
		  o[1] = '\0';
		  printf ("Enter the three digit password : ");
		  scanf ("%d", upwd);
		  uid[0] = 0;
		  sprintf (a, "%d", uid[0]);
		  strcat (a, o);
		  sprintf (b, "%d", upwd[0]);
		  strcat (b, a);
		  if(send (sockfd, b, 6, 0) < 0)		//sending password of the new user to server to create a new user id
			printf("Error in sending to server");
		  else{
		  	goto c;
			printf("sending succesfull");
		  }
	  break;

	case '\n':
	  	  c = '\0';
	break;

	default:
	break;
	}

    }

b:
  key = uid[0];
  if ((msgid = msgget (key, IPC_CREAT | 0666)) < 0)
    {
      perror ("msgget");
    }

/*---------------------------------------Operations-------------------------------------*/
  utflag = 1;
  while (1)
    {
      if (c != 'q')
	{
	  printf ("\n\n\nPress s to start the Quiz\n");
	  printf ("Press i to view friend's score................\n");
	  printf ("Press t to view the topscore................\n");
	  printf ("Press m to send message to a friend.........................\n");
	  printf ("Press r to read message.........................\n");
	  printf ("Press x to exit...................................\n");
	  printf ("Enter your choice : ");
	}
      scanf ("%c", &c);

      switch (c)
	{

	case 's':
	  start(sockfd,uid[0]);		/*To start the quiz .... .... ... ....*/
	  break;
	case 'i':			/*To view  scores .... .... ....*/
	  info (sockfd);
	  break;
	case 't':			/*To view  topscore .... .... ....*/
	  topscore(sockfd);
	  break;
	case 'r':			/*To view recent message..............*/
	  read (sockfd, uid[0]);
	  break;
	case 'm':			/*To send message.....................*/
	  messg (sockfd, uid[0]);
	  break;
	case 'x':
	  
	  sprintf(exitdata,"x%d",uid[0]);
	  send (sockfd, exitdata, 3, 0);
	  exit (1);			/*To exit .... .... ..... .... .....  */
	case '\n':
	  c = 'q';
	  break;
	default:
	  break;
	}
    }
c:
  if (utflag == 0)
    {
      char o;
      while (1)
	{
	  if (o != 'q')
	    {
	      printf ("Press 'a' to add your information...........\n   ");
	    }
	  scanf ("%c", &o);
	  switch (o)
	    {
	    case 'a':
	      {
		char assgndid[2];
		add (sockfd);
		recv (sockfd, assgndid, 1, 0);
		printf
		  ("\n\n\n\t\tCongratulations you have been successfully added to the Quiz Portal\n\t\t\t\t\tenjoy\t:)");
		printf
		  ("\n\n\t\tPlease log in back with your userid and password to continue :)\n\n");
		printf("\n\n\t\tYour user id is: %s    Note it down for future login\n",assgndid);
		addflag = 1;
		exit (1);
		break;
	      }

	    case '\n':
	    	o = 'q';
	    break;
	    default:
		printf("Error in creating user");
	    break;
	    }
	  if (addflag == 1)
	    break;
	}
      goto b;
    }
}

/*-------------------END--------------------*/
