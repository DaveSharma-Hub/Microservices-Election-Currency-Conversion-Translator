/*
Author: Dave Sharma 
UCID: 30093981
Filename: a2.cpp
Date: October 18, 2021
Class: CPSC 441 Assignment 2
*/
/*
References:
http://pages.cpsc.ucalgary.ca/~carey/CPSC441/examples/testserver.c // parts of Dr. Carey's server code used to create main server-side of Proxy
https://d2l.ucalgary.ca/d2l/le/content/401094/viewContent/4896480/View // TA's notes presented on HTTP used in server and client side of Proxy
https://www.geeksforgeeks.org/socket-programming-cc/ //Used in secondary server to telnet to Proxy
https://www.binarytides.com/socket-programming-c-linux-tutorial/ //Used in creating client-side of Proxy
https://sites.google.com/view/sinacpsc441/content //Sina Keshvadi's notes used to understand the required Proxy contents
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <signal.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <chrono>
#include <sys/time.h>


/* Global manifest constants */
#define MAX_MESSAGE_LENGTH 10000
#define SERVERPORTNUM 44101

/* Optional verbose debugging output */
#define DEBUG 1

/* Global variable */
int childsockfd;

/* This is a signal handler to do graceful exit if needed */
void catcher( int sig )
  {
    close(childsockfd);
    exit(0);
  }

