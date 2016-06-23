/*
** Job Arranger for ZABBIX
** Copyright (C) 2012 FitechForce, Inc. All Rights Reserved.
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
**/

/*
** $Date:: 2013-03-06 16:38:01 +0900 #$
** $Revision: 3895 $
** $Author: ossinfra@FITECHLABS.CO.JP $
**/

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>
#include <errno.h>

#ifdef _WINDOWS
#include <io.h>
#include <tchar.h>
#include <windows.h>
#else
#include <unistd.h>
#include <sys/wait.h>
#define O_BINARY 0
#endif

#define SIZE 4096
const char *ext[] = { "start", "end", "ret", "stdout", "stderr", NULL };

int main(int argc, char *argv[])
{
    char *filepath, *cmd;
    char fn[SIZE];
    int i;
    int fd, fd_start, fd_end, fd_ret, fd_stdout, fd_stderr;
    time_t now;

#ifdef _WINDOWS
    char full_cmd[SIZE];
    TCHAR w_cmd[SIZE];
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    DWORD ret;
    LPVOID lpMsgBuf;
    char msg[SIZE];
#else
    pid_t pid;
    int ret;
#endif

    if (argc != 3) {
        fprintf(stderr, "%s output_filepath command\n", argv[0]);
        return -1;
    }
    filepath = argv[1];
    cmd = argv[2];
    ret = 127;

    fd = -1;
    fd_start = -1;
    fd_end = -1;
    fd_ret = -1;
    fd_stdout = -1;
    fd_stderr = -1;
    i = 0;
    while (ext[i] != NULL) {
        sprintf(fn, "%s.%s", filepath, ext[i]);
        fd = open(fn, O_WRONLY | O_BINARY | O_TRUNC);
        if (fd < 0) {
            fprintf(stderr, "%s: %s\n", fn, strerror(errno));
            return -1;
        }
        if (i == 0)
            fd_start = fd;
        else if (i == 1)
            fd_end = fd;
        else if (i == 2)
            fd_ret = fd;
        else if (i == 3)
            fd_stdout = fd;
        else if (i == 4)
            fd_stderr = fd;
        else
            close(fd);
        i++;
    }

    fflush(stdout);
    fflush(stderr);
    dup2(fd_stdout, 1);
    dup2(fd_stderr, 2);

    now = time(NULL);
    if (write(fd_start, &now, sizeof(now)) < 0)
        fprintf(stderr, "fd_start: %s\n", strerror(errno));
    close(fd_start);

#ifdef _WINDOWS
    ZeroMemory(full_cmd, sizeof(full_cmd));
    ZeroMemory(w_cmd, sizeof(w_cmd));
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));
    SetLastError(NO_ERROR);

    sprintf(full_cmd, "cmd /q /k \"%s\" & exit !errorlevel!", cmd);
    MultiByteToWideChar(CP_OEMCP, MB_PRECOMPOSED, full_cmd,
                        strlen(full_cmd), w_cmd, sizeof(w_cmd));

    if (0 == CreateProcess(NULL,        /* no module name (use command line) */
                           w_cmd,       /* name of app to launch */
                           NULL,        /* default process security attributes */
                           NULL,        /* default thread security attributes */
                           TRUE,        /* do not inherit handles from the parent */
                           0,   /* normal priority */
                           NULL,        /* use the same environment as the parent */
                           NULL,        /* launch in the current directory */
                           &si, /* startup information */
                           &pi  /* process information stored upon return */
        )) {
        FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
                      FORMAT_MESSAGE_FROM_SYSTEM |
                      FORMAT_MESSAGE_IGNORE_INSERTS, NULL, GetLastError(),
                      MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                      (LPTSTR) & lpMsgBuf, 0, NULL);
        WideCharToMultiByte(CP_ACP, 0, (LPTSTR) lpMsgBuf, -1, msg, SIZE,
                            NULL, NULL);
        fprintf(stderr, "failed to create process for [%s]: %s", cmd, msg);
        LocalFree(lpMsgBuf);
    } else {
        WaitForSingleObject(pi.hProcess, INFINITE);
        GetExitCodeProcess(pi.hProcess, &ret);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }
#else
    pid = fork();
    if (pid < 0)
        fprintf(stderr, "fork: %s\n", strerror(errno));
    if (pid == 0) {
        execl("/bin/sh", "sh", "-c", cmd, NULL);
        fprintf(stderr, "%s :%s\n", cmd, strerror(errno));
        exit(ret);
    }

    if (pid > 0)
        waitpid(pid, &ret, WUNTRACED);
#endif
    now = time(NULL);
    if (write(fd_end, &now, sizeof(now)) < 0)
        fprintf(stderr, "fd_end: %s\n", strerror(errno));
    close(fd_end);

    if (write(fd_ret, &ret, sizeof(ret)) < 0)
        fprintf(stderr, "fd_ret: %s\n", strerror(errno));
    close(fd_ret);

    close(fd_stdout);
    close(fd_stderr);
    return 0;
}
