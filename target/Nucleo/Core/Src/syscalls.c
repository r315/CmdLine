/**
*****************************************************************************
**
**  File        : syscalls.c
**
**  Abstract    : System Workbench Minimal System calls file
**
** 		          For more information about which c-functions
**                need which of these lowlevel functions
**                please consult the Newlib libc-manual
**
**  Environment : System Workbench for MCU
**
**  Distribution: The file is distributed �as is,� without any warranty
**                of any kind.
**
*****************************************************************************
**
** <h2><center>&copy; COPYRIGHT(c) 2014 Ac6</center></h2>
**
** Redistribution and use in source and binary forms, with or without modification,
** are permitted provided that the following conditions are met:
**   1. Redistributions of source code must retain the above copyright notice,
**      this list of conditions and the following disclaimer.
**   2. Redistributions in binary form must reproduce the above copyright notice,
**      this list of conditions and the following disclaimer in the documentation
**      and/or other materials provided with the distribution.
**   3. Neither the name of Ac6 nor the names of its contributors
**      may be used to endorse or promote products derived from this software
**      without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
** AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
** IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
** DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
** FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
** DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
** SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
** CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
** OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**
*****************************************************************************
*/

#include <sys/stat.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>
#include <sys/times.h>

extern int errno;
extern uint32_t HAL_GetTick(void);

__attribute__((weak)) int _read(int file, char *ptr, int len) {(void)file; (void)ptr; (void)len; return 0; }
__attribute__((weak)) int _write(int file, char *ptr, int len) {(void)file; (void)ptr; return len; }
int _close(int file) {(void)file; return -1;}
int _fstat(int file, struct stat *st) {(void)file;	st->st_mode = S_IFCHR; return 0; }
int _getpid(void) { return 1; }
int _isatty(int file){ (void)file; return 1; }
int _kill(int pid, int sig){(void)pid; (void)sig; errno = EINVAL; return -1; }
int _lseek(int file, int ptr, int dir){(void)file; (void)ptr; (void)dir; return 0; }
int _open(char *path, int flags, ...) {	/* Pretend like we always fail */ (void)path; (void)flags; return -1; }
int _times(struct tms *buf){(void)buf; return -1; }
//void _exit (int status){_kill(status, -1);while (1) {}}		/* Make sure we hang here */
//int _wait(int *status) {(void)status; errno = ECHILD; return -1; }
//int _unlink(char *name){(void)name; errno = ENOENT; return -1; }
//int _stat(char *file, struct stat *st){ (void)file; st->st_mode = S_IFCHR; return 0;}
//int _link(char *old, char *new){(void)old; (void)new; errno = EMLINK; return -1; }
//int _fork(void){ errno = EAGAIN; return -1; }
//int _execve(char *name, char **argv, char **env){(void)name; (void)argv; (void)env;	errno = ENOMEM; return -1; }
clock_t clock(void){ return (clock_t)HAL_GetTick(); }
