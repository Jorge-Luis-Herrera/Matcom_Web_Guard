#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <string.h>

#define MAX_FILES 50000

typedef struct {
    char name[256];
    char path[512];
    FILETIME lastModified;
    int isFile;
} File;

File files[MAX_FILES];
int totalFiles = 0;

void detectDrives()
{
    DWORD drives = GetLogicalDrives();
    char driveLetter[26] = {0};
    char driveletter2[26] = {0};

    for (int i = 0; i < 26; i++)
    {
        if (drives & (1 << i))
        {
            driveLetter[i] = 'A' + i;
            printf("%c:\\ detectado.\n", driveLetter[i]);
        }
    }    while (1)
    {
        drives = GetLogicalDrives();
        
        // Resetear el array temporal
        for (int i = 0; i < 26; i++)
        {
            driveletter2[i] = 0;
        }
        
        // Llenar el array temporal con las unidades actuales
        for (int i = 0; i < 26; i++)
        {
            if (drives & (1 << i))
            {
                driveletter2[i] = 'A' + i;
            }
        }

        // Comparar y detectar cambios
        for (int i = 0; i < 26; i++)
        {
            if (driveLetter[i] != 0 && driveletter2[i] == 0)
            {
                printf("Unidad desconectada: %c:\\\n", driveLetter[i]);
                driveLetter[i] = 0;
            }
            else if (driveLetter[i] == 0 && driveletter2[i] != 0)
            {
                printf("Nueva unidad conectada: %c:\\\n", driveletter2[i]);
                driveLetter[i] = driveletter2[i];
            }
        }

        Sleep(2000);
    }
}

void scanPath(const char* path) {
    WIN32_FIND_DATA data;
    char searchPattern[1024], fullPath[1024];
    
    sprintf(searchPattern, "%s\\*", path);
    HANDLE h = FindFirstFile(searchPattern, &data);
    
    if (h == INVALID_HANDLE_VALUE) return;
    
    do {
        if (strcmp(data.cFileName, ".") == 0 || strcmp(data.cFileName, "..") == 0) continue;
        if (totalFiles >= MAX_FILES) break;
        
        sprintf(fullPath, "%s\\%s", path, data.cFileName);
        
        strcpy(files[totalFiles].name, data.cFileName);
        strcpy(files[totalFiles].path, fullPath);
        files[totalFiles].lastModified = data.ftLastWriteTime;
        files[totalFiles].isFile = !(data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);
        totalFiles++;
        
        if (!(data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0) {
            scanPath(fullPath);
        }
        
    } while (FindNextFile(h, &data));
    
    FindClose(h);
}

File* getFiles(const char* path, int* count) {
    totalFiles = 0;
    scanPath(path);
    *count = totalFiles;
    return files;
}

int compareTime(FILETIME f1, FILETIME f2) {
    return (f1.dwLowDateTime == f2.dwLowDateTime && f1.dwHighDateTime == f2.dwHighDateTime);
}

int findFile(File* array, int count, const char* path) {
    for (int i = 0; i < count; i++) {
        if (strcmp(array[i].path, path) == 0) {
            return i;
        }
    }
    return -1;
}

void monitorUSB(const char* device) {
    printf("Iniciando monitoreo: %s\n", device);
    
    File* prevState = (File*)malloc(MAX_FILES * sizeof(File));
    File* currState = (File*)malloc(MAX_FILES * sizeof(File));
    
    if (!prevState || !currState) {
        printf("Error de memoria\n");
        return;
    }
    
    totalFiles = 0;
    scanPath(device);
    int prevCount = totalFiles;
    
    for (int i = 0; i < prevCount; i++) {
        prevState[i] = files[i];
    }
    
    printf("Archivos iniciales: %d\n", prevCount);
    
    while (1) {
        totalFiles = 0;
        scanPath(device);
        int currCount = totalFiles;
        
        for (int i = 0; i < currCount; i++) {
            currState[i] = files[i];
        }
        
        // Archivos eliminados
        for (int i = 0; i < prevCount; i++) {
            if (findFile(currState, currCount, prevState[i].path) == -1) {
                printf("ELIMINADO: %s\n", prevState[i].path);
            }
        }
        
        // Archivos nuevos
        for (int i = 0; i < currCount; i++) {
            if (findFile(prevState, prevCount, currState[i].path) == -1) {
                printf("NUEVO: %s\n", currState[i].path);
            }
        }
        
        // Archivos modificados
        for (int i = 0; i < currCount; i++) {
            int prevIndex = findFile(prevState, prevCount, currState[i].path);
            if (prevIndex != -1) {
                if (currState[i].isFile && 
                    !compareTime(currState[i].lastModified, prevState[prevIndex].lastModified)) {
                    printf("MODIFICADO: %s\n", currState[i].path);
                }
            }
        }
        
        prevCount = currCount;
        for (int i = 0; i < currCount; i++) {
            prevState[i] = currState[i];
        }
        
        Sleep(3000);
    }
    
    free(prevState);
    free(currState);
}

int j()
{
    printf("Monitor de dispositivos USB\n");
    
    DWORD drives = GetLogicalDrives();
    char driveLetter[26] = {0};
    char driveletter2[26] = {0};

    for (int i = 0; i < 26; i++)
    {
        if (drives & (1 << i))
        {
            driveLetter[i] = 'A' + i;
            printf("Unidad %c: detectada\n", driveLetter[i]);
        }
    }
    
    printf("Esperando cambios...\n");
    
    while (1)
    {
        drives = GetLogicalDrives();
        
        for (int i = 0; i < 26; i++)
        {
            driveletter2[i] = 0;
        }
        
        for (int i = 0; i < 26; i++)
        {
            if (drives & (1 << i))
            {
                driveletter2[i] = 'A' + i;
            }
        }

        for (int i = 0; i < 26; i++)
        {
            if (driveLetter[i] != 0 && driveletter2[i] == 0)
            {
                printf("Dispositivo %c: desconectado\n", driveLetter[i]);
                driveLetter[i] = 0;
            }
            else if (driveLetter[i] == 0 && driveletter2[i] != 0)
            {
                printf("Dispositivo %c: conectado\n", driveletter2[i]);
                driveLetter[i] = driveletter2[i];
                  char devicePath[4];
                sprintf(devicePath, "%c:", driveletter2[i]);
                
                monitorUSB(devicePath);
            }
        }

        Sleep(2000);
    }
    
    return 0;
}
