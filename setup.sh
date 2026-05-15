#!/bin/bash
#
# Script d'installation pour le projet Spider Robot
# Installe arduino-cli, le core AVR et les bibliothèques
#

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

echo "=== Installation Spider Robot ==="
echo ""

# 1. Arduino CLI
echo "[1/3] Arduino CLI..."
if [ -f "bin/arduino-cli" ]; then
    echo "      Déjà installé: $(./bin/arduino-cli version | head -1)"
else
    echo "      Téléchargement..."
    mkdir -p bin
    cd bin
    curl -fsSL https://raw.githubusercontent.com/arduino/arduino-cli/master/install.sh | sh
    cd ..
    echo "      Installé: $(./bin/arduino-cli version | head -1)"
fi
echo ""

# 2. Installation du core AVR (Nano = ATmega328P)
echo "[2/3] Installation du core AVR..."
./bin/arduino-cli core update-index
if ./bin/arduino-cli core list | grep -q "^arduino:avr"; then
    echo "      arduino:avr (déjà installé)"
else
    echo "      arduino:avr (installation...)"
    ./bin/arduino-cli core install "arduino:avr"
fi
echo ""

# 3. Installation des bibliothèques
echo "[3/3] Installation des bibliothèques..."
if [ -f "libraries.txt" ]; then
    while IFS= read -r line || [ -n "$line" ]; do
        # Ignorer les commentaires et lignes vides
        line=$(echo "$line" | sed 's/#.*//' | xargs)
        if [ -n "$line" ]; then
            if ./bin/arduino-cli lib list | grep -q "^${line}"; then
                echo "      $line (déjà installée)"
            else
                echo "      $line (installation...)"
                ./bin/arduino-cli lib install "$line"
            fi
        fi
    done < "libraries.txt"
else
    echo "      Fichier libraries.txt non trouvé"
fi
echo ""

echo "=== Installation terminée ==="
echo ""
echo "Pour compiler un sketch :"
echo "  ./bin/arduino-cli compile --fqbn arduino:avr:nano sketches/Nano/MonSketch"
echo ""
echo "Pour uploader (Nano avec bootloader ancien : utiliser :cpu=atmega328old) :"
echo "  ./bin/arduino-cli upload --fqbn arduino:avr:nano --port /dev/ttyUSB0 sketches/Nano/MonSketch"
echo ""
