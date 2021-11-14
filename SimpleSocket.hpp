#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <arpa/inet.h>


class SimpleSocket{
    private:
        static const uint16_t MAXLINE = 1280;

        __int32_t simpleSocket;
        __int32_t broadcastOption = 0;

        struct sockaddr_in servaddr;    

        struct {


        }toSend;

        struct{
            struct sockaddr_in cliaddr;
            uint32_t cliaddrLen = 0;
            char data[MAXLINE];
            __int32_t dataLen = 0;

        }received;

    public:
        SimpleSocket(){
            simpleSocket = 
            socket(AF_INET, SOCK_DGRAM, 0);
	        if (simpleSocket == -1){
		        perror("ERROR: failed to create simple socket\n");
		        exit(-1);
        	}

            //Making sure that the structs that will hold our socket details are clear
            memset(&servaddr, 0, sizeof(servaddr));
            memset(&received.cliaddr, 0, sizeof(received.cliaddr));

            //Using IPv4 with the binding address structue
            servaddr.sin_family    = AF_INET; // IPv4

            //Calculating length of the structure that will receive the information
            received.cliaddrLen = sizeof(received.cliaddr);
        }

        
        void setBroadcastOption(__int32_t option){
            broadcastOption = option;
            if (setsockopt(simpleSocket, SOL_SOCKET, SO_BROADCAST, &broadcastOption, sizeof(broadcastOption)) == -1){
                perror("ERROR: failed set the broadcast option to raw socket\n");
            }
        }

        __int32_t getBroadcastOption(){
            return broadcastOption;
        }

        void bindSocket(char *address, __int32_t port){
            if (strcmp(address, "ANY") == 0 || strcmp(address, "0.0.0.0") == 0)
                servaddr.sin_addr.s_addr = INADDR_ANY;
            else
                servaddr.sin_addr.s_addr = inet_addr(address); // ??????? not sure
            servaddr.sin_port = htons(port);

            if ( bind(simpleSocket, (const struct sockaddr *)&servaddr, sizeof(servaddr)) == -1)
            {
                perror("Bind the simple socket failed\n");
                exit(-1);
            }
        }

        int socketReceive(){
            received.dataLen = recvfrom(simpleSocket, (char *)received.data, MAXLINE, MSG_WAITALL, ( struct sockaddr *) &received.cliaddr, &received.cliaddrLen);

            if (received.dataLen == -1)
                return -1;
            
            return 0;
        }

        void getClientAddress(char *clientAddress, __int32_t *clientPort){
            *clientPort = ntohs(received.cliaddr.sin_port);

            strcpy(clientAddress, inet_ntoa(received.cliaddr.sin_addr));
        }

        void getClientMessage(char *data, int *dataLength){
            *dataLength = received.dataLen;
            
            memcpy(data, received.data, received.dataLen);
        }



};
