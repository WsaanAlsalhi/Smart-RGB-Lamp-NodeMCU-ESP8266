/* 
  IoT with Microcontrollers
  Smart RGB Lamp (NodeMCU ESP8266)
  - Access Point (AP) mode WiFi
  - Embedded web page with:
      * Modern color wheel (browser color input)
      * Purple preset buttons
      * RGB sliders
      * Live background color change
  - Pins (NodeMCU):
      RED  -> D5 (GPIO14)
      GREEN-> D6 (GPIO12)
      BLUE -> D7 (GPIO13)
  - Assumes Common Cathode RGB LED (common -> GND)
  - Comments: English
*/

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

// ------------------- AP Configuration -------------------
const char* AP_SSID = "RGB-Workshop";
const char* AP_PASS = "12345678"; // min 8 chars

// ------------------- Pins (NodeMCU) -------------------
const uint8_t PIN_R = D5; // GPIO14
const uint8_t PIN_G = D6; // GPIO12
const uint8_t PIN_B = D7; // GPIO13

ESP8266WebServer server(80);

// Current RGB values (0-255)
volatile int valR = 120;
volatile int valG = 60;
volatile int valB = 140;

// Map 0-255 to PWM range 0-1023 for analogWrite on ESP8266
int toPwm(int v) {
  return map(constrain(v, 0, 255), 0, 255, 0, 1023);
}

// Apply RGB to pins
void applyRGB() {
  analogWrite(PIN_R, toPwm(valR));
  analogWrite(PIN_G, toPwm(valG));
  analogWrite(PIN_B, toPwm(valB));
}

