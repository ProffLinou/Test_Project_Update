#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#define BUFFER_SIZE 256

int cree_socket_tcp_client(int argc, char** argv)
{
	struct sockaddr_in adresse;
	int sock;
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)// On gère les cas d'erreur
	{
		fprintf(stderr, "Erreur socket\n");
		return -1;
	}
	memset(&adresse, 0, sizeof(struct sockaddr_in));
	adresse.sin_family = AF_INET;
	adresse.sin_port = htons(atoi(argv[2]));
	inet_aton(argv[1], &adresse.sin_addr);
	if (connect(sock, (struct sockaddr*) &adresse,sizeof(struct sockaddr_in)) < 0)
	{
		close(sock);
		fprintf(stderr, "Erreur connect\n");
		return -1;
	}
	return sock;
}

int main(int argc, char** argv)
{


	char bufferR[BUFFER_SIZE];
	char bufferW[BUFFER_SIZE];
	time_t	heure;
	int sock_client=cree_socket_tcp_client(argc,argv); // On crée le socket
	time(&heure);
	printf("Client connecte au serveur %s !\n",ctime(&heure));
	while(1){
		memset(&bufferW, 0 ,BUFFER_SIZE);
		memset(&bufferR, 0 , BUFFER_SIZE);
		recv(sock_client,bufferR,BUFFER_SIZE,0);
		printf("%s",bufferR);
		if(strcmp("Donner votre réponse:",bufferR)==0){
			fgets(bufferW, BUFFER_SIZE, stdin);
			printf("\n");
			time(&heure);
			printf("message envoyé %s",ctime(&heure)); 
			if(send(sock_client, bufferW, BUFFER_SIZE, 0) < 0){
				perror("Erreur d'envoie");
				return -1;
			}
		}
		memset(&bufferR, 0 , BUFFER_SIZE);
		if(strcmp("Cloture immediate %s \n  ",bufferR)==0){
			
				return -1;
			}
		

	}
	return 0;
}
