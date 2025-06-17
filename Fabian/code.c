#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <windows.h>
#include <tlhelp32.h>
#include <iphlpapi.h>
#include <psapi.h>
#include <openssl/sha.h>
#include <time.h>

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "psapi.lib")
#pragma comment(lib, "crypt32.lib")

#define MAX_PATH 260
#define MAX_FILES 1000
#define MAX_PROCESSES 500
#define PORT_RANGE_START 1
#define PORT_RANGE_END 1024
#define DEFAULT_CPU_THRESHOLD 70
#define DEFAULT_RAM_THRESHOLD 50
#define DEFAULT_CHANGE_THRESHOLD 10

// Estructuras de datos
typedef struct {
    char path[MAX_PATH];
    unsigned char hash[SHA256_DIGEST_LENGTH];
    FILETIME creation_time;
    FILETIME last_access;
    FILETIME last_write;
    DWORD attributes;
    DWORD size;
} FileEntry;

typedef struct {
    DWORD pid;
    char name[MAX_PATH];
    float cpu_usage;
    SIZE_T memory_usage;
    time_t last_seen;
} ProcessInfo;

typedef struct {
    int port;
    char service[50];
    char status[20];
} PortInfo;

// Configuración global
typedef struct {
    float cpu_threshold;
    float ram_threshold;
    float change_threshold;
} Config;

// Variables globales
FileEntry baseline[MAX_FILES];
int baseline_count = 0;
ProcessInfo processes[MAX_PROCESSES];
int process_count = 0;
Config config = {DEFAULT_CPU_THRESHOLD, DEFAULT_RAM_THRESHOLD, DEFAULT_CHANGE_THRESHOLD};

// Funciones principales
void load_config();
void usb_scan();
void process_monitor();
void port_scan();
void generate_report(const char* type);
void print_banner();

// Funciones auxiliares
void calculate_hash(const char* path, unsigned char* output);
int is_suspicious_change(const char* path, FileEntry* new_entry);
void scan_directory(const char* base_path);
void check_resource_usage();
void scan_ports(int start, int end);
const char* get_service_name(int port);
int is_whitelisted(const char* process_name);
void detect_usb_drives();
void WINAPI usb_change_handler(PVOID lpParam, BOOLEAN TimerOrWaitFired);

int main() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        fprintf(stderr, "Error inicializando Winsock\n");
        return 1;
    }

    load_config();
    print_banner();

    int choice;
    do {
        printf("\n=== MatComGuard - Menu Principal ===\n");
        printf("1. Escanear sistema de archivos (USB)\n");
        printf("2. Escanear procesos (Memoria/CPU)\n");
        printf("3. Escanear puertos\n");
        printf("4. Escanear todo\n");
        printf("5. Salir\n");
        printf("Seleccione una opcion: ");
        scanf("%d", &choice);

        switch(choice) {
            case 1:
                usb_scan();
                generate_report("filesystem");
                break;
            case 2:
                process_monitor();
                generate_report("process");
                break;
            case 3:
                port_scan();
                generate_report("ports");
                break;
            case 4:
                usb_scan();
                process_monitor();
                port_scan();
                generate_report("full");
                break;
            case 5:
                printf("Saliendo del sistema MatComGuard...\n");
                break;
            default:
                printf("Opcion invalida. Intente nuevamente.\n");
        }
    } while(choice != 5);

    WSACleanup();
    return 0;
}

void load_config() {
    printf("Cargando configuracion...\n");
    // En una implementación real, leeríamos de un archivo de configuración
    config.cpu_threshold = DEFAULT_CPU_THRESHOLD;
    config.ram_threshold = DEFAULT_RAM_THRESHOLD;
    config.change_threshold = DEFAULT_CHANGE_THRESHOLD;
}

void usb_scan() {
    printf("\n=== Iniciando escaneo de dispositivos USB ===\n");
    detect_usb_drives();
}

void detect_usb_drives() {
    char drive_letter[4] = "A:\\";
    DWORD drives = GetLogicalDrives();

    for (char i = 'A'; i <= 'Z'; i++) {
        drive_letter[0] = i;
        UINT drive_type = GetDriveTypeA(drive_letter);

        if (drive_type == DRIVE_REMOVABLE) {
            printf("Dispositivo USB detectado: %s\n", drive_letter);
            scan_directory(drive_letter);
        }
    }
}

