#include "caps_monitor.h"

#include <windows.h>
#include <string>
#include <algorithm>

namespace {
constexpr wchar_t kRegistryKey[] = L"Software\\CapsLockNotifier";
constexpr wchar_t kRunKey[] = L"Software\\Microsoft\\Windows\\CurrentVersion\\Run";
constexpr wchar_t kRunValue[] = L"CapsLockNotifier";

std::wstring AppPath() {
    wchar_t path[MAX_PATH]{};
    DWORD length = GetModuleFileNameW(nullptr, path, MAX_PATH);
    return std::wstring(path, length);
}

void ConfigureAutostart(bool enabled) {
    HKEY key = nullptr;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, kRunKey, 0, KEY_SET_VALUE, &key) != ERROR_SUCCESS) {
        return;
    }

    if (enabled) {
        std::wstring command = L"\"" + AppPath() + L"\"";
        RegSetValueExW(key, kRunValue, 0, REG_SZ,
            reinterpret_cast<const BYTE*>(command.c_str()),
            static_cast<DWORD>((command.size() + 1) * sizeof(wchar_t)));
    } else {
        RegDeleteValueW(key, kRunValue);
    }

    RegCloseKey(key);
}

} // namespace

namespace caps {

CapsMonitor::CapsMonitor() : settings_() {
    HKEY key = nullptr;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, kRegistryKey, 0, KEY_READ, &key) != ERROR_SUCCESS) {
        return;
    }

    wchar_t message[512]{};
    DWORD type = 0;
    DWORD size = sizeof(message);
    if (RegQueryValueExW(key, L"Message", nullptr, &type,
                         reinterpret_cast<BYTE*>(message), &size) == ERROR_SUCCESS && type == REG_SZ) {
        settings_.message = message;
    }

    DWORD value = 0;
    size = sizeof(value);
    if (RegQueryValueExW(key, L"Duration", nullptr, &type,
                         reinterpret_cast<BYTE*>(&value), &size) == ERROR_SUCCESS) {
        settings_.durationSeconds = std::max(1, std::min(60, static_cast<int>(value)));
    }

    size = sizeof(value);
    if (RegQueryValueExW(key, L"Sound", nullptr, &type,
                         reinterpret_cast<BYTE*>(&value), &size) == ERROR_SUCCESS) {
        settings_.sound = value != 0;
    }

    size = sizeof(value);
    if (RegQueryValueExW(key, L"Autostart", nullptr, &type,
                         reinterpret_cast<BYTE*>(&value), &size) == ERROR_SUCCESS) {
        settings_.autostart = value != 0;
    }

    RegCloseKey(key);
}

CapsMonitor::~CapsMonitor() {
    stop();
}

void CapsMonitor::start() {
    running_ = true;
}

void CapsMonitor::stop() {
    running_ = false;
}

bool CapsMonitor::isRunning() const {
    return running_;
}

bool CapsMonitor::isCapsLockActive() const {
    return (GetKeyState(VK_CAPITAL) & 1) != 0;
}

Settings CapsMonitor::loadSettings() const {
    return settings_;
}

void CapsMonitor::saveSettings(const Settings& settings) {
    settings_ = settings;

    HKEY key = nullptr;
    if (RegCreateKeyExW(HKEY_CURRENT_USER, kRegistryKey, 0, nullptr, 0,
                        KEY_SET_VALUE, nullptr, &key, nullptr) != ERROR_SUCCESS) {
        return;
    }

    RegSetValueExW(key, L"Message", 0, REG_SZ,
                   reinterpret_cast<const BYTE*>(settings_.message.c_str()),
                   static_cast<DWORD>((settings_.message.size() + 1) * sizeof(wchar_t)));

    DWORD duration = static_cast<DWORD>(settings_.durationSeconds);
    RegSetValueExW(key, L"Duration", 0, REG_DWORD,
                   reinterpret_cast<const BYTE*>(&duration), sizeof(DWORD));

    DWORD sound = settings_.sound ? 1 : 0;
    RegSetValueExW(key, L"Sound", 0, REG_DWORD,
                   reinterpret_cast<const BYTE*>(&sound), sizeof(DWORD));

    DWORD autostart = settings_.autostart ? 1 : 0;
    RegSetValueExW(key, L"Autostart", 0, REG_DWORD,
                   reinterpret_cast<const BYTE*>(&autostart), sizeof(DWORD));

    RegCloseKey(key);
    ConfigureAutostart(settings_.autostart);
}

std::wstring CapsMonitor::getCurrentMessage() const {
    return settings_.message;
}

} // namespace caps
