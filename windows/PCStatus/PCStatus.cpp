#include <Windows.h>
#include <stdio.h>
#include <process.h>
#include <shellapi.h>
#include <Pdh.h>
#include "resource.h"
#include "Serial.h"

#define IDC_MAIN_BUTTON	101			// Button identifier
#define IDC_MAIN_BUTTON_CLOSE 103
#define IDC_MAIN_BUTTON_HIDE 108
#define WM_NOT_ID 105

/* Arrays */

static char * comPorts[7] = {"COM1", "COM2", "COM3", "COM4", "COM5", "COM6", "COM7"};
static char * comBitra[7] = {"2400", "4800", "9600", "19200", "38400", "57600", "115200"};

/* Handles */

HWND	hEdit;
HWND	hWndComboBox;
HWND	hWndComboBox2;
HWND	hWndButton;

/* Stucts & Globals */
static int readyToSend = 0;

static	NOTIFYICONDATA nidApp;
static	PDH_HQUERY cpuQuery;
static	PDH_HCOUNTER cpuTotal;

/* Function Prototupes */

void	cpuUsageInit();
int		cpuUsageValue();
void	SerialWrite(byte cpuUsage, int mPU); 
void	LiveInfoMessage(void *);
int		memUsageMSB(int n);
int		memUsageLSB(int n);

/* Class Init */

CSerial Serial;

/* Special friend */

LRESULT CALLBACK WinProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam);

/* Big Bang */

int WINAPI WinMain(HINSTANCE hInst,HINSTANCE hPrevInst,LPSTR lpCmdLine,int nShowCmd)
{
	WNDCLASSEX wClass;
	ZeroMemory(&wClass,sizeof(WNDCLASSEX));
	wClass.cbClsExtra=NULL;
	wClass.cbSize=sizeof(WNDCLASSEX);
	wClass.cbWndExtra=NULL;
	wClass.hbrBackground=(HBRUSH)COLOR_WINDOW;
	wClass.hCursor=LoadCursor(NULL,IDC_ARROW);
	wClass.hIcon=LoadIcon(hInst, MAKEINTRESOURCE(IDI_ICON1));
	wClass.hIconSm=NULL;
	wClass.hInstance=hInst;
	wClass.lpfnWndProc=(WNDPROC)WinProc;
	wClass.lpszClassName="Window Class";
	wClass.lpszMenuName=NULL;
	wClass.style=CS_HREDRAW|CS_VREDRAW;

	if(!RegisterClassEx(&wClass))
	{
		int nResult = GetLastError();
		MessageBox(NULL, "Window class creation failed\r\n", "Window Class Failed", MB_ICONERROR);
	}
	char windowName[256];
	LoadString(hInst, IDS_APP_TITLE, windowName, 256);

	HWND hWnd=CreateWindowEx(NULL,
			"Window Class",
			windowName,
			WS_OVERLAPPED | WS_DLGFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU,
			200,
			200,
			460, // laius
			120, // korgus
			NULL,
			NULL,
			hInst,
			NULL);

	if(!hWnd)
	{
		int nResult = GetLastError();
		MessageBox(NULL, "Window creation failed\r\n", "Window Creation Failed", MB_ICONERROR);
	}

    ShowWindow(hWnd,nShowCmd);

	nidApp.cbSize = sizeof(NOTIFYICONDATA); 
	nidApp.hWnd = hWnd;
	nidApp.uID = 106;
	nidApp.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP; 
	nidApp.hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_ICON1)); 
	nidApp.uCallbackMessage = WM_NOT_ID;

	LoadString(hInst, IDS_APP_TITLE, nidApp.szTip, 256);

	Shell_NotifyIcon(NIM_ADD, &nidApp);

	MSG msg;
	ZeroMemory(&msg,sizeof(MSG));

	while(GetMessage(&msg,NULL,0,0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
		
	}

	return 0;
}

