/*
** Job Arranger for ZABBIX
** Copyright (C) 2012 FitechForce, Inc. All Rights Reserved.
** Copyright (C) 2013 Daiwa Institute of Research Business Innovation Ltd. All Rights Reserved.
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
** $Date:: 2015-03-13 10:45:13 +0900 #$
** $Revision: 6943 $
** $Author: sypark@FITECHLABS.CO.JP $
**/

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>

#ifdef _WINDOWS
#include <io.h>
#include <tchar.h>
#include <windows.h>
#else
#include <unistd.h>
#include <sys/wait.h>
#define O_BINARY 0
#endif

#ifdef _WINDOWS
#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <Userenv.h>
#include <winsafer.h>
#else
#include <pwd.h>     /* getpwuid_r() */
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

    char *ptruser;
    char *ptrpasswd;

#ifdef _WINDOWS
    char full_cmd[SIZE];
    TCHAR w_cmd[SIZE];
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    DWORD ret;
    LPVOID lpMsgBuf;
    char      msg[SIZE];
    char      wuser[256];
    char      wpwd[256];
    wchar_t   user[256];
    wchar_t   pwd[256];
    size_t    wLen;
    HANDLE              hToken;
    PROFILEINFOW        profileInfo;
    STARTUPINFO         lpStartupInfo;
    PROCESS_INFORMATION lpProcessInformation;
    //DWORD               exitCode;
    LPVOID              lpEnvironment;
    DWORD               dwSize;
    WCHAR               szUserProfile[256];
#else
    pid_t pid;
    int ret;
#endif

    if (argc != 5) {
        fprintf(stderr, "%s output_filepath command\n", argv[0]);
        return -1;
    }
    filepath = argv[1];
    cmd = argv[2];
    ret = 127;

    ptruser   = argv[3];
    ptrpasswd = argv[4];

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

    sprintf(wuser, "%s", ptruser);
    sprintf(wpwd, "%s",  ptrpasswd);
    mbstowcs_s(&wLen, user, sizeof(user)/2, wuser, _TRUNCATE);
    mbstowcs_s(&wLen, pwd,  sizeof(pwd)/2,  wpwd,  _TRUNCATE);

