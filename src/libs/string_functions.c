#include "string_functions.h"

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


// int is_equal(char* s1, char* s2){
//     int count = 0;
//     while(s1[count] != '\0' || s2[count] != '\0'){
//         if(s1[count] != s2[count])return 0;
//     }
//     if(s1[count] == '\0' && s2 == '\0') return 1;
//     return 0;
// }