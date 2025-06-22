# Security Monitoring System - Graphical Interface

## Description
Graphical interface developed with Windows API that integrates the three monitoring systems:
- USB device monitor (Jorge)
- Process and memory monitor (Fabian) 
- Port scanner (Leo)

## Features

### User Interface
- **4 main buttons:**
  1. **Scan File System** - Monitors USB devices and file changes (Jorge's function)
  2. **Scan Memory** - Analyzes processes and RAM usage (Fabian's function)  
  3. **Scan Ports** - Scans specified port range and identifies services (Leo's function)
  4. **Scan All** - **RUNS ALL 3 SCANS IN PARALLEL** (like the Main/main.c implementation)

### Port Range Configuration
- **Start Port** and **End Port** input fields
- Default range: 1-1024
- Validates input (1-65535 range)
- Automatically corrects if start > end

### Integrated Console
- Text area that displays all results in real time
- Timestamps for each operation
- Automatic scrolling
- Monospace font (Consolas) for better readability
- "Clear" button to reset the output

### Execution Control
- Button blocking during active scans
- Execution in separate threads to avoid blocking the UI
- Status and progress messages

## Compilation

### Method 1: Use the compilation script
```bash
compile.bat
```

### Method 2: Manual compilation
```bash
gcc -o gui.exe gui.c -lws2_32 -lpsapi -mwindows
```

## Usage

1. Run `gui.exe`
2. Select the desired scan type
3. Results will appear in real time in the integrated console
4. Use "Clear" to reset the screen between scans

## Integrated Functions

### Jorge - USB Monitor
- Detects device connection/disconnection
- Monitors file changes (new, modified, deleted)
- Recursive directory scanning

### Fabian - Process Monitor
- Monitors CPU and RAM usage per process
- Detects suspicious processes
- Terminates processes that exceed memory thresholds
- Whitelist of trusted processes

### Leo - Port Scanner
- Scans user-defined port ranges (default 1-1024)
- **OPTIMIZED THREAD POOLING**: Limited to 50 concurrent threads maximum
- Processes ports in batches to prevent system resource exhaustion
- Identifies services associated with open ports
- Classifies ports as safe or potentially dangerous
- Security statistics and comprehensive reporting

## System Requirements

- Windows 7 or higher
- GCC compiler (MinGW recommended)
- Libraries: ws2_32, psapi

## Technical Notes

- The application uses multithreading to avoid blocking the interface
- Implements mutex for safe synchronization
- Dynamic memory handling for scalability
- Responsive interface that adapts to window size
