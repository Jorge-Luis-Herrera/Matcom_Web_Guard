#include <winsock2.h>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdarg.h>

#define IDC_BUTTON_USB 1001
#define IDC_BUTTON_MEMORY 1002  
#define IDC_BUTTON_PORTS 1003
#define IDC_BUTTON_ALL 1004
#define IDC_OUTPUT 1005
#define IDC_CLEAR 1006
#define IDC_PORT_START 1007
#define IDC_PORT_END 1008
#define IDC_PORT_START_LABEL 1009
#define IDC_PORT_END_LABEL 1010

HWND hOutput;
HWND hMainWindow;
HWND hPortStart;
HWND hPortEnd;
volatile BOOL scanning = FALSE;
HANDLE hScanThread = NULL;
DWORD currentScanThreadId = 0;

// Declaraciones de funciones
void AppendToOutput(const char* text);
void AddTimestamp();
void EnableButtons(BOOL enable);
void StopCurrentScan();
void StartNewScan(LPTHREAD_START_ROUTINE scanFunction, const char* scanName);
DWORD WINAPI ScanThreadUSB(LPVOID lpParam);
DWORD WINAPI ScanThreadMemory(LPVOID lpParam);
DWORD WINAPI ScanThreadPorts(LPVOID lpParam);
DWORD WINAPI ScanThreadAll(LPVOID lpParam);

// Función personalizada para capturar printf
int custom_printf(const char* format, ...) {
    va_list args;
    va_start(args, format);
    
    char temp_buffer[1024];
    int result = vsnprintf(temp_buffer, sizeof(temp_buffer), format, args);
    
    // Enviar a la interfaz gráfica
    if (hOutput) {
        AppendToOutput(temp_buffer);
    }
    
    va_end(args);
    return result;
}

// Redefinir printf ANTES de incluir los archivos de Jorge, Fabian y Leo
#define printf custom_printf

#include "../Jorge/C_Programs/Port_Scanner.c"
#include "../Fabian/SO.c"
#include "../Leo/main.c"

// Restaurar printf original después de los includes
#undef printf

typedef struct {
    int scanType;
} ScanData;

void AppendToOutput(const char* text) {
    if (!hOutput) return;
    
    int len = GetWindowTextLength(hOutput);
    SendMessage(hOutput, EM_SETSEL, len, len);
    SendMessage(hOutput, EM_REPLACESEL, FALSE, (LPARAM)text);
    SendMessage(hOutput, EM_SETSEL, len + strlen(text), len + strlen(text));
    SendMessage(hOutput, EM_SCROLLCARET, 0, 0);
}

void AddTimestamp() {
    SYSTEMTIME st;
    GetLocalTime(&st);
    char timestamp[100];
    sprintf(timestamp, "\r\n[%02d:%02d:%02d] ", st.wHour, st.wMinute, st.wSecond);
    AppendToOutput(timestamp);
}

DWORD WINAPI ScanThreadUSB(LPVOID lpParam) {
    scanning = TRUE;
    AddTimestamp();
    AppendToOutput("Starting USB file system scan...\r\n");
    
    j(); // Jorge's function for USB scanning
    
    AddTimestamp();
    AppendToOutput("USB scan completed.\r\n");
    scanning = FALSE;
    return 0;
}

DWORD WINAPI ScanThreadMemory(LPVOID lpParam) {
    scanning = TRUE;
    AddTimestamp();
    AppendToOutput("Starting memory and process scan...\r\n");
    
    f(); // Fabian's function for process monitoring
    
    AddTimestamp();
    AppendToOutput("Memory scan completed.\r\n");
    scanning = FALSE;
    return 0;
}

DWORD WINAPI ScanThreadPorts(LPVOID lpParam) {
    scanning = TRUE;
    AddTimestamp();
    AppendToOutput("Starting port scan...\r\n");
    
    // Get port range from input fields
    char startPortText[10] = "1";
    char endPortText[10] = "1024";
    
    if (hPortStart && hPortEnd) {
        GetWindowText(hPortStart, startPortText, sizeof(startPortText));
        GetWindowText(hPortEnd, endPortText, sizeof(endPortText));
    }
    
    int start_port = atoi(startPortText);
    int end_port = atoi(endPortText);
    
    // Validate port range
    if (start_port < 1 || start_port > 65535) start_port = 1;
    if (end_port < 1 || end_port > 65535) end_port = 1024;
    if (start_port > end_port) {
        int temp = start_port;
        start_port = end_port;
        end_port = temp;
    }
    
    char rangeMessage[100];
    sprintf(rangeMessage, "Scanning ports %d to %d with thread pooling...\r\n", start_port, end_port);
    AppendToOutput(rangeMessage);
    
    // Call Leo's function with improved thread management
    l(start_port, end_port);
    
    AddTimestamp();
    AppendToOutput("Port scan completed.\r\n");
    scanning = FALSE;
    return 0;
}

