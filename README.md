#  Smart RGB Lamp — NodeMCU ESP8266 (Access Point Mode)

A standalone **IoT RGB Lamp** project using the **NodeMCU ESP8266** microcontroller.  
This smart lamp hosts its own Wi-Fi network and interactive **web dashboard** — no internet or external router needed!

---

##  Features

-  **Wi-Fi Access Point (AP)** — works completely offline  
-  **Built-in Web Interface**
  - Modern color picker (browser color wheel)
  - Purple-themed preset buttons
  - RGB sliders for fine control
  - Live background color sync
-  **Instant LED Response**
  - Updates LED color in real time
-  **Mood / Pulse Effect**
  - Smooth pulsing of selected color
-  **Mobile Friendly**
  - Responsive layout and large touch targets

---

##  Hardware Setup

| LED Pin | NodeMCU Pin | GPIO | Notes |
|----------|--------------|------|-------|
| Red      | D5           | 14   | PWM supported |
| Green    | D6           | 12   | PWM supported |
| Blue     | D7           | 13   | PWM supported |
| Common   | GND          | —    | For **common cathode RGB LED** |

>  If you have a **common anode LED**, invert the PWM logic (e.g., `analogWrite(PIN_R, 1023 - toPwm(valR));`).

> Add ESP8266 Board to Arduino IDE:
1. Open Arduino IDE → File → Preferences
2. In the “Additional Boards Manager URLs” field, paste this line:
 " https://arduino.esp8266.com/stable/package_esp8266com_index.json "
3. Go to Tools → Board → Boards Manager
4. Search for ESP8266 → click Install
5. Then select your board:
6. Tools → Board → NodeMCU 1.0 (ESP-12E Module)
   
---

##  Requirements

- **Board:** NodeMCU / ESP8266 (tested with ESP-12E)
- **IDE:** Arduino IDE (v1.8.19+ or Arduino 2.x)
- **Libraries:**
  - [ESP8266WiFi.h](https://github.com/esp8266/Arduino)
  - [ESP8266WebServer.h](https://github.com/esp8266/Arduino)

> Install **ESP8266 board package** via *Tools → Board Manager → esp8266 by ESP8266 Community*.

---

##  How to Use

1. **Flash the sketch** to your NodeMCU.
2. The board creates a Wi-Fi hotspot:

   | SSID | Password |
   |-------|-----------|
   | `RGB-Workshop` | `12345678` |

3. On your phone or laptop:
   - Connect to the Wi-Fi **RGB-Workshop**
   - Open a browser and visit → **http://192.168.4.1**
4. Control your RGB LED from the web page!



