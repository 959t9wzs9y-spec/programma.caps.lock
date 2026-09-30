// programma.ciulla.cpp
// Notificatore Caps Lock nativo Win32: compatibile con Windows 10 e Windows 11.
// Compilazione MSVC:
//   cl /std:c++17 /O2 /W4 /DUNICODE /D_UNICODE programma.ciulla.cpp user32.lib gdi32.lib shell32.lib advapi32.lib
// Compilazione MinGW-w64:
//   g++ -std=c++17 -O2 -municode programma.ciulla.cpp -o CapsLockNotifier.exe -mwindows -luser32 -lgdi32 -lshell32 -ladvapi32

#define UNICODE
#define _UNICODE
#include <windows.h>
#include <shellapi.h>
#include <shlobj.h>
#include <string>
#include <algorithm>

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "advapi32.lib")

namespace {
constexpr wchar_t kClassName[] = L"CapsLockNotifier.MainWindow";
constexpr wchar_t kPopupClassName[] = L"CapsLockNotifier.Popup";
constexpr wchar_t kRegistryKey[] = L"Software\\CapsLockNotifier";
constexpr wchar_t kRunKey[] = L"Software\\Microsoft\\Windows\\CurrentVersion\\Run";
constexpr wchar_t kRunValue[] = L"CapsLockNotifier";
constexpr UINT WM_TRAY = WM_APP + 1;
constexpr UINT_PTR TIMER_CAPS = 1;
constexpr UINT_PTR TIMER_POPUP = 2;
constexpr UINT ID_SAVE = 100;
constexpr UINT ID_MESSAGE = 101;
constexpr UINT ID_DURATION = 102;
constexpr UINT ID_SOUND = 103;
constexpr UINT ID_AUTOSTART = 104;
constexpr UINT ID_TRAY_SHOW = 200;
constexpr UINT ID_TRAY_EXIT = 201;

constexpr COLORREF kBgColor = RGB(245, 247, 250);
constexpr COLORREF kHeaderColor = RGB(30, 82, 165);
constexpr COLORREF kAccentColor = RGB(255, 136, 64);
constexpr COLORREF kTextColor = RGB(18, 23, 31);
constexpr COLORREF kSubTextColor = RGB(80, 90, 110);

struct Settings {
    std::wstring message = L"CAPS LOCK ATTIVO!";
    DWORD durationSeconds = 3;
    bool sound = true;
    bool autostart = false;
};

HINSTANCE g_instance = nullptr;
HWND g_main = nullptr;
HWND g_popup = nullptr;
HWND g_message = nullptr;
HWND g_duration = nullptr;
HWND g_sound = nullptr;
HWND g_autostart = nullptr;
NOTIFYICONDATAW g_tray{};
Settings g_settings;
bool g_previousCaps = false;
HFONT g_titleFont = nullptr;
HFONT g_normalFont = nullptr;
HFONT g_smallFont = nullptr;

std::wstring AppPath() {
    wchar_t path[MAX_PATH]{};
    DWORD length = GetModuleFileNameW(nullptr, path, MAX_PATH);
    return std::wstring(path, length);
}

void LoadSettings() {
    HKEY key = nullptr;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, kRegistryKey, 0, KEY_READ, &key) != ERROR_SUCCESS) return;
    wchar_t message[512]{};
    DWORD type = 0, bytes = sizeof(message);
    if (RegQueryValueExW(key, L"Message", nullptr, &type, reinterpret_cast<BYTE*>(message), &bytes) == ERROR_SUCCESS && type == REG_SZ)
        g_settings.message = message;
    DWORD value = 0, size = sizeof(value);
    if (RegQueryValueExW(key, L"Duration", nullptr, &type, reinterpret_cast<BYTE*>(&value), &size) == ERROR_SUCCESS)
        g_settings.durationSeconds = std::clamp<DWORD>(value, 1, 60);
    size = sizeof(value);
    if (RegQueryValueExW(key, L"Sound", nullptr, &type, reinterpret_cast<BYTE*>(&value), &size) == ERROR_SUCCESS) g_settings.sound = value != 0;
    size = sizeof(value);
    if (RegQueryValueExW(key, L"Autostart", nullptr, &type, reinterpret_cast<BYTE*>(&value), &size) == ERROR_SUCCESS) g_settings.autostart = value != 0;
    RegCloseKey(key);
}