void scan_directory(const char* base_path) {
    WIN32_FIND_DATAA find_data;
    char search_path[MAX_PATH];
    snprintf(search_path, MAX_PATH, "%s\\*", base_path);

    HANDLE hFind = FindFirstFileA(search_path, &find_data);
    if (hFind == INVALID_HANDLE_VALUE) return;

    do {
        if (strcmp(find_data.cFileName, ".") == 0 || strcmp(find_data.cFileName, "..") == 0)
            continue;

        char full_path[MAX_PATH];
        snprintf(full_path, MAX_PATH, "%s\\%s", base_path, find_data.cFileName);

        if (find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            scan_directory(full_path);
        } else {
            FileEntry new_entry;
            strcpy_s(new_entry.path, MAX_PATH, full_path);
            new_entry.creation_time = find_data.ftCreationTime;
            new_entry.last_access = find_data.ftLastAccessTime;
            new_entry.last_write = find_data.ftLastWriteTime;
            new_entry.attributes = find_data.dwFileAttributes;
            new_entry.size = find_data.nFileSizeLow;

            calculate_hash(full_path, new_entry.hash);

            if (is_suspicious_change(full_path, &new_entry)) {
                printf("[ALERTA] Cambio sospechoso detectado: %s\n", full_path);
            }

            // Actualizar baseline
            if (baseline_count < MAX_FILES) {
                baseline[baseline_count] = new_entry;
                baseline_count++;
            }
        }
    } while (FindNextFileA(hFind, &find_data));

    FindClose(hFind);
}

void calculate_hash(const char* path, unsigned char* output) {
    FILE* file = fopen(path, "rb");
    if (!file) return;

    SHA256_CTX sha256;
    SHA256_Init(&sha256);

    unsigned char buffer[1024];
    size_t bytes;
    while ((bytes = fread(buffer, 1, sizeof(buffer), file))) {
        SHA256_Update(&sha256, buffer, bytes);
    }
    
    SHA256_Final(output, &sha256);
    fclose(file);
}

int is_suspicious_change(const char* path, FileEntry* new_entry) {
    // Buscar en el baseline
    for (int i = 0; i < baseline_count; i++) {
        if (strcmp(baseline[i].path, path) == 0) {
            // 1. Cambio de tamaño inusual
            long size_diff = labs((long)new_entry->size - (long)baseline[i].size);
            if (size_diff > (baseline[i].size * 10)) {
                return 1;
            }
            
            // 2. Cambio de atributos
            if (new_entry->attributes != baseline[i].attributes) {
                return 1;
            }
            
            // 3. Cambio de hash (contenido)
            if (memcmp(new_entry->hash, baseline[i].hash, SHA256_DIGEST_LENGTH) != 0) {
                return 1;
            }
            
            // 4. Cambio de timestamp
            if (CompareFileTime(&new_entry->last_write, &baseline[i].last_write) != 0) {
                return 1;
            }
            
            break;
        }
    }
    return 0;
}

void process_monitor() {
    printf("\n=== Monitoreo de procesos ===\n");
    
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        printf("Error al crear snapshot de procesos\n");
        return;
    }

    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);

    if (!Process32First(hSnapshot, &pe32)) {
        CloseHandle(hSnapshot);
        return;
    }

    process_count = 0;
    do {
        HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pe32.th32ProcessID);
        if (hProcess) {
            PROCESS_MEMORY_COUNTERS pmc;
            if (GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc))) {
                FILETIME createTime, exitTime, kernelTime, userTime;
                if (GetProcessTimes(hProcess, &createTime, &exitTime, &kernelTime, &userTime)) {
                    ULARGE_INTEGER cpu_time;
                    cpu_time.LowPart = userTime.dwLowDateTime;
                    cpu_time.HighPart = userTime.dwHighDateTime;
                    
                    float cpu_usage = (float)(cpu_time.QuadPart / 10000.0); // Simplificado
                    
                    processes[process_count].pid = pe32.th32ProcessID;
                    strcpy_s(processes[process_count].name, MAX_PATH, pe32.szExeFile);
                    processes[process_count].cpu_usage = cpu_usage;
                    processes[process_count].memory_usage = pmc.WorkingSetSize / (1024 * 1024); // MB
                    processes[process_count].last_seen = time(NULL);
                    process_count++;
                }
            }
            CloseHandle(hProcess);
        }
    } while (Process32Next(hSnapshot, &pe32) && process_count < MAX_PROCESSES);

    CloseHandle(hSnapshot);
    check_resource_usage();
}

