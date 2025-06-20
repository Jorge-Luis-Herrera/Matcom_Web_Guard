# C Libraries used and documentation about it

## Documentation Structure
- [Custom Functions](Custom_Functions.md) - Detailed documentation of project-specific functions
- [Standard Functions](Standard_Functions.md) - Complete reference for C standard library and Windows API functions used

## Standard C Libraries

### stdio.h - Standard Input/Output functions
**Documentation Sources:**
- [W3Schools C Reference](https://www.w3schools.com/c/c_ref_stdio.php)
- [GeeksforGeeks stdio.h](https://www.geeksforgeeks.org/input-output-system-calls-c-create-open-close-read-write/)

**Functions Used:**
- `printf()` - Print formatted output to console
- `sprintf()` - Format string and store in buffer

### stdlib.h - Standard Library functions
**Documentation Sources:**
- [W3Schools stdlib Reference](https://www.w3schools.com/c/c_ref_stdlib.php)
- [GeeksforGeeks Dynamic Memory](https://www.geeksforgeeks.org/dynamic-memory-allocation-in-c-using-malloc-calloc-free-and-realloc/)
- [Stanford CS Education Library](http://cslibrary.stanford.edu/102/PointersAndMemory.pdf)

**Functions Used:**
- `malloc()` - Allocate dynamic memory
- `free()` - Free allocated memory

### string.h - String manipulation functions
**Documentation Sources:**
- [W3Schools string Reference](https://www.w3schools.com/c/c_ref_string.php)
- [GeeksforGeeks String Functions](https://www.geeksforgeeks.org/strings-in-c-2/)

**Functions Used:**
- `strcmp()` - Compare two strings
- `strcpy()` - Copy string

## Windows API Libraries

### windows.h - Windows API functions
**Documentation Sources:**
- [Microsoft Official Documentation](https://docs.microsoft.com/en-us/windows/win32/api/)
- [Windows Internals Book Series - Microsoft Press](https://docs.microsoft.com/en-us/sysinternals/resources/windows-internals)
- [Charles Petzold's Programming Windows](https://www.charlespetzold.com/books.html)

**Functions Used:**
- `GetLogicalDrives()` - Get bitmask of available drives
- `FindFirstFile()` - Find first file in directory
- `FindNextFile()` - Find next file in directory search
- `FindClose()` - Close file search handle
- `Sleep()` - Suspend execution for specified time

**Data Structures:**
- `WIN32_FIND_DATA` - Structure for file information
- `FILETIME` - Structure for file time information
- `HANDLE` - Generic handle type
- `DWORD` - 32-bit unsigned integer type

## Innovation Notes

### Technical Innovation
This project innovates by implementing USB device monitoring on Windows platform using native Windows API, while most similar projects target Linux systems. The implementation provides real-time file system monitoring specifically designed for USB threat detection.

### Academic Context
**Related Research Areas:**
- **Cybersecurity:** USB-based attack vectors and prevention mechanisms
- **Operating Systems:** Real-time file system monitoring techniques
- **Computer Security:** Device-based threat detection systems

**Novelty Factors:**
1. **Platform-specific Implementation:** Leverages Windows-native APIs for optimal performance
2. **Real-time Detection:** Implements continuous monitoring with minimal system overhead
3. **Educational Value:** Demonstrates practical application of systems programming concepts