#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <signal.h>
#include <time.h>
#include <sys/socket.h>
#define BUFFER_SIZE 256
#define HOTE "127.0.0.1" //adresse IP
#define PORT 33016

int cree_socket_tcp_ip()
{
	int sock;
	struct sockaddr_in adresse;
	if ((sock = socket(PF_INET, SOCK_STREAM, 0)) < 0) { // IP et TCP
		fprintf(stderr, "Erreur socket.	");//On gère les cas d'erreur
		return -1;
	}
	memset(&adresse, 0, sizeof(struct sockaddr_in));
	adresse.sin_family = PF_INET; // Protocol IP
	adresse.sin_port =htons(PORT); // port 33016
	inet_aton(HOTE, &adresse.sin_addr); // IP localhost
	if (bind(sock, (struct sockaddr*) &adresse,sizeof(struct sockaddr_in)) < 0)//On vérifie si le bind c'est bien passé
	{
		close(sock);
		fprintf(stderr, "Erreur bind");
		return -1;
	}
	return sock;
}


int affiche_adresse_socket(int sock)
{
	struct sockaddr_in adresse;
	socklen_t longueur;
	longueur = sizeof(struct sockaddr_in);
	if (getsockname(sock, (struct sockaddr*)&adresse, &longueur) < 0)
	{
		fprintf(stderr, "Erreur getsockname");
		return -1;
	}
	printf("IP = %s, Port = %u\n",inet_ntoa(adresse.sin_addr),ntohs(adresse.sin_port));
	return 0;
}

char bufferERR[BUFFER_SIZE];
int flag =0;
int nb_clients=0;

void Cloture_serveur(int sig){
	time_t	heure;
	memset(&bufferERR, 0, sizeof(bufferERR));
	time(&heure);
	int i=sprintf(bufferERR, "Cloture immediate à bientot!\n %s",ctime(&heure));
	flag=1;
    printf("signal recu %d \n",sig);
    time(&heure);
	printf("Cloture\n au revoir à bientot!");
	printf(" %s",ctime(&heure));


}

void Nombre_de_client(int sig){
time_t	heure;
	printf("signal recu %d \n",sig);
	printf("nombre de clients connectes est: %d \n",nb_clients);
		time(&heure);

		printf(" %s",ctime(&heure));
}

void traite_connection(int sock1, int sock2)
{
	char bufferR[BUFFER_SIZE];
	char bufferW[BUFFER_SIZE];




	time_t	heure;
	memset(&bufferR, 0, BUFFER_SIZE);
	memset(&bufferW, 0, BUFFER_SIZE);

	int i=sprintf(bufferW, "Donner votre réponse:");
	if(send(sock1,bufferW,BUFFER_SIZE,0)<0){
		perror("erreur d'envoie");//
		exit(1);
	}
	if(recv(sock1,bufferR,BUFFER_SIZE,0)<0){
		perror("erreur de reception");
		exit(1);
	}

	memset(&bufferW, 0, BUFFER_SIZE);
	time(&heure);

	sprintf(bufferW, "Le client distant dit: %s \n message reçu  %s", bufferR,ctime(&heure));
	memset(&bufferR, 0, BUFFER_SIZE);
	printf("\n");
	time(&heure);
	printf("message ecrit %s",ctime(&heure));
	if(send(sock2, bufferW, BUFFER_SIZE, 0) < 0){
		perror("erreur d'envoie");
		exit(1);
	}

if(sock1){
	printf("Connexion : locale (socks1_connectee) \n");//affiche_adresse_socket(sock1)
	}
	if(sock2){
	printf("Connexion : locale (socks2_connectee) \n");//affiche_adresse_socket(sock2)
    }
}



int main(void)
{
	int sock_contact;
	int sock_connectee1;
	int sock_connectee2;
	time_t	heure;
	struct sockaddr_in adresse1;
	struct sockaddr_in adresse2;



	pid_t pid_fils;
	if ( (sock_contact = cree_socket_tcp_ip()) < 0)
		return -1;
	listen(sock_contact, 2);
	socklen_t lg = sizeof(struct sockaddr_in);
	while (1)
	{

		sock_connectee1 = accept(sock_contact,(struct sockaddr*)&adresse1,&lg);
		sock_connectee2 = accept(sock_contact,(struct sockaddr*)&adresse2,&lg);
		if (sock_connectee1 < 0 )
		{
			fprintf(stderr, "Erreur accept\n");
			return -1;
		}else{
			nb_clients++;
		}
		if (sock_connectee2 < 0 )
		{
			fprintf(stderr, "Erreur accept\n");
			return -1;
		}else{
			nb_clients++;
		}
		if ( (pid_fils = fork())==-1)
		{
			fprintf(stderr, "Erreur fork\n");
			return -1;
		}
		if (pid_fils == 0) {// processus fils  se charge de traiter la communication entre les clients
			close(sock_contact);
			time(&heure);
			printf("Bonjour il est %s",ctime(&heure));
			printf("je suis pid numero %d \n",getpid() );
			printf("Serveur ok\n");
			signal(SIGUSR1,Cloture_serveur);
			signal(SIGUSR2,Nombre_de_client);

			while(1){
				//chat
				if(flag==0)//pas de sigusr1
				traite_connection(sock_connectee1,sock_connectee2);
				if(flag==0)//pas de sigusr1
				traite_connection(sock_connectee2,sock_connectee1);


					if (flag==1)//sigusr1
					{
						
						if(send(sock_connectee1,bufferERR,strlen(bufferERR),0)<0){
							perror("erreur d'envoie urgence");
							exit(1);
						}
						if(send(sock_connectee2,bufferERR,strlen(bufferERR),0)<0){
							perror("erreur d'envoie urgence");
							exit(1);
						}
						exit(1);
					}

			}
		}else {// processus pere close le socket 
			if(flag==1)
				return -1;

		close(sock_connectee1);
		close(sock_connectee2);

}

	}
	return 0;
}
