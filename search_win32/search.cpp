
#define UNICODE
#define _UNICODE
#include <windows.h>
#include <string>
#include <fstream>
#include <cstring>
#include <cwchar>
#include <vector>
#include <algorithm>

#pragma comment(lib,"user32.lib")
#pragma comment(lib,"gdi32.lib")

int BUTTON_WIDTH = 80;
int BUTTON_HEIGHT = 24;
int BUTTON_FONT_SIZE = 8;
int BUTTON_GAP_X = 6;
int BUTTON_GAP_Y = 6;
int WINDOW_PADDING = 10;
int TARGET_SIZE = 16;
bool AUTO_ENTER = false;

int DEFAULT_X = 100;
int DEFAULT_Y = 100;

const wchar_t* CONFIG_FILE = L"search_conf.txt";

const wchar_t* DEFAULT_TEXTS[6] = {
    L"1",L"2",L"3",
    L"4",L"5",L"6"
};

HWND g_mainWnd = nullptr;
HWND g_targetWnd = nullptr;
HWND g_buttons[6] = {};
HWND g_editWnd = nullptr;

bool g_dragging = false;
POINT g_dragOffset = { 0,0 };

HFONT g_buttonFont = nullptr;

std::wstring g_texts[6];

const int ID_BUTTON_BASE = 3000;
const int ID_RESET = 4000;


std::string WideToUTF8(const std::wstring& text)
{
    if (text.empty())return "";

    int len = WideCharToMultiByte(
        CP_UTF8, 0,
        text.c_str(), -1,
        nullptr, 0, nullptr, nullptr);

    if (len <= 0)return "";

    std::vector<char> buffer(len);

    WideCharToMultiByte(
        CP_UTF8, 0,
        text.c_str(), -1,
        buffer.data(), len,
        nullptr, nullptr);

    return std::string(buffer.data());
}

std::wstring UTF8ToWide(const std::string& text)
{
    if (text.empty())return L"";

    int len = MultiByteToWideChar(
        CP_UTF8, 0,
        text.c_str(), -1,
        nullptr, 0);

    if (len <= 0)return L"";

    std::vector<wchar_t> buffer(len);

    MultiByteToWideChar(
        CP_UTF8, 0,
        text.c_str(), -1,
        buffer.data(), len);

    return std::wstring(buffer.data());
}


void SaveConfig()
{
    std::ofstream file("search_conf.txt", std::ios::binary);
    if (!file)return;

    unsigned char bom[3] = { 0xEF,0xBB,0xBF };
    file.write((char*)bom, 3);

    file << "WIDTH=" << BUTTON_WIDTH << "\n";
    file << "HEIGHT=" << BUTTON_HEIGHT << "\n";
    file << "FONT_SIZE=" << BUTTON_FONT_SIZE << "\n";
    file << "GAP_X=" << BUTTON_GAP_X << "\n";
    file << "GAP_Y=" << BUTTON_GAP_Y << "\n";
    file << "PADDING=" << WINDOW_PADDING << "\n";
    file << "TARGET_SIZE=" << TARGET_SIZE << "\n";
    file << "AUTO_ENTER=" << (AUTO_ENTER ? 1 : 0) << "\n";
    file << "TARGET_X=" << DEFAULT_X << "\n";
    file << "TARGET_Y=" << DEFAULT_Y << "\n";

    for (int i = 0; i < 6; i++)
    {
        file << "TEXT" << i + 1 << "=";
        file << WideToUTF8(g_texts[i]);
        file << "\n";
    }
}


