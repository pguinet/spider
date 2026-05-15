# Spider Robot

Robot quadrupède 12 DOF (4 pattes × 3 articulations) à base d'**Arduino Nano** + IO Expansion Shield.

## Installation rapide

```bash
./setup.sh
```

Ce script installe automatiquement :
- Arduino CLI (dans `bin/`)
- Le core `arduino:avr` (Nano = ATmega328P)
- Toutes les bibliothèques listées dans `libraries.txt`

## Structure

```
spider/
├── bin/                  # arduino-cli (téléchargé par setup.sh)
├── docs/                 # documentation de référence (vidéos, schémas, STL)
├── sketches/
│   └── Nano/             # sketches pour l'Arduino Nano
├── libraries.txt         # bibliothèques requises
├── setup.sh              # installation automatique
└── CLAUDE.md             # contexte projet pour Claude Code
```

## Carte

| Carte | FQBN |
|-------|------|
| Arduino Nano (bootloader récent) | `arduino:avr:nano` |
| Arduino Nano (bootloader ancien — clones) | `arduino:avr:nano:cpu=atmega328old` |

## Utilisation

Compiler un sketch :

```bash
./bin/arduino-cli compile --fqbn arduino:avr:nano sketches/Nano/MonSketch
```

Uploader sur la carte :

```bash
./bin/arduino-cli upload --fqbn arduino:avr:nano --port /dev/ttyUSB0 sketches/Nano/MonSketch
```

## Ajouter une bibliothèque

1. L'installer : `./bin/arduino-cli lib install "NomLib"`
2. L'ajouter à `libraries.txt`
3. L'ajouter dans le `@dependencies` du sketch concerné
