import json
import os
import socket
import subprocess
import sys
import threading
import time
from pathlib import Path


class BackendClient:
    HOST = "127.0.0.1"
    PORT = 45678

    def __init__(self, backend_exe: str | None = None):
        base = Path(__file__).resolve().parents[1]
        default_path = base.parent / "backend" / "build" / "bin" / "caps_monitor_app.exe"
        if backend_exe is None:
            backend_exe = str(default_path)
        self.backend_exe = backend_exe
        self.process = None
        self.socket = None

    def start(self) -> None:
        if os.path.exists(self.backend_exe):
            self.process = subprocess.Popen([self.backend_exe], stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
            time.sleep(0.5)
            return

        raise FileNotFoundError(f"Backend executable not found: {self.backend_exe}")

    def stop(self) -> None:
        if self.process is not None:
            self.process.terminate()
            self.process.wait(timeout=5)

    def read_status(self) -> dict:
        if self.process is None or self.process.stdout is None:
            return {"caps_lock": False, "message": "CAPS LOCK ATTIVO!"}

        line = self.process.stdout.readline()
        if not line.strip():
            return {"caps_lock": False, "message": "CAPS LOCK ATTIVO!"}

        try:
            return json.loads(line)
        except json.JSONDecodeError:
            return {"caps_lock": False, "message": "CAPS LOCK ATTIVO!"}

    def send_settings(self, payload: dict) -> None:
        if self.process is None:
            return
        self.process.stdin.write(json.dumps(payload) + "\n")
        self.process.stdin.flush()
