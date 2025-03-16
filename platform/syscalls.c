#include "vmio.h"  // Include MRE file system headers
#include "vmlog.h"
#include <sys/stat.h>
#include <errno.h>

// NEWLIB Interface

#ifndef SEEK_SET
#define SEEK_SET 0  // Beginning of file
#endif

#ifndef SEEK_CUR
#define SEEK_CUR 1  // Current position
#endif

#ifndef SEEK_END
#define SEEK_END 2  // End of file
#endif

#ifndef O_RDONLY
#define O_RDONLY 0x0000  // Open for reading only
#endif

#ifndef O_WRONLY
#define O_WRONLY 0x0001  // Open for writing only
#endif

#ifndef O_RDWR
#define O_RDWR 0x0002    // Open for reading and writing
#endif

#ifndef O_CREAT
#define O_CREAT 0x0200   // Create file if it does not exist
#endif

#ifndef O_TRUNC
#define O_TRUNC 0x0400   // Truncate file to zero length
#endif

#ifndef O_APPEND
#define O_APPEND 0x0008  // Append mode
#endif

#ifndef O_BINARY
#define O_BINARY 0x8000  // Binary mode (Windows/Embedded)
#endif


int _close(int file) {
    vm_file_close((VMFILE)file);
    return 0;  // Return success
}

int _lseek(int file, int ptr, int dir) {
    VMINT base;
    if (dir == SEEK_SET) base = BASE_BEGIN;
    else if (dir == SEEK_CUR) base = BASE_CURR;
    else if (dir == SEEK_END) base = BASE_END;
    else return -1; // Invalid mode

    return vm_file_seek((VMFILE)file, ptr, base);
}

int _read(int file, char *ptr, int len) {
    VMUINT nread = 0;
    VMINT result = vm_file_read((VMFILE)file, ptr, len, &nread);
    return (result < 0) ? -1 : nread;
}

int _write(int file, char *ptr, int len) {
    VMUINT written = 0;

    if (file == 1) { // stdout
        for (int i = 0; i < len; i++) {
            if (ptr[i] == '\n') vm_log_info("\r");
            vm_log_info("%c", ptr[i]);
        }
        return len;
    } else if (file == 2) { // stderr
        for (int i = 0; i < len; i++) {
            if (ptr[i] == '\n') vm_log_error("\r");
            vm_log_error("%c", ptr[i]);
        }
        return len;
    } else {
        VMINT result = vm_file_write((VMFILE)file, ptr, len, &written);
        return (result < 0) ? -1 : written;
    }
}

int _fstat(int file, struct stat *st) {
    st->st_mode = S_IFCHR;
    return 0;
}

int _isatty(int file) {
    return 1;
}

int _open(const char *path, int flags, ...) {
    VMUINT mode = (flags & O_RDWR) ? MODE_WRITE : MODE_READ;
    VMUINT binary = (flags & O_BINARY) ? 1 : 0;
    VMFILE file = vm_file_open((VMWSTR)path, mode, binary);
    return (file < 0) ? -1 : (int)file;
}
