#include "SimpleSocket.hpp"
#include "RawSocket.hpp"

//const unsigned char SERVER_MESSAGE[] = "Server~Open~1~";
//const uint8_t FIRST_SPECIAL_BYTE = 4;
//const uint8_t SECOND_SPECIAL_BYTE = 2;
//const uint16_t DESTINATION_PORT = 47777;
//const uint16_t SOURCE_PORT = 56947;
const uint16_t MAXLINE = 1500;

void signalCatch(int socket);

int main(int argc, char** argv) {
    // Ensure proper usage
	__int8_t sourceIpNo = 2;
    __int8_t destIpNo = 4;
	__int8_t listeningPortNo = 6;
	__int8_t destPortNo = 8;
    switch (argc){
        case 1:{
            printf("Usage: -s SpoofedSourceIp -d DestinationIp -l ListeningPort -dp destinationPort\n");
            return 0;
        }

        case 9:{
            __int8_t verif = 0;
            for (__int8_t i=1; i<argc; i++){
                if (strcmp("-s", argv[i]) == 0){
                    sourceIpNo = i+1;
                    verif += 3;
                }
                if (strcmp("-d", argv[i]) == 0){
                    destIpNo = i+1;
                    verif += 5;
                }
				if (strcmp("-l" ,argv[i]) == 0){
					listeningPortNo = i+1;
					verif += 11;
				}
				if (strcmp("-dp" ,argv[i]) == 0){
					destPortNo = i + 1;
					verif += 45;
				}
            }

            if ((sourceIpNo % 2 == 0) && (destIpNo % 2 == 0) && (listeningPortNo % 2 == 0) && (destPortNo % 2 == 0) && verif == 64)
                break;
        }

        default:{
            printf("Incorrect usage\n");
            return 0;
        }

    }


    signal(SIGINT, signalCatch);

	RawSocket rawSocket;
	rawSocket.setDestination(argv[destIpNo], atoi(argv[destPortNo]));

	SimpleSocket simpleSocket;
	simpleSocket.bindSocket("0.0.0.0", atoi(argv[listeningPortNo]));


	printf("Starting spoofing the packets...\n");
	for(;;) {
		printf("Receiving packet\n");
		if (simpleSocket.socketReceive() == 0)
			printf("My simple socket received data successfuly\n");

		int port;
		char clientAddress[100];

		simpleSocket.getClientAddress(clientAddress, &port);
		rawSocket.setSource(argv[sourceIpNo], port);
		
		int hereLen = 0;
		char hereData[1500];

		simpleSocket.getClientMessage(hereData, &hereLen);
		rawSocket.setData(hereData, hereLen);

		printf("Sending packet\n");
		rawSocket.sendDatagram();
	}
  
  return 0;
}

void signalCatch(int socket) {
    printf("\nClosing sockets.\n");
    close(socket);
    exit(0);
}