void LoadConfig()
{
    for (int i = 0; i < 6; i++)
        g_texts[i] = DEFAULT_TEXTS[i];

    std::ifstream file("search_conf.txt", std::ios::binary);

    if (!file)
    {
        for (int i = 0; i < 6; i++)
            g_texts[i] = DEFAULT_TEXTS[i];

        SaveConfig();
        return;
    }

    std::string data(
        (std::istreambuf_iterator<char>(file)),
        std::istreambuf_iterator<char>());

    if (data.size() >= 3 &&
        (unsigned char)data[0] == 0xEF &&
        (unsigned char)data[1] == 0xBB &&
        (unsigned char)data[2] == 0xBF)
    {
        data.erase(0, 3);
    }

    size_t start = 0;

    while (start < data.size())
    {
        size_t end = data.find('\n', start);

        if (end == std::string::npos)
            end = data.size();

        std::string line = data.substr(
            start,
            end - start);

        while (!line.empty() &&
            (line.back() == '\r' ||
                line.back() == '\n'))
        {
            line.pop_back();
        }

        size_t equal = line.find('=');

        if (equal != std::string::npos)
        {
            std::string key =
                line.substr(0, equal);

            std::string value =
                line.substr(equal + 1);

            if (key == "WIDTH")
            {
                int v = atoi(value.c_str());
                if (v > 10)BUTTON_WIDTH = v;
            }
            else if (key == "HEIGHT")
            {
                int v = atoi(value.c_str());
                if (v > 10)BUTTON_HEIGHT = v;
            }
            else if (key == "FONT_SIZE")
            {
                int v = atoi(value.c_str());
                if (v > 1)BUTTON_FONT_SIZE = v;
            }
            else if (key == "GAP_X")
            {
                int v = atoi(value.c_str());
                if (v >= 0)BUTTON_GAP_X = v;
            }
            else if (key == "GAP_Y")
            {
                int v = atoi(value.c_str());
                if (v >= 0)BUTTON_GAP_Y = v;
            }
            else if (key == "PADDING")
            {
                int v = atoi(value.c_str());
                if (v >= 0)WINDOW_PADDING = v;
            }
            else if (key == "TARGET_SIZE")
            {
                int v = atoi(value.c_str());
                if (v > 2)TARGET_SIZE = v;
            }
            else if (key == "AUTO_ENTER")
            {
                AUTO_ENTER = (atoi(value.c_str()) != 0);
            }
            else if (key == "TARGET_X")
            {
                DEFAULT_X = atoi(value.c_str());
            }
            else if (key == "TARGET_Y")
            {
                DEFAULT_Y = atoi(value.c_str());
            }
            else if (key == "TEXT1")
            {
                g_texts[0] = UTF8ToWide(value);
            }
            else if (key == "TEXT2")
            {
                g_texts[1] = UTF8ToWide(value);
            }
            else if (key == "TEXT3")
            {
                g_texts[2] = UTF8ToWide(value);
            }
            else if (key == "TEXT4")
            {
                g_texts[3] = UTF8ToWide(value);
            }
            else if (key == "TEXT5")
            {
                g_texts[4] = UTF8ToWide(value);
            }
            else if (key == "TEXT6")
            {
                g_texts[5] = UTF8ToWide(value);
            }
        }

        if (end == data.size())
            break;

        start = end + 1;
    }
}


void UpdateButtonTexts()
{
    for (int i = 0; i < 6; i++)
    {
        if (g_buttons[i])
        {
            SetWindowTextW(
                g_buttons[i],
                g_texts[i].c_str());
        }
    }
}


void ResetTargetPosition()
{
    RECT rc;

    if (!GetWindowRect(g_mainWnd, &rc))
        return;

    int x =
        rc.right -
        WINDOW_PADDING -
        TARGET_SIZE;

    int y =
        rc.bottom -
        WINDOW_PADDING -
        TARGET_SIZE;

    DEFAULT_X = x;
    DEFAULT_Y = y;

    SetWindowPos(
        g_targetWnd,
        HWND_TOPMOST,
        DEFAULT_X,
        DEFAULT_Y,
        TARGET_SIZE,
        TARGET_SIZE,
        SWP_NOACTIVATE);

    ShowWindow(
        g_targetWnd,
        SW_SHOWNOACTIVATE);

    SaveConfig();
}

