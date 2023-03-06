#include <Windows.h>
#include <iostream>

HHOOK keyboardHook;

const int WindowWidth = 800;
const int WindowHeight = 600;
const int Speed = 5;

const int Square_SizeX = 25;
const int Square_SizeY = 25;
static int SquareX = WindowWidth / 2;
static int SquareY = WindowHeight / 2;

static int DirectionX = 1;
static int DirectionY = 1;

const int Paddle_SizeX = 25;
const int Paddle_SizeY = 100;
const int PaddleX = 50;
static int PaddleY = WindowHeight / 2;

const int Paddle2_SizeX = 25;
const int Paddle2_SizeY = 100;
const int Paddle2X = 750;
static int Paddle2Y = WindowHeight / 2;

static bool Up = false;
static bool Down = false;

// Declare callback function for the keyboard hook
LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode >= 0) {
        if (wParam == WM_KEYDOWN) {
            KBDLLHOOKSTRUCT* pKeyboardStruct = (KBDLLHOOKSTRUCT*)lParam;
            if (pKeyboardStruct->vkCode == 'W') {
                Up = true;
            }
            else if (pKeyboardStruct->vkCode == 'S') {
                Down = true;
            }
        }
        else if (wParam == WM_KEYUP) {
            KBDLLHOOKSTRUCT* pKeyboardStruct = (KBDLLHOOKSTRUCT*)lParam;
            if (pKeyboardStruct->vkCode == 'W') {
                Up = false;
            }
            else if (pKeyboardStruct->vkCode == 'S') {
                Down = false;
            }
        }
    }

    return CallNextHookEx(keyboardHook, nCode, wParam, lParam);
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{

    switch (uMsg)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        // Set background color to black
        SetBkColor(hdc, RGB(0, 0, 0));
        ExtTextOut(hdc, 0, 0, ETO_OPAQUE, &ps.rcPaint, NULL, 0, NULL);

        // Draw the square
        HBRUSH hBrush = CreateSolidBrush(RGB(255, 255, 255));
        RECT rect = { SquareX - Square_SizeX / 2, SquareY - Square_SizeY / 2, SquareX + Square_SizeX / 2, SquareY + Square_SizeY / 2 };
        RECT paddle = { PaddleX - Paddle_SizeX / 2, PaddleY - Paddle_SizeY / 2, PaddleX + Paddle_SizeX / 2, PaddleY + Paddle_SizeY / 2 };
        RECT paddle2 = { Paddle2X - Paddle2_SizeX / 2, Paddle2Y - Paddle2_SizeY / 2, Paddle2X + Paddle2_SizeX / 2, Paddle2Y + Paddle2_SizeY / 2 };
        hBrush = CreateSolidBrush(RGB(255, 255, 255));
        FillRect(hdc, &rect, hBrush);
        FillRect(hdc, &paddle, hBrush);
        FillRect(hdc, &paddle2, hBrush);
        DeleteObject(hBrush);

        EndPaint(hwnd, &ps);
        break;
    }
    case WM_TIMER:

        // Move the paddle up and down
        // Up
        if (Up) {
            PaddleY -= Speed;
        }
        // Down
        else if (Down) {
            PaddleY += Speed;
        }
        // Check if the paddle is in the roof
        if (PaddleY - 50 <= 0 && !Down) {
            PaddleY = 0 + 50;
        }
        // Check if the paddle is in the ground
        else if (PaddleY + 90 >= WindowHeight && !Up) {
            PaddleY = WindowHeight - 90;
        }

        // Move the square up and down

        // Up
        if (DirectionY == 0) {
            SquareY -= Speed;
            Paddle2Y = SquareY;
        }
        // Down
        else if (DirectionY == 1) {
            SquareY += Speed;
            Paddle2Y = SquareY;
        }

        // Check if the circle hits the the top or bottom
        // Top
        if (SquareY <= 0) {
            DirectionY = 1;
        }
        // Bottom
        else if (SquareY - Square_SizeY >= WindowHeight - Square_SizeY * 3) {
            DirectionY = 0;
        }

        // Move the square left and right
        // Left
        if (DirectionX == 0) {
            SquareX -= Speed;
        }
        // Right
        else if (DirectionX == 1) {
            SquareX += Speed;
        }

        // Check if the player hits the left or right walls
        // Left
        if (SquareX <= 0) {
            DestroyWindow(hwnd);
            std::cout << "Bro, you suck." << std::endl;
            return 0;
        }
        // Right
        else if (SquareX - Square_SizeX >= WindowWidth - Square_SizeX * 2) {
            DestroyWindow(hwnd);
            std::cout << "HOW THE HELL DID THIS HAPPEN?!" << std::endl;
            return 0;
        }

        // Check for collison with the paddles.
        // Left paddle
        if (SquareX + Square_SizeX >= PaddleX &&
            SquareX <= PaddleX + Paddle_SizeX &&
            SquareY + Square_SizeY >= PaddleY &&
            SquareY <= PaddleY + Paddle_SizeY) {
            DirectionX = 1;
        }
        // Right paddle
        if (SquareX <= Paddle2X + Paddle2_SizeY &&
            SquareX + Square_SizeX >= Paddle2X &&
            SquareY + Square_SizeY >= Paddle2Y &&
            SquareY <= Paddle2Y + Paddle2_SizeX) {
            DirectionX = 0;
        }
       

        // Redraw the window to show the new circle position
        InvalidateRect(hwnd, NULL, TRUE);
        break;
    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    return 0;
}

int main() {
    // Set up the keyboard hook
    keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, NULL, 0);

    // Register the window class.
    const TCHAR CLASS_NAME[] = TEXT("Pong");

    WNDCLASS wc = {};

    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    // Create the window.
    HWND hwnd = CreateWindowEx(
        0,
        CLASS_NAME,
        TEXT("Pong"),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, WindowWidth, WindowHeight,
        NULL,
        NULL,
        GetModuleHandle(NULL),
        NULL
    );

    if (hwnd == NULL)
    {
        return 0;
    }

    // Show the window.
    ShowWindow(hwnd, SW_SHOWDEFAULT);
    UpdateWindow(hwnd);

    // Start the timer for moving the circle
    SetTimer(hwnd, 1, 25, NULL);

    // Run the message loop.
    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return static_cast<int>(msg.wParam);
}