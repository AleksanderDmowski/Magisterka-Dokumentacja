#include <WiFi.h>
#include <WebServer.h>

// Dane sieci Wi-Fi
const char* ssid = "nazwa sieci wifi";
const char* password = "haslo sieci wifi";

// Ustawione sterowanie stanem NISKIM (Low Level Trigger) odwrócone specjalnie
#define RELAY_ON  LOW
#define RELAY_OFF HIGH

const int pinLed = 2; // Dioda statusowa na ESP32 (aktywna stanem HIGH) pokazująca że pzyjmowane są sygnały na wejścia (na wypadek lużnych pinów wyjścia)
const int pinButton1 = 26;
const int pinButton2 = 27;

WebServer server(80);

// Zmienne przechowujące aktualny stan przekaźników w pamięci
bool relay1Active = false;
bool relay2Active = false;

const char HTML_PAGE[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="pl">
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=no">
  <title>ESP32 Control</title>
  
  <meta name="mobile-web-app-capable" content="yes">
  <meta name="theme-color" content="#121212">
  <link rel="icon" type="image/svg+xml" href="data:image/svg+xml,<svg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 100 100'><rect width='100' height='100' rx='20' fill='%23ff8400'/><circle cx='50' cy='50' r='30' fill='none' stroke='white' stroke-width='8'/><line x1='50' y1='25' x2='50' y2='50' stroke='white' stroke-width='8' stroke-linecap='round'/></svg>">

  <style>
    body { font-family: Arial, sans-serif; text-align: center; margin-top: 40px; background: #121212; color: white; user-select: none; -webkit-user-select: none; }
    h2 { font-size: 22px; margin-bottom: 5px; }
    h4 { font-size: 14px; color: #aaa; margin-top: 0; margin-bottom: 30px; }
    .btn {
      width: 220px; height: 85px; font-size: 22px; font-weight: bold; margin: 15px;
      border: none; border-radius: 14px; background-color: #ff8400; color: white;
      cursor: pointer; touch-action: none; user-select: none; -webkit-user-select: none;
    }
    .btn:active { background-color: #0056b3; transform: scale(0.96); }
  </style>
</head>
<body>
  <h2>Sterownik do przesłon okiennych</h2>
  <h4>Karta rozszerzeń WiFi na ESP32</h4>

  <button class="btn" id="btn1">Góra</button>
  <br>
  <button class="btn" id="btn2">Dół</button>

  <script>
    function setupButton(id, pathName) {
      const btn = document.getElementById(id);
      let isPressed = false;

      function start(e) {
        if (e.cancelable) e.preventDefault();
        if (!isPressed) {
          isPressed = true;
          fetch('/' + pathName + '/on').catch(err => console.log(err));
        }
      }

      function stop(e) {
        if (e.cancelable) e.preventDefault();
        if (isPressed) {
          isPressed = false;
          fetch('/' + pathName + '/off').catch(err => console.log(err));
        }
      }

      btn.addEventListener('pointerdown', start);
      btn.addEventListener('pointerup', stop);
      btn.addEventListener('pointerleave', stop);
      btn.addEventListener('pointercancel', stop);
    }

    setupButton('btn1', 'btn1');
    setupButton('btn2', 'btn2');
  </script>
</body>
</html>
)rawliteral";

// Funkcja aktualizująca stan diody LED na podstawie zmiennych stanu
void updateLedState() {
  if (relay1Active || relay2Active) {
    digitalWrite(pinLed, HIGH); // Włącz diodę na GPIO 2
  } else {
    digitalWrite(pinLed, LOW);  // Wyłącz diodę na GPIO 2
  }
}

void setup() {
  Serial.begin(115200);

  //konfiguracja pinów jako OUTPUT
  pinMode(pinButton1, OUTPUT);
  pinMode(pinButton2, OUTPUT);
  pinMode(pinLed, OUTPUT);

  //ustawienie bezpiecznego stanu WYŁĄCZONEGO na starcie (wyjścia przekaźników w stan HIGH)
  digitalWrite(pinButton1, RELAY_OFF);
  digitalWrite(pinButton2, RELAY_OFF);
  digitalWrite(pinLed, LOW);

  relay1Active = false;
  relay2Active = false;
  updateLedState();

  //łączenie z siecią Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Łączenie z Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nPołączono!");
  Serial.print("Adres IP: ");
  Serial.println(WiFi.localIP());

  //obsługa ścieżek serwera HTTP
  server.on("/", []() { 
    server.send(200, "text/html", HTML_PAGE); 
  });

  //obsługa przekaźnika 1 (pin 26)
  server.on("/btn1/on", []() { 
    digitalWrite(pinButton1, RELAY_ON); 
    relay1Active = true;
    updateLedState();
    Serial.println("Góra ON (pin 26 -> LOW)");
    server.send(200, "text/plain", "OK"); 
  });
  
  server.on("/btn1/off", []() { 
    digitalWrite(pinButton1, RELAY_OFF); 
    relay1Active = false;
    updateLedState();
    Serial.println("Góra OFF (pin 26 -> HIGH)");
    server.send(200, "text/plain", "OK");
  });

  //obsługa przekaźnika 2 (pin 27)
  server.on("/btn2/on", []() { 
    digitalWrite(pinButton2, RELAY_ON); 
    relay2Active = true;
    updateLedState();
    Serial.println("Dół ON (pin 27 -> LOW)");
    server.send(200, "text/plain", "OK"); 
  });
  
  server.on("/btn2/off", []() { 
    digitalWrite(pinButton2, RELAY_OFF); 
    relay2Active = false;
    updateLedState();
    Serial.println("Dół OFF (pin 27 -> HIGH)");
    server.send(200, "text/plain", "OK"); 
  });

  server.begin();
}

void loop() {
  server.handleClient();
}