void ConfigureAutostart(bool enabled) {
    HKEY key = nullptr;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, kRunKey, 0, KEY_SET_VALUE, &key) != ERROR_SUCCESS) return;
    if (enabled) {
        std::wstring command = L"\"" + AppPath() + L"\"";
        RegSetValueExW(key, kRunValue, 0, REG_SZ, reinterpret_cast<const BYTE*>(command.c_str()), static_cast<DWORD>((command.size() + 1) * sizeof(wchar_t)));
    } else {
        RegDeleteValueW(key, kRunValue);
    }
    RegCloseKey(key);
}

void SaveSettings() {
    wchar_t text[512]{};
    GetWindowTextW(g_message, text, ARRAYSIZE(text));
    g_settings.message = text[0] ? text : L"CAPS LOCK ATTIVO!";
    g_settings.durationSeconds = std::clamp<DWORD>(static_cast<DWORD>(GetDlgItemInt(g_main, ID_DURATION, nullptr, FALSE)), 1, 60);
    g_settings.sound = SendMessageW(g_sound, BM_GETCHECK, 0, 0) == BST_CHECKED;
    g_settings.autostart = SendMessageW(g_autostart, BM_GETCHECK, 0, 0) == BST_CHECKED;

    HKEY key = nullptr;
    if (RegCreateKeyExW(HKEY_CURRENT_USER, kRegistryKey, 0, nullptr, 0, KEY_SET_VALUE, nullptr, &key, nullptr) == ERROR_SUCCESS) {
        RegSetValueExW(key, L"Message", 0, REG_SZ, reinterpret_cast<const BYTE*>(g_settings.message.c_str()), static_cast<DWORD>((g_settings.message.size() + 1) * sizeof(wchar_t)));
        RegSetValueExW(key, L"Duration", 0, REG_DWORD, reinterpret_cast<const BYTE*>(&g_settings.durationSeconds), sizeof(DWORD));
        DWORD sound = g_settings.sound ? 1 : 0;
        DWORD autostart = g_settings.autostart ? 1 : 0;
        RegSetValueExW(key, L"Sound", 0, REG_DWORD, reinterpret_cast<const BYTE*>(&sound), sizeof(DWORD));
        RegSetValueExW(key, L"Autostart", 0, REG_DWORD, reinterpret_cast<const BYTE*>(&autostart), sizeof(DWORD));
        RegCloseKey(key);
    }
    ConfigureAutostart(g_settings.autostart);
    MessageBoxW(g_main, L"Impostazioni salvate correttamente.", L"Caps Lock Notifier", MB_OK | MB_ICONINFORMATION);
}

void PositionPopup() {
    RECT work{};
    SystemParametersInfoW(SPI_GETWORKAREA, 0, &work, 0);
    RECT popup{};
    GetWindowRect(g_popup, &popup);
    const int margin = 24;
    SetWindowPos(g_popup, HWND_TOPMOST, work.right - (popup.right - popup.left) - margin,
                 work.bottom - (popup.bottom - popup.top) - margin, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE | SWP_SHOWWINDOW);
}

void ShowPopup() {
    SetWindowTextW(g_popup, g_settings.message.c_str());
    PositionPopup();
    SetTimer(g_main, TIMER_POPUP, g_settings.durationSeconds * 1000, nullptr);
    if (g_settings.sound) MessageBeep(MB_ICONEXCLAMATION);
}

void HidePopup() {
    KillTimer(g_main, TIMER_POPUP);
    ShowWindow(g_popup, SW_HIDE);
}

void CheckCapsLock() {
    const bool active = (GetKeyState(VK_CAPITAL) & 1) != 0;
    if (active && !g_previousCaps) ShowPopup();
    g_previousCaps = active;
}

