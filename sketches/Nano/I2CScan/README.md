# I2CScan

Scanne le bus I2C (adresses 1 à 126) et affiche les périphériques qui répondent. **Diagnostic** de la liaison entre le Nano et le PCA9685.

## Quand l'utiliser

- À la mise en route, pour vérifier que le PCA9685 est correctement câblé.
- Après tout changement de câblage I2C ou d'alim logique du PCA9685.
- Si un sketch utilisant le PCA9685 ne marche pas et qu'on suspecte un souci I2C.

## Pré-requis

PCA9685 câblé :
- `SDA` → Nano `A4`
- `SCL` → Nano `A5`
- `VCC` → 5V Nano (alim logique du PCA9685, indispensable)
- `GND` → GND Nano (masse commune)

`V+` (alim servo) **peut rester débranché** pour ce test — il sert uniquement aux servos, pas à la puce.

## Compiler et uploader

```bash
./bin/arduino-cli compile --fqbn arduino:avr:nano sketches/Nano/I2CScan/I2CScan.ino
./bin/arduino-cli upload --fqbn arduino:avr:nano --port /dev/ttyUSB0 sketches/Nano/I2CScan/I2CScan.ino
```

Pour Philippe : `:cpu=atmega328old`.

## Lire la sortie série

Dans un terminal interactif (cf. CLAUDE.md sur les contraintes de lecture série) :

```bash
./bin/arduino-cli monitor -p /dev/ttyUSB0 -c baudrate=9600
```

## Observation attendue

```
Scan I2C en cours...
  -> peripherique trouve a 0x40
Total : 1 peripherique(s).
```

L'adresse par défaut du PCA9685 est `0x40`. Si des ponts d'adresse sont soudés sur la carte, elle peut changer (`0x41`..`0x7F`).

Si rien n'est détecté : vérifier le câblage SDA/SCL (parfois étiquetés à l'envers selon la face de la carte), la masse commune, et l'alim 5V de VCC.
