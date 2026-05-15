# ServoPose

Place chaque servo à un angle précis et statique via la série. **Outil d'exploration** pour vérifier le sens de rotation d'un servo, sa plage utile, ou trouver l'angle qui donne une posture précise.

## Quand l'utiliser

- Vérifier le sens de rotation d'une articulation (par ex. "à quel angle FL_FEMUR pointe vers le haut ?").
- Tester la plage mécanique d'un servo (jusqu'où il peut aller sans forcer).
- Préparer manuellement une posture avant de l'inscrire dans un sketch automatique.

## Pré-requis

- PCA9685 + alim servo OK.
- Offsets calibrés via [ServoTrim](../ServoTrim/README.md) (intégrés dans SpiderConfig.h). Le sketch utilise `calibratedTicks` automatiquement.

## Compiler et uploader

Pour Laurent :
```bash
./bin/arduino-cli compile --libraries sketches/Nano/libraries \
  --build-property "build.extra_flags=-DROBOT_LAURENT" \
  --fqbn arduino:avr:nano \
  sketches/Nano/ServoPose/ServoPose.ino
./bin/arduino-cli upload --fqbn arduino:avr:nano \
  --port /dev/serial/by-id/usb-1a86_USB_Serial-if00-port0 \
  sketches/Nano/ServoPose/ServoPose.ino
```

Pour Philippe : `-DROBOT_PHILIPPE`, FQBN `:cpu=atmega328old`, port `usb-1a86_USB2.0-Serial-...`.

## Moniteur série interactif

```bash
./bin/arduino-cli monitor -p /dev/ttyUSB0 -c baudrate=9600
```

## Commandes série

| Commande | Effet |
|----------|-------|
| `<idx> <angle>` | Positionne servo `idx` (0-11) à `angle` (0-180°). Ex: `0 60`, `5 120` |
| `c` | Tous les servos à 90° (centre) |
| `p` | Affiche les angles courants |
| `h` | Aide |

## Index → articulation

| idx | nom | idx | nom |
|-----|-----|-----|-----|
| 0 | FR_COXA | 6 | RL_COXA |
| 1 | FR_FEMUR | 7 | RL_FEMUR |
| 2 | FR_TIBIA | 8 | RL_TIBIA |
| 3 | FL_COXA | 9 | RR_COXA |
| 4 | FL_FEMUR | 10 | RR_FEMUR |
| 5 | FL_TIBIA | 11 | RR_TIBIA |

## Notes

- Au démarrage, tous les servos passent à 90°. Robot suspendu recommandé si tu ne sais pas dans quelle position il est.
- Pas d'interpolation : le mouvement vers la cible est immédiat. Évite les sauts trop brusques sur un robot monté (faire des petits pas, par ex. `0 80` puis `0 70` puis `0 60` plutôt que `0 30` direct).
