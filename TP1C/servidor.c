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

int inicializarSocketAddr(const char *proto, const char *portstr, struct sockaddr_storage *storage);
int palpiteRepetido(char *historicoPalpites, char *palpite);
char *checarPalpite(char *historicoPalpites, char *palpite, char *palavra);
int getNumeroAcertos(char *resposta);

void usage(char **argv) {
	printf("Uso: %s <v4|v6> <PORTA-servidor>\n", argv[0]);
	printf("Exemplo de uso: %s v4 51511\n", argv[0]);
	exit(EXIT_FAILURE);
}


int main(int argc, char *argv[]){

	if (argc != 3) {
        usage(argv);
    }

	struct sockaddr_storage storage;
    if (0 != inicializarSocketAddr(argv[1], argv[2], &storage)) {
        usage(argv);
    }

	int sockfd;
    sockfd = socket(storage.ss_family, SOCK_STREAM, 0);
    
	if((sockfd) < 0){
        printf("\n Erro : Não foi possível criar o socket \n");
        exit(1);
    } 
    
	int enable = 1;
    if (0 != setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int))) {
        printf("Erro na função setsockopt");
        exit(1);
    }

    struct sockaddr *addr = (struct sockaddr *)(&storage);
    if (0 != bind(sockfd, addr, sizeof(storage))) {
        printf("Erro no bind");
        exit(1);
    }
    if (0 != listen(sockfd, 10)) {
        printf("Erro no listen");
        exit(1);
    }

    char *palavra = "servidor";
    char inicioJogo[2];
    size_t controleFimJogo;;    
    char buf[BUFSZ];
    char *historicoPalpites;
    historicoPalpites = malloc(BUFSZ);
    char *resposta;
    size_t count;

    
    while (1){
        inicioJogo[0] = '1';
        inicioJogo[1] = '0';
        controleFimJogo = strlen(palavra);
        inicioJogo[1] = intToChar(controleFimJogo);
        inicioJogo[2] = '\0';
        resposta = malloc(controleFimJogo+2);   
           
        memset(historicoPalpites, 0, BUFSZ);
        memset(buf, 0, BUFSZ);

        int clientfd;
        struct sockaddr_storage client_storage;
        struct sockaddr *client_addr = (struct sockaddr *)(&client_storage);
        socklen_t client_addrlen = sizeof(client_storage);

        clientfd = accept(sockfd, client_addr, &client_addrlen);
        if (clientfd == -1) {
            printf("Erro ao tentar conectar-se ao servidor\n");
            exit(1);
        };
        
        count = send(clientfd, inicioJogo, strlen(inicioJogo), 0) ;
        if(count != strlen(inicioJogo)){
            printf("Erro no envio da mensagem\n");
            exit(1);
        }

    
        while(controleFimJogo != 0){
            count = recv(clientfd, buf, BUFSZ, 0);
            
            resposta = checarPalpite(historicoPalpites, buf, palavra);
            strcat(historicoPalpites, &buf[1]);

            int acertos = getNumeroAcertos(resposta);
            controleFimJogo = controleFimJogo - acertos;

            if(controleFimJogo == 0){//controle da vitória
                send(clientfd, "4", strlen("4"), 0);
                close(clientfd);
                break;
            }
            else{
                count = send(clientfd, resposta, strlen(resposta), 0) ;
                if(count != strlen(resposta)){
                    printf("Erro no envio da mensagem\n");
                    exit(1);
                }

            };
        };
       


    }
    
    close(sockfd);
    free(resposta);
    free(historicoPalpites);
    return 1;
}


int inicializarSocketAddr(const char *proto, const char *portstr, struct sockaddr_storage *storage) {
   
    uint16_t port = (uint16_t)atoi(portstr); 
    if (port == 0) {
        return -1;
    }
    port = htons(port); 

    memset(storage, 0, sizeof(*storage));
    if (0 == strcmp(proto, "v4")) {
        struct sockaddr_in *addr4 = (struct sockaddr_in *)storage;
        addr4->sin_family = AF_INET;
        addr4->sin_addr.s_addr = INADDR_ANY;
        addr4->sin_port = port;
        return 0;
    } else if (0 == strcmp(proto, "v6")) {
        struct sockaddr_in6 *addr6 = (struct sockaddr_in6 *)storage;
        addr6->sin6_family = AF_INET6;
        addr6->sin6_addr = in6addr_any;
        addr6->sin6_port = port;
        return 0;
    } else {
        return -1;
    }
};


int palpiteRepetido(char *historicoPalpites, char *palpite){
    char *m;

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
        free(posicoesCorretas);
        free(posicao);
        return resposta;
    };

    int i = 0;
    char* m;
    for (m = palavra; *m!= '\0'; m++){

        if (tolower(*m) == tolower(palpite[1])){            
            letrasCorretas++;
            posicao[0] = intToChar(i+1);
            strcat(posicoesCorretas, posicao); 

        }
        i++;
    }

    resposta[1] = intToChar(letrasCorretas);
    strcat(resposta, posicoesCorretas); 
    
    free(posicoesCorretas);
    free(posicao);

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

