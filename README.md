1. Download simulation project from [project file](project_files).
2. Download the zip [src](src) file compile in IDE and upload the ,hex file to Arduino and run.
3. After uploading just run the code.
4. Find project report here. [Report](Report.pdf
)

---

> **Note:** This documentation section was added after the submission deadline, to describe in more detail how the project works and how to use it. The source code, simulation files and report were not changed; only this documentation below content was updated.

> **Safety notice:** this project was designed and verified **in simulation only** ...

## How the Project Works
...
---

> **Safety notice:** this project was designed and verified **in simulation only** (Proteus, 220 V / 50 Hz supply). Do not connect it to real mains. A real build needs galvanic isolation, a fuse, a MOV for voltage spikes, a proper enclosure, and compliance testing.

## How the Project Works

The device protects an appliance from over-voltage and over-current. It measures the true RMS voltage and current every mains cycle, opens a relay when a limit is exceeded, reconnects when it is safe again, and locks out if faults keep repeating. All limits can be changed from the device using an LCD and buttons.

**Default pin map** (change it in `config.h` if your wiring differs):

| Function | Pin |
|---|---|
| Voltage sense | A0 |
| Current sense | A1 |
| Relay driver | D8 |
| LEDs: green, amber, red | D22, D23, D24 |
| Buttons: UP, DOWN, SELECT, BACK, RESET | D26, D27, D28, D29, D30 (each to GND, internal pull-ups) |
| LCD: RS, E, D4, D5, D6, D7 | D40, D41, D42, D43, D44, D45 |

### Measurement

A Timer1 interrupt at 8 kHz alternately samples A0 and A1, giving 4 kS/s per channel, which is 80 samples per 50 Hz cycle. Once per cycle the RMS of the AC part is calculated (which removes the 2.5 V bias) and converted to volts and amps with calibration gains:

| Quantity | Gain | Derived from |
|---|---|---|
| Voltage | 0.933 V per ADC count (used .09421) | (5 V / 1024) ÷ 0.1279 divider ratio × 24.44 transformer ratio |
| Current | 0.074 A per ADC count | (5 V / 1024) ÷ 0.066 V/A sensor sensitivity |


### Settings

| Setting | Default | User Upadting Range |
|---|---|---|
| Over-voltage limit | 250 V | 230 to 300 V |
| Over-current limit | 10 A | 1 to 12 A |
| Trip delay (mild faults) | 40 ms | 0 to 1000 ms |
| Reconnect delay | 5000 ms | 1000 to 60000 ms |
| Lockout: max faults | 5 | 1 to 20 |
| Lockout: window | 300 s | 10 to 3600 s |



## How to Use It

### Compile tip

The Arduino IDE needs the main `.ino` file to sit in a folder with the same name. If the IDE asks to move the sketch into a folder, accept. Select the board **Arduino Mega or Mega 2560**. The `LiquidCrystal` library used by the display is included with the IDE. In Proteus, load the `.hex` file into the Mega part.

### LEDs

| LED | Meaning |
|---|---|
| Green solid | Normal, load connected |
| Yellow solid | Fault detected, load disconnected, waiting to reconnect |
| Red solid | Lockout, not safe to reset |
| Red blinking | Lockout, safe to press RESET |

### LCD and buttons

- The **status screen** shows live voltage and current, the load state, and the number of faults counted inside the lockout window.
- Press **SELECT** to open the settings menu. **UP / DOWN** choose a setting, **SELECT** starts editing it, **UP / DOWN** change the value, **SELECT** saves it, and **BACK** cancels or goes back.
- The menu returns to the status screen after 30 seconds without a button press.
- **RESET** works at any time and is not part of the menu.

### Try it in simulation

1. Run the simulation with a 220 V, 50 Hz source. After the reconnect delay the green LED turns on and the relay closes.
2. Raise the source above the over-voltage limit 250V. The relay opens and the YELLOW LED turns on. Return to voltage less than 250 V and the load reconnects after the reconnect delay (5 Sec in simulation time).
3. To test over-current limit use setting to change it default value to lower than load current 9.2A (0-9A). This disconnect device and again when over-current limit changed above load current 9.2 A via user interface the load reconnects in this case for demonestration purpose i ignored recoonect delay(explained in video). The device trips, reconnects, and trips again until the lockout count is reached, then the red LED lights.
4. In lockout, press RESET. It is refused while the voltage is high and accepted once the voltage is safe and the red LED blinks.
5. Open the menu and change a limit, for example the over-voltage limit to 240 V, and check that a 245 V source now trips.

Proteus runs well below real time with the 8 kHz interrupt, so judge delays by the simulated clock, not by wall-clock time.

## Source Code Structure

```
├── VoltageProtector.ino   main application: setup(), loop(), maps state to LEDs
├── config.h               pins, constants, default calibration gains
├── adc_sampler.h/.cpp     timer-driven sampling of A0 and A1
├── measurement.h/.cpp     RMS calculation and unit conversion
├── cal_voltage.h/.cpp     voltage calibration gain (with range check)
├── cal_current.h/.cpp     current calibration gain (with range check)
├── settings.h/.cpp        adjustable settings with defaults and limits
├── protection.h/.cpp      fault detection, reconnect and lockout state machine
├── relay.h/.cpp           relay driver (starts open)
├── leds.h/.cpp            status LEDs
├── buttons.h/.cpp         debounced buttons
├── display.h/.cpp         16x2 LCD output
└── ui.h/.cpp              status screen and settings menu
```

Each module has one responsibility and talks to the others through its header. Hardware access is isolated in the drivers, and the protection logic never touches the ADC directly.

## Test Results (Simulation)

| Test | Expected | Observed |
|---|---|---|
|Voltage≤250 (wrong value in report 220 V) && Fault ≤Fix number of fault | Safe load operating green LED on  | Achived |
| 220≥Voltage≤250 && Fault ≤Fix number | Load operating Blue LED on for abnormal  | Not Achieved |
| Voltage≥250 && Fault ≤Fix number of fault  | Load Disconnected RED LED on restart when voltage≤250 | Achieved |
| Voltage≤250 && Fault ≥Fix number of fault   | Load Disconnected Manual Reset needed | Achieved |
Measured RMS voltage matched the source setting and measured RMS current matched the simulated meters. 

## Limitations and Planned Work
- Settings are stored in RAM and return to their defaults at power-off. EEPROM storage with a checksum is planned.
- The computer communication link (a serial command protocol for reading data and changing settings) was a plan but not yet integrated due to time limitations.
- Very fast transients (microsecond spikes) are out of scope. Real hardware would use a fuse, a MOV and a hardware fast-trip.
