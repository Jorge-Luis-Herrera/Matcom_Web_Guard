#include <windows.h>
#include <stdio.h>

// Devuelve el espacio libre en bytes de C:
ULONGLONG Scan()
{
    ULARGE_INTEGER freeBytesAvailable, totalBytes, totalFreeBytes;
    if (GetDiskFreeSpaceExA("C:\\", &freeBytesAvailable, &totalBytes, &totalFreeBytes))
    {
        return freeBytesAvailable.QuadPart;
    }
    return 0;
}

void Detect_Changes()
{
    static ULONGLONG last_free = 0;
    ULONGLONG current_free = Scan();
    if (last_free == 0)
    {
        last_free = current_free;
        return;
    }
    ULONGLONG threshold = 0; //Diferencia mínima para considerar un cambio sospechoso
    LONGLONG diff = (LONGLONG)current_free - (LONGLONG)last_free;
    if (llabs(diff) > (LONGLONG)threshold)
    {
        printf("cambio sospechoso\n");
        FILE* f = fopen("cambio_sospechoso.log", "a");
        if (f) {
            fprintf(f, "cambio sospechoso (prueba)\n");
            fclose(f);
        }
        last_free = current_free;
    }
}