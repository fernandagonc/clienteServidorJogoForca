#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "common.h"

void usage(char **argv) {
	printf("Uso: %s <IP-servidor> <PORT-servidor>\n", argv[0]);
	printf("Exemplo de uso: %s 127.0.0.1 51511\n", argv[0]);
	exit(EXIT_FAILURE);
}

#define BUFSZ 1024
#define palpiteSZ 100

int parseAddr(const char *addrstr, const char *portstr, struct sockaddr_storage *storage);
void acertosNasPosicoes(char *respostaServidor);


int main(int argc, char *argv[]){

    if(argc != 3){
		usage(argv);
    } 

	struct sockaddr_storage storage;
	if (0 != parseAddr(argv[1], argv[2], &storage)) {
		usage(argv);
	} 

	int sockfd;
	sockfd = socket(storage.ss_family, SOCK_STREAM, 0);
    
    if((sockfd) < 0){
        printf("\n Erro : Não foi possível criar o socket \n");
        exit(1);
    } 
	
	struct sockaddr *addr = (struct sockaddr *)(&storage);

    if(0 != connect(sockfd, addr, sizeof(storage))){
       printf("\n Erro : Falha na conexão\n");
       exit(1);
    } 

	char servidorBuffer[BUFSZ];
	char palpite[2];
	size_t count;
	memset(palpite, 0, palpiteSZ+1);
	memset(servidorBuffer, 0, BUFSZ);
	palpite[0] = '2';

	while(1){
       	
        count = recv(sockfd, servidorBuffer, BUFSZ, 0);
		if(charToInt(servidorBuffer[0]) == 1){
			printf("A palavra a ser adivinhada tem %s letras\n", &servidorBuffer[1]);
		}
		else if(charToInt(servidorBuffer[0]) == 3){
			acertosNasPosicoes(servidorBuffer);
		}
		
		if(charToInt(servidorBuffer[0]) == 4){
			printf("Você acertou todas as letras da palavra!\n");
			break;
		}
		printf("Digite um palpite> ");
		char letra;
		scanf(" %c", &letra);
		palpite[1] = letra;
		count = send(sockfd, palpite, strlen(palpite), 0);

		if (count != strlen(palpite)) {
			printf("Erro no envio do palpite\n");
			close(sockfd);
			exit(1);
		}
		else{
			memset(palpite, 0, palpiteSZ+1);
			memset(servidorBuffer, 0, BUFSZ);
			palpite[0] = '2';
		}		
	}

	close(sockfd);

    return 0;
};

int parseAddr(const char *addrstr, const char *portstr, struct sockaddr_storage *storage) {
    if (addrstr == NULL || portstr == NULL) {
        return -1;
    }

    uint16_t port = (uint16_t)atoi(portstr); 
    if (port == 0) {
        return -1;
    }
    port = htons(port); 

    struct in_addr inaddr4; // 32-bit IP address
    if (inet_pton(AF_INET, addrstr, &inaddr4)) {
        struct sockaddr_in *addr4 = (struct sockaddr_in *)storage;
        addr4->sin_family = AF_INET;
        addr4->sin_port = port;
        addr4->sin_addr = inaddr4;
        return 0;
    }

    struct in6_addr inaddr6; // 128-bit IPv6 address
    if (inet_pton(AF_INET6, addrstr, &inaddr6)) {
        struct sockaddr_in6 *addr6 = (struct sockaddr_in6 *)storage;
        addr6->sin6_family = AF_INET6;
        addr6->sin6_port = port;
        memcpy(&(addr6->sin6_addr), &inaddr6, sizeof(inaddr6));
        return 0;
    }

    return -1;
};

void acertosNasPosicoes(char *respostaServidor){
	char* p;
	p = &respostaServidor[1];
	printf("Essa letra aparece %d vez(es) \n", charToInt(*p));
	
	if(charToInt(*p) == 0){
		printf("\n");
		return;
	}

	printf("Posição(ões) na palavra: ");
    for (p = &respostaServidor[2]; *p!= '\0'; p++){
        printf("%d, ", charToInt(*p));
    }
	printf("\n \n");
	return;

};