#include "aux_functions.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* converter_inteiro_string(int num){
    char *string = (char*)malloc(sizeof(char) * 10);
    int i;
    for(i = 0;num > 0; i++){
        int mod = num % 10;
        string[i] = mod + '0';
        num /= 10;
    }
    string[i] = '\0';
    int string_length = strlen(string);
    for(i = 0; i < string_length/2; i++){
        int aux = string[i];
        string[i] = string[string_length - i - 1];
        string[string_length - i - 1] = aux;
    }
    return string;
}

// abre o arquivo em modo leitura, caso não for possivel a leitura o programa será fechado.
FILE* open_file_read_mode(char path[]){
    FILE* fp = fopen(path, "r");
    if(fp == NULL){
        fprintf(stderr,"Erro ao ler o arquivo %s\n", path);
        exit(EXIT_FAILURE);
    }
    return fp;
}

// void verificar_alocacao(void* alloc, char* var_name){
//     if(alloc == NULL){
//         if(!var_name){
//             perror("Não foi possivel alocar memória para uma variável\n");
//         }else{
//             fprintf(stderr,"Não foi possivel alocar memória para %s\n",var_name);
//         }
//         exit(EXIT_FAILURE);
//     }
// }


// int is_equal(char* s1, char* s2){
//     int count = 0;
//     while(s1[count] != '\0' || s2[count] != '\0'){
//         if(s1[count] != s2[count])return 0;
//     }
//     if(s1[count] == '\0' && s2 == '\0') return 1;
//     return 0;
// }