LRESULT CALLBACK WinProc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	switch(msg)
	{
		case WM_CREATE:
		{

			HGDIOBJ hfDefault=GetStockObject(DEFAULT_GUI_FONT);

			HWND hStaticText =  CreateWindowEx(
				NULL,
				"STATIC",
				"Wait...",
				WS_CHILD | WS_VISIBLE | SS_LEFT, 
				10,
				50, 
				100,
				40,
				hWnd, 
				NULL, 
				GetModuleHandle(NULL), 
				NULL);

			SendMessage(hStaticText, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE,0));

			_beginthread(LiveInfoMessage , 0 , hStaticText);

			// Create a push button
			
			hWndButton = CreateWindowEx(NULL,
				"BUTTON",
				"CONNECT",
				WS_TABSTOP|WS_VISIBLE|
				WS_CHILD|BS_DEFPUSHBUTTON,
				220,
				10,
				100,
				24,
				hWnd,
				(HMENU)IDC_MAIN_BUTTON,
				GetModuleHandle(NULL),
				NULL);

			SendMessage(hWndButton, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE,0));

			HWND hWndClose = CreateWindowEx(NULL,
				"BUTTON",
				"QUIT",
				WS_TABSTOP|WS_VISIBLE|
				WS_CHILD|BS_DEFPUSHBUTTON,
				330,
				10,
				100,
				24,
				hWnd,
				(HMENU)IDC_MAIN_BUTTON_CLOSE,
				GetModuleHandle(NULL),
				NULL);

			SendMessage(hWndClose, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE,0));

			HWND hWndHide = CreateWindowEx(NULL,
				"BUTTON",
				"HIDE",
				WS_TABSTOP|WS_VISIBLE|
				WS_CHILD|BS_DEFPUSHBUTTON,
				330,
				50,
				100,
				24,
				hWnd,
				(HMENU)IDC_MAIN_BUTTON_HIDE,
				GetModuleHandle(NULL),
				NULL);

			SendMessage(hWndHide, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE,0));

			hWndComboBox = CreateWindowEx(NULL, 
				"COMBOBOX",
				"", 
				CBS_DROPDOWN | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE,
				10, 
				10, 
				80, 
				200, 
				hWnd, 
				NULL, 
				GetModuleHandle(NULL),
				NULL);

			SendMessage(hWndComboBox, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE,0));

			for(int co=0; co < 7; co++)
			{
				SendMessage(hWndComboBox,(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM) comPorts[co]);
			}
			SendMessage(hWndComboBox, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);

			hWndComboBox2 = CreateWindowEx(NULL, 
				"COMBOBOX",
				"", 
				CBS_DROPDOWN | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE,
				100, 
				10, 
				80, 
				200, 
				hWnd, 
				NULL, 
				GetModuleHandle(NULL),
				NULL);

			SendMessage(hWndComboBox2, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE,0));

			for(int bit=0; bit < 7; bit++)
			{
				SendMessage(hWndComboBox2,(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM) comBitra[bit]);
			}

			SendMessage(hWndComboBox2, CB_SETCURSEL, (WPARAM)5, (LPARAM)0);

		}
		break;

		case WM_COMMAND:
			switch(LOWORD(wParam))
            {
				case IDC_MAIN_BUTTON:
				{
					if(!readyToSend){
						int ItemIndex1 = SendMessage(hWndComboBox, (UINT) CB_GETCURSEL, (WPARAM) 0, (LPARAM) 0);
						int ItemIndex2 = SendMessage(hWndComboBox2, (UINT) CB_GETCURSEL, (WPARAM) 0, (LPARAM) 0);

						if(!(Serial.Open((ItemIndex1+1), atoi(comBitra[ItemIndex2])))){
							MessageBox(NULL, "Some sort of error!", "Alert", MB_ICONERROR);
						}else{
							readyToSend = 1;
							SendMessage(hWndButton, WM_SETTEXT, (WPARAM)0, (LPARAM)"DISCONNECT");
						}
					}else{
						Serial.Close();
						readyToSend = 0;
						SendMessage(hWndButton, WM_SETTEXT, (WPARAM)0, (LPARAM)"CONNECT");
					}
				}
				break;

				case IDC_MAIN_BUTTON_CLOSE:
				{
					Serial.Close();

					Shell_NotifyIcon(NIM_DELETE,&nidApp);
					PostQuitMessage(0);
					
					return 0;
				}
				break;

				case IDC_MAIN_BUTTON_HIDE:
				{
					ShowWindow(hWnd, SW_MINIMIZE);
					ShowWindow(hWnd, SW_HIDE);
				}
				break;
			}
			break;
		case WM_NOT_ID:
		{
			switch(LOWORD(lParam))
			{
				case WM_LBUTTONDBLCLK:
				{
					ShowWindow(hWnd, SW_SHOW);
					ShowWindow(hWnd, SW_SHOWNORMAL);
				}
				break;
			}
		}
		break;

		case WM_SYSCOMMAND:
		{
			switch(LOWORD(wParam))
			{
				case SC_MINIMIZE:
				{
					//CloseWindow(hWnd);
				}
				break;
			}
		}
		break;

		case WM_DESTROY:
		{
			Shell_NotifyIcon(NIM_DELETE,&nidApp);
			PostQuitMessage(0);
			return 0;
		}
		break;
	}

	return DefWindowProc(hWnd,msg,wParam,lParam);
}