DWORD WINAPI ScanThreadAll(LPVOID lpParam) {
    scanning = TRUE;
    AddTimestamp();
    AppendToOutput("Starting parallel system scan...\r\n");
    AppendToOutput("All three scans will run simultaneously.\r\n\r\n");
    
    // Get port range from input fields for Leo's scanner
    char startPortText[10] = "1";
    char endPortText[10] = "1024";
    
    if (hPortStart && hPortEnd) {
        GetWindowText(hPortStart, startPortText, sizeof(startPortText));
        GetWindowText(hPortEnd, endPortText, sizeof(endPortText));
    }
    
    int start_port = atoi(startPortText);
    int end_port = atoi(endPortText);
    
    // Validate port range
    if (start_port < 1 || start_port > 65535) start_port = 1;
    if (end_port < 1 || end_port > 65535) end_port = 1024;
    if (start_port > end_port) {
        int temp = start_port;
        start_port = end_port;
        end_port = temp;
    }
    
    char rangeMessage[100];
    sprintf(rangeMessage, "Port scan will cover range %d to %d\r\n\r\n", start_port, end_port);
    AppendToOutput(rangeMessage);
    
    // Create three threads for parallel execution
    HANDLE hThreadJorge, hThreadFabian, hThreadLeo;
    DWORD idJorge, idFabian, idLeo;
    
    AppendToOutput("=== STARTING PARALLEL SCANS ===\r\n");
    
    // Create Jorge's thread (USB/File system monitoring)
    hThreadJorge = CreateThread(NULL, 0, ScanThreadUSB, NULL, 0, &idJorge);
    AppendToOutput("Jorge thread started - USB/File monitoring\r\n");
    
    // Create Fabian's thread (Memory/Process monitoring) 
    hThreadFabian = CreateThread(NULL, 0, ScanThreadMemory, NULL, 0, &idFabian);
    AppendToOutput("Fabian thread started - Memory/Process monitoring\r\n");
    
    // Create Leo's thread (Port scanning)
    hThreadLeo = CreateThread(NULL, 0, ScanThreadPorts, NULL, 0, &idLeo);
    AppendToOutput("Leo thread started - Port scanning\r\n\r\n");
    
    // Wait for all threads to complete
    HANDLE allThreads[3] = {hThreadJorge, hThreadFabian, hThreadLeo};
    WaitForMultipleObjects(3, allThreads, TRUE, INFINITE);
    
    // Clean up thread handles
    CloseHandle(hThreadJorge);
    CloseHandle(hThreadFabian); 
    CloseHandle(hThreadLeo);
    
    AddTimestamp();
    AppendToOutput("All parallel scans completed successfully.\r\n");
    scanning = FALSE;
    return 0;
}

void StopCurrentScan() {
    if (hScanThread && scanning) {
        AddTimestamp();
        AppendToOutput("Stopping previous scan...\r\n");
        
        TerminateThread(hScanThread, 0);
        CloseHandle(hScanThread);
        hScanThread = NULL;
        currentScanThreadId = 0;
        scanning = FALSE;
        
        Sleep(100); // Brief pause for stabilization
    }
}

void StartNewScan(LPTHREAD_START_ROUTINE scanFunction, const char* scanName) {
    StopCurrentScan();
    
    scanning = TRUE;
    // DO NOT disable buttons - keep them active to allow changes
    
    AddTimestamp();
    char message[200];
    sprintf(message, "Starting %s...\r\n", scanName);
    AppendToOutput(message);
    
    hScanThread = CreateThread(NULL, 0, scanFunction, NULL, 0, &currentScanThreadId);
    if (hScanThread) {
        SetTimer(hMainWindow, 1, 500, NULL); // Check every 500ms
    } else {
        AddTimestamp();
        AppendToOutput("Error: Could not create scan thread.\r\n");
        scanning = FALSE;
    }
}

