#include <stdio.h>
#include <windows.h>
#include "Detect_Devices.c"
#include "Detect_Changes.c"

HANDLE hMutex;
HANDLE hPipeRead, hPipeWrite;

DWORD WINAPI thread_detect_devices(LPVOID lpParam) {
    char signal = 1;
    while (1) {
        WaitForSingleObject(hMutex, INFINITE);
        Detect_Devices();
        ReleaseMutex(hMutex);

        Sleep(2000); // Espera 2 segundos

        // Señala al otro hilo que puede continuar
        DWORD written;
        WriteFile(hPipeWrite, &signal, 1, &written, NULL);

        // Espera señal para continuar
        DWORD read;
        ReadFile(hPipeRead, &signal, 1, &read, NULL);
    }
    return 0;
}

DWORD WINAPI thread_detect_changes(LPVOID lpParam) {
    char signal = 1;
    while (1) {
        // Espera señal para continuar
        DWORD read;
        ReadFile(hPipeRead, &signal, 1, &read, NULL);
        WaitForSingleObject(hMutex, INFINITE);
        Detect_Changes();
        ReleaseMutex(hMutex);

        Sleep(2000); // Espera 2 segundos

        // Señala al otro hilo que puede continuar
        DWORD written;
        WriteFile(hPipeWrite, &signal, 1, &written, NULL);
    }
    return 0;
}

DWORD WINAPI thread_scan_full(LPVOID lpParam) {
    while (1) {
        // Si existe el archivo de petición de escaneo profundo
        if (GetFileAttributesA("scan_request.txt") != INVALID_FILE_ATTRIBUTES) {
            // Pausa monitoreo
            WaitForSingleObject(hMutex, INFINITE);
            printf("Iniciando escaneo profundo...\n");
            // Llama a Scan_Pc.exe como proceso externo
            system("\"Scan_Pc.exe\"");
            printf("Escaneo profundo finalizado.\n");
            // Borra la petición
            remove("scan_request.txt");
            ReleaseMutex(hMutex);
        }
        Sleep(500); // Polling simple
    }
    return 0;
}

int main()
{
    // Limpia el log de cambios sospechosos al iniciar
    FILE* flog = fopen("cambio_sospechoso.log", "w");
    if (flog) fclose(flog);

    hMutex = CreateMutex(NULL, FALSE, NULL);

    // Usa dos pipes para comunicación bidireccional
    HANDLE pipe1Read, pipe1Write, pipe2Read, pipe2Write;
    if (!CreatePipe(&pipe1Read, &pipe1Write, NULL, 0)) {
        printf("Error creando pipe1\n");
        return 1;
    }
    if (!CreatePipe(&pipe2Read, &pipe2Write, NULL, 0)) {
        printf("Error creando pipe2\n");
        return 1;
    }

    // Hilo 1: lee de pipe1Read, escribe en pipe2Write
    // Hilo 2: lee de pipe2Read, escribe en pipe1Write
    // Asignar globales para compatibilidad con el código existente
    hPipeRead = pipe1Read;
    hPipeWrite = pipe2Write;

    // Inicializa la señal para que el primer hilo comience
    char signal = 1;
    DWORD written;
    WriteFile(hPipeWrite, &signal, 1, &written, NULL);

    HANDLE hThread1 = CreateThread(NULL, 0, thread_detect_devices, NULL, 0, NULL);
    // Para el segundo hilo, intercambia los pipes
    hPipeRead = pipe2Read;
    hPipeWrite = pipe1Write;
    HANDLE hThread2 = CreateThread(NULL, 0, thread_detect_changes, NULL, 0, NULL);

    HANDLE hThreadScan = CreateThread(NULL, 0, thread_scan_full, NULL, 0, NULL);

    // Espera a que el usuario cierre el programa manualmente (Ctrl+C)
    WaitForSingleObject(hThread1, INFINITE);
    WaitForSingleObject(hThread2, INFINITE);
    // Puedes cerrar el hilo de escaneo profundo si lo deseas:
    // WaitForSingleObject(hThreadScan, INFINITE);

    CloseHandle(hThread1);
    CloseHandle(hThread2);
    CloseHandle(hThreadScan);
    CloseHandle(hMutex);
    CloseHandle(pipe1Read);
    CloseHandle(pipe1Write);
    CloseHandle(pipe2Read);
    CloseHandle(pipe2Write);
    return 0;
}