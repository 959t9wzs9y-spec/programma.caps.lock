# Documentazione della nuova architettura 2026

## Obiettivo

La UI deve essere moderna e facile da usare, mentre la logica del monitoraggio del Caps Lock resta in C++ per essere affidabile e nativa su Windows.

## Struttura proposta

```text
programma.caps.lock/
├── backend/
│   ├── CMakeLists.txt
│   ├── include/
│   │   └── caps_monitor.h
│   └── src/
│       ├── caps_monitor.cpp
│       └── main.cpp
├── frontend/
│   ├── core/
│   │   ├── ipc.py
│   │   └── settings.py
│   ├── ui/
│   │   ├── popup.py
│   │   ├── settings_window.py
│   │   ├── style.py
│   │   └── style.qss
│   ├── main.py
│   ├── requirements.txt
│   └── settings.json
├── CMakeLists.txt
├── README.md
└── .gitignore
```

## Architettura

- `backend/`: contiene la logica in C++.
  - monitora lo stato del Caps Lock
  - legge e salva le impostazioni nel registro Windows
  - può essere usata come servizio o processo in background

- `frontend/`: contiene l'interfaccia in Python.
  - PyQt6 per la UI moderna
  - `settings_window.py` per la configurazione
  - `popup.py` per la notifica visiva
  - `core/ipc.py` per la comunicazione col backend

## Flusso di esecuzione

1. Python avvia la UI.
2. La UI legge le impostazioni da `settings.json`.
3. La UI avvia il backend C++ (o si collega ad un processo già attivo).
4. Il backend monitora lo stato del Caps Lock e invia informazioni di stato alla UI.
5. La UI mostra un popup moderno quando il Caps Lock viene attivato.

## Build

### Backend C++

```bash
cmake -S . -B build
cmake --build build
```

### Frontend Python

```bash
python -m venv .venv
.venv\Scripts\activate
pip install -r frontend/requirements.txt
python frontend/main.py
```

## Nota

Questa struttura è la base corretta per separare bene la responsabilità del progetto:
- C++ per il monitoraggio e la logica del sistema
- Python/PyQt6 per la UI moderna e il design
