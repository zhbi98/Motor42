/**
 * @file retarget.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "retarget.h"
#include <errno.h>
#include <sys/time.h>

/*********************
 *      DEFINES
 *********************/

#if !defined(OS_USE_SEMIHOSTING)

#define STDIN_FILENO  0U
#define STDOUT_FILENO 1U
#define STDERR_FILENO 2U

/**********************
 *      TYPEDEFS
 **********************/

UART_HandleTypeDef * gHuart = NULL;

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Initialize retargeting with a UART handle.
 * @param huart Pointer to UART handle for I/O operations.
 */
void RetargetInit(UART_HandleTypeDef * huart)
{
    gHuart = huart;

    /**Disable I/O buffering for STDOUT stream, 
    so thatchars are sent out as soon as 
    they are printed.*/
    setvbuf(stdout, NULL, _IONBF, 0);

    /*Disables buffering for stdout 
    to ensure immediate output.*/
}

/**
 * Check if a file descriptor refers to a terminal.
 * @param fd File descriptor to check.
 * @return 1 if valid (STDIN/OUT/ERR), 0 otherwise, 
 * EBADF Set in errno for invalid descriptors.
 */
int _isatty(int fd)
{
    if (fd >= STDIN_FILENO && 
        fd <= STDERR_FILENO) return 1;
    errno = EBADF;
    return 0;
}

/**
 * Write data to UART for stdout/stderr.
 * @param fd File descriptor (STDOUT_FILENO or STDERR_FILENO).
 * @param ptr Pointer to data buffer.
 * @param len Number of bytes to write.
 * @return Number of bytes successfully transmitted, 
 * 1 for invalid descriptors.
 */
int _write(int fd, char * ptr, int len)
{
    HAL_StatusTypeDef _res = HAL_ERROR;

    /*Uses blocking HAL_UART_Transmit with 0xFFFF timeout.*/

    if (fd == STDOUT_FILENO || fd == STDERR_FILENO) {
        while (_res != HAL_OK) {
            _res = HAL_UART_Transmit(gHuart, 
                (uint8_t *)ptr, len, 0XFFFF);
        }
        return len;
    } else return -1;
}

/**
 * Close a file descriptor (no-op for stdio).
 * @param fd File descriptor to close.
 * @return 0 for success (STDIN/OUT/ERR), -1 otherwise, 
 * EBADF Set in errno for invalid descriptors.
 */
int _close(int fd)
{
    if (fd >= STDIN_FILENO && 
        fd <= STDERR_FILENO) return 0;
    errno = EBADF;
    return -1;
}

/**
 * Unsupported file seek operation.
 * @param fd File descriptor (ignored).
 * @param ptr Offset (ignored).
 * @param dir Seek direction (ignored).
 * @return -1 always, EBADF Set in errno.
 */
int _lseek(int fd, int ptr, int dir)
{
    (void) fd;
    (void) ptr;
    (void) dir;

    errno = EBADF;
    return -1;
}

/**
 * Read data from UART (stub implementation).
 * @param fd File descriptor (must be STDIN_FILENO).
 * @param ptr Pointer to receive buffer.
 * @param len Number of bytes to read.
 * @return 1 if successful, EIO on error.
 */
int _read(int fd, char * ptr, int len)
{
    HAL_StatusTypeDef hstatus;

    if (fd == STDIN_FILENO) {
        /*hstatus = HAL_UART_Receive(gHuart, 
        (uint8_t *) ptr, 1, HAL_MAX_DELAY);*/
        if (hstatus == HAL_OK) return 1;
        else return EIO;
    }
    errno = EBADF;
    return -1;

    /*Currently commented out; returns EIO by default.*/
}

/**
 * Get file status (always a character device for stdio).
 * @param fd File descriptor to check.
 * @param st Pointer to stat structure.
 * @return 0 for success (STDIN/OUT/ERR), -1 otherwise, 
 * EBADF Set in errno for invalid descriptors.
 */
int _fstat(int fd, struct stat * st)
{
    if (fd >= STDIN_FILENO && 
        fd <= STDERR_FILENO) {
        st->st_mode = S_IFCHR;
        return 0;
    }

    errno = EBADF;
    return 0;
}

#endif /*#if !defined(OS_USE_SEMIHOSTING)*/