void AddLabel(HWND parent, const wchar_t* text, int x, int y, int width, int height, HFONT font = nullptr) {
    HWND label = CreateWindowW(L"STATIC", text, WS_CHILD | WS_VISIBLE, x, y, width, height, parent, nullptr, g_instance, nullptr);
    if (font) SendMessageW(label, WM_SETFONT, reinterpret_cast<WPARAM>(font), TRUE);
    SetWindowTextW(label, text);
}

void PaintMainWindow(HWND hwnd) {
    PAINTSTRUCT ps{};
    HDC dc = BeginPaint(hwnd, &ps);
    RECT rect{};
    GetClientRect(hwnd, &rect);

    HBRUSH bgBrush = CreateSolidBrush(kBgColor);
    FillRect(dc, &rect, bgBrush);
    DeleteObject(bgBrush);

    RECT header{};
    header.left = 0;
    header.top = 0;
    header.right = rect.right;
    header.bottom = 70;

    HBRUSH headerBrush = CreateSolidBrush(kHeaderColor);
    FillRect(dc, &header, headerBrush);
    DeleteObject(headerBrush);

    SetTextColor(dc, RGB(255, 255, 255));
    SetBkMode(dc, TRANSPARENT);
    SelectObject(dc, g_titleFont);
    RECT titleRect = header;
    titleRect.left += 18;
    titleRect.top += 18;
    DrawTextW(dc, L"Caps Lock Notifier", -1, &titleRect, DT_LEFT | DT_SINGLELINE);

    HBRUSH accentBrush = CreateSolidBrush(kAccentColor);
    RECT accentRect = rect;
    accentRect.top = 70;
    accentRect.bottom = 74;
    FillRect(dc, &accentRect, accentBrush);
    DeleteObject(accentBrush);

    EndPaint(hwnd, &ps);
}

void CreateMainControls(HWND hwnd) {
    AddLabel(hwnd, L"Messaggio da mostrare quando Caps Lock viene attivato:", 24, 92, 512, 22, g_smallFont);
    g_message = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", g_settings.message.c_str(),
                                WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL | WS_TABSTOP,
                                24, 118, 512, 32, hwnd,
                                reinterpret_cast<HMENU>(ID_MESSAGE), g_instance, nullptr);
    SendMessageW(g_message, WM_SETFONT, reinterpret_cast<WPARAM>(g_normalFont), TRUE);
    SetWindowTextW(g_message, g_settings.message.c_str());

    AddLabel(hwnd, L"Durata della notifica (secondi, 1-60):", 24, 168, 300, 22, g_smallFont);
    g_duration = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", std::to_wstring(g_settings.durationSeconds).c_str(),
                                 WS_CHILD | WS_VISIBLE | ES_NUMBER | WS_TABSTOP,
                                 24, 194, 96, 32, hwnd,
                                 reinterpret_cast<HMENU>(ID_DURATION), g_instance, nullptr);
    SendMessageW(g_duration, WM_SETFONT, reinterpret_cast<WPARAM>(g_normalFont), TRUE);

    g_sound = CreateWindowW(L"BUTTON", L"Riproduci un suono", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_AUTOCHECKBOX,
                           24, 240, 240, 28, hwnd, reinterpret_cast<HMENU>(ID_SOUND), g_instance, nullptr);
    SendMessageW(g_sound, BM_SETCHECK, g_settings.sound ? BST_CHECKED : BST_UNCHECKED, 0);
    SendMessageW(g_sound, WM_SETFONT, reinterpret_cast<WPARAM>(g_normalFont), TRUE);

    g_autostart = CreateWindowW(L"BUTTON", L"Avvia automaticamente con Windows", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_AUTOCHECKBOX,
                               24, 274, 360, 28, hwnd, reinterpret_cast<HMENU>(ID_AUTOSTART), g_instance, nullptr);
    SendMessageW(g_autostart, BM_SETCHECK, g_settings.autostart ? BST_CHECKED : BST_UNCHECKED, 0);
    SendMessageW(g_autostart, WM_SETFONT, reinterpret_cast<WPARAM>(g_normalFont), TRUE);

    HWND save = CreateWindowW(L"BUTTON", L"Salva impostazioni", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_DEFPUSHBUTTON,
                             24, 316, 200, 38, hwnd, reinterpret_cast<HMENU>(ID_SAVE), g_instance, nullptr);
    SendMessageW(save, WM_SETFONT, reinterpret_cast<WPARAM>(g_titleFont), TRUE);
}

