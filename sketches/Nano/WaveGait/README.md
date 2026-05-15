# WaveGait

Marche **wave gait statique** : à chaque pas, une seule patte est en l'air et les 3 autres au sol poussent le corps vers l'avant. Cycle dans l'ordre **diagonal alterné** FR → RL → FL → RR.

## Cycle d'un pas

Pour la patte courante :
1. **Lift** (200 ms) : FEMUR + TIBIA replient encore pour décoller le pied.
2. **Swing** (400 ms) : COXA avance de `+COXA_AMP` ; pendant ce temps les 3 autres COXA reculent chacune de `COXA_AMP/3` (le corps avance).
3. **Drop** (200 ms) : FEMUR + TIBIA reviennent à leur position pose-debout.

Après 4 pas complets, toutes les COXA reviennent à leur position de départ.

## Quand l'utiliser

Premier test de **locomotion**, une fois que la pose debout fonctionne ([StandPose](../StandPose/README.md)).

## Pré-requis

- Tout ce que demande [StandPose](../StandPose/README.md).
- **Premier essai pattes en l'air** (robot sur support), pour observer le pattern sans charge.
- Surface lisse (parquet, table) pour le test au sol — sur tapis ou moquette les SG90 manquent de couple pour faire glisser les pieds.

## Compiler et uploader

Pour Laurent :
```bash
./bin/arduino-cli compile --libraries sketches/Nano/libraries \
  --build-property "build.extra_flags=-DROBOT_LAURENT" \
  --fqbn arduino:avr:nano \
  sketches/Nano/WaveGait/WaveGait.ino
./bin/arduino-cli upload --fqbn arduino:avr:nano \
  --port /dev/serial/by-id/usb-1a86_USB_Serial-if00-port0 \
  sketches/Nano/WaveGait/WaveGait.ino
```

Pour Philippe : `-DROBOT_PHILIPPE`, FQBN `:cpu=atmega328old`, port `usb-1a86_USB2.0-Serial-...`.

## Paramètres ajustables (en tête du sketch)

| Const | Défaut | Effet |
|-------|--------|-------|
| `COXA_AMP` | 30 | Amplitude swing COXA (°). **Divisible par 3** pour éviter le drift (chaque ground-step recule de `AMP/3`). |
| `LIFT_AMP` | 25 | Amplitude levée FEMUR+TIBIA |
| `LIFT_MS` / `SWING_MS` / `DROP_MS` | 200 / 400 / 200 | Durées de chaque sous-phase |

Et par patte, dans `LEGS[]` :
| Champ | Rôle |
|-------|------|
| `coxa_stand`, `femur_stand`, `tibia_stand` | Angles pose debout |
| `lift_dir` (±1) | Sens pour replier davantage le FEMUR/TIBIA |
| `coxa_dir` (±1) | Sens du COXA pour avancer la patte. **À valider** : si le robot recule ou tourne, inverser certains `coxa_dir`. |

## Observation

```
=== WaveGait : Laurent ===
Pose debout OK. Wave gait dans 2s...
>>> Swing FR
>>> Swing RL
>>> Swing FL
>>> Swing RR
>>> Swing FR
...
```

Vérifier :
- L'ordre des pattes (diagonale alternée).
- Le sens : le robot avance ou recule ?
- La stabilité : pas de balancement excessif quand une patte est en l'air.
- Les COXA reviennent bien à leur position de départ à la fin de chaque cycle de 4 pas (sinon drift).

## Notes

- Le sketch applique `calibratedTicks`, donc les offsets de `SpiderConfig.h` sont actifs.
- L'algorithme suppose une pose debout symétrique (COXA=90 partout). La pose asymétrique de StandPose Laurent (gauche H, droite X) **n'est pas reprise** ici — le wave gait part d'un H pur.
