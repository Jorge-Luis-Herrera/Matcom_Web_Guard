#include <stdio.h>
#include <windows.h>
#include <string.h>
#include <time.h>

ULONGLONG Scan_Files_Directory()
{
    ULARGE_INTEGER freeBytesAvailable, totalBytes, totalFreeBytes;
    if (GetDiskFreeSpaceExA("C:\\", &freeBytesAvailable, &totalBytes, &totalFreeBytes))
    {
        return freeBytesAvailable.QuadPart;
    }
    return 0;
}

static HWND g_hwndEdit = NULL;
static HWND g_hwndMain = NULL;

void notify(const char *message)
{
    MessageBoxA(NULL, message, "Matcom Guard", MB_OK | MB_ICONINFORMATION);
}

void Detect_Devices()
{
    printf("escaneando dispositivos\n");

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
        notify("dispositivo conectado");
    }
    else if (count < prev_count)
    {
        printf("dispositivo desconectado\n");
        notify("dispositivo desconectado");
    }
    prev_count = count;
}
