import sys
from pathlib import Path

from PyQt6.QtWidgets import QApplication

from frontend.core.settings import AppSettings
from frontend.ui.settings_window import SettingsWindow
from frontend.ui.style import load_style


def main() -> int:
    app = QApplication(sys.argv)
    app.setApplicationName("CapsLockNotifier")

    settings = AppSettings(Path(__file__).resolve().parent / "settings.json")
    settings.load()

    window = SettingsWindow(settings)
    window.resize(620, 550)
    window.show()

    app.setStyleSheet(load_style())
    return app.exec()


if __name__ == "__main__":
    raise SystemExit(main())
