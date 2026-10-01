from PyQt6.QtWidgets import QWidget, QVBoxLayout, QHBoxLayout, QLabel, QLineEdit, QCheckBox, QPushButton, QFrame

from frontend.core.settings import AppSettings


class SettingsWindow(QWidget):
    def __init__(self, settings: AppSettings):
        super().__init__()
        self.settings = settings
        self.setWindowTitle("CapsLockNotifier")
        self.setObjectName("main")

        root = QVBoxLayout(self)
        root.setContentsMargins(0, 0, 0, 0)
        root.setSpacing(0)

        header = QFrame()
        header.setObjectName("header")
        header_layout = QVBoxLayout(header)
        header_layout.setContentsMargins(24, 20, 24, 20)

        title = QLabel("Caps Lock Notifier")
        title.setObjectName("title")
        subtitle = QLabel("Interfaccia moderna con logica backend in C++")
        subtitle.setObjectName("subtitle")
        header_layout.addWidget(title)
        header_layout.addWidget(subtitle)

        card = QFrame()
        card.setObjectName("card")
        card_layout = QVBoxLayout(card)
        card_layout.setContentsMargins(24, 24, 24, 24)
        card_layout.setSpacing(18)

        label_message = QLabel("Messaggio da mostrare")
        self.message_field = QLineEdit(self.settings.message)
        self.message_field.setPlaceholderText("CAPS LOCK ATTIVO!")

        row = QHBoxLayout()
        duration_label = QLabel("Durata (secondi)")
        self.duration_field = QLineEdit(str(self.settings.duration_seconds))
        self.duration_field.setMaximumWidth(120)
        row.addWidget(duration_label)
        row.addStretch()
        row.addWidget(self.duration_field)

        self.sound_check = QCheckBox("Riproduci un suono")
        self.sound_check.setChecked(self.settings.sound)

        self.autostart_check = QCheckBox("Avvia automaticamente con Windows")
        self.autostart_check.setChecked(self.settings.autostart)

        buttons = QHBoxLayout()
        buttons.addStretch()
        self.save_button = QPushButton("Salva impostazioni")
        buttons.addWidget(self.save_button)

        self.status_label = QLabel("Pronto")
        self.status_label.setObjectName("status")

        card_layout.addWidget(label_message)
        card_layout.addWidget(self.message_field)
        card_layout.addLayout(row)
        card_layout.addWidget(self.sound_check)
        card_layout.addWidget(self.autostart_check)
        card_layout.addLayout(buttons)
        card_layout.addWidget(self.status_label)

        root.addWidget(header)
        root.addWidget(card)

        self.save_button.clicked.connect(self.save_settings)

    def save_settings(self):
        self.settings.message = self.message_field.text() or "CAPS LOCK ATTIVO!"
        try:
            self.settings.duration_seconds = max(1, min(60, int(self.duration_field.text() or "3")))
        except ValueError:
            self.settings.duration_seconds = 3
        self.settings.sound = self.sound_check.isChecked()
        self.settings.autostart = self.autostart_check.isChecked()
        self.settings.save()
        self.status_label.setText("✓ Impostazioni salvate")
