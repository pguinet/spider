# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Contexte du projet

Tu es un développeur pour les micro-contrôleurs Arduino, en particulier pour ce projet de robot araignée à base d'Arduino Nano. Robot "araignée" type quadrupède 12 DOF (4 pattes × 3 articulations) piloté par un Arduino Nano avec un IO Expansion Shield. Le dossier `docs/` contient la documentation de référence (vidéos, schémas, codes d'origine, fichiers STL pour impression 3D).

Tu as accès à l'arduino CLI dans le dossier `bin` du répertoire courant.

## ⚠️ Port série - Accès direct interdit

**Bug connu** : Claude Code ne ferme pas correctement les file descriptors vers les ports série. Les commandes bash qui accèdent directement au port série **bloquent définitivement la session**.

**Interdit** :
- `cat /dev/ttyUSB0`
- `stty -F /dev/ttyUSB0 ...`
- Tout accès bash direct au port série

**Pour lire la sortie série** : Utilise uniquement l'arduino-cli monitor avec un timeout :
```bash
timeout 30 ./bin/arduino-cli monitor -p /dev/ttyUSB0 -c baudrate=9600
```

## Commandes courantes

Installation initiale (arduino-cli, core AVR, bibliothèques de `libraries.txt`) :
```bash
./setup.sh
```

Compiler un sketch :
```bash
./bin/arduino-cli compile --fqbn arduino:avr:nano sketches/Nano/<projet>/<projet>.ino
```

Uploader sur la carte :
```bash
./bin/arduino-cli upload --fqbn arduino:avr:nano --port /dev/ttyUSB0 sketches/Nano/<projet>/<projet>.ino
```

Lire la sortie série (avec timeout obligatoire — cf. section ci-dessus) :
```bash
timeout 30 ./bin/arduino-cli monitor -p /dev/ttyUSB0 -c baudrate=9600
```

Installer une nouvelle bibliothèque (penser à mettre à jour `libraries.txt` ensuite) :
```bash
./bin/arduino-cli lib install "NomLib"
```

Si l'upload échoue avec `avrdude: stk500_recv(): programmer is not responding`, essayer le FQBN `arduino:avr:nano:cpu=atmega328old` (clones avec ancien bootloader).

## Git

**Projet personnel** : Pas de numéro Jira pour les commits. Utilise des messages descriptifs sans préfixe.

Tu peux me tutoyer.

## Conventions de code

### En-tête de sketch

Chaque sketch doit avoir un en-tête standardisé :

```c
/*
 * NomDuSketch - Spider Robot (Arduino Nano)
 *
 * Description courte du sketch.
 *
 * Board: Arduino Nano (ATmega328P)
 * FQBN: arduino:avr:nano (option :cpu=atmega328old si bootloader ancien)
 *
 * @dependencies Lib1, Lib2
 */
```

- Le champ `@dependencies` liste les bibliothèques externes requises (séparées par des virgules)
- Si aucune bibliothèque externe n'est requise, utiliser `@dependencies (aucune)`
- Les bibliothèques intégrées au core (Wire, SPI, EEPROM...) ne sont pas listées

### Gestion des bibliothèques

- Le fichier `libraries.txt` à la racine liste toutes les bibliothèques du projet
- Quand tu ajoutes une nouvelle bibliothèque, mets à jour ce fichier
- Le script `setup.sh` installe automatiquement toutes les bibliothèques

### Header partagé `SpiderConfig.h`

Configuration commune (adresse PCA9685, ticks PWM, mapping canal→articulation) dans `sketches/Nano/libraries/SpiderConfig/SpiderConfig.h`. Sélection du robot **à la compilation** via un define :

```bash
./bin/arduino-cli compile \
  --libraries sketches/Nano/libraries \
  --build-property "build.extra_flags=-DROBOT_PHILIPPE" \
  --fqbn arduino:avr:nano \
  sketches/Nano/<projet>/<projet>.ino
```

Remplacer `ROBOT_PHILIPPE` par `ROBOT_LAURENT` selon la cible. Le header `#error` si aucun n'est défini. Les sketches qui n'incluent pas `SpiderConfig.h` (ex: `Blink`, `I2CScan`) n'ont pas besoin de ces options.

## Structure du projet

```
spider/
├── bin/                  # arduino-cli (non versionné)
├── docs/                 # documentation de référence (non versionnée, lourde)
├── sketches/
│   └── Nano/             # sketches pour l'Arduino Nano
├── libraries.txt         # bibliothèques requises
├── setup.sh              # installation automatique
└── CLAUDE.md             # ce fichier
```

## Arduino Nano

Carte classique avec **ATmega328P** (8 bits, 16 MHz, 32 KB Flash, 2 KB SRAM, 1 KB EEPROM).

**USB** : convertisseur CH340 ou FTDI selon les clones → port `/dev/ttyUSB0` (parfois `/dev/ttyACM0` pour les Nano officiels récents).

**LED intégrée** : pin 13 (`LED_BUILTIN`)

**Pins** :
- D0 (RX), D1 (TX) : série matérielle (utilisé par USB, éviter pendant l'upload)
- D2-D13 : digital I/O (D3, D5, D6, D9, D10, D11 PWM)
- A0-A7 : entrées analogiques (A6/A7 = analogique pur, pas de digital I/O)
- A4 (SDA), A5 (SCL) : I2C
- D10 (SS), D11 (MOSI), D12 (MISO), D13 (SCK) : SPI

**FQBN** :
- `arduino:avr:nano` — bootloader récent (par défaut)
- `arduino:avr:nano:cpu=atmega328old` — clones avec ancien bootloader (vitesse 57600 au lieu de 115200)

## Robot araignée — généralités

**Mécanique** : 4 pattes × 3 servos = **12 servos SG90** (ou MG90S).

**Articulations par patte** (depuis l'épaule vers le pied) :
- Hanche (rotation horizontale, axe Z)
- Cuisse (haut/bas, axe horizontal)
- Genou (flexion, axe horizontal)

**Alimentation** : les SG90 consomment ~150 mA chacun en mouvement, soit potentiellement 2 A en pic. **Ne jamais alimenter les servos depuis le 5V du Nano** — utiliser une alimentation séparée (battery pack 5-6V ou BEC) avec masse commune.

**IO Expansion Shield** : facilite le câblage des 12 servos en exposant les pins du Nano avec leurs propres rails 5V/GND. Le shield se branche par-dessus le Nano.

**Bibliothèques typiques** :
- `Servo` : pilotage standard (limité à 12 servos sur AVR, soit pile la limite ici)
- `FlexiTimer2` : interruption timer pour rythmer les pas indépendamment de la boucle principale
- `NewPing` : capteur HC-SR04 pour évitement d'obstacles

**Communication sans fil** (selon montage) :
- HC-05/HC-06 : Bluetooth classique sur série (TX/RX du Nano)
- HC-08/HM-10 : BLE
