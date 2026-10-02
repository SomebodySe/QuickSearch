#include "searchmanager.h"
#include <QClipboard>
#include <QGuiApplication>

#include <windows.h>


// 模拟键盘按下
static void keyDown(WORD key)
{
    INPUT input = {};
    input.type = INPUT_KEYBOARD;
    input.ki.wVk = key;

    SendInput(
        1,
        &input,
        sizeof(INPUT));
}


// 模拟键盘松开
static void keyUp(WORD key)
{
    INPUT input = {};
    input.type = INPUT_KEYBOARD;
    input.ki.wVk = key;
    input.ki.dwFlags = KEYEVENTF_KEYUP;

    SendInput(
        1,
        &input,
        sizeof(INPUT));
}


// 模拟按下再松开
static void pressKey(WORD key)
{
    keyDown(key);
    keyUp(key);
}

static void pressCtrlKey(WORD key)
{
    keyDown(VK_CONTROL);
    pressKey(key);
    keyUp(VK_CONTROL);
}


// 模拟鼠标左键点击
static void leftClick()
{
    INPUT inputs[2] = {};

    inputs[0].type = INPUT_MOUSE;
    inputs[0].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;

    inputs[1].type = INPUT_MOUSE;
    inputs[1].mi.dwFlags = MOUSEEVENTF_LEFTUP;

    SendInput(
        2,
        inputs,
        sizeof(INPUT));
}


void SearchManager::executeSearch(
    const QString& text,
    const QPoint& targetPosition,
    bool autoEnter)
{
    // 记录当前鼠标位置
    POINT oldMousePosition{};

    GetCursorPos(&oldMousePosition);

    // 移动到红点
    SetCursorPos(
        targetPosition.x(),
        targetPosition.y());

    // 点击目标输入框
    leftClick();
    Sleep(50);

    // Ctrl+A
    pressCtrlKey('A');

    // 把按钮文字放入剪贴板
    QClipboard* clipboard =
        QGuiApplication::clipboard();

    clipboard->setText(text);

    // Ctrl+V
    pressCtrlKey('V');

    // 如果需要自动回车
    if (autoEnter)
    {
        pressKey(VK_RETURN);
    }

    // 恢复鼠标位置
    SetCursorPos(
        oldMousePosition.x,
        oldMousePosition.y);
}