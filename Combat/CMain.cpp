#pragma comment(lib, "winmm")
#pragma comment(lib, "comctl32.lib")
#include <math.h>
#include <iostream>
#include <cstdio>
#include <cstring>
#include <algorithm>
#include <string>
#include <vector>
#include <wtypes.h>
#include <windowsx.h>
#include <winbase.h>
#include <windows.h>
#include <winuser.h>
#include <tchar.h>
#include <psapi.h>
#include <time.h>
#include <dos.h>
#include <conio.h>
#include "Function.h"
#include "resource.h"
#include <vector>
using namespace std;


LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM) ;
//窗口消息处理程序	（窗口句柄，消息类型，消息数据1，消息数据2）


int WINAPI WinMain (HINSTANCE	hInstance,
					HINSTANCE	hPrevInstance,
					PSTR		szCmdLine,
					int			iCmdShow)
{
	static	int	cxClient, cyClient;
	RECT	rectMain;
	POINT	pTemp;



	static		TCHAR szAppName[] = TEXT ("Combat") ;
	HWND		hwnd ;
	MSG			msg ;
	WNDCLASS	wndclass ;
	wndclass.style			= CS_HREDRAW | CS_VREDRAW ;

	wndclass.lpfnWndProc	= WndProc ;
	wndclass.cbClsExtra		= 0 ;
	wndclass.cbWndExtra		= 0 ;
	wndclass.hInstance		= hInstance ;
	wndclass.hIcon			= LoadIcon (NULL, IDI_APPLICATION) ;
	wndclass.hCursor		= LoadCursor (NULL, IDC_ARROW) ;
	wndclass.hbrBackground	= (HBRUSH) GetStockObject (WHITE_BRUSH) ;
	wndclass.lpszMenuName	= NULL ;
	wndclass.lpszClassName	= szAppName ;

	if (!RegisterClass (&wndclass))
	{
		MessageBox (NULL,
			TEXT ("This program requires Windows NT!"),
			szAppName,
			MB_ICONERROR) ;
		return 0 ;
	}

	hwnd	= CreateWindow(	szAppName,					// window class name
		TEXT ("Combat"),				// window caption
		WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_CAPTION,		// window style
		0,				// initial x position
		0,				// initial y position
		CW_USEDEFAULT,				// initial x size
		CW_USEDEFAULT,				// initial y size
		NULL,						// parent window handle
		NULL,						// window menu handle
		hInstance,					// program instance handle
		NULL) ;						// creation parameters
	ShowWindow (hwnd, iCmdShow) ;
	UpdateWindow (hwnd) ;

	GetClientRect(hwnd, &rectMain);
	cxClient	= rectMain.right - rectMain.left;
	cyClient	= rectMain.bottom- rectMain.top;
	pTemp.x		= cxClient/3;
	pTemp.y		= cyClient/3;
	while (GetMessage (&msg, NULL, 0, 0))
	{
		TranslateMessage (&msg) ;
		DispatchMessage (&msg) ;
	}
	return msg.wParam ;
}


LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static	PLAYER	pPlayer;
	static	vector <AMMO>	AmmoVec;


	static	HDC		hDC, hMemDC;
	static	int		iTime;
	static	BOOL	bCursorState;
	RECT			rectMain;

	static	HBITMAP	hBitmap, hOldBitmap;


	PAINTSTRUCT		ps ;
	POINT			pTemp;

	static	HINSTANCE	hInstance ;
	static	RECT	rect ;

	static	int		cxChar, cyChar;
	static	int		cxClient, cyClient;	//窗口宽度和高度
	static	BOOL	bPause;
	static	BOOL	bGameOver;
	static	BOOL	bShownInfo;
	int	i, j;


	RECT	rectText;
	static	RECT	hrect;

	TCHAR	szStr[128];


	switch (message)
	{
	case WM_CREATE:
		SendMessage(hwnd,
			WM_SETICON,
			(WPARAM)TRUE,
			(LPARAM)LoadIcon(GetModuleHandle(NULL),
			(LPCTSTR)IDI_ICON1));
	//	MessageBox(hwnd, "Combat v0.6.0，冰火梦幻个人作品。", "欢迎试用", MB_OK);
		//initialization
		SetTimer(hwnd, TIMER_SEC,	TIME_REFRESH, NULL) ;
		SetTimer(hwnd, TIMER_PLAYER_AMMO, TIME_PLAYER_FIRE, NULL);
		GetClientRect (hwnd, &rect) ;	//
		cxChar	= LOWORD(GetDialogBaseUnits());
		cyChar	= HIWORD(GetDialogBaseUnits());
		cxClient= rect.right-rect.left;
		cyClient= rect.bottom-rect.top;

		bCursorState	= TRUE;
		pPlayer			= PLAYER(32, 32, 1, 0, cxClient/2, cyClient-32, 5, 5, "Player.bmp");
		AmmoVec.clear();
		return 0;
	case WM_TIMER:
		switch (wParam)
		{
		case TIMER_SEC:
			GetClientRect(hwnd, &rectMain);

			//获得2个前台DC//////////////////////////////////////
			hDC		= GetDC(hwnd);
			hMemDC	= CreateCompatibleDC(hDC);

			hBitmap	= CreateCompatibleBitmap(hDC, rectMain.right, rectMain.bottom);
			hOldBitmap	= (HBITMAP)SelectObject(hMemDC, hBitmap);
			/////////////////////////////////
			pPlayer.Draw(hDC, hMemDC);
			pPlayer.Move(cxClient, cyClient);
			for (i=0 ; i<AmmoVec.size() ; i++)
			{
				AmmoVec[i].Draw(hDC, hMemDC);
				if (AmmoVec[i].Move(cxClient, cyClient))
				{
					AmmoVec.erase(AmmoVec.begin()+i);
				}
			}
			//////Draw over
			BitBlt(hDC,	0,	0,	cxClient,	cyClient,
				hMemDC,	0, 0, SRCCOPY);


			SelectObject(hMemDC, hOldBitmap);
			DeleteDC(hMemDC);
			ReleaseDC(hwnd, hDC);
			DeleteObject(hBitmap);
			break;
		case	TIMER_PLAYER_AMMO:
			if (pPlayer.iHP > 0 && pPlayer.bFiring)
			{
				AmmoVec.push_back(AMMO(8, 8, 0, 1,
					pPlayer.cxPosition-8,
					pPlayer.cyPosition-10,
					-10, -15, "Ammo0.bmp"));
				AmmoVec.push_back(AMMO(8, 37, 0, 1,
					pPlayer.cxPosition,
					pPlayer.cyPosition-16,
					0, -15, "Ammo2.bmp"));
				AmmoVec.push_back(AMMO(8, 8, 0, 1,
					pPlayer.cxPosition+8,
					pPlayer.cyPosition-10,
					+10, -15, "Ammo0.bmp"));
				PlaySound("Ammo0.wav", NULL, SND_FILENAME | SND_ASYNC);
			}
			break;
		}
		return 0;
	case	WM_KEYDOWN:
		switch (wParam)
		{
		case	VK_LEFT:
			pPlayer.bMoveLeft	= TRUE;
			pPlayer.bMoveRight	= FALSE;
			break;
		case	VK_RIGHT:
			pPlayer.bMoveRight	= TRUE;
			pPlayer.bMoveLeft	= FALSE;
			break;
		case	VK_UP:
			pPlayer.bMoveUp		= TRUE;
			pPlayer.bMoveDown	= FALSE;
			break;
		case	VK_DOWN:
			pPlayer.bMoveDown	= TRUE;
			pPlayer.bMoveUp		= FALSE;
			break;
		case	'P':
			break;
		case	VK_CONTROL:
			if (FALSE == pPlayer.bFiring)
			{
				pPlayer.bFiring	= TRUE;
				if (pPlayer.iHP > 0 && pPlayer.bFiring)
				{
					AmmoVec.push_back(AMMO(8, 8, 0, 1,
						pPlayer.cxPosition-8,
						pPlayer.cyPosition-10,
						-5, -12, "Ammo0.bmp"));
					AmmoVec.push_back(AMMO(21, 105, 0, 1,
						pPlayer.cxPosition,
						pPlayer.cyPosition-16,
						0, -10, "Ammo1.bmp"));
					AmmoVec.push_back(AMMO(8, 8, 0, 1,
						pPlayer.cxPosition+8,
						pPlayer.cyPosition-10,
						+5, -12, "Ammo0.bmp"));
					PlaySound("Ammo0.wav", NULL, SND_FILENAME | SND_ASYNC);
				}
			}
			break;
		case	VK_ESCAPE:
			PostQuitMessage (0) ;
			break;
		case	VK_RETURN:
			break;
		}
		return 0;
	case	WM_KEYUP:
		switch (wParam)
		{
		case	VK_LEFT:
			pPlayer.bMoveLeft	= FALSE;
			break;
		case	VK_RIGHT:
			pPlayer.bMoveRight	= FALSE;
			break;
		case	VK_UP:
			pPlayer.bMoveUp		= FALSE;
			break;
		case	VK_DOWN:
			pPlayer.bMoveDown	= FALSE;
			break;
		case	VK_CONTROL:
			pPlayer.bFiring		= FALSE;
			break;
		}
		return 0;
	case   WM_COMMAND :
		return 0;
	case	WM_LBUTTONUP:
		return 0;
	case	WM_RBUTTONUP:
		return 0;
	case	WM_SIZE:
		cxClient = LOWORD (lParam) ;
		cyClient = HIWORD (lParam) ;
		rect.right	= rect.left + cxClient;
		rect.bottom	= rect.top + cyClient;
		return 0 ;
	case   WM_PAINT:
		hDC = BeginPaint (hwnd, &ps) ;
		EndPaint (hwnd, &ps) ;
		return 0 ;

	case WM_NCMOUSEMOVE:
		{
			if(bCursorState == FALSE)
			{
				bCursorState = TRUE;
				ShowCursor(TRUE);
			}
		}
		break;
	case WM_MOUSEMOVE:
		{
			if(bCursorState == TRUE)
			{
				bCursorState = FALSE;
				ShowCursor(FALSE);
			}
			// 然后做别的事情
		}
		break;
		// 其他的case项目就不用变了

	case   WM_DESTROY:
		KillTimer(hwnd, TIMER_SEC);
		KillTimer(hwnd, TIMER_PLAYER_AMMO);
		PostQuitMessage (0) ;
		return 0 ;
	}
	return DefWindowProc (hwnd, message, wParam, lParam) ;
}