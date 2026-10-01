#pragma once

#include <string>

namespace caps {

struct Settings {
    std::wstring message = L"CAPS LOCK ATTIVO!";
    int durationSeconds = 3;
    bool sound = true;
    bool autostart = false;
};

class CapsMonitor {
public:
    CapsMonitor();
    ~CapsMonitor();

    void start();
    void stop();
    bool isRunning() const;

    bool isCapsLockActive() const;
    Settings loadSettings() const;
    void saveSettings(const Settings& settings);

    std::wstring getCurrentMessage() const;

private:
    bool running_ = false;
    bool previousCaps_ = false;
    Settings settings_;
};

} // namespace caps
