#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>

struct hostent *hp;
struct sockaddr_in adr_distant;

void construire_message(char *message, char motif, int lg) //construction du message
{
    int i;

    for (i=0;i<lg;i++){
         message[i] = motif;
    }

}
void afficher_message(char *message, int lg) 
{
    int i;

    printf("message construit : ");

    for (i=0;i<lg;i++){
        printf("%c", message[i]); printf("\n");
    }

}

int main (int argc, char **argv)
{
	int c;
	extern char *optarg;
	extern int optind;
	int nb_message = -1; /* Nb de messages à envoyer ou à recevoir, par défaut : 10 en émission, infini en réception */
	int source = -1 ; /* 0=puits, 1=source */
    int lg = 30;
	while ((c = getopt(argc, argv, "psn:")) != -1) {
		switch (c) {
		case 'p':
			if (source == 1) {
				printf("usage: cmd [-p|-s][-n ##]\n");
				exit(1);
			}
			source = 0;
			break;

		case 's':
			if (source == 0) {
				printf("usage: cmd [-p|-s][-n ##]\n");
				exit(1) ;
			}
			source = 1;
			break;

		case 'n':
			nb_message = atoi(optarg);
			break;

		default:
			printf("usage: cmd [-p|-s][-n ##]\n");
			break;
		}
	}

	if (source == -1) {
		printf("usage: cmd [-p|-s][-n ##]\n");
		exit(1) ;
	}

	if (source == 1){
		printf("on est dans le source\n");
		
		//creation du socket
		int addr_intern;
		addr_intern=socket(AF_INET,SOCK_STREAM,0); 

		//construction de l'adresse du socket auqel on souhaite envoyer des messages
		memset((char*)&adr_distant,0,sizeof(adr_distant));
		adr_distant.sin_family = AF_INET;
		int port;
		port = atoi(argv[argc-1]);
		port = htons(port);
		adr_distant.sin_port = port; 

		//résolution du nom d'hote
		char *post = argv[argc-2];
		if((hp=gethostbyname(post))==NULL)
		{
			printf("erreur gethostbyname\n");
			exit(1);
		}
		memcpy((char*)&(adr_distant.sin_addr.s_addr),hp->h_addr,hp->h_length);

		struct sockaddr_in * padr_dest;
		padr_dest = &(adr_distant);

		int lg_adr_dest = sizeof(adr_distant);

		char * message;
		message = malloc(lg*sizeof(char));
		construire_message(message,'j',lg);


		//phase de connexion:
		int connexion;
		connexion=connect(addr_intern,(struct sockaddr*)padr_dest,lg_adr_dest);
		if (connexion == 0 ){			
			//envoie du message
			int result;
			result = sendto(addr_intern,message,lg,0,(struct sockaddr*)padr_dest,lg_adr_dest);

			if(result == -1){
				printf("Echec de l'envoi du message");
			}
		}
		else{
			printf("connexion échouée");
		}

	}
	else
		printf("on est dans le puits\n");




	if (nb_message != -1) {
		if (source == 1)
			printf("nb de tampons à envoyer : %d\n", nb_message);
		else
			printf("nb de tampons à recevoir : %d\n", nb_message);
	} else {
		if (source == 1) {
			nb_message = 10 ;
			printf("nb de tampons à envoyer = 10 par défaut\n");
		} else
		printf("nb de tampons à envoyer = infini\n");

	}

    return 0;

}
