    /*
Author: Dave Sharma
Filename: clienta2.cpp
Date: October 18, 2021
*/
/*
References:
http://pages.cpsc.ucalgary.ca/~carey/CPSC441/examples/testserver.c // parts of Dr. Carey's server code used to create main server-side of Proxy
https://d2l.ucalgary.ca/d2l/le/content/401094/viewContent/4896480/View // TA's notes presented on HTTP used in server and client side of Proxy
https://www.geeksforgeeks.org/socket-programming-cc/ //Used in secondary server to telnet to Proxy
https://www.binarytides.com/socket-programming-c-linux-tutorial/ //Used in creating client-side of Proxy
https://sites.google.com/view/sinacpsc441/content //Sina Keshvadi's notes used to understand the required Proxy contents
*/
#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <iostream>
#include <string.h> // memset
#include <unistd.h> // close
#include <cstdlib>
#include <string>
#include <cstring>

using namespace std;

#define PORT 44101
// void show_message(int n, string message);
int keyValue(char* holder);

int main(int argc, char const *argv[])
{
    
    // Set destination info
    puts("Welcome to portal\n");
    int sock=0, valread;
    struct sockaddr_in servaddr;
    unsigned int len=sizeof(servaddr);

    if((sock=socket(AF_INET,SOCK_STREAM,0))<0){
        printf("\nSocket error");
        return -1;
    }
    //set server port and use IPV4
    memset(&servaddr,0,sizeof(servaddr));

    servaddr.sin_family=AF_INET;
    servaddr.sin_port=htons(PORT);
    
    if(inet_pton(AF_INET,"136.159.5.25",&servaddr.sin_addr)<=0)
    {
        //if(inet_pton(AF_INET,"136.159.5.27",&servaddr.sin_addr)<=0){
                        printf("\nConversion Failed");
                        //return -1;
        //}
    }
    if(connect(sock,(struct sockaddr *)&servaddr,sizeof(servaddr))<0)
    {   
        memset(&servaddr,0,sizeof(servaddr));
        servaddr.sin_family=AF_INET;
         servaddr.sin_port=htons(PORT);
        if(inet_pton(AF_INET,"136.159.5.27",&servaddr.sin_addr)<=0){
                        printf("\nConnection failed");
                        return -1;
        }
        if(connect(sock,(struct sockaddr *)&servaddr,sizeof(servaddr))<0)
        {
            printf("\nConnection failed");
            return -1;
        }
    }
    bool stayIn=true; //to stay in client loop
    char response[1024]; //response from user
    char sendTo [1024]; //sending response to indirection server
    char buffer[10000]; //buffer from the indirection server
    
puts("Enter Translator,Currency-Converter or Voting\n");
    while(stayIn){
        
       //if(strstr(buffer,"Encryption key is: ")==nullptr){
        bzero(buffer,10000);
        bzero(response,1024);
        bzero(sendTo,1024);

        scanf("%s",response);
        strcpy(sendTo,response);
       // }

        if(sendto(sock,(const char*)sendTo,strlen(sendTo),0,(const struct sockaddr *)&servaddr,sizeof(servaddr))<0){
        printf("unable send message\n");
        return -1;
        }
        bzero(response,1024);
        bzero(sendTo,1024);
        bzero(buffer,10000);
        int checkerd;
        sleep(1);
        if((checkerd=recv(sock,buffer,10000,0))<0){
        //     
        // if(checkerd=recvfrom(sock, (char *)buffer, 10000, 0, ( struct sockaddr *) &servaddr,&len)<0){
                printf("unable to recieve message\n");
                return -1;
            }
           if(checkerd>=0){
            puts(buffer);
           } 
        // if(strstr(buffer,"Encryption Key is: ")!=nullptr){
        //     bzero(response,1024);
        //     scanf("%s",response);
        //     int tmp=5 * atoi(response);
        //     bzero(response,1024);
        //     strcpy(sendTo,to_string(tmp).c_str());
        //     bzero(buffer,10000);
        // }
        if(strstr(buffer,"Exiting Indirection Server")!=nullptr){
            stayIn=false;
        }
    }
    /*
    
L1:
    puts("Please enter Translator, Currency-Converter, Voting or exit: ");
    bzero(response,1024);
	scanf("%s",response);
    strcpy(sendTo,response);
    if(strstr(response,"exit")!=nullptr){
        stayIn=false;
    }
    if(strstr(response,"Translator")==nullptr&&strstr(response,"Currency-Converter")==nullptr&&strstr(response,"Voting")==nullptr&&strstr(response,"exit")==nullptr){
        goto L1;
    }
    if(sendto(sock,(const char*)sendTo,strlen(sendTo),0,(const struct sockaddr *)&servaddr,sizeof(servaddr))<0){
        printf("unable send message\n");
        return -1;
    }


    
bzero(buffer,10000);
         if(recv(sock,buffer,10000,0)<0){
                printf("unable to recieve message\n");
                return -1;
            }
        
        if(strstr(buffer,"Translator Off!")!=nullptr){
            puts(buffer);
            goto L1;
        }
        if(strstr(buffer,"Translator On!")!=nullptr){
            bzero(buffer,strlen(buffer));
            if(recv(sock,buffer,10000,0)<0){
                printf("unable to recieve message\n");
                return -1;
            }
            goto TEST;
        }
        if(strstr(buffer,"Currency-Converter Off!")!=nullptr){
            puts(buffer);
            goto L1;
        }
        if(strstr(buffer,"Currency-Converter On!")!=nullptr){
            bzero(buffer,strlen(buffer));
            if(recv(sock,buffer,10000,0)<0){
                printf("unable to recieve message\n");
                return -1;
            }
            goto TEST;
        }
        if(strstr(buffer,"Voting Off!")!=nullptr){
            puts(buffer);
            goto L1;
        }
        if(strstr(buffer,"Voting On!")!=nullptr){
            bzero(buffer,strlen(buffer));
            if(recv(sock,buffer,10000,0)<0){
                printf("unable to recieve message\n");
                return -1;
            }
            goto TEST;
        }
        //
    bzero(buffer,10000);
    bzero(response,1024);
    
    while(stayIn){

redo:
    bzero(response,1024);
     bzero(buffer,10000);
     bzero(sendTo,1024);
        //sleep(1);

    if(recv(sock,buffer,10000,0)<0){
         //   if(recvfrom(sock, (char *)buffer, 10000, 0, ( struct sockaddr *) &servaddr,&len)<0){

        printf("unable to recieve message\n");
        return -1;
    }
    
    //determine what to type for menu//
    if(strstr(buffer,"Type Translator, Currency-Converter, Voting or exit to choose: ")!=nullptr){
        L2:
        puts(buffer);
        bzero(response,1024);
        scanf("%s",response);
        bzero(sendTo,1024);
        strcpy(sendTo,response);
        if(strstr(response,"Translator")==nullptr&&strstr(response,"Currency-Converter")==nullptr &&strstr(response,"Voting")==nullptr&&strstr(response,"exit")==nullptr){
            goto L2;
        }
        if(sendto(sock,(const char*)sendTo,strlen(sendTo),0,(const struct sockaddr *)&servaddr,sizeof(servaddr))<0){
             printf("unable send message\n");
            return -1;
        }

        bzero(buffer,10000);
                sleep(1);
         if(recv(sock,buffer,10000,0)<0){
            //if(recvfrom(sock, (char *)buffer, 10000, 0, ( struct sockaddr *) &servaddr,&len)<0){

                printf("unable to recieve message\n");
                return -1;
            }
        //puts(buffer);
        if(strstr(buffer,"Translator Off!")!=nullptr){
            puts(buffer);
            goto redo;
        }
        if(strstr(buffer,"Translator On!")!=nullptr){
            bzero(buffer,strlen(buffer));
            if(recv(sock,buffer,10000,0)<0){
               //     if(recvfrom(sock, (char *)buffer, 10000, 0, ( struct sockaddr *) &servaddr,&len)<0){

                printf("unable to recieve message\n");
                return -1;
            }
            goto TEST;
        }
        if(strstr(buffer,"Currency-Converter Off!")!=nullptr){
            puts(buffer);
            goto redo;
        }
        if(strstr(buffer,"Currency-Converter On!")!=nullptr){
            bzero(buffer,strlen(buffer));
           if(recv(sock,buffer,10000,0)<0){
                  // if(recvfrom(sock, (char *)buffer, 10000, 0, ( struct sockaddr *) &servaddr,&len)<0){

                printf("unable to recieve message\n");
                return -1;
            }
            goto TEST;
        }
        if(strstr(buffer,"Voting Off!")!=nullptr){
            puts(buffer);
            goto redo;
        }
        if(strstr(buffer,"Voting On!")!=nullptr){
            bzero(buffer,strlen(buffer));
            if(recv(sock,buffer,10000,0)<0){
                    //if(recvfrom(sock, (char *)buffer, 10000, 0, ( struct sockaddr *) &servaddr,&len)<0){

                printf("unable to recieve message\n");
                return -1;
            }
            goto TEST;
        }
    }
    
    
TEST:
        if(strstr(buffer,"Please enter an English word: ")!=nullptr){
            puts(buffer);
            bzero(response,1024);
            bzero(sendTo,1024);
            scanf("%s",response);
            strcpy(sendTo,response);

            if(sendto(sock,(const char*)sendTo,strlen(sendTo),MSG_CONFIRM,(const struct sockaddr *)&servaddr,sizeof(servaddr))<0){
                printf("unable send message\n");
                return -1;
            }
            bzero(buffer,10000);
            if(recv(sock,buffer,10000,0)<0){
                printf("error\n");
                //bzero(buffer,strlen(buffer));
                return -1;
            }else{  
                puts(buffer);
            }
            goto redo;
        }

        if(strstr(buffer,"Please enter a currency and dollar amount :")!=nullptr){
            puts(buffer);
            bzero(response,1024);
            bzero(sendTo,1024);
            scanf("%s",response);
            strcpy(sendTo,response);

            if(sendto(sock,(const char*)sendTo,strlen(sendTo),MSG_CONFIRM,(const struct sockaddr *)&servaddr,sizeof(servaddr))<0){
        printf("unable send message\n");
        return -1;
            }
            bzero(buffer,10000);
            if(recv(sock,buffer,10000,0)<0){
        printf("unable to recieve message\n");
        return -1;
            }
            puts(buffer);
            goto redo;
        }

        if(strstr(buffer,"Please enter either Show-Candidates, Secure-Voting or Exit-Voting : ")!=nullptr || strstr(buffer,"Please enter either Show-Candidates,Secure-Voting, Voting-Summary or Exit-Voting: ")!=nullptr){
            L3:
                puts(buffer);
            bzero(response,1024);
            bzero(sendTo,1024);
            scanf("%s",response);
            strcpy(sendTo,response);
            if((strstr(response,"Show-Candidates")==nullptr&&strstr(response,"Secure-Voting")==nullptr &&strstr(response,"Exit-Voting")==nullptr) &&(strstr(response,"Show-Candidates")==nullptr&&strstr(response,"Secure-Voting")==nullptr &&strstr(response,"Exit-Voting")==nullptr&&strstr(response,"Voting-Summary")==nullptr)){
                goto L3;
            }   
            if(sendto(sock,(const char*)sendTo,strlen(sendTo),MSG_CONFIRM,(const struct sockaddr *)&servaddr,sizeof(servaddr))<0){
        printf("unable send message\n");
        return -1;
            }
            bzero(buffer,10000);
            if(recv(sock,buffer,10000,0)<0){
        printf("unable to recieve message\n");
        return -1;
            }
            if(strstr(buffer,"Encryption Key is: ")){ //if entering the ID, use encryption key
            bzero(response,strlen(response));
            int key=keyValue(buffer);
            puts("Please enter ID of you Vote: ");
            scanf("%s",response);
            int tmp=key * atoi(response);
            bzero(response,1024);
            strcpy(response,to_string(tmp).c_str());

            bzero(sendTo,1024);
            strcpy(sendTo,response);
                if(sendto(sock,(const char*)sendTo,strlen(sendTo),MSG_CONFIRM,(const struct sockaddr *)&servaddr,sizeof(servaddr))<0){
                     printf("unable send message\n");
                        return -1;
                }
                bzero(buffer,strlen(buffer));
                if(recv(sock,buffer,10000,0)<0){
                printf("unable to recieve message\n");
                return -1;
                }
                if(strstr(buffer,"true")){
                puts("\nYou Voted!\n");
                }
                else{
                    puts("\nPlease revote and enter valid ID\n");
                }
            }
            else{
            puts(buffer);
            }
            goto redo;
        }

        else if(strstr(buffer,"Exiting Indirection Server")!=nullptr){
            stayIn=false;
        }
        //exit the indirection server 
    
    }
    */
    puts("Goodbye\n");
    return 0;
}

//determine the encryption key value to multiply by ID
int keyValue(char* holder){
    int counter=19;
    char tmp[3]={0};
    int i=0;
    for(;counter<strlen(holder);counter++){
        if(holder[counter]<='9' && holder[counter]>='0'){
            tmp[i]=holder[counter];
            i++;
        }
        else{
            break;
        }
    }
    return atoi(tmp);
}
