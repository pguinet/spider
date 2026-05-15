# ServoTrim

Calibrage **interactif** des 12 servos à la position neutre (90°). Permet d'ajuster un offset individuel par servo (en ticks PCA9685, 1 tick ≈ 0.88°) pour compenser un palonnier monté légèrement de travers.

## Quand l'utiliser

Après le réglage mécanique des palonniers avec [ServoCenter](../ServoCenter/README.md), pour affiner ce qui ne peut pas l'être à la main (palonnier qui a 2° de jeu, etc.). Le résultat alimente le tableau `SERVO_TICK_OFFSETS[12]` dans [SpiderConfig.h](../libraries/SpiderConfig/SpiderConfig.h).

## Pré-requis

- PCA9685 + alim servo OK.
- Mapping câblage validé (cf. [ServoSweep](../ServoSweep/README.md)).
- Tu sais quel robot tu calibres (Laurent ou Philippe).

## Compiler et uploader

Pour Laurent :
```bash
./bin/arduino-cli compile --libraries sketches/Nano/libraries \
  --build-property "build.extra_flags=-DROBOT_LAURENT" \
  --fqbn arduino:avr:nano \
  sketches/Nano/ServoTrim/ServoTrim.ino
./bin/arduino-cli upload --fqbn arduino:avr:nano \
  --port /dev/serial/by-id/usb-1a86_USB_Serial-if00-port0 \
  sketches/Nano/ServoTrim/ServoTrim.ino
```

Pour Philippe : remplacer `-DROBOT_LAURENT` par `-DROBOT_PHILIPPE`, FQBN `arduino:avr:nano:cpu=atmega328old`, port `usb-1a86_USB2.0-Serial-if00-port0`.

## Moniteur série interactif

```bash
./bin/arduino-cli monitor -p /dev/ttyUSB0 -c baudrate=9600
```

## Commandes série

| Commande | Effet |
|----------|-------|
| `0` à `11` | Sélectionne un servo (index logique, cf. `SERVO_CHANNELS` dans SpiderConfig.h) |
| `+` / `-` | Ajuste ±1 tick (~0.88°) sur le servo courant |
| `+N` / `-N` | Ajuste ±N ticks (ex: `+5`) |
| `r` | Reset offset du servo courant à 0 |
| `ra` | Reset all |
| `p` | Affiche la table des 12 offsets |
| `h` | Aide |

## Workflow type

1. Upload, ouvrir le moniteur.
2. Pour chaque servo désaligné :
   - Le sélectionner (`<idx>`)
   - Ajuster (`+`, `-`, etc.) jusqu'à ce que le palonnier soit pile à 90° visuel
3. `p` pour afficher tous les offsets.
4. Copier la table, l'intégrer dans `SERVO_TICK_OFFSETS` de `SpiderConfig.h` sous le `#if defined(ROBOT_X)` correspondant.

## Notes

- L'offset est **clampé** entre -100 et +100 ticks.
- Les valeurs ne sont pas persistées en EEPROM : à chaque reset/upload, tout repart à 0. C'est pour ça qu'on les copie dans `SpiderConfig.h` une fois validées.
