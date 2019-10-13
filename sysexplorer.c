// ΓΛΗΓΟΡΗΣ ΓΙΩΡΓΟΣ ΑΜ 2008030100

#include <stdio.h>
#include "sysexplorer.h"
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/fcntl.h>

#define Port 11880

void* PageCreator(void*  );
char *path_cat (const char *str1, char *str2);
const char* getpathname(char* );
void *get_in_addr(struct sockaddr *sa);
const char* getpat(char* );
void findFunc(char* ,char*,int*);

void create_server_socket(int PortNum)
	{
	int socketfd=socket (AF_INET, SOCK_STREAM, 0);

	int sopt=1;

	struct sockaddr_in serv_addr;

	setsockopt ( socketfd , SOL_SOCKET, SO_REUSEADDR, &sopt, sizeof(sopt));


	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(Port);
	serv_addr.sin_addr.s_addr = INADDR_ANY;


	if (bind(socketfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr))==-1)
	{
		printf("Error in Binding");
	}


	if (listen(socketfd,5)==-1)
	{
		printf("Error in listening");
	}


	while (1)
	{
		socklen_t sin_size = sizeof serv_addr;


		int new_fd = accept(socketfd, (struct sockaddr *)&serv_addr, &sin_size);
		if (new_fd == -1)
		{
			printf("Error in accept");
		}



		char s[INET6_ADDRSTRLEN];
		inet_ntop(serv_addr.sin_family,get_in_addr((struct sockaddr *)&serv_addr),s, sizeof s);
		printf("server: got connection from %s\n", s);



		pthread_t t_id=0;
		pthread_create(&t_id,0,&PageCreator,(void*)new_fd);

		pthread_detach(t_id);


	}
	}

void* PageCreator(void* fd )
		{

			int *new_fd=(int *)fd;

			ssize_t r_c;
			char buf[1500];
			r_c = read((int)new_fd, buf, 1500);
            puts(buf);


            char* path=malloc(200);
            sprintf(path,"%s",getpathname(buf));
                          printf("pathname:");
                          puts(path);



              if (buf[0]=='P')
              {

                findFunc(buf,path,new_fd);


                }


			struct dirent *dp;
			char *dir_path=malloc(200);
            dir_path=path;
			DIR *dir;


			char response[60000];


			if ((dir=opendir(dir_path))!=NULL)
			{


			char html1[300];
			strcat(html1,"HTTP/1.1 200 OK \n\n <html> <head> <title>");
			strcat(html1,path);
			strcat(html1,"</title> <style> h1.dirlist {background-color: pink; } </style> </head>  <body> <h1 class=""dirlist"">Directory listing:");
			strcat(html1,path);
			strcat(html1,"</h1> <form method='POST'> Find pattern <input type='text' name='pattern' value="" > <input type='submit' value='Find'/> <input type='reset' value='Clear''/> </form> <table><thead> <tr> <th> Name </th> <th> Type </th> <th> Size </th> </tr></thead>");
			sprintf(response,"%s",html1);


			while ((dp=readdir(dir)) != NULL)
			{

				char* tmp=malloc(2000);

				tmp = path_cat(dir_path, dp->d_name);

				struct stat fs;

				int filesize=0;
				int filetype=0;

				if( lstat( tmp, &fs ) != -1 )
				 {
					filesize =  fs.st_size;
				 	filetype= fs.st_mode;
				 }

				char startHTML[]="<tbody><tr><td><a href=";
				strcat(response,startHTML);

				strcat(response,tmp);

				char midlleHTML[]="/>.</a></td><td></td><tr><td>";
				strcat(response,midlleHTML);
				strcat(response,tmp);

				char end1HTML[]="  <td>";
				strcat(response,end1HTML);


				if (filetype & 0100000)
				{
				char type[]="Regular file";
				strcat(response,type);
				}
				else if (filetype & 0040000)
				{
				char type2[]="directory ";
				strcat(response,type2);
				}
				else if (filetype & 0020000)
				{
				char type3[]="Character device ";
				strcat(response,type3);
				}
				else
				{
				char type4[]="Unknown";
				strcat(response,type4);
				}

				char end2HTML[]=  "</td><td> ";
				strcat(response,end2HTML);

				char *fsize=malloc(100);
				sprintf(fsize,"%d",filesize);
				strcat(response,fsize);

				char end3HTML[]=" </td></tr>";
				strcat(response,end3HTML);

				free(tmp);
				tmp=NULL;
			}

			char end4HTML[]="</tbody></table><body></html>";
			strcat(response,end4HTML);





			send((int)new_fd, response, strlen(response), 0);


			close((int)new_fd);
			return 0;

			}


			if (errno==2)
			{
				printf("Error occured. Errno value is : %d\n",errno);

				char html[]="HTTP/1.1 404 Not Found \n\n <HTML> <HEAD><TITLE> Error 404 </TITLE></HEAD> <body> </br>Error 404 File Not Found </body> </HTML>";
				sprintf(response,"%s",html);
				//puts(response);

				send((int)new_fd, response, strlen(response), 0);

				close((int)new_fd);
				return 0;
			      }

				printf("Error occured. Errno value is : %d\n",errno);

				char html[]="HTTP/1.1 500 Internal Server Error \n\n <HTML> <HEAD><TITLE> Internal Server Error </TITLE></HEAD> <body> </br>Error 500 Internal Server Error </body> </HTML>";
				sprintf(response,"%s",html);


				send((int)new_fd, response, strlen(response), 0);

				close((int)new_fd);
				return 0;

		}


	int main()
	{
		create_server_socket(Port);

		return 0;
	}


	char *path_cat (const char *str1, char *str2) {

		size_t str1_len = strlen(str1);
		size_t str2_len = strlen(str2);
		char *result;

		result = malloc((str1_len+str2_len+1)*sizeof *result);
		strcpy (result,str1);
		int i,j;
		for(i=str1_len, j=0; ((i<(str1_len+str2_len)) && (j<str2_len));i++, j++) {
			result[i]=str2[j];
		}
		result[str1_len+str2_len]='\0';
		return result;
	}


	void *get_in_addr(struct sockaddr *sa)
			{
				if (sa->sa_family == AF_INET)
				{
					return &(((struct sockaddr_in*)sa)->sin_addr);
				}

				return &(((struct sockaddr_in6*)sa)->sin6_addr);
			}


	const char* getpathname(char* buffer)
				{
					int i;
					char *pathname;
					const char * foo;
					char str2[] = " HTTP";

					int position = (strstr(buffer,str2)-buffer-1);

					pathname=malloc(strlen(buffer)*sizeof(char));
					for (i=4;i<=position;i++)
					{
						pathname[i-4]=buffer[i];
					  }

					foo=pathname;
					return foo;

				}

