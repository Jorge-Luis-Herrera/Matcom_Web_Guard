// Este es el main principal del sistema (main.c), aquí se gestiona la lógica de hilos y el escaneo bajo demanda.
// Para compilar y enlazar correctamente los módulos, asegúrate de incluir Scan_Pc.c y compilar Scan_Pc.c con -lole32 si usas funciones COM.
//gcc main.c -o main.exe -lole32
#include <stdio.h>
#include <windows.h>
#include <time.h>
#include "Detect_Devices.c"
#include "Detect_Changes.c"

HANDLE hMutex;

// Hilo para detectar dispositivos
DWORD WINAPI ThreadDetectDevices(LPVOID lpParam)
{
    while (1)
    {
        WaitForSingleObject(hMutex, INFINITE);
        Detect_Devices();
        ReleaseMutex(hMutex);
        Sleep(2000);
    }
    return 0;
}

// Hilo para detectar cambios
DWORD WINAPI ThreadDetectChanges(LPVOID lpParam)
{
    // Espera 1 segundo antes de la primera ejecución para alternar
    Sleep(1000);
    while (1)
    {
        WaitForSingleObject(hMutex, INFINITE);
        Detect_Changes();
        ReleaseMutex(hMutex);
        Sleep(2000);
    }
    return 0;
}

int main()
{
    hMutex = CreateMutex(NULL, FALSE, NULL);

    HANDLE hThread1 = CreateThread(NULL, 0, ThreadDetectDevices, NULL, 0, NULL);
    HANDLE hThread2 = CreateThread(NULL, 0, ThreadDetectChanges, NULL, 0, NULL);


    WaitForSingleObject(hThread1, INFINITE);
    WaitForSingleObject(hThread2, INFINITE);

    CloseHandle(hMutex);
    return 0;
}