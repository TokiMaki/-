#include "GameClient.h"
#include "Timer.h"

GameClient gameClient;

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam);
DWORD WINAPI TestThread(LPVOID arg);

HINSTANCE g_hinst;
HWND hDlg;

//int main() {
//    while (1) {
 //       gameClient.Update();
  //  }
//}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	HWND 	hwnd;
	MSG 	msg;
	WNDCLASSEX	WndClass;
	g_hinst = hInstance;
	WndClass.cbSize = sizeof(WNDCLASSEX);
	WndClass.style = CS_HREDRAW | CS_VREDRAW;
	WndClass.lpfnWndProc = WndProc;
	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hInstance = hInstance;
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	WndClass.lpszMenuName = NULL;
	WndClass.lpszClassName = ("Window Class Name");
	WndClass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	RegisterClassEx(&WndClass);

	hwnd = CreateWindow(("Window Class Name"), ("Tetris"), WS_OVERLAPPEDWINDOW,
		0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, NULL, NULL, hInstance, NULL);

	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);

	CreateThread(NULL, 0, TestThread, (LPVOID)&gameClient, 0, NULL);

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return (int)msg.wParam;

    return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam){
	HDC hDC, memDC;
	PAINTSTRUCT ps;

	switch (iMsg) {
	case WM_CREATE:
		SetTimer(hWnd, 1, 1000 / 60, NULL);
		break;
	case WM_PAINT:
		hDC = BeginPaint(hWnd, &ps);
		memDC = CreateCompatibleDC(hDC);

		gameClient.Paint(hDC);
		//BitBlt(hDC, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, memDC, 0, 0, SRCCOPY);

		DeleteDC(memDC);
		DeleteDC(hDC);
		EndPaint(hWnd, &ps);
		break;
	case WM_TIMER:
		switch (wParam)
		{
		case 1:
			SendMessage(hWnd, WM_PAINT, wParam, lParam);
			break;
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}
	
	return(DefWindowProc(hWnd, iMsg, wParam, lParam));
}


DWORD __stdcall TestThread(LPVOID arg)
{
	GameClient* clientMain = (GameClient*)arg;
	while (1) {
		clientMain->Update();
	}
	return 0;
}
