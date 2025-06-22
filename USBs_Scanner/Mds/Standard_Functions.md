# Standard Library and Windows API Functions Documentation

## C Standard Library Functions Used

### String Manipulation Functions (string.h)

## `strcmp(const char *str1, const char *str2)`
**Purpose:** Compares two null-terminated strings lexicographically
**Parameters:**
- `str1` - First string to compare
- `str2` - Second string to compare
**Return:** int - 0 if equal, <0 if str1 < str2, >0 if str1 > str2
**Usage in project:** Comparing filenames and paths, skipping "." and ".." directories

---

## `strcpy(char *dest, const char *src)`
**Purpose:** Copies source string to destination buffer
**Parameters:**
- `dest` - Destination buffer
- `src` - Source string to copy
**Return:** char* - Pointer to destination
**Usage in project:** Copying filenames and paths into Archivo structure

---

## `sprintf(char *str, const char *format, ...)`
**Purpose:** Formats and stores string in buffer
**Parameters:**
- `str` - Destination buffer
- `format` - Format string
- `...` - Variable arguments
**Return:** int - Number of characters written
**Usage in project:** Creating search patterns ("path\\*") and device paths ("D:")

---

### Memory Management Functions (stdlib.h)

## `malloc(size_t size)`
**Purpose:** Allocates dynamic memory block
**Parameters:**
- `size` - Number of bytes to allocate
**Return:** void* - Pointer to allocated memory, NULL if failed
**Usage in project:** Allocating arrays for previous and current file states

---

## `free(void *ptr)`
**Purpose:** Deallocates previously allocated memory
**Parameters:**
- `ptr` - Pointer to memory block to free
**Return:** void
**Usage in project:** Freeing allocated state arrays after monitoring

---

### Input/Output Functions (stdio.h)

## `printf(const char *format, ...)`
**Purpose:** Prints formatted output to stdout
**Parameters:**
- `format` - Format string
- `...` - Variable arguments
**Return:** int - Number of characters printed
**Usage in project:** Displaying device events, file changes, and status messages

---

## Windows API Functions (windows.h)

### Drive Detection Functions

## `GetLogicalDrives()`
**Purpose:** Retrieves bitmask representing available logical drives
**Parameters:** None
**Return:** DWORD - Bitmask where each bit represents a drive (A=bit0, B=bit1, etc.)
**Documentation:** [Microsoft Docs](https://docs.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-getlogicaldrives)
**Usage in project:** Detecting USB device connections and disconnections

---

### File System Search Functions

## `FindFirstFile(LPCSTR lpFileName, LPWIN32_FIND_DATA lpFindFileData)`
**Purpose:** Searches for first file/directory matching specified pattern
**Parameters:**
- `lpFileName` - Search pattern (e.g., "C:\\*")
- `lpFindFileData` - Pointer to WIN32_FIND_DATA structure
**Return:** HANDLE - Search handle, INVALID_HANDLE_VALUE if failed
**Documentation:** [Microsoft Docs](https://docs.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-findfirstfilea)
**Usage in project:** Starting directory traversal for file scanning

---

## `FindNextFile(HANDLE hFindFile, LPWIN32_FIND_DATA lpFindFileData)`
**Purpose:** Continues file search started by FindFirstFile
**Parameters:**
- `hFindFile` - Search handle from FindFirstFile
- `lpFindFileData` - Pointer to WIN32_FIND_DATA structure
**Return:** BOOL - TRUE if file found, FALSE if no more files
**Documentation:** [Microsoft Docs](https://docs.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-findnextfilea)
**Usage in project:** Iterating through all files in directory

---

## `FindClose(HANDLE hFindFile)`
**Purpose:** Closes file search handle
**Parameters:**
- `hFindFile` - Search handle to close
**Return:** BOOL - TRUE if successful
**Documentation:** [Microsoft Docs](https://docs.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-findclose)
**Usage in project:** Cleaning up after directory scan completion

---

### System Functions

## `Sleep(DWORD dwMilliseconds)`
**Purpose:** Suspends execution for specified time
**Parameters:**
- `dwMilliseconds` - Sleep duration in milliseconds
**Return:** void
**Documentation:** [Microsoft Docs](https://docs.microsoft.com/en-us/windows/win32/api/synchapi/nf-synchapi-sleep)
**Usage in project:** Adding delays between monitoring cycles (2-3 seconds)

---

## Windows Data Structures

### `WIN32_FIND_DATA`
**Purpose:** Contains file information returned by file search functions
**Key Fields Used:**
- `cFileName[260]` - Filename
- `dwFileAttributes` - File attributes (directory, file, etc.)
- `ftLastWriteTime` - Last modification time
**Documentation:** [Microsoft Docs](https://docs.microsoft.com/en-us/windows/win32/api/minwinbase/ns-minwinbase-win32_find_dataa)

---

### `FILETIME`
**Purpose:** 64-bit value representing time as 100-nanosecond intervals since January 1, 1601
**Fields:**
- `dwLowDateTime` - Low-order 32 bits
- `dwHighDateTime` - High-order 32 bits
**Documentation:** [Microsoft Docs](https://docs.microsoft.com/en-us/windows/win32/api/minwinbase/ns-minwinbase-filetime)
**Usage in project:** Detecting file modification times for change detection

---

### `HANDLE`
**Purpose:** Generic handle type for Windows objects
**Usage in project:** File search handles returned by FindFirstFile
**Documentation:** [Microsoft Docs](https://docs.microsoft.com/en-us/windows/win32/winprog/windows-data-types)

---

### `DWORD`
**Purpose:** 32-bit unsigned integer (0 to 4,294,967,295)
**Usage in project:** Drive bitmasks, sleep intervals, file attributes
**Documentation:** [Microsoft Docs](https://docs.microsoft.com/en-us/windows/win32/winprog/windows-data-types)

---

## Constants Used

### `FILE_ATTRIBUTE_DIRECTORY`
**Value:** 0x00000010
**Purpose:** File attribute indicating directory
**Usage:** Distinguishing files from folders during scan

### `INVALID_HANDLE_VALUE`
**Value:** (HANDLE)-1
**Purpose:** Invalid handle constant
**Usage:** Error checking for FindFirstFile failures

### `MAX_ARCHIVOS`
**Value:** 50000
**Purpose:** Maximum number of files to track
**Usage:** Array size limits and bounds checking
