#include <windows.h>
#include <string.h>
#include <stdio.h>

static HWND g_hwndEdit = NULL;
static HWND g_hwndMain = NULL;

void notify_hola(const char* message) {
    MessageBoxA(NULL, message, "Matcom Guard", MB_OK | MB_ICONINFORMATION);
}