// 编辑按钮

void FinishEdit(bool save)
{
    if (!g_editWnd)return;

    int index =
        (int)(INT_PTR)
        GetPropW(
            g_editWnd,
            L"ButtonIndex");

    if (index >= 0 && index < 6)
    {
        if (save)
        {
            int len =
                GetWindowTextLengthW(
                    g_editWnd);

            std::vector<wchar_t> buffer(
                len + 1);

            GetWindowTextW(
                g_editWnd,
                buffer.data(),
                len + 1);

            g_texts[index] = buffer.data();

            SetWindowTextW(
                g_buttons[index],
                g_texts[index].c_str());

            SaveConfig();
        }
    }

    RemovePropW(
        g_editWnd,
        L"ButtonIndex");

    DestroyWindow(g_editWnd);

    g_editWnd = nullptr;
}

void StartEdit(int index)
{
    if (index < 0 || index >= 6)
        return;

    if (g_editWnd)
        FinishEdit(true);

    RECT rc;

    GetWindowRect(
        g_buttons[index],
        &rc);

    POINT p1 = {
        rc.left,
        rc.top
    };

    POINT p2 = {
        rc.right,
        rc.bottom
    };

    ScreenToClient(
        g_mainWnd,
        &p1);

    ScreenToClient(
        g_mainWnd,
        &p2);

    g_editWnd = CreateWindowExW(
        WS_EX_CLIENTEDGE,
        L"EDIT",
        g_texts[index].c_str(),
        WS_CHILD |
        WS_VISIBLE |
        ES_AUTOHSCROLL,
        p1.x,
        p1.y,
        p2.x - p1.x,
        p2.y - p1.y,
        g_mainWnd,
        nullptr,
        GetModuleHandleW(nullptr),
        nullptr);

    if (!g_editWnd)
        return;

    SetPropW(
        g_editWnd,
        L"ButtonIndex",
        (HANDLE)(INT_PTR)index);

    SendMessageW(
        g_editWnd,
        EM_SETSEL,
        0,
        -1);

    SetFocus(g_editWnd);
}



