import json
from dataclasses import dataclass, asdict
from pathlib import Path


@dataclass
class AppSettings:
    message: str = "CAPS LOCK ATTIVO!"
    duration_seconds: int = 3
    sound: bool = True
    autostart: bool = False
    file_path: str | None = None

    def __post_init__(self):
        if self.file_path is not None:
            self.file_path = str(Path(self.file_path))

    def load(self) -> "AppSettings":
        if self.file_path is None:
            return self

        path = Path(self.file_path)
        if not path.exists():
            return self

        try:
            with path.open("r", encoding="utf-8") as handle:
                payload = json.load(handle)
        except (json.JSONDecodeError, OSError):
            return self

        for key, value in payload.items():
            if hasattr(self, key):
                setattr(self, key, value)
        return self

    def save(self) -> None:
        if self.file_path is None:
            return

        path = Path(self.file_path)
        path.parent.mkdir(parents=True, exist_ok=True)
        with path.open("w", encoding="utf-8") as handle:
            json.dump(asdict(self), handle, ensure_ascii=False, indent=2)

    def to_dict(self) -> dict:
        return asdict(self)
