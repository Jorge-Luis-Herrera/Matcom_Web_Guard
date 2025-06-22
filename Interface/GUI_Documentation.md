# GUI.C Code Documentation

## General Description
The `gui.c` file implements a graphical user interface (GUI) using the native Windows API. This application integrates three different monitoring systems into a single unified interface.

## Code Structure

### Headers and Definitions
```c
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
```
Includes necessary libraries for Windows window creation, file handling, memory and time management.

### Constant Definitions
```c
#define IDC_BUTTON_USB 1001
#define IDC_BUTTON_MEMORY 1002  
#define IDC_BUTTON_PORTS 1003
#define IDC_BUTTON_ALL 1004
#define IDC_OUTPUT 1005
#define IDC_CLEAR 1006
```
Unique identifiers for each interface control (buttons and text area).

### Global Variables
- `HWND hOutput`: Handle to the text control where results are displayed
- `HWND hMainWindow`: Handle to the main window
- `volatile BOOL scanning`: Flag indicating if a scan is in progress
- `HANDLE hScanThread`: Handle to the current scan thread

## Main Functions

### AppendToOutput()
```c
void AppendToOutput(const char* text)
```
**Purpose**: Adds text to the interface output area
**Functionality**: 
- Gets the current text length
- Positions the cursor at the end
- Inserts the new text
- Automatically scrolls to show the most recent text

### AddTimestamp()
```c
void AddTimestamp()
```
**Purpose**: Adds a timestamp to the output area
**Functionality**:
- Gets the current system time
- Formats the time as [HH:MM:SS]
- Calls AppendToOutput() to display it

### Scan Thread Functions

#### ScanThreadUSB()
- Executes Jorge's `j()` function to monitor USB
- Changes the `scanning` state during execution
- Adds start and finish messages

#### ScanThreadMemory()
- Executes Fabian's `f()` function to monitor processes
- Implements the same state control logic

#### ScanThreadPorts()
- Executes Leo's `l()` function to scan ports
- Same control structure as the previous ones

#### ScanThreadAll()
- **PARALLEL EXECUTION**: Creates three separate threads to run all scans simultaneously
- Gets port range from input fields for Leo's scanner
- Creates Jorge's thread (USB/File system monitoring)
- Creates Fabian's thread (Memory/Process monitoring)  
- Creates Leo's thread (Port scanning with user-defined range)
- Waits for all threads to complete using WaitForMultipleObjects()
- Provides real-time status updates for each thread
- Properly cleans up all thread handles

#### Port Range Management
- Validates port range inputs (1-65535)
- Sets default values if inputs are invalid
- Automatically swaps start/end if start > end
- Passes range parameters to Leo's scanning function
- **THREAD POOLING OPTIMIZATION**: Leo's scanner now uses batched thread execution
  - Maximum 50 concurrent threads at any time
  - Prevents system resource exhaustion on large port ranges
  - Processes 1-60 ports: 1 batch, 1-1024 ports: ~21 batches
  - Eliminates crashes from excessive thread creation

### EnableButtons()
```c
void EnableButtons(BOOL enable)
```
**Purpose**: Enables or disables all scan buttons
**Usage**: Prevents multiple simultaneous scans

## Procedimiento de Ventana (WindowProc)

### WM_CREATE
Handles the initial window creation:
- Creates all buttons with their positions and sizes
- Creates the output text area
- Configures the monospace font (Consolas)
- Shows welcome message

### WM_COMMAND
Handles button clicks:
- Checks if there's a scan in progress
- Disables buttons during execution
- Creates threads for each type of scan
- Starts timer for state control

### WM_TIMER
Monitors thread states:
- Checks if scanning has finished
- Re-enables buttons when finished
- Cleans up thread resources

### WM_SIZE
Handles window resizing:
- Adjusts the text area size
- Repositions the "Clear" button

### WM_DESTROY
Cleans up resources when closing:
- Terminates active threads
- Closes handles
- Terminates the application

## Main Function WinMain()

### Class Registration
```c
WNDCLASS wc = {0};
wc.lpfnWndProc = WindowProc;
// ... additional configuration
RegisterClass(&wc);
```

### Window Creation
```c
HWND hwnd = CreateWindow(className, "Security Monitoring System",
    WS_OVERLAPPEDWINDOW | WS_VISIBLE,
    CW_USEDEFAULT, CW_USEDEFAULT, 850, 600,
    NULL, NULL, hInstance, NULL);
```

### Message Loop
```c
while (GetMessage(&msg, NULL, 0, 0)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
}
```

## Technical Features

### Multithreading
- Each scan runs in a separate thread
- Interface remains responsive during long operations
- Concurrency control to prevent multiple scans

### Memory Management
- Proper use of Windows handles
- Automatic resource cleanup
- Memory leak prevention

### Responsive Interface
- Automatic control resizing
- Automatic text area scrolling
- Visual feedback of operation status

## Compilation
The file requires the following libraries:
- `ws2_32`: For network functions (port scanner)
- `psapi`: For process information (memory monitor)
- `-mwindows` flag: For GUI application (no DOS console)