void DrawGradient(HDC hdc, const RECT& rect, COLORREF color1, COLORREF color2) {
    const int height = rect.bottom - rect.top;
    for (int y = 0; y < height; ++y) {
        const double t = height > 1 ? static_cast<double>(y) / (height - 1) : 0.0;
        const int r = static_cast<int>(GetRValue(color1) + (GetRValue(color2) - GetRValue(color1)) * t);
        const int g = static_cast<int>(GetGValue(color1) + (GetGValue(color2) - GetGValue(color1)) * t);
        const int b = static_cast<int>(GetBValue(color1) + (GetBValue(color2) - GetBValue(color1)) * t);

        HPEN pen = CreatePen(PS_SOLID, 1, RGB(r, g, b));
        SelectObject(hdc, pen);
        MoveToEx(hdc, rect.left, rect.top + y, nullptr);
        LineTo(hdc, rect.right, rect.top + y);
        DeleteObject(pen);
    }
}

LRESULT CALLBACK PopupProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    if (message == WM_PAINT) {
        PAINTSTRUCT ps{};
        HDC dc = BeginPaint(hwnd, &ps);
        RECT rect{};
        GetClientRect(hwnd, &rect);

        HBRUSH fillBrush = CreateSolidBrush(RGB(255, 136, 64));
        FillRect(dc, &rect, fillBrush);
        DeleteObject(fillBrush);

        RECT inner = rect;
        InflateRect(&inner, -6, -6);
        HBRUSH innerBrush = CreateSolidBrush(RGB(255, 150, 80));
        FillRect(dc, &inner, innerBrush);
        DeleteObject(innerBrush);

        SetBkMode(dc, TRANSPARENT);
        SetTextColor(dc, RGB(255, 255, 255));
        SelectObject(dc, g_titleFont);

        RECT textRect = rect;
        textRect.left += 18;
        textRect.right -= 18;
        textRect.top += 12;
        textRect.bottom -= 12;
        DrawTextW(dc, g_settings.message.c_str(), -1, &textRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

        EndPaint(hwnd, &ps);
        return 0;
    }
    if (message == WM_NCHITTEST) return HTTRANSPARENT;
    return DefWindowProcW(hwnd, message, wParam, lParam);
}

