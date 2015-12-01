/*
Vipul Garg (201001049) and Vidhan Agarwal (201001034)

Quiz server process  
*/

#include "serveroperations.h"					//Header file including all libraries and functions used in server process
int
main (void)
{
  int sockfd, j;
  pid_t pid;
  char spid[10];
  char c[10], a[10];
  int o;
  struct sockaddr_in server = { AF_INET, 7001, INADDR_ANY };
  static struct sigaction act;
  act.sa_handler = catcher;
  sigfillset (&(act.sa_mask));
  sigaction (SIGPIPE, &act, NULL);

  fp = fopen ("clients.txt", "w");				//clients.txt contains pid of all users
  fclose (fp);

  /* set up the transport end point */
  if ((sockfd = socket (AF_INET, SOCK_STREAM, 0)) == -1)
    {
      perror ("socket call failed");
      exit (1);
    }
  /* bind an address to the end point */
  if (bind (sockfd, (struct sockaddr *) &server, SIZE) == -1)
    {
      perror ("bind call failed");
      exit (1);
    }
  /* start listening for incoming connections */
  if (listen (sockfd, 5) == -1)
    {
      perror ("listen call failed");
      exit (1);
    }

  printf("\n\n\t\tSERVER RUNNING...READY TO ACCEPT CLIENT REQUESTS...\n\n");

  for (;;)
    {
      /* accept a connection */
      if ((newsockfd = accept (sockfd, NULL, NULL)) == -1)
	{
	  perror ("accept call failed");
	  continue;
	}
      /* spawn a child to deal with the connection */
      if (fork () == 0)
	{
	  fp = fopen ("clients.txt", "a");
	  pid = getpid ();
	  sprintf (spid, "%d", pid);
	  fprintf (fp, "%s\n", spid);
	  fscanf (fp, "%s", spid);
	  fclose (fp);
	a:
	  if (recv (newsockfd, c, 6, 0) > 0)
	    {
		int goid, gflag = 0;
	      o = c[4];
	      if (o == 'n')
		{
		  int nl = 1;
		  FILE *f1;
		  f1 = fopen ("info.txt", "r");			//info.txt contains all the information about users
		  fseek (f1, 0L, 0);


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
		  uid = nl;
		  int id[2];
		  id[0] = uid;
		  id[1] = "\0";
		  char aid[2];
		  strncpy (a, c, 3);				//a = upwd;
		  a[3] = NULL;
		  upwd = atoi (a);
		  FILE *f0 = fopen ("users.txt", "a");		//user.txt contains the user ids and passwords of all the users
		  fprintf (f0, "%d\t%d\n", uid, upwd);		//writing data of the newly registered user
		  fclose (f0);
		  sprintf(aid,"%d%d",id[0],id[1]);
		  send(newsockfd, aid, sizeof(aid), 0);

		  createlog(3,uid);				//Generating log entry for new user
		}
	      else
								//Checking the correctness of password input by user
		{
		  int oid, opwd, flag = 0;
		  char r;
		  uid = (int) c[3] - 48;
		  strncpy (a, c, 3);
		  a[3] = NULL;
		  upwd = atoi (a);
		  FILE *f0 = fopen ("users.txt", "r");
		  while (feof (f0) == 0)
		    {
		      fscanf (f0, "%d\t%d\n", &oid, &opwd);
		      if (oid == uid && opwd == upwd)
			{
			  flag = 1;
			  gflag = flag;
			  goid = oid;
			  r = 'c';
			  send (newsockfd, &r, 1, 0);
			  break;
			  
			}
		    }

		  if (flag == 0)
		    {
		      r = 'i';
		      if (upwd != 0)
			{
			  send (newsockfd, &r, 1, 0);
			}
		      goto a;
		    }
		  fclose (f0);
		}

	  if(gflag == 1)		
		  createlog(0,goid);			//Genrating log entry for user login

	    }

		
	  while (recv (newsockfd, c, 10, 0) > 0)
	    {
	      strncpy (a, c, 1);
	      a[1] = NULL;
	      switch (a[0])
		{
		case 's':
		  createlog(1,atoi(&c[1]));		//Generate log entry for user starting the quiz 
		  results(c[1]);			/*To compute the results.... .... ....*/
		  break;
		case 'a':				/*To add a user.................... */
		  add ();
		  break;
		case 'i':				/*To view friend's score .... .... .... */
		  info (c[1]);
		  break;
		case 't':				/*To view top score .... .... .... */
		  topscore();
		  break;
		case 'w':				/*To write a message to another user....... */
		  messg (c[1], c[2]);
		  break;
		case 'x':
		  createlog(2,atoi(&c[1]));		//Generating log entry for user logging out
		  exit (1);				/*To exit .... .... ..... .... ..... */
		  break;
		default:
		  break;
		}
	    }
  /* when client is no longer sending information the socket can be closed and the child process terminated */
	  close (newsockfd);
	  exit (0);
	}
      /* parent doesn't need the newsockfd */
      close (newsockfd);
    }
}

/*-------------------END--------------------*/