/* Main program for server */
int main()
  {
	  //int SERVERPORTNUM;
    struct sockaddr_in server, client;
    static struct sigaction act;
    char messagein[MAX_MESSAGE_LENGTH];
    char messageout[MAX_MESSAGE_LENGTH];
    int parentsockfd, c, pid, conns;
	

	int translatorsockfd;
	struct sockaddr_in translatorservaddr;
		// int transsockcheck;
		// struct sockaddr_in transservaddrcheck;

	int currencysockfd;
	struct sockaddr_in currencyservaddr;
			// int currencysockcheck;
			// struct sockaddr_in currencyservaddrcheck;

	int votersockfd;
	struct sockaddr_in voterservaddr;
			// int votersockfdcheck;
			// struct sockaddr_in voterservaddrcheck;
    /* Set up a signal handler to catch any unusual termination conditions. */
    act.sa_handler = catcher;
    sigfillset(&(act.sa_mask));
    sigaction(SIGPIPE, &act, NULL);

    /* Initialize server sockaddr structure */
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(SERVERPORTNUM);
    server.sin_addr.s_addr = htonl(INADDR_ANY);

    /* set up the transport-level end point to use TCP */
    if( (parentsockfd = socket(PF_INET, SOCK_STREAM, 0)) == -1 )
      {
	fprintf(stderr, "testserver: socket() call failed!\n");
	exit(1);
      }

    /* bind a specific address and port to the end point */
    if( bind(parentsockfd, (struct sockaddr *)&server, sizeof(struct sockaddr_in) ) == -1 )
      {
	fprintf(stderr, "testserver: bind() call failed!\n");
	exit(1);
      }

    /* start listening for incoming connections from clients */
    if( listen(parentsockfd, 5) == -1 )
      {
	fprintf(stderr, "testserver: listen() call failed!\n");
	exit(1);
      }

    /* initialize message strings just to be safe (null-terminated) */
    bzero(messagein, MAX_MESSAGE_LENGTH);
    bzero(messageout, MAX_MESSAGE_LENGTH);

    fprintf(stderr, "Welcome! I am the CPSC 441 test server for L02!!\n");
    fprintf(stderr, "Server listening on TCP port %d...\n\n", SERVERPORTNUM);

    conns = 0;
    c = sizeof(struct sockaddr_in);
    
		int checkVoting=0;
		int showAll=0;
		bool bonus=true;
		int bonus2=0;


		struct timeval tv;
			tv.tv_sec = 3;
			tv.tv_usec=0;
    /* Main loop: server loops forever listening for requests */
	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
      //using std::chrono::system_clock;

    for( ; ; )
      {
	/* accept a connection */
	if( (childsockfd = accept(parentsockfd, (struct sockaddr *)&client, (socklen_t *) &c)) == -1 )
	  {
	    fprintf(stderr, "testserver: accept() call failed!\n");
	    exit(1);
	  }
	puts("FOOR");
	/* increment server's counter variable */
	conns++;
	
	/* try to create a child process to deal with this new client */
	pid = fork();

	/* use process id (pid) returned by fork to decide what to do next */
	if( pid < 0 )
	  {
	    fprintf(stderr, "testserver: fork() call failed!\n");
	    exit(1);
	  }
	else if( pid == 0 )
	  {
	    /* this is the child process doing this part */

	    /* don't need the parent listener socket that was inherited */
	    close(parentsockfd);

	    /* obtain the message from this client */
	    int bytes;
	    int done = 0;
		unsigned int size2;
	    while( !done )
	      {
		if(checkVoting==1){
			goto L1;
		}
		puts("CHECKITOUT");
		bzero(messagein, MAX_MESSAGE_LENGTH);
		if(bytes = recv(childsockfd, messagein, MAX_MESSAGE_LENGTH, 0)<0){
				puts("Error");
		}
		// size2 = sizeof(client); 
		// //sleep(1);
		// if(bytes = recvfrom(childsockfd, (char *)messagein, MAX_MESSAGE_LENGTH, 0, ( struct sockaddr *) &client,&size2)<0){//
		// 	puts("could not recieve");
		// }
		puts(messagein);

		
		if(strstr(messagein,"Translator")!=nullptr){//to determine if Translator server requested
			int transsockcheck;
			struct sockaddr_in transservaddrcheck;
			memset(&transservaddrcheck,0,sizeof(transservaddrcheck));
			if((transsockcheck=socket(AF_INET,SOCK_DGRAM,0))<0){ //IPPROTO_UDP
				perror("socket creation failed");
				exit(EXIT_FAILURE);
			}
			// struct timeval tv;
			// tv.tv_sec = 3;
			// tv.tv_usec=0;
				if (setsockopt(transsockcheck, SOL_SOCKET, SO_RCVTIMEO,&tv,sizeof(tv)) < 0) {
   				 perror("Error Translator Closed\n");
				}

			transservaddrcheck.sin_family=AF_INET;
			transservaddrcheck.sin_port=htons(8080);
			transservaddrcheck.sin_addr.s_addr=inet_addr("136.159.5.25");
			bzero(messagein,strlen(messagein));
			strcpy(messagein,"10");
			if(sendto(transsockcheck,(const char*)messagein,strlen(messagein),MSG_CONFIRM,(const struct sockaddr *)&transservaddrcheck,sizeof(transservaddrcheck))<0){
				printf("Unable to send message\n");
        		return -1;
			}
			memset(&transservaddrcheck,0,sizeof(transservaddrcheck));
			transservaddrcheck.sin_family=AF_INET;
			transservaddrcheck.sin_port=htons(8080);
			transservaddrcheck.sin_addr.s_addr=inet_addr("136.159.5.27");
			if(sendto(transsockcheck,(const char*)messagein,strlen(messagein),MSG_CONFIRM,(const struct sockaddr *)&transservaddrcheck,sizeof(transservaddrcheck))<0){
				printf("Unable to send message\n");
        		return -1;
			}
			bzero(messagein,strlen(messagein));
			int l=recv(transsockcheck, messagein, MAX_MESSAGE_LENGTH, 0);
			if(l<0){
					/*Test server working*/
				bzero(messageout, MAX_MESSAGE_LENGTH);
				strcpy(messageout,"Translator Off!\nType Translator, Currency-Converter, Voting or exit to choose: ");
				//if(send(childsockfd, messageout, strlen(messageout), 0) < 0)
				if(sendto(childsockfd,(const char*)messageout,strlen(messagein),MSG_CONFIRM,(const struct sockaddr *)&client,sizeof(client))<0)

		  			{
		    		fprintf(stderr, "Send failed on connection %d\n", conns);
		  			}
				goto Outside;
			} else{
				bzero(messageout, MAX_MESSAGE_LENGTH);
				strcpy(messageout,"Translator On!\n");
				//if(send(childsockfd, messageout, strlen(messageout), 0) < 0)
				if(sendto(childsockfd,(const char*)messageout,strlen(messagein),MSG_CONFIRM,(const struct sockaddr *)&client,sizeof(client))<0)

		  			{
		    		fprintf(stderr, "Send failed on connection %d\n", conns);
		  			}
			}
			
			memset(&translatorservaddr,0,sizeof(translatorservaddr));
			if((translatorsockfd=socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP))<0){
				perror("socket creation failed");
				exit(EXIT_FAILURE);
			}
			
			//fill server info
			translatorservaddr.sin_family=AF_INET;
			translatorservaddr.sin_port=htons(8080);
			translatorservaddr.sin_addr.s_addr=inet_addr("136.159.5.25");//IP address set as 136.159.5.25

			bzero(messageout, MAX_MESSAGE_LENGTH);
			strcpy(messageout,"Please enter an English word: ");
			if(send(childsockfd, messageout, strlen(messageout), 0) < 0)
		//if(sendto(childsockfd,(const char*)messageout,strlen(messagein),MSG_CONFIRM,(const struct sockaddr *)&client,sizeof(client))<0)
		  {
		    fprintf(stderr, "Send failed on connection %d\n", conns);
		  }
		

		  bzero(messageout,strlen(messageout));
		  bzero(messagein, MAX_MESSAGE_LENGTH);
		  bytes = recv(childsockfd, messagein, MAX_MESSAGE_LENGTH, 0);
			int n;
			unsigned int len=sizeof(translatorservaddr);
			if(sendto(translatorsockfd,(const char*)messagein,strlen(messagein),MSG_CONFIRM,(const struct sockaddr *)&translatorservaddr,sizeof(translatorservaddr))<0){
				printf("Unable to send message\n");
        		return -1;
			}
			memset(&translatorservaddr,0,sizeof(translatorservaddr));
		  	translatorservaddr.sin_family=AF_INET;
			translatorservaddr.sin_port=htons(8080);
			translatorservaddr.sin_addr.s_addr=inet_addr("136.159.5.27");
			if(sendto(translatorsockfd,(const char*)messagein,strlen(messagein),MSG_CONFIRM,(const struct sockaddr *)&translatorservaddr,sizeof(translatorservaddr))<0){
				printf("Unable to send message\n");
        		return -1;
			}

			char buffertranslator[MAX_MESSAGE_LENGTH];
			bzero(buffertranslator,strlen(buffertranslator));
			puts(messagein);
			if((n=recvfrom(translatorsockfd, (char *)buffertranslator,MAX_MESSAGE_LENGTH,MSG_WAITALL,(struct sockaddr*)&translatorservaddr, &len))<0){
				printf("Error while receiving server's msg\n");
		
        		return -1;
			}else{
				buffertranslator[n]='\0';
				puts(buffertranslator);
				if( send(childsockfd, buffertranslator, strlen(buffertranslator), 0) < 0)
		  			{
		    fprintf(stderr, "Send failed on connection %d\n", conns);
		  		}
			}
		}
	
		else if(strstr(messagein,"Currency-Converter")!=nullptr){//to determine if Currency-Converter requested
			int currencysockcheck;
			struct sockaddr_in currencyservaddrcheck;
			memset(&currencyservaddrcheck,0,sizeof(currencyservaddrcheck));
			if((currencysockcheck=socket(AF_INET,SOCK_DGRAM,0))<0){//IPPROTO_UDP
				perror("socket creation failed");
				exit(EXIT_FAILURE);
			}
			// struct timeval tv;
			// tv.tv_sec = 3;
			// tv.tv_usec=0;
				if (setsockopt(currencysockcheck, SOL_SOCKET, SO_RCVTIMEO,&tv,sizeof(tv)) < 0) {
   				 perror("Error Translator Closed\n");
				}

			currencyservaddrcheck.sin_family=AF_INET;
			currencyservaddrcheck.sin_port=htons(8081);
			currencyservaddrcheck.sin_addr.s_addr=inet_addr("136.159.5.25");
			bzero(messagein,strlen(messagein));
			strcpy(messagein,"11");
			if(sendto(currencysockcheck,(const char*)messagein,strlen(messagein),MSG_CONFIRM,(const struct sockaddr *)&currencyservaddrcheck,sizeof(currencyservaddrcheck))<0){
				printf("Unable to send message\n");
        		return -1;
			}
			memset(&currencyservaddrcheck,0,sizeof(currencyservaddrcheck));
			currencyservaddrcheck.sin_family=AF_INET;
			currencyservaddrcheck.sin_port=htons(8081);
			currencyservaddrcheck.sin_addr.s_addr=inet_addr("136.159.5.27");
			if(sendto(currencysockcheck,(const char*)messagein,strlen(messagein),MSG_CONFIRM,(const struct sockaddr *)&currencyservaddrcheck,sizeof(currencyservaddrcheck))<0){
				printf("Unable to send message\n");
        		return -1;
			}
			bzero(messagein,strlen(messagein));
			int l=recv(currencysockcheck, messagein, MAX_MESSAGE_LENGTH, 0);
			if(l<0){
					/*Test server working*/
				bzero(messageout, MAX_MESSAGE_LENGTH);
				strcpy(messageout,"Currency-Converter Off!\nType Translator, Currency-Converter, Voting or exit to choose: ");
				if(send(childsockfd, messageout, strlen(messageout), 0) < 0)
		  			{
		    		fprintf(stderr, "Send failed on connection %d\n", conns);
		  			}
				goto Outside;
			} else{
				bzero(messageout, MAX_MESSAGE_LENGTH);
				strcpy(messageout,"Currency-Converter On!\n");
				if(send(childsockfd, messageout, strlen(messageout), 0) < 0)
		  			{
		    		fprintf(stderr, "Send failed on connection %d\n", conns);
		  			}
			}
			
			//money exchange
			memset(&currencyservaddr,0,sizeof(currencyservaddr));
			if((currencysockfd=socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP))<0){
				perror("socket creation failed");
				exit(EXIT_FAILURE);
			}
			//fill server info
			currencyservaddr.sin_family=AF_INET;
			currencyservaddr.sin_port=htons(8081);
			currencyservaddr.sin_addr.s_addr=inet_addr("136.159.5.25");//IP address set as 136.159.5.25
			bzero(messageout, MAX_MESSAGE_LENGTH);
			strcpy(messageout,"Please enter a currency and dollar amount :");
			if(send(childsockfd, messageout, strlen(messageout), 0) < 0)
		  {
		    fprintf(stderr, "Send failed on connection %d\n", conns);
		  }
		  bzero(messagein, MAX_MESSAGE_LENGTH);
		  bytes = recv(childsockfd, messagein, MAX_MESSAGE_LENGTH, 0);
			int n;
			unsigned int len=sizeof(currencyservaddr);
			if(sendto(currencysockfd,(const char*)messagein,strlen(messagein),MSG_CONFIRM,(const struct sockaddr *)&currencyservaddr,sizeof(currencyservaddr))<0){
				printf("Unable to send message\n");
        		return -1;
			}

			memset(&currencyservaddr,0,sizeof(currencyservaddr));
			currencyservaddr.sin_family=AF_INET;
			currencyservaddr.sin_port=htons(8081);
			currencyservaddr.sin_addr.s_addr=inet_addr("136.159.5.27");

			if(sendto(currencysockfd,(const char*)messagein,strlen(messagein),MSG_CONFIRM,(const struct sockaddr *)&currencyservaddr,sizeof(currencyservaddr))<0){
				printf("Unable to send message\n");
        		return -1;
			}

			char buffertranslator[MAX_MESSAGE_LENGTH];
			bzero(buffertranslator,strlen(buffertranslator));
			puts(messagein);
			if((n=recvfrom(currencysockfd, (char *)buffertranslator,MAX_MESSAGE_LENGTH,MSG_WAITALL,(struct sockaddr*)&currencyservaddr, &len))<0){
				printf("Error while receiving server's msg\n");
				// bzero(buffertranslator,strlen(buffertranslator));
				// strcpy(buffertranslator,"Translator Microserver Closed\n");
				// buffertranslator[n]='\0';
				// puts(buffertranslator);
				// if( send(childsockfd, buffertranslator, strlen(buffertranslator), 0) < 0)
		  		// 	{
		    	// 	fprintf(stderr, "Send failed on connection %d\n", conns);
		  		// 	}
				//break;
        		return -1;
			}
			else{
			buffertranslator[n]='\0';
			puts(buffertranslator);
				if( send(childsockfd, buffertranslator, strlen(buffertranslator), 0) < 0)
		  		{
		    	fprintf(stderr, "Send failed on connection %d\n", conns);
		  		}
			}
		}
		else if(strstr(messagein,"Voting")!=nullptr){	//to determine if Voting server requested
				// std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
			puts("TEST1\n");
L1:
			 int votersockfdcheck;
			struct sockaddr_in voterservaddrcheck;
			std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
			long timerCheck= std::chrono::duration_cast<std::chrono::seconds>(end - begin).count();
			//time_t tt=system_clock::to_time_t ( system_clock::now() );

			if(timerCheck>=60){
				bonus=false;
				showAll=1;
			}
			//puts(ctime(&tt));
			printf("%d",timerCheck);
			bonus2=1;
			checkVoting=1;

		memset(&voterservaddrcheck,0,sizeof(voterservaddrcheck));
			if((votersockfdcheck=socket(AF_INET,SOCK_DGRAM,0))<0){ //IPPROTO_UDP
				perror("socket creation failed");
				exit(EXIT_FAILURE);
			}
			// struct timeval tv;
			// tv.tv_sec = 3;
			// tv.tv_usec=0;
				if (setsockopt(votersockfdcheck, SOL_SOCKET, SO_RCVTIMEO,&tv,sizeof(tv)) < 0) {
   				 perror("Error Translator Closed\n");
				}

			voterservaddrcheck.sin_family=AF_INET;
			voterservaddrcheck.sin_port=htons(8082);
			voterservaddrcheck.sin_addr.s_addr=inet_addr("136.159.5.25");
			bzero(messagein,strlen(messagein));
			strcpy(messagein,"12");
			if(sendto(votersockfdcheck,(const char*)messagein,strlen(messagein),MSG_CONFIRM,(const struct sockaddr *)&voterservaddrcheck,sizeof(voterservaddrcheck))<0){
				printf("Unable to send message\n");
        		return -1;
			}
			memset(&voterservaddrcheck,0,sizeof(voterservaddrcheck));
			voterservaddrcheck.sin_family=AF_INET;
			voterservaddrcheck.sin_port=htons(8082);
			voterservaddrcheck.sin_addr.s_addr=inet_addr("136.159.5.27");
			if(sendto(votersockfdcheck,(const char*)messagein,strlen(messagein),MSG_CONFIRM,(const struct sockaddr *)&voterservaddrcheck,sizeof(voterservaddrcheck))<0){
				printf("Unable to send message\n");
        		return -1;
			}
			bzero(messagein,strlen(messagein));
			int l=recv(votersockfdcheck, messagein, MAX_MESSAGE_LENGTH, 0);
			if(l<0){
					/*Test server working*/
				bzero(messageout, MAX_MESSAGE_LENGTH);
				strcpy(messageout,"Voting Off!\nType Translator, Currency-Converter, Voting or exit to choose: ");
				if(send(childsockfd, messageout, strlen(messageout), 0) < 0)
		  			{
		    		fprintf(stderr, "Send failed on connection %d\n", conns);
		  			}
					  checkVoting=0;
				goto Outside;
			} else{
				bzero(messageout, MAX_MESSAGE_LENGTH);
				strcpy(messageout,"Voting On!\n");
				if(send(childsockfd, messageout, strlen(messageout), 0) < 0)
		  			{
		    		fprintf(stderr, "Send failed on connection %d\n", conns);
		  			}
			}

			memset(&voterservaddr,0,sizeof(voterservaddr));
			if((votersockfd=socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP))<0){
				perror("socket creation failed");
				exit(EXIT_FAILURE);
			}
			//fill server info
			voterservaddr.sin_family=AF_INET;
			voterservaddr.sin_port=htons(8082);
			voterservaddr.sin_addr.s_addr=inet_addr("136.159.5.25");//IP address set as 136.159.5.25
			bzero(messageout, MAX_MESSAGE_LENGTH);
			bzero(messagein, MAX_MESSAGE_LENGTH);

			if(showAll==0){
				strcpy(messageout,"Please enter either Show-Candidates, Secure-Voting or Exit-Voting : ");
			}
			else{
				strcpy(messageout,"Please enter either Show-Candidates,Secure-Voting, Voting-Summary or Exit-Voting: ");
			}
		/*send to childsock the message to enter sub-menu*/
			 if(sendto(childsockfd,(const char*)messageout,strlen(messageout),MSG_CONFIRM,(const struct sockaddr *)&client,sizeof(client))<0){
        		printf("unable send message\n");
        		return -1;
    		}
			
		  if(recv(childsockfd, messagein, MAX_MESSAGE_LENGTH, 0)<0){
			  printf("recieve failed\n");
			  return -1;
		  }
		

		  puts(messagein);
			int n;
			unsigned int len=sizeof(voterservaddr);

			if(strstr(messagein,"Show-Candidates")!=nullptr){
				checkVoting=1;
				//puts("TEST1\n");
				if(sendto(votersockfd,(const char*)messagein,strlen(messagein),MSG_CONFIRM,(const struct sockaddr *)&voterservaddr,sizeof(voterservaddr))<0){
					printf("Unable to send message\n");
        			//return -1;
				}
				memset(&voterservaddr,0,sizeof(voterservaddr));
						voterservaddr.sin_family=AF_INET;
						voterservaddr.sin_port=htons(8082);
						voterservaddr.sin_addr.s_addr=inet_addr("136.159.5.27");
						if(sendto(votersockfd,(const char*)messagein,strlen(messagein),MSG_CONFIRM,(const struct sockaddr *)&voterservaddr,sizeof(voterservaddr))<0){
							printf("Unable to send message\n");
        					return -1;
						}
				
				char buffertranslator[MAX_MESSAGE_LENGTH];
				bzero(buffertranslator,strlen(buffertranslator));
				puts(messagein);
				if((n=recvfrom(votersockfd, (char *)buffertranslator,MAX_MESSAGE_LENGTH,MSG_WAITALL,(struct sockaddr*)&voterservaddr, &len))<0){
					printf("Error while receiving server's msg\n");
        			return -1;
				}
				buffertranslator[n]='\0';
				puts(buffertranslator);
				if( send(childsockfd, buffertranslator, strlen(buffertranslator), 0) < 0)
		  			{
		    		fprintf(stderr, "Send failed on connection %d\n", conns);
		  			}
				}
			else if(strstr(messagein,"Secure-Voting")!=nullptr && (bonus==true)){
				checkVoting=1;
					if(sendto(votersockfd,(const char*)messagein,strlen(messagein),MSG_CONFIRM,(const struct sockaddr *)&voterservaddr,sizeof(voterservaddr))<0){
					printf("Unable to send message\n");
        			return -1;
					
					}
					    memset(&voterservaddr,0,sizeof(voterservaddr));
						voterservaddr.sin_family=AF_INET;
						voterservaddr.sin_port=htons(8082);
						voterservaddr.sin_addr.s_addr=inet_addr("136.159.5.27");
						if(sendto(votersockfd,(const char*)messagein,strlen(messagein),MSG_CONFIRM,(const struct sockaddr *)&voterservaddr,sizeof(voterservaddr))<0){
							printf("Unable to send message\n");
        					return -1;
						}
				char buffertranslator[MAX_MESSAGE_LENGTH];
				bzero(buffertranslator,strlen(buffertranslator));
				puts(messagein);
				if((n=recvfrom(votersockfd, (char *)buffertranslator,MAX_MESSAGE_LENGTH,MSG_WAITALL,(struct sockaddr*)&voterservaddr, &len))<0){
					printf("Error while receiving server's msg\n");
        			return -1;
				}
	
				buffertranslator[n]='\0';
				char tmpSendBack[MAX_MESSAGE_LENGTH];
				strcpy(tmpSendBack,"Encryption Key is: ");
				strcat(tmpSendBack,buffertranslator);
				strcat(tmpSendBack,"\n");
				strcat(tmpSendBack,"Please enter ID: ");
				//puts(buffertranslator);
				if( send(childsockfd, tmpSendBack, strlen(tmpSendBack), 0) < 0)
		  			{
		    		fprintf(stderr, "Send failed on connection %d\n", conns);
		  			}
				bzero(messagein,strlen(messagein));
				bytes = recv(childsockfd, messagein, MAX_MESSAGE_LENGTH, 0);
				if(sendto(votersockfd,(const char*)messagein,strlen(messagein),MSG_CONFIRM,(const struct sockaddr *)&voterservaddr,sizeof(voterservaddr))<0){
					printf("Unable to send message\n");
        			return -1;
					}
					
				//showAll=1;//
				//char buffertranslator[MAX_MESSAGE_LENGTH];
				bzero(buffertranslator,strlen(buffertranslator));

				if((n=recvfrom(votersockfd, (char *)buffertranslator,MAX_MESSAGE_LENGTH,MSG_WAITALL,(struct sockaddr*)&voterservaddr, &len))<0){
					printf("Error while receiving server's msg\n");
        			return -1;
				}
				buffertranslator[n]='\0';
				puts(buffertranslator);
				if( send(childsockfd, buffertranslator, strlen(buffertranslator), 0) < 0)
		  			{
		    		fprintf(stderr, "Send failed on connection %d\n", conns);
		  			}
			}
			if(strstr(messagein,"Secure-Voting")!=nullptr&& (bonus==false)){
			bzero(messageout,strlen(messageout));
			strcpy(messageout,"Cannot Vote Anymore Sorry!\n");
			if(send(childsockfd, messageout, strlen(messageout), 0) < 0)
		  		{
			 fprintf(stderr, "Send failed on connection %d\n", conns);
		  		}
			}
			
			else if((strstr(messagein,"Voting-Summary")!=nullptr && showAll==1)){
				checkVoting=1;
				if(sendto(votersockfd,(const char*)messagein,strlen(messagein),MSG_CONFIRM,(const struct sockaddr *)&voterservaddr,sizeof(voterservaddr))<0){
					printf("Unable to send message\n");
        			return -1;
				}
				memset(&voterservaddr,0,sizeof(voterservaddr));
						voterservaddr.sin_family=AF_INET;
						voterservaddr.sin_port=htons(8082);
						voterservaddr.sin_addr.s_addr=inet_addr("136.159.5.27");
						if(sendto(votersockfd,(const char*)messagein,strlen(messagein),MSG_CONFIRM,(const struct sockaddr *)&voterservaddr,sizeof(voterservaddr))<0){
							printf("Unable to send message\n");
        					return -1;
						}
				char buffertranslator[MAX_MESSAGE_LENGTH];
				bzero(buffertranslator,strlen(buffertranslator));
				puts(messagein);
				if((n=recvfrom(votersockfd, (char *)buffertranslator,MAX_MESSAGE_LENGTH,MSG_WAITALL,(struct sockaddr*)&voterservaddr, &len))<0){
					printf("Error while receiving server's msg\n");
        			return -1;
				}
				buffertranslator[n]='\0';
				puts(buffertranslator);
				if( send(childsockfd, buffertranslator, strlen(buffertranslator), 0) < 0)
		  			{
		    		fprintf(stderr, "Send failed on connection %d\n", conns);
		  			}
				}
			else if(strstr(messagein,"Exit-Voting")){
				char buffertranslator[MAX_MESSAGE_LENGTH];

				checkVoting=0;
				bzero(buffertranslator,strlen(buffertranslator));
				strcpy(buffertranslator,"Exited Voting\n");
				if( send(childsockfd, buffertranslator, strlen(buffertranslator), 0) < 0)
		  			{
		    		fprintf(stderr, "Send failed on connection %d\n", conns);
		  			}
				puts("EXITTTTT\n");
			}
			//for voting
		}
		else if(strstr(messagein,"exit")!=nullptr){//exit the entire program
							char buffertranslator[MAX_MESSAGE_LENGTH];
			bzero(buffertranslator,strlen(buffertranslator));
				strcpy(buffertranslator,"Exiting Indirection Server\n");
				if( send(childsockfd, buffertranslator, strlen(buffertranslator), 0) < 0)
		  			{
		    		fprintf(stderr, "Send failed on connection %d\n", conns);
		  			}
				goto L2;
		}

Outside:
		//printf("Child about to send message: %s\n", messageout);
		printf("%d",checkVoting);
		if(checkVoting==0){
			bzero(messageout, strlen(messageout));
		  	strcpy(messageout,"\nType Translator, Currency-Converter, Voting or exit to choose: ");
		/* send the result message back to the client */
			if( send(childsockfd, messageout, strlen(messageout), 0) < 0)
		  	{
		   		 fprintf(stderr, "Send failed on connection %d\n", conns);
		  	}
			  puts("\nSend mEss\n");
		}
		

		/* clear out message strings again to be safe */
		bzero(messagein, MAX_MESSAGE_LENGTH);
		bzero(messageout, MAX_MESSAGE_LENGTH);
	      }
		  L2:

	    /* when client is no longer sending information to us, */
	    /* the socket can be closed and the child process terminated */
	    fprintf(stderr, "Shutting down child process for conn %d\n", conns);
	    //close(childsockfd);
		close(parentsockfd);
	    exit(0);
	  } /* end of then part for child */
	else
	  {
	    /* the parent process is the one doing this part */
	    fprintf(stderr, "Created child process %d to handle that client\n", pid);
	    fprintf(stderr, "Parent going back to job of listening...\n\n");

	    /* parent doesn't need the childsockfd */
	    close(childsockfd);
	  }
      }
  }