void check_resource_usage() {
    for (int i = 0; i < process_count; i++) {
        if (is_whitelisted(processes[i].name)) continue;
        
        if (processes[i].cpu_usage > config.cpu_threshold) {
            printf("[ALERTA] Proceso '%s' (PID: %lu) usa %.2f%% CPU (Umbral: %.2f%%)\n",
                   processes[i].name, processes[i].pid,
                   processes[i].cpu_usage, config.cpu_threshold);
        }
        
        if (processes[i].memory_usage > config.ram_threshold) {
            printf("[ALERTA] Proceso '%s' (PID: %lu) usa %lu MB RAM (Umbral: %.2f MB)\n",
                   processes[i].name, processes[i].pid,
                   processes[i].memory_usage, config.ram_threshold);
        }
    }
}

int is_whitelisted(const char* process_name) {
    const char* whitelist[] = {"explorer.exe", "svchost.exe", "winlogon.exe", "csrss.exe", "System"};
    int count = sizeof(whitelist) / sizeof(whitelist[0]);
    
    for (int i = 0; i < count; i++) {
        if (strstr(process_name, whitelist[i])) {
            return 1;
        }
    }
    return 0;
}

void port_scan() {
    printf("\n=== Escaneo de puertos ===\n");
    
    SOCKET sock;
    struct sockaddr_in sa;
    sa.sin_family = AF_INET;
    sa.sin_addr.s_addr = inet_addr("127.0.0.1");
    
    printf("Escaneando puertos locales (%d-%d)...\n", PORT_RANGE_START, PORT_RANGE_END);
    for (int port = PORT_RANGE_START; port <= PORT_RANGE_END; port++) {
        sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (sock == INVALID_SOCKET) continue;
        
        sa.sin_port = htons(port);
        
        // Configurar timeout
        DWORD timeout = 100; // 100ms
        setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));
        setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (char*)&timeout, sizeof(timeout));
        
        if (connect(sock, (struct sockaddr*)&sa, sizeof(sa)) == 0) {
            printf("[ABIERTO] Puerto %d: %s\n", port, get_service_name(port));
        }
        closesocket(sock);
    }
}

const char* get_service_name(int port) {
    switch(port) {
        case 21: return "FTP";
        case 22: return "SSH";
        case 23: return "Telnet";
        case 25: return "SMTP";
        case 53: return "DNS";
        case 80: return "HTTP";
        case 110: return "POP3";
        case 135: return "RPC";
        case 139: return "NetBIOS";
        case 443: return "HTTPS";
        case 445: return "SMB";
        case 3389: return "RDP";
        default:
            if (port > 1024) return "Servicio no estandar";
            return "Desconocido";
    }
}

void generate_report(const char* type) {
    printf("\n=== Generando reporte (%s) ===\n", type);
    
    time_t now = time(NULL);
    printf("Fecha del reporte: %s", ctime(&now));
    
    if(strcmp(type, "filesystem") == 0 || strcmp(type, "full") == 0) {
        printf("\n-- Dispositivos USB --\n");
        printf("Archivos en baseline: %d\n", baseline_count);
    }
    
    if(strcmp(type, "process") == 0 || strcmp(type, "full") == 0) {
        printf("\n-- Procesos monitorizados --\n");
        printf("Procesos detectados: %d\n", process_count);
        for(int i = 0; i < process_count && i < 5; i++) {
            printf("PID: %lu, Nombre: %s, CPU: %.2f%%, RAM: %luMB\n",
                   processes[i].pid, processes[i].name,
                   processes[i].cpu_usage, processes[i].memory_usage);
        }
    }
    
    if(strcmp(type, "ports") == 0 || strcmp(type, "full") == 0) {
        printf("\n-- Puertos escaneados --\n");
        printf("Puertos del %d al %d\n", PORT_RANGE_START, PORT_RANGE_END);
    }
    
    printf("\nReporte generado con exito.\n");
}

void print_banner() {
    printf("\n");
    printf("==============================================\n");
    printf("    MatCom Guard - Sistema de Proteccion\n");
    printf("    Version 1.0 - Protegiendo tu reino digital\n");
    printf("==============================================\n");
    printf("Umbrales configurados:\n");
    printf("  CPU: %.2f%%, RAM: %.2fMB, Cambios: %.2f%%\n", 
           config.cpu_threshold, config.ram_threshold, config.change_threshold);
    printf("==============================================\n");
}