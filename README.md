# Backdoor for Windows written on C 

### !!!Before compiling!!!
Enter same IP address of server machine in both backdoor.c and server.c files.

## Compilation 
To compile exe files on linux, need to install mingw:
```shell script
sudo apt update
sudo apt install mingw-w64
```

Server compilation: 
```shell script
gcc server.c -o server  
```

Backdoor compilation (windows 32 bit program can be executed on both 32 and 64 windows OS):
```shell script
i686-w64-mingw32-gcc -o malware.exe backdoor.c -lwsock32 -lwininet 
```

run server: 
```shell script
./server
```