void ExecuteSearch(int index)
{
    if (index < 0 || index >= 6)
        return;

    if (g_editWnd)
        FinishEdit(true);

    POINT oldMouse;

    GetCursorPos(&oldMouse);

    RECT rc;

    GetWindowRect(
        g_targetWnd,
        &rc);

    int x =
        (rc.left + rc.right) / 2;

    int y =
        (rc.top + rc.bottom) / 2;

    ShowWindow(
        g_targetWnd,
        SW_HIDE);

    SetCursorPos(x, y);

    INPUT click[2] = {};

    click[0].type = INPUT_MOUSE;
    click[0].mi.dwFlags =
        MOUSEEVENTF_LEFTDOWN;

    click[1].type = INPUT_MOUSE;
    click[1].mi.dwFlags =
        MOUSEEVENTF_LEFTUP;

    SendInput(
        2,
        click,
        sizeof(INPUT));

    Sleep(20);

    INPUT ctrlA[4] = {};

    ctrlA[0].type = INPUT_KEYBOARD;
    ctrlA[0].ki.wVk = VK_CONTROL;

    ctrlA[1].type = INPUT_KEYBOARD;
    ctrlA[1].ki.wVk = 'A';

    ctrlA[2].type = INPUT_KEYBOARD;
    ctrlA[2].ki.wVk = 'A';
    ctrlA[2].ki.dwFlags =
        KEYEVENTF_KEYUP;

    ctrlA[3].type = INPUT_KEYBOARD;
    ctrlA[3].ki.wVk = VK_CONTROL;
    ctrlA[3].ki.dwFlags =
        KEYEVENTF_KEYUP;

    SendInput(
        4,
        ctrlA,
        sizeof(INPUT));

    Sleep(20);

    if (OpenClipboard(nullptr))
    {
        EmptyClipboard();

        std::wstring text =
            g_texts[index];

        SIZE_T bytes =
            (text.size() + 1) *
            sizeof(wchar_t);

        HGLOBAL hMem =
            GlobalAlloc(
                GMEM_MOVEABLE,
                bytes);

        if (hMem)
        {
            void* ptr =
                GlobalLock(hMem);

            if (ptr)
            {
                memcpy(
                    ptr,
                    text.c_str(),
                    bytes);

                GlobalUnlock(hMem);

                SetClipboardData(
                    CF_UNICODETEXT,
                    hMem);
            }
            else
            {
                GlobalFree(hMem);
            }
        }

        CloseClipboard();
    }

    INPUT paste[4] = {};

    paste[0].type = INPUT_KEYBOARD;
    paste[0].ki.wVk = VK_CONTROL;

    paste[1].type = INPUT_KEYBOARD;
    paste[1].ki.wVk = 'V';

    paste[2].type = INPUT_KEYBOARD;
    paste[2].ki.wVk = 'V';
    paste[2].ki.dwFlags =
        KEYEVENTF_KEYUP;

    paste[3].type = INPUT_KEYBOARD;
    paste[3].ki.wVk = VK_CONTROL;
    paste[3].ki.dwFlags =
        KEYEVENTF_KEYUP;

    SendInput(
        4,
        paste,
        sizeof(INPUT));

    if (AUTO_ENTER)
    {
        Sleep(20);

        INPUT enter[2] = {};

        enter[0].type = INPUT_KEYBOARD;
        enter[0].ki.wVk = VK_RETURN;

        enter[1].type = INPUT_KEYBOARD;
        enter[1].ki.wVk = VK_RETURN;
        enter[1].ki.dwFlags =
            KEYEVENTF_KEYUP;

        SendInput(
            2,
            enter,
            sizeof(INPUT));
    }

    SetCursorPos(
        oldMouse.x,
        oldMouse.y);

    ShowWindow(
        g_targetWnd,
        SW_SHOWNOACTIVATE);

    SetWindowPos(
        g_targetWnd,
        HWND_TOPMOST,
        DEFAULT_X,
        DEFAULT_Y,
        TARGET_SIZE,
        TARGET_SIZE,
        SWP_NOACTIVATE);

    SaveConfig();
}

//红点窗口
LRESULT CALLBACK TargetWndProc(
    HWND hwnd,
    UINT msg,
    WPARAM wParam,
    LPARAM lParam)
{
    switch (msg)
    {
    case WM_LBUTTONDOWN:
        g_dragging = true;

        g_dragOffset.x =
            (short)LOWORD(lParam);

        g_dragOffset.y =
            (short)HIWORD(lParam);

        SetCapture(hwnd);

        return 0;

    case WM_MOUSEMOVE:
        if (g_dragging)
        {
            POINT pt;

            GetCursorPos(&pt);

            DEFAULT_X =
                pt.x - g_dragOffset.x;

            DEFAULT_Y =
                pt.y - g_dragOffset.y;

            SetWindowPos(
                hwnd,
                HWND_TOPMOST,
                DEFAULT_X,
                DEFAULT_Y,
                0,
                0,
                SWP_NOSIZE |
                SWP_NOACTIVATE);
        }

        return 0;

    case WM_LBUTTONUP:
        g_dragging = false;

        ReleaseCapture();

        SaveConfig();

        return 0;

    case WM_PAINT:
    {
        PAINTSTRUCT ps;

        HDC hdc =
            BeginPaint(hwnd, &ps);

        RECT rc;

        GetClientRect(
            hwnd,
            &rc);

        HBRUSH bg =
            CreateSolidBrush(
                RGB(40, 40, 40));

        FillRect(
            hdc,
            &rc,
            bg);

        DeleteObject(bg);

        HBRUSH red =
            CreateSolidBrush(
                RGB(255, 0, 0));

        int w =
            rc.right - rc.left;

        int h =
            rc.bottom - rc.top;

        Ellipse(
            hdc,
            1,
            1,
            w - 1,
            h - 1);

        DeleteObject(red);

        EndPaint(hwnd, &ps);

        return 0;
    }
    }

    return DefWindowProcW(
        hwnd,
        msg,
        wParam,
        lParam);
}

