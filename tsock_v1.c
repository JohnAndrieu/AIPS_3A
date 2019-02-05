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
struct sockaddr_in adr_local;

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
		addr_intern=socket(AF_INET,SOCK_DGRAM,0); 

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

		//envoie du message
		int result;
		result = sendto(addr_intern,message,lg,0,(struct sockaddr*)padr_dest,lg_adr_dest);

		if(result == -1){
			printf("Echec de l'envoi du message");
		}

	}
	else{

		printf("on est dans le puits\n");

		/*affectation domaine, n°port et adresse ip*/

		memset((char *)&adr_local,0,sizeof(adr_local)); //reset struct

		adr_local.sin_family = AF_INET; //domaine internet
		int port;
		port = atoi(argv[argc-1]);
		port = htons(port);
		adr_local.sin_port = port;
		adr_local.sin_addr.s_addr = INADDR_ANY;

		int sock;
		int lg_adr_local = sizeof(adr_local);

		/*creation du socket*/

		if((sock = socket(AF_INET,SOCK_DGRAM,0)) == -1){
			printf("echec de la creation du socket\n");
			exit(1);
		}

		/*association @ socket <-> représentation interne*/
		if(bind(sock,(struct sockaddr *)&adr_local,lg_adr_local) == -1){
			printf("echec du bind\n");
			exit(1);
		}

		/*reception des messages*/
		

	}

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
