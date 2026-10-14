# 4.03 – Sensor API, Bus und Emulator

Das vollständige Demo zeigt die Kette Devicetree-Alias → BMI160-Treiber →
`sensor_sample_fetch()` → `sensor_channel_get()` → Anwendungseinheit. Der Emulator wird
außerhalb des Sensorzugriffs mit bekannten Werten befüllt.

```sh
west build -p always -b native_sim \
  training-zephyr/practices/day4/4.03-sensor-api-walkthrough/demo \
  -d build/day4-4.03
./build/day4-4.03/zephyr/zephyr.exe
```

Der Walkthrough ist kein Teilnehmerprojekt und besitzt daher kein `start`/`solution`-Paar.

