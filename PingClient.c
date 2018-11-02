//I copied all of the code from the Socket programming guide in the linux howtos website. I was unable to find the
//author.
//SOURCE: http://www.linuxhowtos.org/data/6/client_udp.c
/* UDP client in the internet domain */
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

void error(const char *);
int main(int argc, char *argv[])
{
   int sock, n;
   unsigned int length;
   struct sockaddr_in server, from;
   struct hostent *hp;
   char buffer[1024];
   
   if (argc != 3) { printf("Usage: server port\n");
                    exit(1);
   }
   //create socket
   sock = socket(AF_INET, SOCK_DGRAM, 0);
   if (sock < 0) error("socket");
   server.sin_family = AF_INET;
   hp = gethostbyname(argv[1]);
   if (hp==0) error("Unknown host");

   bcopy((char *)hp->h_addr, 
        (char *)&server.sin_addr,
         hp->h_length);
   server.sin_port = htons(atoi(argv[2]));
   length=sizeof(struct sockaddr_in);

   //variable for storing 
   struct timeval tv, timeout;
   double max = 0.0, min = 999999999.0, total;
   int numLostPackets = 0, numRecvdPackets = 0;
   for(int i = 1; i < 11; i++)
   {
       gettimeofday(&tv, NULL);
       double milliseconds = (tv.tv_sec) / 1000 + (tv.tv_usec) * 1000;
       sprintf(buffer, "%s%d%s%f", "PING ", i, " ", milliseconds);
       n = sendto(sock, buffer, sizeof(buffer), 0, (const struct sockaddr *)&server, length);
       if (n < 0) perror("Error sending to socket");
       
       timeout.tv_sec = 0;
       timeout.tv_usec = 1000;
       if(setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0)
       {
           perror("Error");
       }

       n = read(sock, buffer, sizeof(buffer));

       if(n < 0)
       {
       }
       else
       {
           struct timeval recvdtime;
           gettimeofday(&recvdtime, NULL);
           double recvdmilliseconds = (recvdtime.tv_sec) / 1000 + (recvdtime.tv_usec) * 1000;
           numRecvdPackets++;
           char output[1024];
           int chtoi[1024];
           int seqNum = 0;
           char *token;

           token = strtok(buffer, " ");
           printf("%s%s%s%s", token, " received from ", argv[1], " : seq#=");
           token = strtok(NULL, " ");
           printf("%s time=", token);
           token = strtok(NULL, " ");

           double servMilliseconds = strtod(token, NULL);
           double diffMilliseconds = recvdmilliseconds - servMilliseconds;

           total += diffMilliseconds;
           if(diffMilliseconds > max)
           {
               max = diffMilliseconds;
           }
           else if(diffMilliseconds < min)
           {
               min = diffMilliseconds;
           }
           
           printf("%f ms\n", diffMilliseconds);
           memset(buffer, 0, 1024);
       } 
   }
   int percent = (10 - numRecvdPackets) * 10;
   printf("--- ping statistics --- %d packets transmitted, %d received, %d%% packet loss rtt min/avg/max = %f %f %f ms", 10, numRecvdPackets, percent, min, total / numRecvdPackets, max);
   close(sock);
   return 0;
}

void error(const char *msg)
{
    perror(msg);
    exit(0);
}
