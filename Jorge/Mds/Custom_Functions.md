# Custom Functions Documentation

## File Monitoring and USB Device Detection System

### Core Functions

## `detectDrives()`
**Purpose:** Detects and monitors logical drives for connection/disconnection events
**Parameters:** None
**Return:** void

**Description:**
Continuously monitors system drives using GetLogicalDrives() API. Maintains two arrays to compare previous and current drive states, detecting when USB devices are connected or disconnected.

**Algorithm:**
1. Get initial drive bitmask
2. Store current drives in array
3. Loop indefinitely checking for changes
4. Compare previous vs current state
5. Report connect/disconnect events

---

## `scanPath(const char* path)`
**Purpose:** Recursively scans a directory path and stores file/folder information
**Parameters:** 
- `path` - Directory path to scan
**Return:** void

**Description:**
Uses Windows FindFirstFile/FindNextFile API to traverse directory structure recursively. Stores file metadata (name, path, modification time, type) in global array.

**Algorithm:**
1. Create search pattern (path\\*)
2. Initialize file search handle
3. Iterate through directory entries
4. Skip special directories (. and ..)
5. Store file information in global array
6. Recursively call for subdirectories

---

## `getFiles(const char* path, int* count)`
**Purpose:** Wrapper function to get all files from a path
**Parameters:**
- `path` - Directory path to scan
- `count` - Pointer to store file count
**Return:** File* - Pointer to files array

**Description:**
Resets global file counter, calls scanPath(), and returns the populated array with file count.

---

## `compareTime(FILETIME f1, FILETIME f2)`
**Purpose:** Compares two Windows FILETIME structures
**Parameters:**
- `f1` - First FILETIME structure
- `f2` - Second FILETIME structure
**Return:** int - 1 if equal, 0 if different

**Description:**
Compares both low and high parts of FILETIME structures to determine if two file times are identical.

---

## `findFile(File* array, int count, const char* path)`
**Purpose:** Searches for a file by path in the files array
**Parameters:**
- `array` - Array of File structures
- `count` - Number of elements in array
- `path` - File path to search for
**Return:** int - Index if found, -1 if not found

**Description:**
Linear search through file array comparing paths using strcmp(). Returns index of first match or -1 if not found.

---

## `monitorUSB(const char* device)`
**Purpose:** Monitors USB device for file system changes in real-time
**Parameters:**
- `device` - Device path (e.g., "D:")
**Return:** void

**Description:**
Main monitoring function that detects file additions, deletions, and modifications on USB devices.

**Algorithm:**
1. Allocate memory for previous and current states
2. Perform initial scan and store baseline
3. Continuously scan device every 3 seconds
4. Compare states to detect:
   - Deleted files (in previous but not current)
   - New files (in current but not previous)
   - Modified files (same path, different timestamp)
5. Update previous state with current state
6. Repeat monitoring loop

**Change Detection Logic:**
- **Deletions:** File exists in previous state but not in current
- **Additions:** File exists in current state but not in previous  
- **Modifications:** File exists in both states with different timestamps

---

## Global Variables

### `File files[MAX_FILES]`
**Purpose:** Global array storing file information
**Type:** Static array of File structures
**Size:** 50,000 elements maximum

### `int totalFiles`
**Purpose:** Counter for total files currently stored
**Type:** Integer
**Usage:** Reset before each scan, incremented for each file found

---

## Data Structures

### `File` Structure
```c
typedef struct {
    char name[256];           // File/folder name
    char path[512];           // Complete file path
    FILETIME lastModified;    // Last modification timestamp
    int isFile;               // 1 = file, 0 = folder
} File;
```

**Purpose:** Stores essential file metadata for monitoring
**Fields:**
- `name` - Base filename without path
- `path` - Full absolute path
- `lastModified` - Windows FILETIME structure
- `isFile` - Boolean flag distinguishing files from directories