//主窗口

LRESULT CALLBACK MainWndProc(
    HWND hwnd,
    UINT msg,
    WPARAM wParam,
    LPARAM lParam)
{
    switch (msg)
    {
    case WM_CREATE:
    {
        g_buttonFont =
            CreateFontW(
                -BUTTON_FONT_SIZE * 2,
                0,
                0,
                0,
                FW_NORMAL,
                FALSE,
                FALSE,
                FALSE,
                DEFAULT_CHARSET,
                OUT_DEFAULT_PRECIS,
                CLIP_DEFAULT_PRECIS,
                DEFAULT_QUALITY,
                DEFAULT_PITCH |
                FF_DONTCARE,
                L"Microsoft YaHei");

        for (int i = 0; i < 6; i++)
        {
            int row = i / 3;
            int col = i % 3;

            int x =
                WINDOW_PADDING +
                col *
                (BUTTON_WIDTH +
                    BUTTON_GAP_X);

            int y =
                WINDOW_PADDING +
                row *
                (BUTTON_HEIGHT +
                    BUTTON_GAP_Y);

            g_buttons[i] =
                CreateWindowExW(
                    0,
                    L"BUTTON",
                    g_texts[i].c_str(),
                    WS_CHILD |
                    WS_VISIBLE |
                    BS_PUSHBUTTON,
                    x,
                    y,
                    BUTTON_WIDTH,
                    BUTTON_HEIGHT,
                    hwnd,
                    (HMENU)(INT_PTR)
                    (ID_BUTTON_BASE + i),
                    GetModuleHandleW(nullptr),
                    nullptr);

            SendMessageW(
                g_buttons[i],
                WM_SETFONT,
                (WPARAM)g_buttonFont,
                TRUE);
        }

        int resetY =
            WINDOW_PADDING +
            2 *
            (BUTTON_HEIGHT +
                BUTTON_GAP_Y);

        CreateWindowExW(
            0,
            L"BUTTON",
            L"reset",
            WS_CHILD |
            WS_VISIBLE |
            BS_PUSHBUTTON,
            WINDOW_PADDING,
            resetY,
            BUTTON_WIDTH,
            BUTTON_HEIGHT,
            hwnd,
            (HMENU)(INT_PTR)ID_RESET,
            GetModuleHandleW(nullptr),
            nullptr);

        return 0;
    }

    case WM_COMMAND:
    {
        int id =
            LOWORD(wParam);

        int code =
            HIWORD(wParam);

        if (id >= ID_BUTTON_BASE &&
            id < ID_BUTTON_BASE + 6)
        {
            int index =
                id - ID_BUTTON_BASE;

            if (code == BN_CLICKED)
            {
                ExecuteSearch(index);
                return 0;
            }
        }

        if (id == ID_RESET &&
            code == BN_CLICKED)
        {
            ResetTargetPosition();
            return 0;
        }

        return 0;
    }

    case WM_CONTEXTMENU:
    {
        HWND clicked =
            (HWND)wParam;

        for (int i = 0; i < 6; i++)
        {
            if (clicked == g_buttons[i])
            {
                StartEdit(i);
                return 0;
            }
        }

        return 0;
    }

    case WM_SIZE:
    {
        int resetY =
            WINDOW_PADDING +
            2 *
            (BUTTON_HEIGHT +
                BUTTON_GAP_Y);

        HWND resetWnd =
            GetDlgItem(
                hwnd,
                ID_RESET);

        if (resetWnd)
        {
            SetWindowPos(
                resetWnd,
                nullptr,
                WINDOW_PADDING,
                resetY,
                BUTTON_WIDTH,
                BUTTON_HEIGHT,
                SWP_NOZORDER);
        }

        return 0;
    }

    case WM_MOVE:
    {
        // 主窗口移动后不自动移动红点。
        // 红点仍然保持独立位置。
        return 0;
    }

    case WM_DESTROY:
        if (g_editWnd)
            FinishEdit(true);

        SaveConfig();

        if (g_buttonFont)
            DeleteObject(
                g_buttonFont);

        PostQuitMessage(0);

        return 0;
    }

    return DefWindowProcW(
        hwnd,
        msg,
        wParam,
        lParam);
}


