#pragma once

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>

FILE *open_filename(std::string filename)
{
   FILE *new_handle = NULL;
   char *filename_c = const_cast<char *>(filename.c_str());

   if (filename_c)
   {
      bool bNetwork = false;
      int sfd = -1, socktype;

      if(!strncmp("tcp://", filename_c, 6))
      {
         bNetwork = true;
         socktype = SOCK_STREAM;
      }
      else if(!strncmp("udp://", filename_c, 6))
      {
         bNetwork = true;
         socktype = SOCK_DGRAM;
      }

      if(bNetwork)
      {
         unsigned short port;
         filename_c += 6;
         char *colon;
         if(NULL == (colon = strchr(filename_c, ':')))
         {
            fprintf(stderr, "%s is not a valid IPv4:port, use something like tcp://1.2.3.4:1234 or udp://1.2.3.4:1234\n",
                    filename_c);
            exit(132);
         }
         if(1 != sscanf(colon + 1, "%hu", &port))
         {
            fprintf(stderr,
                    "Port parse failed. %s is not a valid network file name, use something like tcp://1.2.3.4:1234 or udp://1.2.3.4:1234\n",
                    filename_c);
            exit(133);
         }
         char chTmp = *colon;
         *colon = 0;

         struct sockaddr_in saddr;
         saddr.sin_family = AF_INET;
         saddr.sin_port = htons(port);
         if(0 == inet_aton(filename_c, &saddr.sin_addr))
         {
            fprintf(stderr, "inet_aton failed. %s is not a valid IPv4 address\n",
                    filename_c);
            exit(134);
         }
         *colon = chTmp;

         if(0 <= (sfd = socket(AF_INET, socktype | SOCK_NONBLOCK, 0)))
         {
           fprintf(stderr, "Connecting to %s:%hu...", inet_ntoa(saddr.sin_addr), port);

           int iTmp = 1;
           while ((-1 == (iTmp = connect(sfd, (struct sockaddr *) &saddr, sizeof(struct sockaddr_in)))) && (EINTR == errno))
             ;
           if (iTmp < 0)
             fprintf(stderr, "error: %s\n", strerror(errno));
           else
             fprintf(stderr, "connected, sending video...\n");
         }
         else
           fprintf(stderr, "Error creating socket: %s\n", strerror(errno));

         if (sfd >= 0)
            new_handle = fdopen(sfd, "w");
      }
      else
      {
         new_handle = fopen(filename_c, "wb");
      }
   }

   return new_handle;
}
