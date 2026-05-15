# ServoSweep

Balaye chaque canal du PCA9685 (0 à 15) en oscillation douce (±15° autour du neutre) pendant ~3 s par canal, en annonçant le canal en cours dans la série. **Identification du câblage** servo → canal.

## Quand l'utiliser

Au montage initial pour reconstituer le mapping canal PCA9685 → articulation physique. Tu observes quelle articulation oscille quand `Canal X` apparaît dans la série, et tu remplis le tableau pour ton robot dans [SpiderConfig.h](../libraries/SpiderConfig/SpiderConfig.h).

## Pré-requis

- PCA9685 répondant à `0x40` ([I2CScan](../I2CScan/README.md)).
- Alim servo `V+` branchée.
- 12 servos câblés sur le PCA9685 (les 4 canaux inutilisés ne réagiront pas, c'est normal).

## Compiler et uploader

```bash
./bin/arduino-cli compile --fqbn arduino:avr:nano sketches/Nano/ServoSweep/ServoSweep.ino
./bin/arduino-cli upload --fqbn arduino:avr:nano --port /dev/ttyUSB0 sketches/Nano/ServoSweep/ServoSweep.ino
```

Pour Philippe : `:cpu=atmega328old`.

## Lire la sortie série

```bash
./bin/arduino-cli monitor -p /dev/ttyUSB0 -c baudrate=9600
```

## Observation

```
>>> Canal 0 en mouvement   (oscille ~3 s)
>>> Canal 1 en mouvement
...
>>> Canal 15 en mouvement
--- Tour complet termine. Pause 3s avant nouvelle passe. ---
```

Note pour chaque canal annoncé quelle articulation physique bouge. Le sketch boucle indéfiniment, tu peux refaire des passes pour vérifier.

## Notes

- L'amplitude est volontairement étroite (±15°) pour ne pas forcer si une patte est déjà montée.
- Pas de calibration appliquée. Les palonniers peuvent ne pas être pile à 90°, c'est sans importance pour cette identification.