void EnableButtons(BOOL enable) {
    // Function maintained for compatibility, but buttons remain always active
    // to allow switching between scans
    EnableWindow(GetDlgItem(hMainWindow, IDC_BUTTON_USB), TRUE);
    EnableWindow(GetDlgItem(hMainWindow, IDC_BUTTON_MEMORY), TRUE);
    EnableWindow(GetDlgItem(hMainWindow, IDC_BUTTON_PORTS), TRUE);
    EnableWindow(GetDlgItem(hMainWindow, IDC_BUTTON_ALL), TRUE);
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {        case WM_CREATE: {            hMainWindow = hwnd;
            // Create buttons
            CreateWindow("BUTTON", "Scan File System",
                WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                20, 20, 180, 40, hwnd, (HMENU)IDC_BUTTON_USB, NULL, NULL);
                
            CreateWindow("BUTTON", "Scan Memory",
                WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                220, 20, 180, 40, hwnd, (HMENU)IDC_BUTTON_MEMORY, NULL, NULL);
                
            CreateWindow("BUTTON", "Scan Ports",
                WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                420, 20, 180, 40, hwnd, (HMENU)IDC_BUTTON_PORTS, NULL, NULL);
                
            CreateWindow("BUTTON", "Scan All",
                WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                620, 20, 160, 40, hwnd, (HMENU)IDC_BUTTON_ALL, NULL, NULL);
            
            // Create port range labels and input fields
            CreateWindow("STATIC", "Start Port:",
                WS_VISIBLE | WS_CHILD,
                20, 80, 80, 20, hwnd, (HMENU)IDC_PORT_START_LABEL, NULL, NULL);
                
            hPortStart = CreateWindow("EDIT", "1",
                WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER,
                100, 77, 60, 25, hwnd, (HMENU)IDC_PORT_START, NULL, NULL);
                
            CreateWindow("STATIC", "End Port:",
                WS_VISIBLE | WS_CHILD,
                180, 80, 80, 20, hwnd, (HMENU)IDC_PORT_END_LABEL, NULL, NULL);
                
            hPortEnd = CreateWindow("EDIT", "1024",
                WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER,
                250, 77, 60, 25, hwnd, (HMENU)IDC_PORT_END, NULL, NULL);
                
            CreateWindow("BUTTON", "Clear",
                WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                700, 500, 100, 30, hwnd, (HMENU)IDC_CLEAR, NULL, NULL);
            
            // Create text area for output
            hOutput = CreateWindow("EDIT", "",
                WS_VISIBLE | WS_CHILD | WS_BORDER | WS_VSCROLL | 
                ES_MULTILINE | ES_READONLY | ES_AUTOVSCROLL,
                20, 120, 780, 370, hwnd, (HMENU)IDC_OUTPUT, NULL, NULL);
                
            // Configure monospace font
            HFONT hFont = CreateFont(14, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                ANSI_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS,
                DEFAULT_QUALITY, FF_DONTCARE | FIXED_PITCH, "Consolas");
            SendMessage(hOutput, WM_SETFONT, (WPARAM)hFont, TRUE);
            
            AppendToOutput("=== SECURITY MONITORING SYSTEM ===\r\n");
            AppendToOutput("Configure port range (default: 1-1024) and select scan type.\r\n");
            AppendToOutput("Results will appear in this window.\r\n\r\n");
            
            break;
        }
          case WM_COMMAND: {
            switch (LOWORD(wParam)) {                case IDC_BUTTON_USB:
                    StartNewScan(ScanThreadUSB, "file system scan");
                    break;
                    
                case IDC_BUTTON_MEMORY:
                    StartNewScan(ScanThreadMemory, "memory scan");
                    break;
                    
                case IDC_BUTTON_PORTS:
                    StartNewScan(ScanThreadPorts, "port scan");
                    break;
                    
                case IDC_BUTTON_ALL:
                    StartNewScan(ScanThreadAll, "parallel system scan");
                    break;
                      case IDC_CLEAR:
                    SetWindowText(hOutput, "");
                    AppendToOutput("=== SECURITY MONITORING SYSTEM ===\r\n");
                    AppendToOutput("Screen cleared. Ready for new scan.\r\n\r\n");
                    break;
            }
            break;
        }        case WM_TIMER: {
            if (!scanning) {
                KillTimer(hwnd, 1);
                // NO habilitar/deshabilitar botones - mantenerlos siempre activos
                if (hScanThread) {
                    CloseHandle(hScanThread);
                    hScanThread = NULL;
                    currentScanThreadId = 0;
                }
            }
            break;
        }
          case WM_SIZE: {
            // Resize controls when window size changes
            int width = LOWORD(lParam);
            int height = HIWORD(lParam);
            
            if (hOutput) {
                SetWindowPos(hOutput, NULL, 20, 120, width - 40, height - 170, 
                    SWP_NOZORDER);
                SetWindowPos(GetDlgItem(hwnd, IDC_CLEAR), NULL, width - 120, height - 50, 
                    100, 30, SWP_NOZORDER);
            }
            break;
        }
          case WM_DESTROY:
            StopCurrentScan();
            PostQuitMessage(0);
            break;
            
        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    const char* className = "SecurityMonitorWindow";
    
    WNDCLASS wc = {0};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = className;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
      if (!RegisterClass(&wc)) {
        MessageBox(NULL, "Error registering window class!", "Error", MB_OK | MB_ICONERROR);
        return 0;
    }
      HWND hwnd = CreateWindow(className, "Security Monitoring System",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT, 850, 600,
        NULL, NULL, hInstance, NULL);
        
    if (!hwnd) {
        MessageBox(NULL, "Error creating window!", "Error", MB_OK | MB_ICONERROR);
        return 0;
    }
    
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    return msg.wParam;
}