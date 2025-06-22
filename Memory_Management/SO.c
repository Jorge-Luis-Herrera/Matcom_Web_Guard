#include <windows.h>
#include <tlhelp32.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <psapi.h>

#define UMBRAL_CPU_DEF 70.0
#define UMBRAL_RAM_DEF 30.0
#define UMBRAL_TIEMPO_DEF 10
#define CONFIG_FILE "C:\\matcomguard.conf"
#define MAX_LINE_LEN 256
#define MAX_NAME_LEN 256
#define MAX_WHITELIST 20
#define INTERVALO_MONITOREO 1000  // 1 segundo

// Estructura para almacenar información de procesos
typedef struct {
    DWORD pid;
    char nombre[MAX_NAME_LEN];
    FILETIME creationTime;
    FILETIME exitTime;
    FILETIME kernelTime;
    FILETIME userTime;
    SIZE_T workingSetSize;
    time_t lastSeen;
    int alertCounter;
} ProcesoInfo;

// Estructura para configuración
typedef struct {
    double umbralCpu;
    double umbralRam;
    int umbralTiempo;
    char whitelist[MAX_WHITELIST][MAX_NAME_LEN];
    int whitelistCount;
} Config;

// Función para leer la configuración
void leerConfiguracion(Config *cfg) {
    // Valores por defecto
    cfg->umbralCpu = UMBRAL_CPU_DEF;
    cfg->umbralRam = UMBRAL_RAM_DEF;
    cfg->umbralTiempo = UMBRAL_TIEMPO_DEF;
    cfg->whitelistCount = 0;
    
    FILE *file = fopen(CONFIG_FILE, "r");
    if (!file) return;

    char line[MAX_LINE_LEN];
    while (fgets(line, sizeof(line), file)) {
        if (strstr(line, "UMBRAL_CPU=")) {
            sscanf(line, "UMBRAL_CPU=%lf", &cfg->umbralCpu);
        } else if (strstr(line, "UMBRAL_RAM=")) {
            sscanf(line, "UMBRAL_RAM=%lf", &cfg->umbralRam);
        } else if (strstr(line, "UMBRAL_TIEMPO=")) {
            sscanf(line, "UMBRAL_TIEMPO=%d", &cfg->umbralTiempo);
        } else if (strstr(line, "WHITELIST=")) {
            char *token = strtok(line + 10, ",");
            while (token && cfg->whitelistCount < MAX_WHITELIST) {
                // Eliminar espacios y saltos de línea
                char *end = token + strlen(token) - 1;
                while (end > token && (*end == '\n' || *end == ' ' || *end == '\r')) *end-- = '\0';
                strncpy(cfg->whitelist[cfg->whitelistCount], token, MAX_NAME_LEN - 1);
                cfg->whitelist[cfg->whitelistCount][MAX_NAME_LEN - 1] = '\0';
                cfg->whitelistCount++;
                token = strtok(NULL, ",");
            }
        }
    }
    fclose(file);
}

// Verificar si un proceso está en la lista blanca
int enListaBlanca(Config *cfg, const char *nombre) {
    for (int i = 0; i < cfg->whitelistCount; i++) {
        if (_stricmp(cfg->whitelist[i], nombre) == 0) {
            return 1;
        }
    }
    return 0;
}

// Convertir FILETIME a ULONGLONG
ULONGLONG fileTimeToULongLong(FILETIME ft) {
    ULARGE_INTEGER uli;
    uli.LowPart = ft.dwLowDateTime;
    uli.HighPart = ft.dwHighDateTime;
    return uli.QuadPart;
}

