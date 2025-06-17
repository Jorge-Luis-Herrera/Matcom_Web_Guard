#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<windows.h>

int main(){
    // Compilar code.c
    system("gcc -fdiagnostics-color=always -g C:\\Users\\Jorge\\Desktop\\Fabian\\code.c -o C:\\Users\\Jorge\\Desktop\\Fabian\\code.exe -lws2_32 -lssl -lcrypto -lpsapi -liphlpapi");
    // Ejecutar code.exe
    system("C:\\Users\\Jorge\\Desktop\\Fabian\\code.exe");
}