# NamedServoTest

Anime chaque articulation par son **nom logique** (FR_COXA, FR_FEMUR, ...) en oscillation douce ±15° autour du neutre. Sert à valider de bout en bout que le mapping câblage + nommage de [SpiderConfig.h](../libraries/SpiderConfig/SpiderConfig.h) est cohérent avec la mécanique.

## Quand l'utiliser

Après avoir renseigné le mapping dans `SpiderConfig.h` (via ServoSweep + édition manuelle), pour s'assurer qu'effectivement quand le sketch dit "FR_COXA", c'est bien la hanche de la patte avant droite qui bouge.

## Pré-requis

- PCA9685 + alim servo OK.
- Mapping renseigné dans `SpiderConfig.h` pour le robot ciblé.

## Compiler et uploader

Pour Laurent :
```bash
./bin/arduino-cli compile --libraries sketches/Nano/libraries \
  --build-property "build.extra_flags=-DROBOT_LAURENT" \
  --fqbn arduino:avr:nano \
  sketches/Nano/NamedServoTest/NamedServoTest.ino
./bin/arduino-cli upload --fqbn arduino:avr:nano \
  --port /dev/serial/by-id/usb-1a86_USB_Serial-if00-port0 \
  sketches/Nano/NamedServoTest/NamedServoTest.ino
```

Pour Philippe : `-DROBOT_PHILIPPE`, FQBN `:cpu=atmega328old`, port `usb-1a86_USB2.0-Serial-...`.

## Observation

```
=== NamedServoTest : Laurent ===
>>> FR_COXA (canal 0)
>>> FR_FEMUR (canal 1)
>>> FR_TIBIA (canal 2)
>>> FL_COXA (canal 4)
...
```

Pour chaque articulation annoncée, vérifie visuellement que c'est bien la bonne qui oscille. Si une incohérence apparaît (par ex. quand "FL_FEMUR" s'affiche c'est FL_TIBIA qui bouge), le mapping dans `SpiderConfig.h` est faux.

Un tour complet dure ~53 s (12 articulations × ~3.3 s). Le sketch boucle indéfiniment.

## Notes

- Amplitude douce (±15°) volontaire pour ne rien casser.
- N'applique **pas** les offsets calibrés (utilise `angleToTicks` direct). Le sens de rotation reste correct, c'est ce qui compte ici.