// Obtener información de los procesos
ProcesoInfo* obtenerProcesos(int *count) {
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        *count = 0;
        return NULL;
    }
    PROCESSENTRY32 pe;
    pe.dwSize = sizeof(PROCESSENTRY32);
    ProcesoInfo *procesos = NULL;
    int numProcesos = 0;
    if (Process32First(hSnapshot, &pe)) {
        do {
            HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pe.th32ProcessID);
            if (hProcess) {
                FILETIME creationTime, exitTime, kernelTime, userTime;
                if (GetProcessTimes(hProcess, &creationTime, &exitTime, &kernelTime, &userTime)) {
                    PROCESS_MEMORY_COUNTERS pmc;
                    if (GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc))) {
                        procesos = (ProcesoInfo*)realloc(procesos, (numProcesos + 1) * sizeof(ProcesoInfo));
                        if (procesos) {
                            ProcesoInfo *pi = &procesos[numProcesos];
                            pi->pid = pe.th32ProcessID;
                            strncpy(pi->nombre, pe.szExeFile, MAX_NAME_LEN - 1);
                            pi->nombre[MAX_NAME_LEN - 1] = '\0';
                            pi->creationTime = creationTime;
                            pi->exitTime = exitTime;
                            pi->kernelTime = kernelTime;
                            pi->userTime = userTime;
                            pi->workingSetSize = pmc.WorkingSetSize;
                            pi->lastSeen = time(NULL);
                            pi->alertCounter = 0;
                            numProcesos++;
                        }
                    }
                }
                CloseHandle(hProcess);
            }
        } while (Process32Next(hSnapshot, &pe));
    }
    CloseHandle(hSnapshot);
    *count = numProcesos;
    return procesos;
}

// Calcular uso de CPU entre dos mediciones
double calcularUsoCpu(ProcesoInfo *prev, ProcesoInfo *curr) {
    ULONGLONG prevTime = fileTimeToULongLong(prev->userTime) + fileTimeToULongLong(prev->kernelTime);
    ULONGLONG currTime = fileTimeToULongLong(curr->userTime) + fileTimeToULongLong(curr->kernelTime);
    
    double timeDiff = (double)(currTime - prevTime);
    double timeElapsed = difftime(curr->lastSeen, prev->lastSeen) * 10000000.0; // Segundos a unidades de 100ns
    
    if (timeElapsed <= 0) return 0.0;
    
    return (timeDiff / timeElapsed) * 100.0;
}

// Calcular uso de RAM porcentual
double calcularUsoRam(SIZE_T workingSetSize, SIZE_T totalRam) {
    return (workingSetSize / (double)totalRam) * 100.0;
}

// Obtener memoria total del sistema
SIZE_T obtenerMemoriaTotal() {
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);
    if (GlobalMemoryStatusEx(&memInfo)) {
        return memInfo.ullTotalPhys;
    }
    return 0;
}

