# 🔐 CapsLockNotifier - Notificatore Caps Lock Win32

> Un'applicazione nativa Windows per notificare visivamente e acusticamente quando il Caps Lock è attivo. Interfaccia grafica intuitiva, configurazione personalizzata e autostart automatico.

[![License: GPL-3.0](https://img.shields.io/badge/License-GPL3.0-blue.svg)](https://www.gnu.org/licenses/gpl-3.0.html)
[![Windows 10+](https://img.shields.io/badge/Windows-10%2F11-0078D4?logo=windows)](https://www.microsoft.com/windows)
[![C++17](https://img.shields.io/badge/C%2B%2B-17-blue?logo=cplusplus)](https://cplusplus.com/)

## 🌟 Caratteristiche

- ✅ **Interfaccia Grafica Nativa** - Win32 API puro, nessuna dipendenza da framework pesanti
- ✅ **Personalizzazione Completa** - Personalizza il messaggio, la durata e il suono
- ✅ **Notifiche Non-Invasive** - Popup nel basso a destra che non disturba il lavoro
- ✅ **Autostart Automatico** - Avvia con Windows senza intervento
- ✅ **Icona Bandeja del Sistema** - Minimizza nell'area di notifica
- ✅ **Compatibilità Windows 10/11** - Fully optimizzato per entrambi i sistemi
- ✅ **Basso Consumo** - ~5-10 MB RAM, minimo carico CPU
- ✅ **Salvataggio Impostazioni** - Configurazione persiste nel registro Windows

## 📦 Requisiti

### Utenti (per eseguire)
- Windows 10 o successivo (qualsiasi edizione)
- ~5-10 MB di spazio su disco

### Sviluppatori (per compilare)
- **Opzione 1:** Visual Studio 2022 Community (con C++ development tools)
- **Opzione 2:** MinGW-w64
- **Opzione 3:** CMake 3.16+

## 🚀 Installazione & Uso

### Metodo 1: Scarica l'eseguibile precompilato
```bash
# Scarica CapsLockNotifier.exe dalla sezione Releases
# Posiziona il file nella cartella desiderata
# Fai doppio clic per avviare
```

### Metodo 2: Compila tu stesso

#### Su Windows (MSVC)
```bash
build.bat msvc
```

#### Su Windows (MinGW)
```bash
build.bat mingw
```

#### Su Windows (CMake)
```bash
build.bat cmake
```

#### Su Linux/WSL (cross-compile)
```bash
chmod +x build.sh
./build.sh mingw
```

**Output:** L'eseguibile sarà in `build/` o nella directory corrente

## 🎯 Primo Avvio

1. **Esegui l'applicazione:**
   ```bash
   CapsLockNotifier.exe
   ```

2. **Configurazione:** Apparirà la finestra di impostazioni con:
   - Campo per personalizzare il messaggio
   - Campo per la durata della notifica (1-60 secondi)
   - Checkbox per abilitare il suono
   - Checkbox per autostart con Windows

3. **Salva:** Clicca "Salva impostazioni"

4. **Pronto:** Attiva Caps Lock per testare la notifica!

## ⚙️ Configurazione

### Finestra Principale
| Opzione | Descrizione | Valore Predefinito |
|---------|-------------|-------------------|
| Messaggio | Testo mostrato quando Caps Lock è attivo | "CAPS LOCK ATTIVO!" |
| Durata | Secondi per cui la notifica rimane visibile | 3 secondi |
| Suono | Riproduci un beep di sistema | Abilitato |
| Autostart | Avvia automaticamente con Windows | Disabilitato |

### Dati Salvati
Le impostazioni vengono salvate nel Registro Windows:
```text
HKEY_CURRENT_USER\Software\CapsLockNotifier
```

Per ripristinare i valori di default, elimina questa chiave del registro.

## 🔧 Compilazione Manuale

### Con MSVC (Visual Studio)
```bash
# Aprire "Visual Studio 2022 Command Prompt"
cl /std:c++17 /O2 /W4 /DUNICODE /D_UNICODE programma.ciulla.cpp user32.lib gdi32.lib shell32.lib advapi32.lib /Fe:CapsLockNotifier.exe
```

### Con MinGW-w64
```bash
g++ -std=c++17 -O2 -Wall -Wextra -municode programma.ciulla.cpp -o CapsLockNotifier.exe -mwindows -luser32 -lgdi32 -lshell32 -ladvapi32
```

### Con CMake
```bash
mkdir build && cd build
cmake .. -G "Visual Studio 17 2022"
cmake --build . --config Release
```

## 📊 Dimensioni Eseguibile

| Compilatore | Dimensione | Note |
|-------------|-----------|-------|
| MSVC Release | ~350-400 KB | Ottimizzato Microsoft |
| MinGW Release | ~250-300 KB | GCC optimization |
| Debug Build | +1-2 MB | Con simboli debug |

## 🎨 Interfaccia

### Finestra di Configurazione
```text
+-----------------------------------------------------------+
| Caps Lock Notifier - Giuliano Gramaglia                    |
|-----------------------------------------------------------|
| Messaggio da mostrare quando Caps Lock viene attivato:     |
| [ CAPS LOCK ATTIVO!                                        |
|                                                           |
| Durata della notifica (secondi, 1-60): [3]                 |
|                                                           |
| [x] Riproduci un suono                                     |
| [x] Avvia automaticamente con Windows                     |
|                                                           |
|                 [ Salva impostazioni ]                     |
+-----------------------------------------------------------+
```

### Notifica Popup
```text
+--------------------------------------------+
| CAPS LOCK ATTIVO!                          |
+--------------------------------------------+
```

## 🐛 Risoluzione Problemi

### "CapsLockNotifier.exe non si avvia"
- Assicurati di usare Windows 10 o successivo
- Prova da Command Prompt con privilegi di amministratore
- Verifica che le librerie di sistema siano integrate (dovrebbero essere di default)

### "Notifica non compare"
- Verifica che Caps Lock sia effettivamente attivato
- Controlla che la durata sia > 0 secondi
- Prova a spostare la finestra di configurazione (la notifica potrebbe essere nascosta dietro)

### "Autostart non funziona"
- L'eseguibile deve trovarsi in un percorso persistente (non Desktop)
- Disabilita il controllo dell'account utente (UAC) temporaneamente
- Prova ad eseguire come amministratore una volta

### Eliminare dalla startup
1. Apri il programma
2. Deseleziona "Avvia al riavvio del PC"
3. Salva impostazioni
4. Chiudi il programma

## 📝 Licenza

Questo progetto è distribuito sotto la licenza [GNU General Public License v3.0](LICENSE).

Sei libero di:
- ✅ Usare il programma per qualsiasi scopo
- ✅ Modificare il codice
- ✅ Distribuire il programma
- ⚠️ Mantenere la stessa licenza

## 👨‍💻 Autore

Sviluppato da **Giuliano Gramaglia**.

## 🤝 Contributi

Le segnalazioni di bug e i suggerimenti sono benvenuti! Apri un [Issue](../../issues) o una [Pull Request](../../pulls).

## 📚 Struttura del Progetto

```text
programma.caps.lock/
├── programma.ciulla.cpp         # Codice sorgente principale (Win32 API)
├── CMakeLists.txt               # Configurazione CMake
├── build.bat                    # Script compilazione Windows
├── build.sh                     # Script compilazione Linux/WSL
├── COMPILAZIONE.md              # Guida dettagliata compilazione
├── README.md                    # Questo file
├── CapsLockNotifier.exe         # Eseguibile precompilato (Release)
├── LICENSE                      # GPL-3.0
└── .gitignore                   # File di ignora Git
```

## 🔐 Sicurezza

- No telemetria
- No connessioni di rete
- No amministratore richiesto (autostart richiede permessi limitati)
- Codice open-source - verifica quello che vuoi
- Usa solo API di sistema Windows native

## 📞 Supporto

Per problemi, domande o suggerimenti:
1. Consulta [COMPILAZIONE.md](COMPILAZIONE.md) per problemi di compilazione
2. Apri un [Issue](../../issues) per bug report
3. Controlla i [Releases](../../releases) per versioni stabili

---

**Goditi la notifica del tuo Caps Lock!** 🎉
