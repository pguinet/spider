# ServoCenter

Place les 16 canaux du PCA9685 à la position neutre (~90°, 1.5 ms) et les maintient. **Outil de réglage mécanique des palonniers**.

## Quand l'utiliser

- Au premier montage : tous les palonniers sont à 90° physique, ce qui permet de les visser dans leur position de référence.
- Après tout démontage / remplacement de servo.
- Comme état "sûr" quand on veut intervenir sur un robot sans qu'il bouge.

## Pré-requis

- PCA9685 répondant à `0x40` (vérifier avec [I2CScan](../I2CScan/README.md)).
- Alim servo branchée sur `V+` du PCA9685 (alim séparée 5-6 V, masse commune avec le Nano). Sans elle, les servos ne bougent pas.

## Compiler et uploader

```bash
./bin/arduino-cli compile --fqbn arduino:avr:nano sketches/Nano/ServoCenter/ServoCenter.ino
./bin/arduino-cli upload --fqbn arduino:avr:nano --port /dev/ttyUSB0 sketches/Nano/ServoCenter/ServoCenter.ino
```

Pour Philippe : `:cpu=atmega328old`.

## Utilisation

1. Upload → les 16 servos vont immédiatement à 90° (307 ticks).
2. Démonter les palonniers, les remettre dans leur position cible (verticale, parallèle au châssis, etc.), serrer la vis.
3. Le sketch ne change rien après le setup — il maintient juste la consigne PWM.

## Notes

- Ce sketch envoie 307 ticks **sans appliquer les offsets calibrés**. Pour un calibrage fin tenant compte des décalages de palonniers, utiliser [ServoTrim](../ServoTrim/README.md).
- Conserve ce sketch comme "état de repos" si tu veux flasher quelque chose sans qu'un robot fasse un mouvement inattendu.