int f() {
    printf("Guardian del Tesoro Real - Monitoreo Windows\n");
    Config cfg;
    leerConfiguracion(&cfg);
    printf("Configuracion cargada:\n");
    printf("Umbral CPU: %.2f%%\n", cfg.umbralCpu);
    printf("Umbral RAM: %.2f%%\n", cfg.umbralRam);
    printf("Tiempo alerta: %d seg\n", cfg.umbralTiempo);
    printf("Procesos en lista blanca: %d\n", cfg.whitelistCount);
    SIZE_T totalRam = obtenerMemoriaTotal();
    if (!totalRam) {
        fprintf(stderr, "Error: No se pudo obtener la memoria total\n");
        return 1;
    }
    ProcesoInfo *procesosPrev = NULL;
    int countPrev = 0;
    while (1) {
        int countCurr = 0;
        ProcesoInfo *procesosCurr = obtenerProcesos(&countCurr);
        if (!procesosCurr) {
            Sleep(INTERVALO_MONITOREO);
            continue;
        }
        for (int i = 0; i < countCurr; i++) {
            ProcesoInfo *curr = &procesosCurr[i];
            // Buscar en la iteración anterior
            ProcesoInfo *prev = NULL;
            for (int j = 0; j < countPrev; j++) {
                if (procesosPrev[j].pid == curr->pid) {
                    prev = &procesosPrev[j];
                    break;
                }
            }
            // Calcular métricas si tenemos datos anteriores
            double usoCpu = 0.0;
            double usoRam = calcularUsoRam(curr->workingSetSize, totalRam);
            if (prev) {
                usoCpu = calcularUsoCpu(prev, curr);
            }
            // Verificar alertas
            int alerta = 0;
            if (!enListaBlanca(&cfg, curr->nombre)) {
                if (usoCpu > cfg.umbralCpu || usoRam > cfg.umbralRam) {
                    alerta = 1;
                }
            }
            // Actualizar contador de alerta
            if (prev && alerta) {
                curr->alertCounter = prev->alertCounter + 1;
            } else if (alerta) {
                curr->alertCounter = 1;
            } else {
                curr->alertCounter = 0;
            }
            // Emitir alerta si se supera el umbral de tiempo
            if (curr->alertCounter >= cfg.umbralTiempo) {
                SYSTEMTIME st;
                GetLocalTime(&st);
                printf("\n[%04d-%02d-%02d %02d:%02d:%02d] ALERTA: Proceso sospechoso!\n", 
                       st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
                printf("PID: %u\n", curr->pid);
                printf("Nombre: %s\n", curr->nombre);
                printf("Uso CPU: %.2f%%\n", usoCpu);
                printf("Uso RAM: %.2f%%\n", usoRam);
                printf("Tiempo en alerta: %d segundos\n\n", curr->alertCounter);
            }
            // Si el proceso supera el 60% de RAM, terminarlo
            if (usoRam > 60.0 && !enListaBlanca(&cfg, curr->nombre)) {
                HANDLE hProc = OpenProcess(PROCESS_TERMINATE, FALSE, curr->pid);
                if (hProc) {
                    TerminateProcess(hProc, 1);
                    printf("[INFO] Proceso '%s' (PID: %u) terminado por exceder el 60%% de RAM.\n", curr->nombre, curr->pid);
                    CloseHandle(hProc);
                } else {
                    printf("[ERROR] No se pudo terminar el proceso '%s' (PID: %u).\n", curr->nombre, curr->pid);
                }
            }
            // Si el proceso supera 2GB de RAM, buscar por nombre y terminar todos los procesos con ese nombre
            if (curr->workingSetSize > (2ULL * 1024 * 1024 * 1024) && !enListaBlanca(&cfg, curr->nombre)) {
                printf("[INFO] Buscando y terminando procesos con nombre '%s' que superan 2GB de RAM...\n", curr->nombre);
                // Tomar snapshot de procesos
                HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
                if (hSnap != INVALID_HANDLE_VALUE) {
                    PROCESSENTRY32 pe32;
                    pe32.dwSize = sizeof(PROCESSENTRY32);
                    if (Process32First(hSnap, &pe32)) {
                        do {
                            if (_stricmp(pe32.szExeFile, curr->nombre) == 0) {
                                HANDLE hProc = OpenProcess(PROCESS_TERMINATE, FALSE, pe32.th32ProcessID);
                                if (hProc) {
                                    TerminateProcess(hProc, 1);
                                    printf("[INFO] Proceso '%s' (PID: %u) terminado por exceder 2GB de RAM.\n", pe32.szExeFile, pe32.th32ProcessID);
                                    CloseHandle(hProc);
                                } else {
                                    printf("[ERROR] No se pudo terminar el proceso '%s' (PID: %u).\n", pe32.szExeFile, pe32.th32ProcessID);
                                }
                            }
                        } while (Process32Next(hSnap, &pe32));
                    }
                    CloseHandle(hSnap);
                }
            }
        }
        // Liberar datos anteriores y actualizar
        free(procesosPrev);
        procesosPrev = procesosCurr;
        countPrev = countCurr;
        Sleep(INTERVALO_MONITOREO);
    }
    free(procesosPrev);
    return 0;
}