// ------------------- Embedded HTML Page -------------------
String pageHTML() {
  String html = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>ESP RGB Workshop</title>
  <style>
    :root { --card-bg: rgba(255,255,255,0.08); --glass: rgba(255,255,255,0.06); }
    body { font-family: Arial, Helvetica, sans-serif; margin:0; padding:16px; color:#fff; transition: background-color 300ms; }
    .container { max-width:700px; margin:0 auto; }
    h2 { margin:0 0 8px 0; font-weight:600; }
    p.small { margin:6px 0 16px 0; color:rgba(255,255,255,0.9); }
    .card { background:var(--card-bg); padding:12px; border-radius:10px; box-shadow: 0 6px 18px rgba(0,0,0,0.25); margin-bottom:12px; }
    .presets { display:flex; gap:8px; flex-wrap:wrap; }
    .btn { padding:8px 10px; border-radius:8px; border:none; cursor:pointer; color:#fff; font-weight:600; box-shadow:0 4px 8px rgba(0,0,0,0.25); }
    .sliders { display:flex; flex-direction:column; gap:8px; }
    .row { display:flex; gap:8px; align-items:center; }
    .label { width:42px; }
    input[type=range] { width:100%; }
    #colorBox { height:80px; border-radius:8px; margin-top:8px; box-shadow: inset 0 -4px 20px rgba(0,0,0,0.2); border:1px solid rgba(255,255,255,0.06); }
    footer { margin-top:12px; font-size:12px; color:rgba(255,255,255,0.85); text-align:center; }
    .info { font-size:13px; color:rgba(255,255,255,0.9); margin-top:8px; }
    .toprow { display:flex; justify-content:space-between; align-items:center; gap:12px; }
    @media (max-width:520px){ .toprow{flex-direction:column; align-items:stretch} }
  </style>
</head>
<body>
  <div class="container">
    <div class="toprow">
      <div>
        <h2>IoT RGB Lamp</h2>
        <p class="small">Connect to WiFi <strong>RGB-Workshop</strong> (pw: 12345678) → Open <strong>http://192.168.4.1</strong></p>
      </div>
      <div class="info">Live & Local — No internet required</div>
    </div>

    <div class="card">
      <div style="display:flex;justify-content:space-between;align-items:center">
        <div>
          <strong>Purple Presets</strong>
          <p class="small">Tap a preset to apply instantly</p>
        </div>
        <div>
          <input id="colorPicker" type="color" value="#7a3fbf" title="Color Wheel / Picker" style="height:40px;width:64px;border-radius:8px;border:none">
        </div>
      </div>

      <div class="presets" style="margin-top:12px">
        <!-- Purple themed presets -->
        <button class="btn" style="background:#EDE7F6" onclick="presetHex('#EDE7F6', '#000')">Lavender</button>
        <button class="btn" style="background:#D1C4E9" onclick="presetHex('#D1C4E9')">Lilac</button>
        <button class="btn" style="background:#B39DDB" onclick="presetHex('#B39DDB')">Light Purple</button>
        <button class="btn" style="background:#7E57C2" onclick="presetHex('#7E57C2')">Violet</button>
        <button class="btn" style="background:#6A1B9A" onclick="presetHex('#6A1B9A')">Deep Purple</button>
        <button class="btn" style="background:#880E4F" onclick="presetHex('#880E4F')">Magenta</button>
      </div>

      <div id="colorBox" style="background:#7A3FBF"></div>
    </div>

    <div class="card">
      <strong>RGB Sliders</strong>
      <div class="sliders" style="margin-top:8px">
        <div class="row"><div class="label">R</div><input id="r" type="range" min="0" max="255" value="120" oninput="updateFromSlider()"><div id="rval">120</div></div>
        <div class="row"><div class="label">G</div><input id="g" type="range" min="0" max="255" value="60" oninput="updateFromSlider()"><div id="gval">60</div></div>
        <div class="row"><div class="label">B</div><input id="b" type="range" min="0" max="255" value="140" oninput="updateFromSlider()"><div id="bval">140</div></div>
      </div>
      <div style="margin-top:8px; display:flex; gap:8px;">
        <button class="btn" style="background:#333" onclick="applyCurrent()">Apply</button>
        <button class="btn" style="background:#555" onclick="setMood()">Pulse</button>
        <button class="btn" style="background:#222" onclick="resetDefault()">Reset</button>
      </div>
    </div>

    <div class="card">
      <strong>How to use</strong>
      <ol style="padding-left:18px; margin:8px 0 0 0;">
        <li>Connect phone to <strong>RGB-Workshop</strong>.</li>
        <li>Open browser → go to <strong>192.168.4.1</strong>.</li>
        <li>Use the Color Picker (wheel), or tap purple presets, or use sliders.</li>
        <li>Background + LED update instantly.</li>
      </ol>
    </div>

    <footer>ESP8266 Access Point • NodeMCU • Enjoy!</footer>
  </div>

<script>
  // Initial values match server defaults
  var r = 120, g = 60, b = 140;

  // Helpers
  function rgbToHex(r,g,b){
    return "#" + [r,g,b].map(function(x){
      var s = parseInt(x).toString(16);
      return (s.length==1) ? "0"+s : s;
    }).join("");
  }

  function hexToRgb(hex) {
    hex = hex.replace('#','');
    if(hex.length===3){ hex = hex.split('').map(h=>h+h).join(''); }
    var bigint = parseInt(hex, 16);
    return {r:(bigint>>16)&255, g:(bigint>>8)&255, b:bigint&255};
  }

  function sendToESP(rv, gv, bv){
    // Send via simple GET
    var xhr = new XMLHttpRequest();
    xhr.open('GET', '/set?r='+rv+'&g='+gv+'&b='+bv, true);
    xhr.send();
  }

  function updateUI(){
    document.getElementById('r').value = r;
    document.getElementById('g').value = g;
    document.getElementById('b').value = b;
    document.getElementById('rval').innerText = r;
    document.getElementById('gval').innerText = g;
    document.getElementById('bval').innerText = b;
    document.getElementById('colorBox').style.background = 'rgb(' + r + ',' + g + ',' + b + ')';
    document.body.style.backgroundColor = 'rgb(' + r + ',' + g + ',' + b + ')';
    // Sync color picker
    document.getElementById('colorPicker').value = rgbToHex(r,g,b);
  }

  function applyCurrent(){
    r = parseInt(document.getElementById('r').value);
    g = parseInt(document.getElementById('g').value);
    b = parseInt(document.getElementById('b').value);
    updateUI();
    sendToESP(r,g,b);
  }

  function updateFromSlider(){
    r = parseInt(document.getElementById('r').value);
    g = parseInt(document.getElementById('g').value);
    b = parseInt(document.getElementById('b').value);
    document.getElementById('rval').innerText = r;
    document.getElementById('gval').innerText = g;
    document.getElementById('bval').innerText = b;
    document.getElementById('colorBox').style.background = 'rgb(' + r + ',' + g + ',' + b + ')';
    document.body.style.backgroundColor = 'rgb(' + r + ',' + g + ',' + b + ')';
    // send quick update
    sendToESP(r,g,b);
    // update color picker
    document.getElementById('colorPicker').value = rgbToHex(r,g,b);
  }

  // Preset button handler using hex color
  function presetHex(hex){
    var c = hexToRgb(hex);
    r = c.r; g = c.g; b = c.b;
    updateUI();
    sendToESP(r,g,b);
  }

  // Color picker (wheel) handler
  document.getElementById('colorPicker').addEventListener('input', function(e){
    var c = hexToRgb(e.target.value);
    r = c.r; g = c.g; b = c.b;
    updateUI();
    sendToESP(r,g,b);
  });

  // Simple pulse effect (client-side)
  var pulseInterval = null;
  function setMood(){
    if(pulseInterval){
      clearInterval(pulseInterval);
      pulseInterval = null;
      return;
    }
    var base = {r:r, g:g, b:b};
    var dir = 1;
    var step = 8;
    pulseInterval = setInterval(function(){
      base.r = Math.min(255, Math.max(0, base.r + dir*step));
      base.g = Math.min(255, Math.max(0, base.g + dir*step));
      base.b = Math.min(255, Math.max(0, base.b + dir*step));
      if(base.r>=250 || base.r<=5) dir *= -1;
      r = Math.round(base.r); g = Math.round(base.g); b = Math.round(base.b);
      updateUI();
      sendToESP(r,g,b);
    }, 180);
  }

  function resetDefault(){
    r = 120; g = 60; b = 140;
    updateUI();
    sendToESP(r,g,b);
  }

  // Initialize UI on load
  window.onload = function(){
    updateUI();
    // initial send to make sure LED matches UI
    sendToESP(r,g,b);
  };
</script>
</body>
</html>
)rawliteral";

  return html;
}

// Root handler
void handleRoot() {
  server.send(200, "text/html", pageHTML());
}

// /set?r=..&g=..&b=..
void handleSet() {
  if (server.hasArg("r")) valR = server.arg("r").toInt();
  if (server.hasArg("g")) valG = server.arg("g").toInt();
  if (server.hasArg("b")) valB = server.arg("b").toInt();
  applyRGB();
  server.send(200, "text/plain", "OK");
}

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println("Starting NodeMCU RGB AP...");

  // Setup pins
  pinMode(PIN_R, OUTPUT);
  pinMode(PIN_G, OUTPUT);
  pinMode(PIN_B, OUTPUT);
  analogWriteRange(1023); // set PWM range

  // Start AP
  WiFi.softAP(AP_SSID, AP_PASS);
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP); // usually 192.168.4.1

  // Setup server routes
  server.on("/", handleRoot);
  server.on("/set", handleSet);

  server.begin();
  Serial.println("HTTP server started");
  Serial.println("Open -> http://192.168.4.1");
}

void loop() {
  server.handleClient();
}
