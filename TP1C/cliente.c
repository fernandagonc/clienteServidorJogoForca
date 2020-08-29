#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "common.h"
//converter padrão de rede - função host to network / network to host
void usage(char **argv) {
	printf("Uso: %s <IP-servidor> <PORT-servidor>\n", argv[0]);
	printf("Exemplo de uso: %s 127.0.0.1 51511\n", argv[0]);
	exit(EXIT_FAILURE);
}

#define BUFSZ 1024
#define palpiteSZ 100

int main(int argc, char *argv[]){

    if(argc != 3){
		usage(argv);
    } 

	struct sockaddr_storage storage;
	if (0 != addrparse(argv[1], argv[2], &storage)) {
		usage(argv);
	} 

	int sockfd;
	sockfd = socket(storage.ss_family, SOCK_STREAM, 0);
    
    if((sockfd) < 0){
        printf("\n Erro : Não foi possível criar o socket \n");
        return 1;
    } 
	
	struct sockaddr *addr = (struct sockaddr *)(&storage);

    if(0 != connect(sockfd, addr, sizeof(storage))){
       printf("\n Erro : Falha na conexão\n");
       return 1;
    } 

	char servidorBuffer[BUFSZ];
	char palpiteBuffer[palpiteSZ];
	char palpite[palpiteSZ+1] ;
	size_t count;
	memset(palpite, 0, palpiteSZ+1);
	memset(servidorBuffer, 0, BUFSZ);
	memset(palpiteBuffer, 0, palpiteSZ);
	palpite[0] = '2';

	while(1){
       	
        count = recv(sockfd, servidorBuffer, BUFSZ, 0);
		puts(servidorBuffer);

		if(charToInt(servidorBuffer[0]) == 4){
			break;
		}
		printf("Palpite> ");
		fgets(palpiteBuffer, palpiteSZ-1, stdin);
		strcat(palpite, palpiteBuffer);
		count = send(sockfd, palpite, strlen(palpite), 0);

		if (count != strlen(palpite)) {
			printf("Erro no envio do palpite\n");
			close(sockfd);
			exit(1);
		}
		else{
			memset(palpite, 0, palpiteSZ+1);
			memset(servidorBuffer, 0, BUFSZ);
			memset(palpiteBuffer, 0, palpiteSZ);
			palpite[0] = '2';
		}		
	}

	close(sockfd);

    return 0;
}