#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h> 
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





