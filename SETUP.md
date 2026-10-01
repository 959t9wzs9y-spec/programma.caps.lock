# 🚀 Guida Completa per Avviare il Progetto

## Architettura

```
Backend (C++)  ←→  Frontend (Python/PyQt6)
  Logica             UI Moderna
  Monitoring         Configurazione
  Settings           Notifiche
```

---

## ✅ Prerequisiti

### Windows 10/11

**Per il backend C++:**
- Visual Studio 2022 Community (con C++ tools) **OPPURE**
- MinGW-w64 + CMake

**Per la frontend Python:**
- Python 3.9+
- pip

### Ubuntu/WSL

```bash
sudo apt-get update
sudo apt-get install -y python3.10 python3-pip python3-venv cmake g++
```

---

## 📦 Installazione Passo dopo Passo

### 1️⃣ **Clona il Repository**

```bash
git clone https://github.com/959t9wzs9y-spec/programma.caps.lock.git
cd programma.caps.lock
```

### 2️⃣ **Compila il Backend C++**

#### **Opzione A: Su Windows con Visual Studio**

```bash
# Apri Visual Studio Developer Command Prompt

mkdir build
cd build
cmake .. -G "Visual Studio 17 2022"
cmake --build . --config Release
cd ..
```

**Output:** `build/bin/caps_monitor_app.exe`

#### **Opzione B: Su Windows con MinGW**

```bash
mkdir build
cd build
cmake .. -G "MinGW Makefiles"
cmake --build .
cd ..
```

**Output:** `build/bin/caps_monitor_app.exe`

#### **Opzione C: Su Linux/WSL**

```bash
mkdir build
cd build
cmake ..
cmake --build .
cd ..
```

**Output:** `build/bin/caps_monitor_app`

---

### 3️⃣ **Prepara l'Ambiente Python**

#### **Windows**

```bash
python -m venv venv
venv\Scripts\activate
```

#### **Linux/WSL/macOS**

```bash
python3 -m venv venv
source venv/bin/activate
```

---

### 4️⃣ **Installa le Dipendenze Python**

```bash
pip install --upgrade pip
pip install -r frontend/requirements.txt
```

---

## 🎯 Avvio dell'Applicazione

### **Metodo 1: Avvio Completo (CONSIGLIATO)**

Dalla root della cartella del progetto:

```bash
# Attiva l'ambiente virtuale (se non già attivo)
# Windows:
venv\Scripts\activate
# Linux/WSL:
source venv/bin/activate

# Avvia la UI
python frontend/main.py
```

**Cosa succede:**
1. ✅ Python legge le impostazioni salvate
2. ✅ Avvia il backend C++
3. ✅ Mostra la finestra di configurazione
4. ✅ Il backend monitora il Caps Lock
5. ✅ Quando attivi Caps Lock → popup moderno

---

### **Metodo 2: Avvio del Solo Backend**

Se vuoi testare solo il monitoring:

```bash
./build/bin/caps_monitor_app
```

Output (ogni 200ms):
```json
{"caps_lock":false,"message":"CAPS LOCK ATTIVO!"}
{"caps_lock":true,"message":"CAPS LOCK ATTIVO!"}
```

---

## 🔧 Configurazione

### **File di Configurazione**

```
frontend/settings.json
```

Salvataggio automatico quando clicchi "Salva impostazioni" nella UI.

**Contenuto tipico:**
```json
{
  "message": "CAPS LOCK ATTIVO!",
  "duration_seconds": 3,
  "sound": true,
  "autostart": false
}
```

---

## 🎨 Prima Esecuzione

1. Avvia `python frontend/main.py`
2. Apparirà una finestra moderna con il tema scuro/blu
3. Personalizza i campi:
   - Messaggio (default: "CAPS LOCK ATTIVO!")
   - Durata (1-60 secondi)
   - Abilita suono
   - Autostart con Windows (TODO)
4. Clicca "Salva impostazioni"
5. **Attiva Caps Lock** → vedrai il popup blu in basso a destra

---

## ⚙️ Troubleshooting

### ❌ "ModuleNotFoundError: No module named 'PyQt6'"

**Soluzione:**
```bash
pip install PyQt6==6.7.1
```

### ❌ "Backend executable not found"

**Soluzione:**
1. Verifica che il backend sia compilato:
   ```bash
   ls build/bin/caps_monitor_app*
   ```
2. Se non esiste, compila di nuovo:
   ```bash
   mkdir build && cd build
   cmake ..
   cmake --build .
   ```

### ❌ La UI non si avvia

**Verifica:**
```bash
python -c "from PyQt6 import QtWidgets; print('PyQt6 OK')"
```

Se fallisce, reinstalla:
```bash
pip uninstall PyQt6 -y
pip install PyQt6==6.7.1
```

### ❌ Il popup non appare quando attivo Caps Lock

**Cause:**
- Il backend non è partito (controlla output console)
- La UI non è in focus
- Il messaggio è vuoto

**Debug:**
```bash
# Avvia backend da console per vedere errori
./build/bin/caps_monitor_app
```

---

## 📁 Struttura Directory Dopo Setup

```
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
├── build/               ← Creata dopo compilazione
│   └── bin/
│       └── caps_monitor_app.exe
├── venv/               ← Creata dopo python -m venv
│   └── Scripts/
│       └── python.exe
├── CMakeLists.txt
├── README.md
├── ARCHITECTURE.md
└── SETUP.md            ← Questo file
```

---

## 🎯 Prossimi Step (TODO)

- [ ] Implementare comunicazione real-time backend → frontend via socket
- [ ] Animazione smooth del popup
- [ ] Tema scuro/chiaro selezionabile
- [ ] Autostart integrato con Windows Registry
- [ ] Tray icon per minimizzare nell'area di notifica
- [ ] Packaging in .exe singolo con PyInstaller

---

## 💡 Comandi Utili

### Pulisci build

```bash
rm -rf build
```

### Reinstalla dipendenze Python

```bash
pip install -r frontend/requirements.txt --force-reinstall
```

### Vedi output del backend live

```bash
./build/bin/caps_monitor_app 2>&1
```

### Disattiva venv

```bash
deactivate
```

---

## 🚨 Se Qualcosa Non Funziona

1. **Pulisci e ricompila tutto:**
   ```bash
   rm -rf build venv
   mkdir build && cd build && cmake .. && cmake --build . && cd ..
   python -m venv venv && source venv/bin/activate && pip install -r frontend/requirements.txt
   ```

2. **Controlla Python version:**
   ```bash
   python --version  # Deve essere 3.9+
   ```

3. **Verifica che il backend funziona:**
   ```bash
   ./build/bin/caps_monitor_app | head -5
   ```

4. **Abilita output di debug nella UI:**
   Modifica `frontend/main.py` e aggiungi print() dove serve

---

**Pronto! Adesso puoi avviare il progetto con `python frontend/main.py` ✨**
