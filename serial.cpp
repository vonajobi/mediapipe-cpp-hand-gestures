#include <iostream>
#include <windows.h>
#include <string>

#define READ_TIMEOUT 500
#define READ_BUF_SIZE 1024

HANDLE hComm;                   // initialize comport handle
OVERLAPPED osReader = {0}, osWriter = {0};

void handleSuccessRead(const char* buff, DWORD bytesRead){
    std::cout << "Read " << bytesRead << " bytes: " << std::string(buff, bytesRead) << std::endl;
}
void closePort(){
    CloseHandle(hComm);
    CloseHandle(osReader.hEvent);
    CloseHandle(osWriter.hEvent);
}
bool configureSerialPort(){
    DCB dcb = {0};              // device control block
    dcb.DCBlength = sizeof(DCB);

    if(!::GetCommState(hComm, &dcb)){
        std::cerr << "CSerialCommHelper : Failed to Get Comm State Reason: %d",
           GetLastError();
        return false;    
        }

    dcb.BaudRate = CBR_9600;
    dcb.ByteSize = 8;
    dcb.StopBits = ONESTOPBIT;
    dcb.Parity = NOPARITY;

    if(!SetCommState(hComm, &dcb)){
        std::cerr << "CSerialCommHelper : Failed to Set Comm State" << std::endl;
        return false;
    }
    // set timeouts
    COMMTIMEOUTS timeouts = {0};
    timeouts.ReadIntervalTimeout = READ_TIMEOUT;
    timeouts.ReadTotalTimeoutConstant = READ_TIMEOUT;
    timeouts.ReadTotalTimeoutMultiplier = 0;
    timeouts.WriteTotalTimeoutConstant = 0;
    timeouts.WriteTotalTimeoutMultiplier = 0;

    if (!SetCommTimeouts(hComm, &timeouts)) {
        std::cerr << "CSerialCommHelper : Failed to Set Comm Timeouts" << std::endl;
        return false;
    }
    osReader.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    osWriter.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    if(osReader.hEvent == NULL || osWriter.hEvent == NULL){
        std::cerr << "CSerialCommHelper : Failed to Write/Read Overlapped Events" << std::endl;
        return false;
    }

    return true;
}
void serialPortWrite(const std::string &data){
    DWORD dwWrite = 0;
    if (!WriteFile(hComm, data.c_str(), data.length(), &dwWrite, &osWriter)) {
        if (GetLastError() != ERROR_IO_PENDING) {
            std::cerr << "Error writing to serial port" << std::endl;
        } else {
            // Wait for the write operation to complete
            if (!GetOverlappedResult(hComm, &osWriter, &dwWrite, TRUE)) {
                std::cerr << "Error in overlapped write operation" << std::endl;
            }
        }
    }
}
void serialPortRead(){
    char readBuff[READ_BUF_SIZE] = {0};
    DWORD dwRead = 0;
    BOOL fWaitingOnRead = FALSE;

    if(!fWaitingOnRead){
        if (!ReadFile(hComm, readBuff, READ_BUF_SIZE, &dwRead, &osReader)) {
        if (GetLastError() != ERROR_IO_PENDING)     // read not delayed?
            std::cerr << "Error reading from serial port" << std::endl;
        else
            fWaitingOnRead = TRUE;
    }
    else {    
        // read completed immediately
        handleSuccessRead(readBuff, dwRead);
        }
    }
    if (fWaitingOnRead) {
        // Wait for the read operation to complete
        DWORD dwRes = WaitForSingleObject(osReader.hEvent, READ_TIMEOUT);
        switch (dwRes) {
            case WAIT_OBJECT_0:
                if (!GetOverlappedResult(hComm, &osReader, &dwRead, FALSE)) {
                    std::cerr << "Error in overlapped read operation" << std::endl;
                } else {
                    handleSuccessRead(readBuff, dwRead);
                }
                fWaitingOnRead = FALSE;
                break;
            case WAIT_TIMEOUT:
                std::cerr << "Read operation timed out" << std::endl;
                break;
            default:
                std::cerr << "Error in WaitForSingleObject" << std::endl;
                break;
        }
    }
}

// create overlapperd event. must be closed before exiting
int main(){
    hComm = ::CreateFileW(TEXT("COM1"),    //using UNICODE BUILD
                GENERIC_READ | GENERIC_WRITE,
                0,
                0,
                OPEN_EXISTING,
                FILE_FLAG_OVERLAPPED,
                0
                );
    if(hComm == INVALID_HANDLE_VALUE){
        std::cerr << "Error opening serial port. Error log "<< std::endl; 
        return 1;
    };
    // Configure the serial port
    if(!configureSerialPort()) {
        closePort();
        return 1;
    }    
    serialPortWrite(data);      // define data 
    serialPortRead();
    closePort();

    return 0;
}

