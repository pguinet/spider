# ObservePose

Place le robot dans une **pose statique distinctive** pour vérifier le sens de chaque articulation :
- Pattes avant (FR, FL) : COXA pointant vers l'avant
- Pattes arrière (RL, RR) : COXA pointant vers l'arrière
- 4 FEMUR levés (cuisses vers le haut)
- 4 TIBIA repliés (pieds vers le corps)

## Quand l'utiliser

Pour découvrir la **polarité de rotation** des FEMUR/TIBIA d'un robot fraîchement câblé. Tu observes la pose obtenue, et pour chaque articulation qui part dans le mauvais sens, tu inverses son angle (60 ↔ 120) dans le sketch et tu refais l'upload.

C'est ainsi qu'a été établi que :
- **Philippe** : miroir gauche/droite + inversion avant/arrière
- **Laurent** : miroir gauche/droite uniquement

## Pré-requis

- PCA9685 + alim servo OK.
- Mapping câblage validé ([NamedServoTest](../NamedServoTest/README.md)).
- **Robot suspendu sur un support**, pattes dans le vide, pour le premier essai. Si une articulation va dans le mauvais sens et force contre une butée mécanique, on évite la casse.

## Compiler et uploader

Pour Laurent :
```bash
./bin/arduino-cli compile --libraries sketches/Nano/libraries \
  --build-property "build.extra_flags=-DROBOT_LAURENT" \
  --fqbn arduino:avr:nano \
  sketches/Nano/ObservePose/ObservePose.ino
./bin/arduino-cli upload --fqbn arduino:avr:nano \
  --port /dev/serial/by-id/usb-1a86_USB_Serial-if00-port0 \
  sketches/Nano/ObservePose/ObservePose.ino
```

Pour Philippe : `-DROBOT_PHILIPPE`, FQBN `:cpu=atmega328old`, port `usb-1a86_USB2.0-Serial-...`.

## Observation

Le sketch :
1. Met tous les servos à 90° (1 s)
2. Interpole en douceur (2 s) vers la pose cible
3. Reste statique

Si une articulation va dans le sens **inverse** de ce qui est attendu, éditer le bloc d'angles correspondant dans le sketch (par ex. remplacer `RL_FEMUR_DEG = 60` par `RL_FEMUR_DEG = 120`) et relancer.

## Notes

- Pas d'utilisation des offsets calibrés — on cherche juste à valider le **sens** de rotation, pas la précision angulaire absolue.
- Une fois la polarité validée, les angles trouvés alimentent [StandPose](../StandPose/README.md) et [WaveGait](../WaveGait/README.md).
