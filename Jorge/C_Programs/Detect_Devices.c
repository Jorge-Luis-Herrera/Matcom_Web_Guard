#include <stdio.h>
#include <windows.h>
#include "notify.c"

void Detect_Devices()
{
    printf("escaneando dispositivos\n"); // Siempre muestra este mensaje

    static int prev_count = 0;
    DWORD value = GetLogicalDrives();
    int count = 0;
    for (int i = 0; i < 26; i++)
    {
        if (value & (1 << i))
        {
            count++;
        }
    }
    if (count > prev_count)
    {
        printf("dispositivo conectado\n");
        notify_hola("dispositivo conectado");
    }
    else if (count < prev_count)
    {
        printf("dispositivo desconectado\n");
        notify_hola("dispositivo desconectado");
    }
    prev_count = count;
}