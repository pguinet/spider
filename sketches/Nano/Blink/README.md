# Blink

Clignote rapidement la LED intégrée (pin 13) à ~5 Hz. **Utilitaire d'identification physique** quand plusieurs Nanos sont branchés en USB.

## Quand l'utiliser

Pour savoir lequel des Nanos est sur tel port (`/dev/ttyUSB0` vs `/dev/ttyUSB1`). On flashe ce sketch sur le port à identifier, et celui dont la LED clignote rapidement est ce Nano.

## Pré-requis

Juste un Nano connecté en USB. Aucune bibliothèque externe.

## Compiler et uploader

```bash
./bin/arduino-cli compile --fqbn arduino:avr:nano sketches/Nano/Blink/Blink.ino
./bin/arduino-cli upload --fqbn arduino:avr:nano --port /dev/ttyUSB0 sketches/Nano/Blink/Blink.ino
```

Pour Philippe (bootloader ancien) : ajouter `:cpu=atmega328old` au FQBN.

## Observation

LED `L` (pin 13) qui clignote à 5 Hz (100 ms allumée / 100 ms éteinte).
