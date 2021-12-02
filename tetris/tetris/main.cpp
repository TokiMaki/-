#include "GameClient.h"
#include "Timer.h"
#include "resource.h"

GameClient gameClient;

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam);
DWORD WINAPI UpdateThread(LPVOID arg);
DWORD WINAPI CallDrawMsgThread(LPVOID arg);

HINSTANCE g_hinst;
RECT rt;

//int main() {
//    while (1) {
 //       gameClient.Update();
  //  }
//}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	HWND hwnd;
	MSG msg;
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

	//CreateThread(NULL, 0, UpdateThread, (LPVOID)&gameClient, 0, NULL);
	CreateThread(NULL, 0, CallDrawMsgThread, (LPVOID)&hwnd, 0, NULL);

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
	HBITMAP hBitmap;
	BITMAP bmp[2];
	PAINTSTRUCT ps;

	switch (iMsg) {
	case WM_CREATE:
		GetClientRect(hWnd, &rt);
		gameClient.BlockBitmap=(HBITMAP)LoadBitmap(g_hinst, MAKEINTRESOURCE(IDB_BLOCK));
		GetObject(gameClient.BlockBitmap, sizeof(BITMAP), &bmp[0]);
		gameClient.UIBitmap = (HBITMAP)LoadBitmap(g_hinst, MAKEINTRESOURCE(IDB_UI));
		GetObject(gameClient.UIBitmap, sizeof(BITMAP), &bmp[1]);
		//SetTimer(hWnd, 1, 1000 / 60, NULL);
		break;
	case WM_PAINT:
		
		InvalidateRect(hWnd, &rt, FALSE);
		hDC = BeginPaint(hWnd, &ps);

		// 그래픽스 모드 변경
		SetGraphicsMode(hDC, GM_ADVANCED);
		gameClient.ScreenRotate(hDC);
		memDC = CreateCompatibleDC(hDC);

		hBitmap = CreateCompatibleBitmap(hDC, rt.right, rt.bottom);
		SelectObject(memDC, (HBITMAP)hBitmap);
		FillRect(memDC, &rt, (HBRUSH)GetStockObject(WHITE_BRUSH));
		
		gameClient.Paint(memDC);
		BitBlt(hDC,0 , 0, rt.right, rt.bottom, memDC, 0, 0, SRCCOPY);

		EndPaint(hWnd, &ps);
		DeleteObject(hBitmap);
		DeleteDC(memDC);
		break;
	case WM_KEYDOWN:
		gameClient.KeyDown(wParam);
		break;
	case WM_KEYUP:
		gameClient.KeyUp(wParam);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}

	return(DefWindowProc(hWnd, iMsg, wParam, lParam));
}

DWORD __stdcall UpdateThread(LPVOID arg)
{
	GameClient* clientMain = (GameClient*)arg;
	while (1) {
		gameClient.Update();
	}
	return 0;
}

DWORD __stdcall CallDrawMsgThread(LPVOID arg)
{
	CGameTimer DrawTimer;
	DrawTimer.Start();
	HWND* hWnd = (HWND*)arg;
	while (1) {
		DrawTimer.Tick(60.0f);
		SendMessage(*hWnd, WM_PAINT, NULL, NULL);
	}
	return 0;
}