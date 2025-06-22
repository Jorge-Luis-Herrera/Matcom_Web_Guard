# Matcom System Security Monitor

A comprehensive parallel monitoring system designed for educational and scientific purposes. This project demonstrates real-time system monitoring capabilities across multiple security domains.

## 📋 Project Overview

This monitoring system consists of three independent modules that work together to provide comprehensive system surveillance:

- **USB/Filesystem Monitor** - Tracks device connections and file changes
- **Memory/Process Monitor** - Monitors system resources and running processes  
- **Network Port Scanner** - Scans for open network ports and potential vulnerabilities

All modules run simultaneously in parallel threads, providing real-time monitoring without interfering with each other.

## 🔧 System Architecture

### Core Components

The system is built around three main monitoring modules:

#### 1. USB & Filesystem Monitor
**Purpose**: Detects and monitors USB device connections and filesystem changes

**How it works**:
- Continuously scans for connected drives using Windows API
- Detects when USB devices are plugged in or removed
- Monitors file creation, deletion, and modification in real-time
- Tracks up to 50,000 files simultaneously
- Provides instant alerts when changes occur

**Scientific Application**: 
- Data integrity monitoring in research environments
- Unauthorized data transfer detection
- File tampering prevention in scientific datasets

#### 2. Memory & Process Monitor
**Purpose**: Monitors system memory usage and running processes

**How it works**:
- Tracks active system processes and their resource consumption
- Monitors RAM usage patterns and memory allocation
- Detects unusual process behavior or resource spikes
- Provides system performance metrics in real-time

**Scientific Application**:
- Resource optimization for computational research
- Detecting memory leaks in scientific software
- System performance analysis during experiments

#### 3. Network Port Scanner
**Purpose**: Scans network ports to identify open connections and potential security risks

**How it works**:
- Systematically scans network ports (typically 1-1024)
- Uses efficient thread pooling to prevent system overload
- Identifies open ports and their associated services
- Categorizes ports by security risk level
- Implements batch processing to avoid resource exhaustion

**Scientific Application**:
- Network security assessment in research facilities
- Identifying unauthorized network services
- Monitoring research data transmission channels

## 🚀 Getting Started

### Prerequisites

- Windows Operating System
- GCC Compiler (MinGW recommended)
- Administrator privileges (recommended for full monitoring capabilities)

### Installation

1. **Clone or download the project**:
   ```bash
   git clone https://github.com/Jorge-Luis-Herrera/Windows_Vigilant
   cd "Window`s_Vigilant"
   ```

2. **Compile using the automated script (Recommended)**:
   ```bash
   cd Interface
   compile.bat
   ```
   This will compile and automatically run the graphical interface.

3. **Manual compilation (Alternative)**:
   ```bash
   cd Main
   gcc -o Program.exe Program.c -lws2_32
   ```

### Running the Monitor

#### Graphical Interface (Recommended)
```bash
cd Interface
compile.bat
```
The batch file will automatically compile and run the GUI version.

#### Console Version (Alternative)
```bash
cd Main
./Program.exe
```

#### What You'll See:
- Real-time timestamps for each monitoring cycle
- USB device connection/disconnection alerts
- Memory and process status updates
- Network port scan results
- System resource utilization

## 💻 User Interface

### Console Interface
The program displays information in a structured format:

```
Parallel Monitoring System
Jorge: USB Monitor | Fabian: Process Monitor | Leo: Port Scanner

[14:30:25] Jorge - USB Monitor
Device E: connected
NEW: E:\research_data.txt
Jorge - Completed

[14:30:25] Fabian - Process Monitor  
Memory Usage: 45% | Active Processes: 127
Fabian - Completed

[14:30:25] Leo - Port Scanner
Scanning ports 1-1024...
Open ports found: 22 (SSH), 80 (HTTP), 443 (HTTPS)
Leo - Completed
```

### Output Synchronization
- Thread-safe console output prevents overlapping messages
- Each monitor runs independently every 2 seconds
- Timestamps show exact monitoring times
- Status messages confirm successful completion

## 🔬 Educational Value

### Computer Science Concepts Demonstrated

1. **Parallel Programming**:
   - Multi-threading implementation
   - Thread synchronization with mutexes
   - Resource sharing and race condition prevention

2. **System Programming**:
   - Windows API integration
   - Low-level system monitoring
   - Hardware interaction (USB detection)

3. **Network Programming**:
   - Socket programming
   - Port scanning techniques
   - Network security fundamentals

4. **File System Operations**:
   - Real-time file monitoring
   - Directory traversal algorithms
   - File change detection

### Research Applications

- **Cybersecurity Research**: Understanding system vulnerabilities
- **Performance Analysis**: Resource consumption studies
- **Data Integrity**: File system monitoring for research data
- **Network Security**: Port scanning for vulnerability assessment

## ⚙️ Configuration Options

### Port Scanning Range
By default, the scanner checks ports 1-1024. This can be modified in the code:
```c
l(1, 1024); // Scan ports 1 to 1024
```

### Monitoring Intervals
Each monitor runs every 2 seconds. Modify the `Sleep(2000)` values to change intervals.

### File Monitoring Limits
The system tracks up to 50,000 files. Adjust `MAX_FILES` constant if needed.

## 📊 Performance Considerations

- **Memory Usage**: Optimized for systems with 4GB+ RAM
- **CPU Impact**: Low to moderate CPU usage during normal operation
- **Network Impact**: Minimal network traffic during port scanning
- **Thread Management**: Uses controlled threading to prevent system overload

## 🛡️ Security Notes

- Run with administrator privileges for comprehensive monitoring
- Port scanning should only be performed on authorized networks
- Monitor logs may contain sensitive system information
- Use responsibly in educational/research environments only

## 🤝 Contributing

This is an educational project. Students and researchers are welcome to:
- Extend monitoring capabilities
- Improve performance optimizations
- Add new monitoring modules
- Enhance the user interface

## 📝 License

Educational use only. This project is designed for learning purposes and scientific research.

## 👥 Authors

- **Jorge**: USB/Filesystem monitoring implementation
- **Fabian**: Memory/Process monitoring system  
- **Leo**: Network port scanner development

---

*This project demonstrates practical applications of system programming, parallel computing, and cybersecurity monitoring for educational purposes.*
