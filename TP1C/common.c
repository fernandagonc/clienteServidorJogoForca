#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <arpa/inet.h>

char intToChar (int value){
    if(value < 10) 
        return '0' + value;
    else 
        return '0';
    
};

int charToInt (char value){
    return value - '0';
};

int addrparse(const char *addrstr, const char *portstr, struct sockaddr_storage *storage) {
    if (addrstr == NULL || portstr == NULL) {
        return -1;
    }

    uint16_t port = (uint16_t)atoi(portstr); 
    if (port == 0) {
        return -1;
    }
    port = htons(port); // host to network short

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
}


int server_sockaddr_init(const char *proto, const char *portstr, struct sockaddr_storage *storage) {
   
    uint16_t port = (uint16_t)atoi(portstr); // unsigned short
    if (port == 0) {
        return -1;
    }
    port = htons(port); // host to network short

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
            posicao[0] = intToChar(i);
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