//    if  ( strcmp(wuser, "") != 0 ){
    if ((strcmp(wuser, "") != 0 ) && (strcmp(wuser, "(null)"))){
        SetLastError(NO_ERROR);

        // attempt to log a user on to the local computer
        if (!LogonUser(
            user,                      // A pointer to a null-terminated string that specifies the name of the user
            NULL,                      // A pointer to a null-terminated string that specifies the name of the domain or server
            pwd,                       // A pointer to a null-terminated string that specifies the plaintext password for the user
            LOGON32_LOGON_INTERACTIVE, // The type of logon operation to perform
            LOGON32_PROVIDER_DEFAULT,  // Specifies the logon provider
            &hToken                    // // A pointer to a handle variable that receives a handle to a token that represents the specified user
        )
        ) {
            FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
                          FORMAT_MESSAGE_FROM_SYSTEM |
                          FORMAT_MESSAGE_IGNORE_INSERTS, NULL, GetLastError(),
                          MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                          (LPTSTR) & lpMsgBuf, 0, NULL);
            WideCharToMultiByte(CP_ACP, 0, (LPTSTR) lpMsgBuf, -1, msg, SIZE,
                                NULL, NULL);
            fprintf(stderr, "failed to LogonUser function  :  %ws", msg);
            LocalFree(lpMsgBuf);
        }

        ZeroMemory(&profileInfo, sizeof(PROFILEINFO));
        profileInfo.dwSize     = sizeof(PROFILEINFO);
        profileInfo.lpUserName = user;

        // loads the specified user's profile
        if (!LoadUserProfileW(
            hToken,         // Token for the user, which is returned by the LogonUser, CreateRestrictedToken, DuplicateToken, OpenProcessToken, or OpenThreadToken function.
            &profileInfo    // Pointer to a PROFILEINFO structure. 
            )
        ) {
            FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, 
                          NULL, 
                          GetLastError(), 
                          MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
                          (LPTSTR) & lpMsgBuf, 
                          0, 
                          NULL);
            WideCharToMultiByte(CP_ACP, 0, (LPTSTR) lpMsgBuf, -1, msg, SIZE, NULL, NULL);
            fprintf(stderr, "failed to LoadUserProfileW  :  %ws", msg);
            LocalFree(lpMsgBuf);
            CloseHandle(hToken);
        }

        // Retrieves the environment variables for the specified user. This block can then be passed to the CreateProcessAsUser function.
        if (!CreateEnvironmentBlock(
            &lpEnvironment,    // When this function returns, receives a pointer to the new environment block.
            hToken,            // Token for the user, returned from the LogonUser function.
            TRUE               // Specifies whether to inherit from the current process' environment.
            )
        ) {
            FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, 
                          NULL, 
                          GetLastError(), 
                          MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
                          (LPTSTR) & lpMsgBuf, 
                          0, 
                          NULL);
            WideCharToMultiByte(CP_ACP, 0, (LPTSTR) lpMsgBuf, -1, msg, SIZE, NULL, NULL);
            fprintf(stderr, "failed to CreateEnvironmentBlock function  :  %ws", msg);
            LocalFree(lpMsgBuf);
            UnloadUserProfile(hToken, profileInfo.hProfile);
            CloseHandle(hToken);
        }

        // Retrieves the path to the root directory of the specified user's profile.
        dwSize = sizeof(szUserProfile);
        if (!GetUserProfileDirectoryW(
            hToken,         // A token for the user, which is returned by the LogonUser, CreateRestrictedToken, DuplicateToken, OpenProcessToken, or OpenThreadToken function.
            szUserProfile,  // A pointer to a buffer that, when this function returns successfully,
            &dwSize         // Specifies the size of the lpProfileDir buffer.
            )
        ) {
            FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, 
                          NULL, 
                          GetLastError(), 
                          MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
                          (LPTSTR) & lpMsgBuf, 
                          0, 
                          NULL);
            WideCharToMultiByte(CP_ACP, 0, (LPTSTR) lpMsgBuf, -1, msg, SIZE, NULL, NULL);
            fprintf(stderr, "failed to GetUserProfileDirectoryW function  :  %ws", msg);
            LocalFree(lpMsgBuf);
            UnloadUserProfile(hToken, profileInfo.hProfile);
            DestroyEnvironmentBlock(lpEnvironment);
            CloseHandle(hToken);
        }

        ZeroMemory(&lpStartupInfo, sizeof(STARTUPINFO));
        lpStartupInfo.lpDesktop = L"";
        lpStartupInfo.cb = sizeof(STARTUPINFO);

        lpStartupInfo.dwFlags    = STARTF_USESTDHANDLES;
        lpStartupInfo.hStdInput  = GetStdHandle(STD_INPUT_HANDLE);
        lpStartupInfo.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
        lpStartupInfo.hStdError  = GetStdHandle(STD_ERROR_HANDLE);
        ZeroMemory(&lpProcessInformation, sizeof(PROCESS_INFORMATION));

        SetLastError(NO_ERROR);

        // Creates a new process and its primary thread
        if (!CreateProcessAsUser(
            hToken,                     // A handle to the primary token that represents a user
            NULL,                       // The name of the module to be executed
            w_cmd,                      // The command line to be executed
            NULL,                       // A pointer to a SECURITY_ATTRIBUTES structure that specifies a security descriptor for the new process object
            NULL,                       // A pointer to a SECURITY_ATTRIBUTES structure that specifies a security descriptor for the new thread object
            TRUE,                       // Each inheritable handle in the calling process is inherited by the new process
            CREATE_DEFAULT_ERROR_MODE | CREATE_NEW_CONSOLE | CREATE_NEW_PROCESS_GROUP|CREATE_UNICODE_ENVIRONMENT,// The flags that control the priority class and the creation of the process
            lpEnvironment,              // A pointer to an environment block for the new process
            szUserProfile,              // The full path to the current directory for the process
            &lpStartupInfo,             // A pointer to a STARTUPINFO structure
            &lpProcessInformation       // A pointer to a PROCESS_INFORMATION structure
        )) {
            FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, 
                          NULL, 
                          GetLastError(), 
                          MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
                          (LPTSTR) & lpMsgBuf, 
                          0, 
                          NULL);
            WideCharToMultiByte(CP_ACP, 0, (LPTSTR) lpMsgBuf, -1, msg, SIZE, NULL, NULL);
            fprintf(stderr, "failed to CreateProcessAsUser function  :  %ws", msg);
            LocalFree(lpMsgBuf);
            DestroyEnvironmentBlock(lpEnvironment);
            UnloadUserProfile(hToken, profileInfo.hProfile);
            CloseHandle(hToken);
        }

        WaitForSingleObject(lpProcessInformation.hProcess, INFINITE);
        GetExitCodeProcess(lpProcessInformation.hProcess, &ret);
        DestroyEnvironmentBlock(lpEnvironment);
        UnloadUserProfile(hToken, profileInfo.hProfile);
        CloseHandle(hToken);
        CloseHandle(lpProcessInformation.hThread);
        CloseHandle(lpProcessInformation.hProcess);
    }else{

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
    }
#else
    pid = fork();
    if (pid < 0)
        fprintf(stderr, "fork: %s\n", strerror(errno));
    if (pid == 0) {
        // execl("/bin/sh", "sh", "-c", cmd, NULL);
        // fprintf(stderr, "%s :%s\n", cmd, strerror(errno));
        // exit(ret);

        char   user[1024];
        sprintf(user, "%s", ptruser);

        char   passwd[1024];
        sprintf(passwd, "%s", ptrpasswd);

//        if ( ( strcmp(user, "(null)" ) != 0) && (0 == getuid() || 0 == getgid()) ){              /* running as root? */
        if (((strcmp(user, "" ) != 0) && (strcmp(user, "(null)" ) != 0)) && (0 == getuid() || 0 == getgid())){    /* running as root? */
            /* get user */
            struct passwd *pwd;
            uid_t uid  = {0};
            uid_t euid = {0};
            gid_t gid  = {0};
            gid_t egid = {0};

            uid  = getuid();
            euid = geteuid();
            gid  = getgid();
            egid = getegid();

            pwd = getpwnam(user);

            if (NULL == pwd){
                exit(-1);
            }

            uid  = pwd->pw_uid;
            gid  = pwd->pw_gid;

            if (-1 == setgid(gid)){
                exit(-1);
            }

#ifdef HAVE_FUNCTION_INITGROUPS
            if (-1 == initgroups(user, gid)){
                exit(-1);
            }
#endif

            if (-1 == setuid(uid)){
                exit(-1);
            }

#ifdef HAVE_FUNCTION_SETEUID
            if (-1 == setegid(gid) || -1 == seteuid(uid)){
                exit(-1);
            }
#endif
            setsid();

            if (-1 == chdir("/"))
                exit(0);

            umask(0002);

            uid  = getuid();
            euid = geteuid();
            gid  = getgid();
            egid = getegid();

        }

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
