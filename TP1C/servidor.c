#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "common.h"
#include <ctype.h> 

#define BUFSZ 1024


void usage(char **argv) {
	printf("Usage: %s <v4|v6> <PORTA-servidor>\n", argv[0]);
	printf("Example: %s v4 51511\n", argv[0]);
	exit(EXIT_FAILURE);
}



int palpiteRepetido(char *historicoPalpites, char *palpite){
    char *m;
    printf("historico: %s\n", historicoPalpites);

    for (m = historicoPalpites; *m!= '\0'; m++){
        if (tolower(*m) == tolower(palpite[0])){            
            return 1;            
        }
    }
    return 0;
};

char * checarPalpite(char *historicoPalpites, char *palpite, char *palavra){

    int tamanhoPalavra = strlen(palavra);
    char *resposta;
    resposta = malloc(tamanhoPalavra+2);
    memset(resposta, 0, tamanhoPalavra+2);

    char *posicao;
    posicao = malloc(1);
    memset(posicao, 0, 4);

    int letrasCorretas = 0;
    char *posicoesCorretas;
    posicoesCorretas = malloc(tamanhoPalavra);
    memset(posicoesCorretas, 0, tamanhoPalavra);

    resposta[0] = '3';
    resposta[1] = intToChar(letrasCorretas);

    if(palpiteRepetido(historicoPalpites,  &palpite[1])){
        return resposta;
    };

    int i = 0;
    char* m;
    for (m = palavra; *m!= '\0'; m++){

        if (tolower(*m) == tolower(palpite[1])){            
            letrasCorretas++;
            posicao[0] = intToChar(i);
            strcat(posicoesCorretas, posicao); 

        }
        i++;
    }

    resposta[1] = intToChar(letrasCorretas);
    strcat(resposta, posicoesCorretas); 

    return resposta;
};

int getNumeroAcertos(char *resposta){
    char* p;
    for (p = resposta; *p!= '\0'; p++){
        if (*p == resposta[1]){
            return charToInt(*p);  
        }
    }
    return 0;
};

int main(int argc, char *argv[]){

	if (argc < 3) {
        usage(argv);
    }

	struct sockaddr_storage storage;
    if (0 != server_sockaddr_init(argv[1], argv[2], &storage)) {
        usage(argv);
    }

	int sockfd;
    sockfd = socket(storage.ss_family, SOCK_STREAM, 0);
    
	if((sockfd) < 0){
        printf("\n Erro : Não foi possível criar o socket \n");
        return 1;
    } 
    
	int enable = 1;
    if (0 != setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int))) {
        printf("setsockopt");
    }

    struct sockaddr *addr = (struct sockaddr *)(&storage);
    if (0 != bind(sockfd, addr, sizeof(storage))) {
        printf("bind");
    }
    if (0 != listen(sockfd, 10)) {
        printf("listen");
    }

    size_t count;
    int clientfd;
    char inicioJogo[2];
    char *palavra = "servidor";
    inicioJogo[0] = '1';
    inicioJogo[1] = '0';
    size_t controleFimJogo = strlen(palavra);
    inicioJogo[1] = intToChar(controleFimJogo);
    inicioJogo[2] = '\0';
    printf("controle: %zu, %s \n", controleFimJogo, palavra);
    printf("inicioJogo: %s \n", inicioJogo);

    char *resposta;
    resposta = malloc(controleFimJogo+2);
	unsigned total = 0;

    struct sockaddr_storage client_storage;
    struct sockaddr *client_addr = (struct sockaddr *)(&client_storage);
    socklen_t client_addrlen = sizeof(client_storage);
    
    clientfd = accept(sockfd, client_addr, &client_addrlen);
    if (clientfd != -1) {
        printf("connected to %d \n", clientfd);
    };
    
    count = send(clientfd, inicioJogo, strlen(inicioJogo), 0) ;
    if(count != strlen(inicioJogo)){
        printf("Failure Sending Message\n");
        exit(1);
    }
    else{
        printf("Message sent with %zu bytes\n", count);
    };

    char buf[BUFSZ];
    memset(buf, 0, BUFSZ);
    char *historicoPalpites;
    historicoPalpites = malloc(BUFSZ);
    memset(historicoPalpites, 0, BUFSZ);


    while(1){
    
       	while(1){
            count = recv(clientfd, buf + total, BUFSZ - total, 0);
		    printf("msg cliente %zu bytes: %s\n", count, buf); 
 
            // if (count == 0) {
                resposta = checarPalpite(historicoPalpites, buf, palavra);
                strcat(historicoPalpites, &buf[1]);
                puts(resposta);
                int acertos = getNumeroAcertos(resposta);
                controleFimJogo = controleFimJogo - acertos;
                printf("acertos: %d, controle %zu\n", acertos, controleFimJogo); 

                if(controleFimJogo == 0){
                    send(clientfd, "4", strlen("4"), 0);
                    puts("you won!!!");
                    close(sockfd);
                    close(clientfd);
                    return 1;
                }else{
                    count = send(clientfd, resposta, strlen(resposta), 0) ;
                    if(count != strlen(resposta)){
                        printf("Failure Sending Message\n");
                        exit(1);
                    }
                    else{
                        printf("message sent with %zu bytes, %s\n", count, resposta);
                    };

                };
                
                total = 0;
            // }
            // total += count;

        }       
        //close(clientfd);
    }
    
    puts("closing socket");
    close(sockfd);

}