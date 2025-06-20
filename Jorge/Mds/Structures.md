# Data Structures and Types Used

## Custom Structures

### `File` Structure
```c
typedef struct {
    char name[256];           // File/folder name
    char path[512];           // Complete path
    FILETIME lastModified;    // Last modification date
    int isFile;               // 1 if file, 0 if folder
} File;
```
**Purpose:** Store properties of files and folders for monitoring

## Primitive Data Types

### Integer Types
- `int` - Standard integer for counters, indices, and flags
- `DWORD` - 32-bit unsigned integer (Windows API type)

### Character Types
- `char` - Single character and character arrays for strings
- `char[]` - Fixed-size character arrays for names and paths

### Windows API Types
- `HANDLE` - Generic handle for Windows objects
- `FILETIME` - Windows structure for file timestamps
- `WIN32_FIND_DATA` - Windows structure containing file information

## Arrays and Memory Management

### Static Arrays
```c
File files[MAX_FILES];                  // Global array for file storage
char driveLetter[26];                   // Array for drive letters
char driveletter2[26];                  // Temporary array for comparison
```

### Dynamic Arrays
```c
File* prevState;                        // Dynamic array for previous state
File* currState;                        // Dynamic array for current state
```

### Constants
```c
#define MAX_FILES 50000                 // Maximum number of files to handle
```

## Memory Allocation
- **Static allocation** - For global arrays and temporary variables
- **Dynamic allocation** - Using `malloc()` for monitoring arrays
- **Stack allocation** - For local variables and function parameters