void ShowContextMenu() {
    POINT point{};
    GetCursorPos(&point);
    HMENU menu = CreatePopupMenu();
    AppendMenuW(menu, MF_STRING, ID_TRAY_SHOW, L"Apri configurazione");
    AppendMenuW(menu, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(menu, MF_STRING, ID_TRAY_EXIT, L"Esci");
    SetForegroundWindow(g_main);
    TrackPopupMenu(menu, TPM_RIGHTBUTTON, point.x, point.y, 0, g_main, nullptr);
    DestroyMenu(menu);
}

LRESULT CALLBACK MainProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    if (message == WM_CREATE) {
        CreateMainControls(hwnd);
        SetTimer(hwnd, TIMER_CAPS, 150, nullptr);
        return 0;
    }
    if (message == WM_PAINT) {
        PaintMainWindow(hwnd);
        return 0;
    }
    if (message == WM_ERASEBKGND) {
        return TRUE;
    }
    if (message == WM_TIMER) {
        if (wParam == TIMER_CAPS) CheckCapsLock();
        else if (wParam == TIMER_POPUP) HidePopup();
        return 0;
    }
    if (message == WM_COMMAND) {
        if (LOWORD(wParam) == ID_SAVE) {
            SaveSettings();
            InvalidateRect(g_popup, nullptr, TRUE);
            return 0;
        }
        if (LOWORD(wParam) == ID_TRAY_SHOW) {
            ShowWindow(hwnd, SW_SHOW);
            SetForegroundWindow(hwnd);
            return 0;
        }
        if (LOWORD(wParam) == ID_TRAY_EXIT) {
            DestroyWindow(hwnd);
            return 0;
        }
    }
    if (message == WM_CLOSE) {
        ShowWindow(hwnd, SW_HIDE);
        return 0;
    }
    if (message == WM_DESTROY) {
        KillTimer(hwnd, TIMER_CAPS);
        KillTimer(hwnd, TIMER_POPUP);
        Shell_NotifyIconW(NIM_DELETE, &g_tray);
        PostQuitMessage(0);
        return 0;
    }
    if (message == WM_TRAY && lParam == WM_RBUTTONUP) {
        ShowContextMenu();
        return 0;
    }
    if (message == WM_TRAY && lParam == WM_LBUTTONDBLCLK) {
        ShowWindow(hwnd, SW_SHOW);
        SetForegroundWindow(hwnd);
        return 0;
    }
    if (message == WM_CTLCOLOREDIT || message == WM_CTLCOLORSTATIC || message == WM_CTLCOLORBTN) {
        HDC hdc = reinterpret_cast<HDC>(wParam);
        SetTextColor(hdc, kTextColor);
        SetBkColor(hdc, RGB(255, 255, 255));
        return reinterpret_cast<LRESULT>(CreateSolidBrush(RGB(255, 255, 255)));
    }
    return DefWindowProcW(hwnd, message, wParam, lParam);
}
}

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, PWSTR, int show) {
    g_instance = instance;
    LoadSettings();

    g_normalFont = CreateFontW(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI");
    g_smallFont = CreateFontW(14, 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI");
    g_titleFont = CreateFontW(20, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI");

    WNDCLASSW mainClass{};
    mainClass.lpfnWndProc = MainProc;
    mainClass.hInstance = instance;
    mainClass.lpszClassName = kClassName;
    mainClass.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    mainClass.hbrBackground = nullptr;
    RegisterClassW(&mainClass);

    WNDCLASSW popupClass{};
    popupClass.lpfnWndProc = PopupProc;
    popupClass.hInstance = instance;
    popupClass.lpszClassName = kPopupClassName;
    popupClass.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    popupClass.hbrBackground = nullptr;
    RegisterClassW(&popupClass);

    g_main = CreateWindowW(kClassName, L"Caps Lock Notifier",
                          WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
                          CW_USEDEFAULT, CW_USEDEFAULT, 580, 430, nullptr, nullptr, instance, nullptr);

    g_popup = CreateWindowExW(WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_NOACTIVATE, kPopupClassName, L"",
                             WS_POPUP, 0, 0, 420, 120, nullptr, nullptr, instance, nullptr);

    g_tray.cbSize = sizeof(g_tray);
    g_tray.hWnd = g_main;
    g_tray.uID = 1;
    g_tray.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
    g_tray.uCallbackMessage = WM_TRAY;
    g_tray.hIcon = LoadIconW(nullptr, IDI_WARNING);
    wcscpy_s(g_tray.szTip, ARRAYSIZE(g_tray.szTip), L"Caps Lock Notifier");
    Shell_NotifyIconW(NIM_ADD, &g_tray);

    ShowWindow(g_main, show == SW_HIDE ? SW_HIDE : SW_SHOW);
    UpdateWindow(g_main);

    MSG message{};
    while (GetMessageW(&message, nullptr, 0, 0) > 0) {
        TranslateMessage(&message);
        DispatchMessageW(&message);
    }

    if (g_titleFont) DeleteObject(g_titleFont);
    if (g_smallFont) DeleteObject(g_smallFont);
    if (g_normalFont) DeleteObject(g_normalFont);

    return static_cast<int>(message.wParam);
}
