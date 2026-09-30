#!/bin/bash

# Script di compilazione per Linux/WSL - CapsLockNotifier
# Supporta compilazione cross-platform con MinGW

set -e

usage() {
    echo "Uso: ./build.sh [mingw|cmake]"
    echo ""
    echo "Opzioni:"
    echo "  mingw  - Compila con MinGW-w64 (cross-compile per Windows)"
    echo "  cmake  - Usa CMake per la compilazione"
    echo ""
    echo "Esempio: ./build.sh mingw"
    exit 1
}

if [ $# -eq 0 ]; then
    usage
fi

BUILD_TYPE=$1

# Crea directory build se non esiste
mkdir -p build
cd build

case $BUILD_TYPE in
    mingw)
        echo "[*] Compilazione cross-platform con MinGW-w64..."
        x86_64-w64-mingw32-g++ -std=c++17 -O2 -Wall -Wextra -municode ../programma.ciulla.cpp -o CapsLockNotifier.exe -mwindows -luser32 -lgdi32 -lshell32 -ladvapi32
        if [ $? -eq 0 ]; then
            echo "[+] Compilazione completata! Eseguibile: build/CapsLockNotifier.exe"
        else
            echo "[!] Errore durante la compilazione"
            exit 1
        fi
        ;;
    cmake)
        echo "[*] Configurazione con CMake..."
        cmake .. -DCMAKE_C_COMPILER=x86_64-w64-mingw32-gcc -DCMAKE_CXX_COMPILER=x86_64-w64-mingw32-g++ -DCMAKE_RC_COMPILER=x86_64-w64-mingw32-windres
        if [ $? -ne 0 ]; then
            echo "[!] Errore durante la configurazione CMake"
            exit 1
        fi
        echo "[*] Compilazione con CMake..."
        cmake --build . --config Release
        if [ $? -eq 0 ]; then
            echo "[+] Compilazione completata! Eseguibile: build/bin/CapsLockNotifier.exe"
        else
            echo "[!] Errore durante la compilazione"
            exit 1
        fi
        ;;
    *)
        echo "[!] Opzione sconosciuta: $BUILD_TYPE"
        usage
        ;;
esac