void LiveInfoMessage(void *hWnd){

	MEMORYSTATUSEX memInfo;
	HWND hwLocal = (HWND)hWnd;

	cpuUsageInit();

	while(1){

		memInfo.dwLength = sizeof(MEMORYSTATUSEX);
		GlobalMemoryStatusEx(&memInfo);

		int mPU			= memInfo.ullAvailPhys/(1024*1024);
		int cpuUsage	= cpuUsageValue();
		
		if(readyToSend){
			SerialWrite(cpuUsage, mPU);
		}

		char bfr[256];
		sprintf(bfr, "RAM: %i Mb\nCPU: %d %%", mPU, cpuUsage);
		
		SendMessage(hwLocal, WM_SETTEXT, NULL, (LPARAM) bfr);

		Sleep(1000);
	}
}

void cpuUsageInit(){
    PdhOpenQuery(NULL, NULL, &cpuQuery);
    PdhAddCounter(cpuQuery, "\\Processor(_Total)\\% Processor Time", NULL, &cpuTotal);
    PdhCollectQueryData(cpuQuery);
}

int cpuUsageValue(){
    PDH_FMT_COUNTERVALUE counterVal;

    PdhCollectQueryData(cpuQuery);
    PdhGetFormattedCounterValue(cpuTotal, PDH_FMT_LONG, NULL, &counterVal);
    return counterVal.longValue;
}

void SerialWrite(byte cpuUsage, int mPU){

		byte msb		= memUsageMSB(mPU);
		byte lsb		= memUsageLSB(mPU);

		// 0
		Serial.WriteCommByte(0xfa);
		// 1
		Serial.WriteCommByte(cpuUsage);
		// 2
		Serial.WriteCommByte(msb);
		// 3
		Serial.WriteCommByte(lsb);
		// 4
		Serial.WriteCommByte(0x6c);
}

int memUsageLSB( int n ){
	int res = 0;

	int len = 8;

	for(int i=0; i < len; i++){
		if(1 & (n >> i)){
			res |= 1 << i;
		}else{
			res |= 0 << i;
		}
	}

	return res;
}

int memUsageMSB( int n ){
	int res = 0;

	int len = 15;

	for(int i=8; i < len; i++){
		if(1 & (n >> i)){
			res |= 1 << (i-8);
		}else{
			res |= 0 << (i-8);
		}
	}

	return res;
}