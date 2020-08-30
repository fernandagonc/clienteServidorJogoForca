#pragma once

#include <stdlib.h>

#include <arpa/inet.h>


char intToChar(int value);
 
int charToInt(char value);

int addrparse(const char *addrstr, const char *portstr, struct sockaddr_storage *storage);

int server_sockaddr_init(const char *proto, const char *portstr, struct sockaddr_storage *storage);

int palpiteRepetido(char *historicoPalpites, char *palpite);

char *checarPalpite(char *historicoPalpites, char *palpite, char *palavra);

int getNumeroAcertos(char *resposta);