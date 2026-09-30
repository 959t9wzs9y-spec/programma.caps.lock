@echo off
setlocal enabledelayedexpansion

REM Script di compilazione per Windows - CapsLockNotifier
REM Supporta sia MSVC che MinGW

if "%1"=="" (
    echo Uso: build.bat [msvc^|mingw^|cmake]
    echo.
    echo Opzioni:
    echo   msvc    - Compila con Microsoft Visual C++ Compiler
    echo   mingw   - Compila con MinGW-w64
    echo   cmake   - Usa CMake per la compilazione
    echo.
    echo Esempio: build.bat msvc
    exit /b 1
)

set BUILD_TYPE=%1

REM Crea directory build se non esiste
if not exist "build" mkdir build
cd build

if "%BUILD_TYPE%"=="msvc" (
    echo [*] Compilazione con MSVC...
    cl /std:c++17 /O2 /W4 /DUNICODE /D_UNICODE ..\programma.ciulla.cpp user32.lib gdi32.lib shell32.lib advapi32.lib /Fe:CapsLockNotifier.exe
    if errorlevel 1 (
        echo [!] Errore durante la compilazione con MSVC
        exit /b 1
    )
    echo [+] Compilazione completata! Eseguibile: build\CapsLockNotifier.exe
)

if "%BUILD_TYPE%"=="mingw" (
    echo [*] Compilazione con MinGW-w64...
    g++ -std=c++17 -O2 -Wall -Wextra -municode ..\programma.ciulla.cpp -o CapsLockNotifier.exe -mwindows -luser32 -lgdi32 -lshell32 -ladvapi32
    if errorlevel 1 (
        echo [!] Errore durante la compilazione con MinGW
        exit /b 1
    )
    echo [+] Compilazione completata! Eseguibile: build\CapsLockNotifier.exe
)

if "%BUILD_TYPE%"=="cmake" (
    echo [*] Configurazione con CMake...
    cmake .. -G "Visual Studio 17 2022"
    if errorlevel 1 (
        echo [!] Errore durante la configurazione CMake
        exit /b 1
    )
    echo [*] Compilazione con CMake...
    cmake --build . --config Release
    if errorlevel 1 (
        echo [!] Errore durante la compilazione CMake
        exit /b 1
    )
    echo [+] Compilazione completata! Eseguibile: build\bin\CapsLockNotifier.exe
)

endlocal