#include <iostream>
#include <windows.h>

#define READ_TIMEOUT 500

HANDLE hComm = ::CreateFileW(TEXT("COM1"),    //using UNICODE BUILD
            GENERIC_READ|GENERIC_WRITE,
            0,
            0,
            OPEN_EXISTING,
            FILE_FLAG_OVERLAPPED,
            0
            );
//if(hComm == INVALID_HANDLE_VALUE);
DWORD dwRes;
/*WRITING*/
DWORD dwCommEvent;
DWORD dwStoredFlags;
BOOL fWaitingOnStat = FALSE;
OVERLAPPED osStatus = {0};

/*READING*/
DWORD dwRead;
BOOL fWaitingOnRead = FALSE;
OVERLAPPED osReader = {0};
DCB dcb = {9600};    //setting the buad rate to 9600 bps

// create overlapperd event. must be closed before exiting
osReader.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

if(osReader.hEvent == NULL){
    // do stuff
}
if(!fWaitingOnRead){
    if (!ReadFile(hComm, lpBuf, READ_BUF_SIZE, &dwRead, &osReader)) {
      if (GetLastError() != ERROR_IO_PENDING)     // read not delayed?
         // Error in communications; report it.
      else
         fWaitingOnRead = TRUE;
   }
   else {    
      // read completed immediately
      HandleASuccessfulRead(lpBuf, dwRead);
    }
}


