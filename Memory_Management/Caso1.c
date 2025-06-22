#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

int f()
{
    // Simulación de virus: consumo de CPU y RAM
    size_t tam = 10 * 1024 * 1024; // 10 MB
    void *mem = NULL;
    while (1)
    {
        // Consumir memoria poco a poco
        mem = realloc(mem, tam);
        if (mem) {
            memset(mem, rand() % 256, tam);
            tam += 10 * 1024 * 1024; // Aumenta 10 MB cada ciclo
        }
        // Consumir CPU
        for (volatile int i = 0; i < 10000000; i++);
        Sleep(100); // Pequeña pausa para no colgar el sistema
    }
    free(mem);
    return 0;
}