int WINAPI wWinMain(
    HINSTANCE hInstance,
    HINSTANCE,
    PWSTR,
    int nCmdShow)
{
    LoadConfig();

    WNDCLASSW mainClass = {};

    mainClass.lpfnWndProc =
        MainWndProc;

    mainClass.hInstance =
        hInstance;

    mainClass.lpszClassName =
        L"QuickSearchMainClass";

    mainClass.hCursor =
        LoadCursorW(
            nullptr,
            IDC_ARROW);

    mainClass.hbrBackground =
        (HBRUSH)(COLOR_WINDOW + 1);

    RegisterClassW(
        &mainClass);

    WNDCLASSW targetClass = {};

    targetClass.lpfnWndProc =
        TargetWndProc;

    targetClass.hInstance =
        hInstance;

    targetClass.lpszClassName =
        L"QuickSearchTargetClass";

    targetClass.hCursor =
        LoadCursorW(
            nullptr,
            IDC_SIZEALL);

    RegisterClassW(
        &targetClass);

    int mainWidth =
        WINDOW_PADDING * 2 +
        BUTTON_WIDTH * 3 +
        BUTTON_GAP_X * 2 +
        20;

    int mainHeight =
        WINDOW_PADDING * 2 +
        BUTTON_HEIGHT * 3 +
        BUTTON_GAP_Y * 2 +
        40;

    g_mainWnd =
        CreateWindowExW(
            WS_EX_TOPMOST |
            WS_EX_TOOLWINDOW,
            L"QuickSearchMainClass",
            L"search",
            WS_OVERLAPPED |
            WS_CAPTION |
            WS_SYSMENU,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            mainWidth,
            mainHeight,
            nullptr,
            nullptr,
            hInstance,
            nullptr);

    if (!g_mainWnd)
        return 0;

    g_targetWnd =
        CreateWindowExW(
            WS_EX_TOPMOST |
            WS_EX_TOOLWINDOW,
            L"QuickSearchTargetClass",
            L"",
            WS_POPUP,
            DEFAULT_X,
            DEFAULT_Y,
            TARGET_SIZE,
            TARGET_SIZE,
            nullptr,
            nullptr,
            hInstance,
            nullptr);

    if (!g_targetWnd)
        return 0;

    ShowWindow(
        g_mainWnd,
        nCmdShow);

    UpdateWindow(
        g_mainWnd);

    ShowWindow(
        g_targetWnd,
        SW_SHOWNOACTIVATE);

    SetWindowPos(
        g_targetWnd,
        HWND_TOPMOST,
        DEFAULT_X,
        DEFAULT_Y,
        TARGET_SIZE,
        TARGET_SIZE,
        SWP_NOACTIVATE);

    MSG msg = {};

    while (GetMessageW(
        &msg,
        nullptr,
        0,
        0))
    {
        if (g_editWnd &&
            msg.message == WM_KEYDOWN)
        {
            if (msg.wParam == VK_RETURN)
            {
                FinishEdit(true);
                continue;
            }

            if (msg.wParam == VK_ESCAPE)
            {
                FinishEdit(false);
                continue;
            }
        }

        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    return 0;
}
