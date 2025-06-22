#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <time.h>

#include "../Jorge/C_Programs/Port_Scanner.c"
#include "../Fabian/SO.c"
#include "../Leo/main.c"

volatile BOOL continuar_ejecucion = TRUE;
HANDLE mutex_consola;

DWORD WINAPI thread_jorge(LPVOID lpParam) {
    while (continuar_ejecucion) {
        WaitForSingleObject(mutex_consola, INFINITE);
        
        SYSTEMTIME st;
        GetLocalTime(&st);
        printf("\n[%02d:%02d:%02d] Jorge - Monitor USB\n", 
               st.wHour, st.wMinute, st.wSecond);
        
        ReleaseMutex(mutex_consola);
        j();
        
        WaitForSingleObject(mutex_consola, INFINITE);
        printf("Jorge - Completado\n");
        ReleaseMutex(mutex_consola);
        
        Sleep(2000);
    }
    return 0;
}

DWORD WINAPI thread_fabian(LPVOID lpParam) {
    while (continuar_ejecucion) {
        WaitForSingleObject(mutex_consola, INFINITE);
        
        SYSTEMTIME st;
        GetLocalTime(&st);
        printf("\n[%02d:%02d:%02d] Fabian - Monitor Procesos\n", 
               st.wHour, st.wMinute, st.wSecond);
        
        ReleaseMutex(mutex_consola);
        f();
        WaitForSingleObject(mutex_consola, INFINITE);
        printf("Fabian - Completado\n");
        ReleaseMutex(mutex_consola);
        
        Sleep(2000);
    }
    return 0;
}

DWORD WINAPI thread_leo(LPVOID lpParam) {
    while (continuar_ejecucion) {
        WaitForSingleObject(mutex_consola, INFINITE);
        
        SYSTEMTIME st;
        GetLocalTime(&st);
        printf("\n[%02d:%02d:%02d] Leo - Scanner Puertos\n", 
               st.wHour, st.wMinute, st.wSecond);
        
        ReleaseMutex(mutex_consola);
        l(1, 1024);
        
        WaitForSingleObject(mutex_consola, INFINITE);
        printf("Leo - Completado\n");
        ReleaseMutex(mutex_consola);
        
        Sleep(2000);
    }
    return 0;
}

int main()
{
    printf("Sistema de Monitoreo Paralelo\n");
    printf("Jorge: Monitor USB | Fabian: Monitor Procesos | Leo: Scanner Puertos\n\n");

    mutex_consola = CreateMutex(NULL, FALSE, NULL);
    if (mutex_consola == NULL) {
        printf("Error creando mutex\n");
        return 1;
    }

    HANDLE hilo_jorge, hilo_fabian, hilo_leo;
    DWORD id_jorge, id_fabian, id_leo;

    hilo_jorge = CreateThread(NULL, 0, thread_jorge, NULL, 0, &id_jorge);
    if (hilo_jorge == NULL) {
        printf("Error creando hilo Jorge\n");
        CloseHandle(mutex_consola);
        return 1;
    }

    hilo_fabian = CreateThread(NULL, 0, thread_fabian, NULL, 0, &id_fabian);
    if (hilo_fabian == NULL) {
        printf("Error creando hilo Fabian\n");
        CloseHandle(hilo_jorge);
        CloseHandle(mutex_consola);
        return 1;
    }

    hilo_leo = CreateThread(NULL, 0, thread_leo, NULL, 0, &id_leo);
    if (hilo_leo == NULL) {
        printf("Error creando hilo Leo\n");
        CloseHandle(hilo_jorge);
        CloseHandle(hilo_fabian);
        CloseHandle(mutex_consola);
        return 1;
    }

    printf("Hilos activos - Jorge: %lu | Fabian: %lu | Leo: %lu\n", id_jorge, id_fabian, id_leo);
    printf("Presiona Ctrl+C para salir\n\n");
    
    HANDLE hilos[3] = {hilo_jorge, hilo_fabian, hilo_leo};
    WaitForMultipleObjects(3, hilos, TRUE, INFINITE);

    CloseHandle(hilo_jorge);
    CloseHandle(hilo_fabian);
    CloseHandle(hilo_leo);
    CloseHandle(mutex_consola);
    return 0;
}