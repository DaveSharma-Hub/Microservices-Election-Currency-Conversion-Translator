// Server side implementation of UDP client-server model
    /*
Author: Dave Sharma
Filename: Currency.cpp
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
#define PORT     8081
#define MAXLINE 1024
  
using namespace std;
string first(char* holder);
string second(char* holder);
double value(char* holder);

int childsockfd;
// Driver code
int main() {
    int sockfd;
    char buffer[MAXLINE];
    char sendBack[MAXLINE];
    char *hello;// = "Hello from translator service server";
    struct sockaddr_in servaddr, cliaddr;
    string list[4][3]={{"CAD","USD","0.80"},{"CAD","EURO","0.69"},{"CAD","GBP","0.59"},{"CAD","BTC","0.000014"}};
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
        perror("bind failed");
        //exit(EXIT_FAILURE);
                memset(&servaddr, 0, sizeof(servaddr));
        servaddr.sin_family    = AF_INET; // IPv4
        servaddr.sin_addr.s_addr = inet_addr("136.159.5.27");//IP address set as 136.159.5.25
        servaddr.sin_port = htons(PORT);
            if ( bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0 ){
                    perror("bind failed");
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
  
    len = sizeof(servaddr);  //len is value/result //
    bzero(buffer,MAXLINE);
    n = recvfrom(sockfd, (char *)buffer, MAXLINE, MSG_WAITALL, ( struct sockaddr *) &servaddr,&len);//
    if(strstr(buffer,"11")!=nullptr){
           bzero(sendBack,strlen(sendBack));
           strcpy(sendBack,"GOOD");
            sendto(sockfd, (const char *)sendBack, strlen(sendBack), MSG_CONFIRM, (const struct sockaddr *) &servaddr,len);//
    }
    else{
    buffer[n] = '\0';
    printf("Client : %s\n", buffer);
    bzero(sendBack,strlen(sendBack));
    for(int i=0;i<4;i++){
        
            if((strstr(buffer,list[i][0].c_str())!=nullptr) || (strstr(buffer,list[i][1].c_str())!=nullptr)){
               
               if(strstr(first(buffer).c_str(),list[i][0].c_str())!=nullptr && strstr(second(buffer).c_str(),list[i][1].c_str())!=nullptr){
                    double conversion=stod(list[i][2]);
                    double val=value(buffer);
                    val=conversion*val;
                    char tmp[MAXLINE];
                    strcpy(tmp,"$");
                    strcat(tmp,to_string(val).c_str());
                    strcat(tmp," ");
                    strcat(tmp,list[i][1].c_str());
                    strcpy(sendBack,tmp);
                    puts(sendBack);
                    sendto(sockfd, (const char *)sendBack, strlen(sendBack), MSG_CONFIRM, (const struct sockaddr *) &servaddr,len);//
                    break;
               }
               else if(strstr(first(buffer).c_str(),list[i][1].c_str())!=nullptr && strstr(second(buffer).c_str(),list[i][0].c_str())!=nullptr){
                    double conversion=stod(list[i][2]);
                    double val=value(buffer);
                    val=val/conversion;
                    char tmp[MAXLINE];
                    strcpy(tmp,"$");
                    strcat(tmp,to_string(val).c_str());
                    strcat(tmp," ");
                    strcat(tmp,list[i][0].c_str());
                    strcpy(sendBack,tmp);
                    puts(sendBack);

                    sendto(sockfd, (const char *)sendBack, strlen(sendBack), MSG_CONFIRM, (const struct sockaddr *) &servaddr,len);//
                    break;
               }
            }  
    }
    if(strlen(sendBack)==0){
        strcpy(sendBack,"Sorry could not find conversion\n");
    }
    sendto(sockfd, (const char *)sendBack, strlen(sendBack), MSG_CONFIRM, (const struct sockaddr *) &servaddr,len);//
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

string first(char* holder){ //determine what to convert from
    char tmp[strlen(holder)];
    int counter=0;
    for(int i=1;i<strlen(holder);i++){
        if((holder[i]<='9' &&holder[i]>='0') ||(holder[i]=='.')){
            counter++;
        }
        else{
            break;
        }
    }
    counter+=2;
    int i=0;
    for(;counter<strlen(holder);counter++){
        if(holder[counter]<='Z'&&holder[counter]>='A'){
            tmp[i]=holder[counter];
            i++;
        }
        else{
            tmp[i]='\0';
            break;
        }
    }
    return string(tmp);
}
string second(char* holder){ //determine what to convert to
    char tmp[strlen(holder)];
    int counter=0;
    for(int i=1;i<strlen(holder);i++){
        if((holder[i]<='9' &&holder[i]>='0') ||(holder[i]=='.')){
            counter++;
        }
        else{
            break;
        }
    }
    counter+=2;
    int i=0;
    for(;counter<strlen(holder);counter++){
        if(holder[counter]<='Z'&&holder[counter]>='A'){
        }
        else{
            break;
        }
    }
    counter++;
    for(;counter<strlen(holder);counter++){
        if(holder[counter]<='Z'&&holder[counter]>='A'){
            tmp[i]=holder[counter];
            i++;
        }
        else{
            tmp[i]='\0';
            break;
        }
    }
    return string(tmp);
}
double value(char* holder){//determine numerical value of conversion requested
    char tmp[strlen(holder)];
    int counter=0;
    for(int i=1;i<strlen(holder);i++){
        if((holder[i]<='9' &&holder[i]>='0') ||(holder[i]=='.')){
            tmp[counter]=holder[i];
            counter++;
        }
        else{
            break;
        }
    }
    return stod(string(tmp));
}
