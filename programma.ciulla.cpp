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

struct Settings {
    std::wstring message = L"⚠️  CAPS LOCK ATTIVO!";
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
HBRUSH g_bgBrush = nullptr;

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
    g_settings.message = text[0] ? text : L"⚠️  CAPS LOCK ATTIVO!";
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
    MessageBoxW(g_main, L"✅ Impostazioni salvate correttamente!", L"Caps Lock Notifier", MB_OK | MB_ICONINFORMATION);
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
}

void CreateMainControls(HWND hwnd) {
    AddLabel(hwnd, L"📝 Messaggio da mostrare quando Caps Lock viene attivato:", 24, 16, 512, 22, g_titleFont);
    g_message = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", g_settings.message.c_str(), 
                                WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL, 24, 42, 512, 32, hwnd, 
                                reinterpret_cast<HMENU>(ID_MESSAGE), g_instance, nullptr);
    SendMessageW(g_message, WM_SETFONT, reinterpret_cast<WPARAM>(g_normalFont), TRUE);

    AddLabel(hwnd, L"⏱️  Durata della notifica (secondi, 1-60):", 24, 86, 300, 22, g_titleFont);
    g_duration = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", std::to_wstring(g_settings.durationSeconds).c_str(), 
                                 WS_CHILD | WS_VISIBLE | ES_NUMBER, 24, 112, 80, 32, hwnd, 
                                 reinterpret_cast<HMENU>(ID_DURATION), g_instance, nullptr);
    SendMessageW(g_duration, WM_SETFONT, reinterpret_cast<WPARAM>(g_normalFont), TRUE);

    g_sound = CreateWindowW(L"BUTTON", L"🔊 Riproduci un suono", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 
                           24, 158, 250, 28, hwnd, reinterpret_cast<HMENU>(ID_SOUND), g_instance, nullptr);
    SendMessageW(g_sound, BM_SETCHECK, g_settings.sound ? BST_CHECKED : BST_UNCHECKED, 0);
    SendMessageW(g_sound, WM_SETFONT, reinterpret_cast<WPARAM>(g_normalFont), TRUE);

    g_autostart = CreateWindowW(L"BUTTON", L"🚀 Avvia automaticamente con Windows", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 
                               24, 194, 350, 28, hwnd, reinterpret_cast<HMENU>(ID_AUTOSTART), g_instance, nullptr);
    SendMessageW(g_autostart, BM_SETCHECK, g_settings.autostart ? BST_CHECKED : BST_UNCHECKED, 0);
    SendMessageW(g_autostart, WM_SETFONT, reinterpret_cast<WPARAM>(g_normalFont), TRUE);

    HWND save = CreateWindowW(L"BUTTON", L"💾 Salva impostazioni", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON, 
                             24, 240, 200, 36, hwnd, reinterpret_cast<HMENU>(ID_SAVE), g_instance, nullptr);
    SendMessageW(save, WM_SETFONT, reinterpret_cast<WPARAM>(g_titleFont), TRUE);
}

void DrawGradient(HDC hdc, const RECT& rect, COLORREF color1, COLORREF color2) {
    int steps = rect.bottom - rect.top;
    for (int i = 0; i < steps; i++) {
        int r1 = GetRValue(color1);
        int g1 = GetGValue(color1);
        int b1 = GetBValue(color1);
        int r2 = GetRValue(color2);
        int g2 = GetGValue(color2);
        int b2 = GetBValue(color2);

        int r = r1 + (r2 - r1) * i / steps;
        int g = g1 + (g2 - g1) * i / steps;
        int b = b1 + (b2 - b1) * i / steps;

        HPEN pen = CreatePen(PS_SOLID, 1, RGB(r, g, b));
        SelectObject(hdc, pen);
        MoveToEx(hdc, rect.left, rect.top + i, nullptr);
        LineTo(hdc, rect.right, rect.top + i);
        DeleteObject(pen);
    }
}

LRESULT CALLBACK PopupProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    if (message == WM_PAINT) {
        PAINTSTRUCT ps{}; 
        HDC dc = BeginPaint(hwnd, &ps); 
        RECT rect{}; 
        GetClientRect(hwnd, &rect);

        // Gradient background (dark red to orange)
        DrawGradient(dc, rect, RGB(220, 50, 50), RGB(240, 100, 50));

        // Border
        HBRUSH borderBrush = CreateSolidBrush(RGB(255, 150, 50));
        FrameRect(dc, &rect, borderBrush);
        DeleteObject(borderBrush);

        // Text
        SetBkMode(dc, TRANSPARENT);
        SetTextColor(dc, RGB(255, 255, 255));
        HFONT boldFont = CreateFontW(28, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI");
        SelectObject(dc, boldFont);
        DrawTextW(dc, g_settings.message.c_str(), -1, &rect, DT_CENTER | DT_VCENTER | DT_WORDBREAK | DT_SINGLELINE);
        DeleteObject(boldFont);

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
    AppendMenuW(menu, MF_STRING, ID_TRAY_SHOW, L"⚙️  Apri configurazione");
    AppendMenuW(menu, MF_SEPARATOR, 0, nullptr); 
    AppendMenuW(menu, MF_STRING, ID_TRAY_EXIT, L"❌ Esci");
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
    if (message == WM_CTLCOLERBTNFACE || message == WM_CTLCOLORSTATIC) {
        HDC dc = reinterpret_cast<HDC>(wParam);
        SetBkColor(dc, RGB(240, 240, 245));
        return reinterpret_cast<LRESULT>(CreateSolidBrush(RGB(240, 240, 245)));
    }
    return DefWindowProcW(hwnd, message, wParam, lParam);
}
}

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, PWSTR, int show) {
    g_instance = instance; 
    LoadSettings();

    // Create fonts
    g_normalFont = CreateFontW(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI");
    g_titleFont = CreateFontW(18, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI");

    WNDCLASSW mainClass{}; 
    mainClass.lpfnWndProc = MainProc; 
    mainClass.hInstance = instance; 
    mainClass.lpszClassName = kClassName; 
    mainClass.hCursor = LoadCursorW(nullptr, IDC_ARROW); 
    mainClass.hbrBackground = CreateSolidBrush(RGB(240, 240, 245));
    RegisterClassW(&mainClass);

    WNDCLASSW popupClass{}; 
    popupClass.lpfnWndProc = PopupProc; 
    popupClass.hInstance = instance; 
    popupClass.lpszClassName = kPopupClassName; 
    popupClass.hCursor = LoadCursorW(nullptr, IDC_ARROW); 
    popupClass.hbrBackground = nullptr;
    RegisterClassW(&popupClass);

    g_main = CreateWindowW(kClassName, L"⚙️  Caps Lock Notifier", 
                          WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, 
                          CW_USEDEFAULT, CW_USEDEFAULT, 580, 320, nullptr, nullptr, instance, nullptr);
    g_popup = CreateWindowExW(WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_NOACTIVATE, kPopupClassName, L"", 
                             WS_POPUP, 0, 0, 400, 100, nullptr, nullptr, instance, nullptr);

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
    if (g_normalFont) DeleteObject(g_normalFont);
    if (g_bgBrush) DeleteObject(g_bgBrush);

    return static_cast<int>(message.wParam);
}