/*	const char* getlength(char* bf)
		{
		const char s[2] = " ";
					  char *token,*pch;

				      pch= strstr(bf,"ength= ");
					  token = strtok(pch, s);

					   if( token != NULL )
					       {
					        token = strtok(NULL, s);
					           }
		               return token;
		   }*/


	 const char* getpat(char* bfr)
		  {
			  const char s[2] = "=";
			  char *token,*pch;

		      pch= strstr(bfr,"pattern=");
			  token = strtok(pch, s);

			   if( token != NULL )
			       {
			        token = strtok(NULL, s);
			           }
               return token;
		     }


	void indFunc(char* b,char* p,int* n_fd)
	 {


		char response_find[500];

	 	                     char* pat=malloc(200*sizeof(char));
	 	            	  	 sprintf(pat,"%s",getpat(b));
	 	            	  	 FILE * f=NULL;




	 	                  int fd[2],status;


	 	                  int pid = fork();

	 	                  if (pipe(fd) < 0)
	 	                    {
	 	                	  perror("creating pipe");
	 	                	  exit(1);
	 	                        }

	 	                  if (pid==0)
	 	                  {

	 	                	  close(fd[0]);
	 	                	  dup2(fd[1],1);

	 	                	  dup2(open("/dev/null",O_WRONLY),2);
	 	                	  execl("/usr/bin/find", "find","-name",pat,NULL);
	 	                	  perror("execl");
	 	                  }
	 	                  else
	 	                  {
	 	                	  waitpid(pid, &status, WNOHANG);
	                           //free(pat);
	 	                	  close(fd[1]);
	 	                	  dup2(fd[0],0);

	 	                	f=fdopen(fd[0],"r");//dimiourgia filestream gia thn anagnwsi twn apotelesmatwn ths find
	                         close(fd[0]);





	 						char html1[]="HTTP/1.1 200 OK \n\n <html> <head> <title> Find Results is in directory";

	 						char html11[]=" </title> <style> h1.dirlist {background-color: green; } </style> </head> <body> <h1 class=""dirlist"">Find Results is in directory:";

	 						char html12[]="</h1> <table><thead> <tr> <th> Name </th> <th> Type </th> <th> Size </th> </tr></thead>";



	 						strcat(response_find,html1);
	 						strcat(response_find,p);
	 						strcat(response_find,html11);
	 						strcat(response_find,p);
	 						strcat(response_find,html12);


	 						char buffer[300];
	 					int	i=0;//metavlith pou mas voitha na doume ean exoume estw k ena apotelesma apo thn find
	 						while(fgets(buffer,sizeof(buffer),f)!=NULL)//diavasma autwn pou epistrefei h find mesw tou filestream f
	 						{
	 								i++;
	 								int len = strlen(buffer);
	 								if( buffer[len-1] == '\n' )
	 									{buffer[len-1] = 0;}

	 								struct stat fs;

	 								int filesize=0;
	 								int filetype=0;
	 								if( lstat( buffer, &fs ) != -1 )
	 								 {
	 									filesize =  fs.st_size;//Ypologismos megethous arxeiou
	 									filetype= fs.st_mode;//Ypologismos typou arxeiou
	 								 }

	 								char *type=malloc(50);
	 								if (filetype & 0100000)
	 								type="File";
	 								else if (filetype & 0040000)
	 								type="Directory ";
	 								else if (filetype & 0020000)
	 								type="Character device ";
	 								else
	 								type="Unknown";


	 								char startHTML[]="<tbody><tr>";
	 								strcat(response_find,startHTML);


	 								if (filetype & 0040000)
	 								{
	 									char linkHTML[]="<td><a href=";
	 									strcat(response_find,linkHTML);
	 									strcat(response_find,buffer);
	 									char midlle1HTML[]="/>";
	 									strcat(response_find,midlle1HTML);
	 									strcat(response_find,buffer);

	 									char midlle2HTML[]="</a></td> <td>";
	 									strcat(response_find,midlle2HTML);



	 								}
	 								else
	 								{
	 									char startHTML[]="<td>";
	 									strcat(response_find,startHTML);
	 									strcat(response_find,buffer);
	 									char nlinkHTML[]="</td> <td>";
	 									strcat(response_find,nlinkHTML);
	 								}

	 								strcat(response_find,type);

	 								char end2HTML[]=  "</td><td> ";
	 								strcat(response_find,end2HTML);

	 								if (filetype & 0100000)
	 								{
	 									char *fsize=malloc(100);
	 									sprintf(fsize,"%d",filesize);
	 									strcat(response_find,fsize);
	 								}
	 								else
	 								{
	 									strcat(response_find,"");
	 								}

	 								char end3HTML[]=" </td></tr>";
	 								strcat(response_find,end3HTML);


	 			}//while


	 			char end4HTML[]="</tbody></table><body></html>";
	 			strcat(response_find,end4HTML);

	 			if (i==0)//stin periptwsh pou h find den gyrisei tipota tote i==0
	 			{
	 			char html[]="HTTP/1.1 200 OK \n\n <html> <head> <title> Find Results is in directory ";
	 			strcat(html,p);
	 			strcat(html,"</title>  <body> <h1 class=""dirlist""> Found nothing with this pattern in directory :");
	 			strcat(html,p);
	 			strcat(html,"</body></html>");

	 			sprintf(response_find,"%s",html);

	 			send((int)n_fd, response_find, strlen(response_find), 0);
	 			close((int)n_fd);
	 			return 0;
	 			}

	 			}



	 			if (errno==0)
	 			{
	 			send((int)n_fd, response_find, strlen(response_find), 0);
	 			close((int)n_fd);
	 			return 0;
	 			}

	 			else
	 			{
	 			printf("Error occured. Errno value is : %d\n",errno);

	 			char html[]="HTTP/1.1 500 Internal Server Error \n\n <HTML> <HEAD><TITLE> Internal Server Error </TITLE></HEAD> <body> </br>Error 500 Internal Server Error </body> </HTML>";
	 			sprintf(response_find,"%s",html);
	 			send((int)n_fd, response_find, strlen(response_find), 0);
	 			close((int)n_fd);
	 			return 0;

	 			}
	     }

