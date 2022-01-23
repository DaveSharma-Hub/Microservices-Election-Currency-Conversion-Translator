// Server side implementation of UDP client-server model
    /*
Author: Dave Sharma
UCID: 30093981
Filename: Voting.cpp
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
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string>
#define PORT     8082
#define MAXLINE 10000
  
using namespace std;

int removeEnd(char* buffer);
int childsockfd;
// Driver code
int main() {
    int sockfd;
    char buffer[MAXLINE];
    char sendBack[MAXLINE];
    char *hello; // = "Hello from translator service server";
    struct sockaddr_in servaddr, cliaddr;
    string candidates[4][3]={{"Johanson Clark","3","0"},{"Tiffany Larson","7","0"},{"Peterson Gills","1","0"},{"Jasmine Trisha","9","0"}};
    // Creating socket file descriptor
    

    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0 ) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }
      bzero(sendBack,MAXLINE);
    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));
      
    // Filling server information
    servaddr.sin_family    = AF_INET; // IPv4
    servaddr.sin_addr.s_addr = inet_addr("136.159.5.25");//IP address set as 136.159.5.25
    servaddr.sin_port = htons(PORT);
      
    // Bind the socket with the server address
    if ( bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0 )
    {
        //perror("bind failed");
        //exit(EXIT_FAILURE);
            memset(&servaddr, 0, sizeof(servaddr));
        servaddr.sin_family    = AF_INET;
        servaddr.sin_addr.s_addr = inet_addr("136.159.5.27");//IP address set as 136.159.5.27
        servaddr.sin_port = htons(PORT);
        if(bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0){
            perror("2nd bind failed");
            exit(EXIT_FAILURE);
        }
    }

    int conns = 0;
    int c = sizeof(struct sockaddr_in);
    int pid;
    for(;;){
   
      	conns++;
       // pid=fork();
       pid=0;
        if( pid < 0 )
	  { 
	    fprintf(stderr, "testserver: fork() call failed!\n");
	    exit(1);
	  }
      else if(pid==0){
        //close(sockfd);

        while(1){
    unsigned int len;
    int n;
    //bool foundPerson=false;
    len = sizeof(servaddr);  //len is value/result //
    bzero(buffer,MAXLINE);
    n = recvfrom(sockfd, (char *)buffer, MAXLINE, MSG_WAITALL, ( struct sockaddr *) &servaddr,&len);//
    
    if(strstr(buffer,"12")!=nullptr){
            bzero(sendBack,strlen(sendBack));
            strcpy(sendBack,"GOOD");
         sendto(sockfd, (const char *)sendBack, strlen(sendBack), MSG_CONFIRM, (const struct sockaddr *) &servaddr,len);//
    } 
    
        else{
    buffer[n] = '\0';
    printf("Client : %s\n", buffer);
    bzero(sendBack,strlen(sendBack));
    if(strstr(buffer,"Show-Candidates")!=nullptr){
        char holder[1000];
        strcpy(sendBack,"Name             ID Number\n");
        strcat(sendBack,"-----------------------------\n");
        for(int i=0;i<4;i++){
            strcpy(holder,candidates[i][0].c_str());
            strcat(holder,"      ");
            strcat(holder,candidates[i][1].c_str());
            //strcat(holder,"  ");
            //strcat(holder,candidates[i][2]);
            strcat(sendBack,holder);
            strcat(sendBack,"\n");
            bzero(holder,1000);
        }
    sendto(sockfd, (const char *)sendBack, strlen(sendBack), MSG_CONFIRM, (const struct sockaddr *) &servaddr,len);//
    break;
    }
    else if(strstr(buffer,"Secure-Voting")!=nullptr){
        //puts("TTTT\n");
        bzero(sendBack,strlen(sendBack));
        strcpy(sendBack,"5");//encryption key   
        sendto(sockfd, (const char *)sendBack, strlen(sendBack), MSG_CONFIRM, (const struct sockaddr *) &servaddr,len);//
         bzero(buffer,MAXLINE);
         n = recvfrom(sockfd, (char *)buffer, MAXLINE, MSG_WAITALL, ( struct sockaddr *) &servaddr,&len);//
        int encryptedVal=removeEnd(buffer);
        printf("%d",encryptedVal);
        int val=encryptedVal/5; //id value
        bool foundPerson=false;

        for(int i=0;i<4;i++){
            if(atoi(candidates[i][1].c_str())==val){
                int holder=atoi(candidates[i][2].c_str());
                candidates[i][2]=to_string(holder+1);
                foundPerson=true;
                break;
            }
        }
         if(foundPerson){
             bzero(sendBack,strlen(sendBack));
            strcpy(sendBack,"true");
         }else{
             bzero(sendBack,strlen(sendBack));
                strcpy(sendBack,"false");
         }

        sendto(sockfd, (const char *)sendBack, strlen(sendBack), MSG_CONFIRM, (const struct sockaddr *) &servaddr,len);//
        //break;
    }
    else if(strstr(buffer,"Voting-Summary")!=nullptr){
        char holder[1000];
        bzero(sendBack,strlen(sendBack));
        strcpy(sendBack,"Name         ID Number     Votes\n");
        strcat(sendBack,"--------------------------------------\n");
        for(int i=0;i<4;i++){
            strcpy(holder,candidates[i][0].c_str());
            strcat(holder,"    ");
            strcat(holder,candidates[i][1].c_str());
            strcat(holder,"     ");
            strcat(holder,candidates[i][2].c_str());
            strcat(sendBack,holder);
            strcat(sendBack,"\n");
            bzero(holder,1000);
        }
    sendto(sockfd, (const char *)sendBack, strlen(sendBack), MSG_CONFIRM, (const struct sockaddr *) &servaddr,len);//
    break;
    }
//    sendto(sockfd, (const char *)sendBack, strlen(sendBack), MSG_CONFIRM, (const struct sockaddr *) &servaddr,len);//
    //printf("Hello message sent.\n"); 
        }
        
        }
      }
      else
	  {
	    /* the parent process is the one doing this part */
	    fprintf(stderr, "Created child process %d to handle that client\n", pid);
	    fprintf(stderr, "Parent going back to job of listening...\n\n");

	    /* parent doesn't need the childsockfd */
	    close(childsockfd);
      }
    }
    return 0;
}

int removeEnd(char* buffer){
    char tmp[strlen(buffer)];
    for(int i=0;i<strlen(buffer);i++){
        if(buffer[i]<='9'&& buffer[i]>='0'){
            tmp[i]=buffer[i];
        }
        else{
            break;
        }
    }
    int ret=atoi(tmp);
    return ret;
}