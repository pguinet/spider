# StandPose

Met le robot en **posture debout** : COXA neutre (ou décalé pour pose en X/H), FEMUR levés, TIBIA repliés pour que les pieds touchent le sol. Interpolation douce 2 s depuis la pose étendue (90° partout).

## Quand l'utiliser

- Premier test fonctionnel après calibrage : le robot doit tenir sur ses 4 pieds.
- Comme état initial avant un sketch de marche ([WaveGait](../WaveGait/README.md)).

## Pré-requis

- PCA9685 + alim servo OK.
- Mapping câblage validé ([NamedServoTest](../NamedServoTest/README.md)).
- Polarité validée ([ObservePose](../ObservePose/README.md)).
- Offsets calibrés (le sketch applique `calibratedTicks`).
- **Robot suspendu** pour le premier essai. Une fois validé en l'air, on peut le poser au sol.

## Compiler et uploader

Pour Laurent :
```bash
./bin/arduino-cli compile --libraries sketches/Nano/libraries \
  --build-property "build.extra_flags=-DROBOT_LAURENT" \
  --fqbn arduino:avr:nano \
  sketches/Nano/StandPose/StandPose.ino
./bin/arduino-cli upload --fqbn arduino:avr:nano \
  --port /dev/serial/by-id/usb-1a86_USB_Serial-if00-port0 \
  sketches/Nano/StandPose/StandPose.ino
```

Pour Philippe : `-DROBOT_PHILIPPE`, FQBN `:cpu=atmega328old`, port `usb-1a86_USB2.0-Serial-...`.

## Angles par robot (au moment d'écrire)

Les valeurs sont définies via `#if defined(ROBOT_X)` dans le sketch.

**Philippe** — miroir gauche/droite + inversion avant/arrière :
- FR : COXA=90, FEMUR=60, TIBIA=60
- FL : COXA=90, FEMUR=120, TIBIA=120
- RR : COXA=90, FEMUR=120, TIBIA=120 (inversion)
- RL : COXA=90, FEMUR=60, TIBIA=60 (inversion)

**Laurent** — miroir gauche/droite seulement, posture asymétrique (gauche en H, droite en X) :
- FR : COXA=135 (avant), FEMUR=60, TIBIA=60
- FL : COXA=90, FEMUR=120, TIBIA=120
- RR : COXA=45 (arrière), FEMUR=60, TIBIA=60
- RL : COXA=90, FEMUR=120, TIBIA=120

## Notes

- Si une patte va dans le mauvais sens à l'upload : repasser par [ObservePose](../ObservePose/README.md) pour comprendre la polarité, puis ajuster les angles dans le bloc `#if defined(ROBOT_X)` du sketch.
- L'interpolation à 2 s évite les sauts brusques. Si tu changes des angles très différents de la pose courante, garde une marge ou repasse par `ServoCenter` avant.
