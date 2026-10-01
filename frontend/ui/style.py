from pathlib import Path


def load_style() -> str:
    qss_path = Path(__file__).resolve().parent / "style.qss"
    if not qss_path.exists():
        return ""
    return qss_path.read_text(encoding="utf-8")
