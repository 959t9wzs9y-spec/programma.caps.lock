# Guida di Compilazione - CapsLockNotifier

Programma notificatore per Caps Lock con interfaccia grafica nativa Win32, compilabile su Windows 10/11.

## Prerequisiti

### Su Windows

#### Opzione 1: Microsoft Visual Studio
- [Visual Studio 2022 Community Edition](https://visualstudio.microsoft.com/vs/community/)
- Durante l'installazione, selezionare: **Desktop development with C++**

#### Opzione 2: MinGW-w64
- [MinGW-w64](https://www.mingw-w64.org/)
- Assicurarsi che `g++` sia nel PATH di sistema

#### Opzione 3: CMake
- [CMake 3.16+](https://cmake.org/download/)
- Visual Studio o MinGW installato

### Su Linux/WSL (compilazione cross-platform)
```bash
sudo apt-get install mingw-w64 cmake
```

---

## Compilazione

### Windows - Metodo 1: MSVC (Consigliato)

**Usando Visual Studio Command Prompt:**

```bash
# Aprire "Visual Studio 2022 Command Prompt"
cd programma.caps.lock
cl /std:c++17 /O2 /W4 /DUNICODE /D_UNICODE programma.ciulla.cpp user32.lib gdi32.lib shell32.lib advapi32.lib /Fe:CapsLockNotifier.exe
```

**Output:** `CapsLockNotifier.exe` nella directory corrente

### Windows - Metodo 2: MinGW-w64

```bash
cd programma.caps.lock
g++ -std=c++17 -O2 -Wall -Wextra -municode programma.ciulla.cpp -o CapsLockNotifier.exe -mwindows -luser32 -lgdi32 -lshell32 -ladvapi32
```

**Output:** `CapsLockNotifier.exe` nella directory corrente

### Windows - Metodo 3: Script automatico

```bash
cd programma.caps.lock

# Con MSVC
build.bat msvc

# Con MinGW
build.bat mingw

# Con CMake
build.bat cmake
```

### Linux/WSL - Compilazione cross-platform

```bash
cd programma.caps.lock
chmod +x build.sh

# Con MinGW-w64
./build.sh mingw

# Con CMake
./build.sh cmake
```

**Output:** `build/CapsLockNotifier.exe` (pronto per Windows 10/11)

---

## Verifica della compilazione

Dopo una compilazione riuscita, dovresti vedere:

```
[+] Compilazione completata! Eseguibile: build\CapsLockNotifier.exe
```

L'eseguibile sarà disponibile in:
- **Metodo MSVC/MinGW:** `.\CapsLockNotifier.exe`
- **Metodo CMake:** `.\build\bin\CapsLockNotifier.exe`

---

## Primo avvio

```bash
CapsLockNotifier.exe
```

### Configurazione iniziale:
1. Verrà visualizzata la finestra di configurazione
2. Personalizza il messaggio che apparirà quando Caps Lock è attivo
3. Imposta la durata della notifica (1-60 secondi)
4. Abilita/disabilita il suono
5. Seleziona "Avvia automaticamente con Windows" se desideri l'autostart
6. Clicca "Salva impostazioni"

---

## Opzioni di compilazione avanzate

### MSVC - Compilazione a 32-bit
```bash
cl /std:c++17 /O2 /W4 /DUNICODE /D_UNICODE programma.ciulla.cpp user32.lib gdi32.lib shell32.lib advapi32.lib /Fe:CapsLockNotifier.exe
```

### MinGW - Compilazione con debug
```bash
g++ -std=c++17 -g -Wall -Wextra -municode programma.ciulla.cpp -o CapsLockNotifier.exe -mwindows -luser32 -lgdi32 -lshell32 -ladvapi32
```

### CMake - Configurazione personalizzata
```bash
mkdir build && cd build
cmake .. -G "Visual Studio 17 2022" -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release
```

---

## Risoluzione problemi

### "cl.exe non trovato"
- Aprire **Visual Studio 2022 Command Prompt** (non il prompt normale)
- O aggiungere il percorso di Visual Studio al PATH

### "g++ non trovato"
- Assicurarsi che MinGW-w64 sia installato
- Aggiungere `C:\Program Files\mingw-w64\x86_64-8.1-win32-seh-rt_v6-rev0\mingw64\bin` al PATH

### "Errore di linking"
- Verificare che tutte le librerie siano disponibili (user32, gdi32, shell32, advapi32)
- Su Windows 10/11, queste dovrebbero essere presenti di default

### Eseguibile non parte
- Verificare che il sistema sia Windows 10 o superiore
- Provare da Command Prompt con privilegi di amministratore

---

## Dimensione eseguibile

- **MSVC Release:** ~350-400 KB
- **MinGW Release:** ~250-300 KB
- **Con debug:** +1-2 MB

---

## Supporto

Per problemi o domande, controlla il file README.md o contatta il maintainer.

Buona compilazione! 🚀
