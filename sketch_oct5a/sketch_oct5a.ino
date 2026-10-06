#include <WiFi.h>
#include <WebServer.h>
#include <ESP32Servo.h>

const bool USAR_AP = false;

const char* WIFI_SSID  = "A15 de zize";
const char* WIFI_SENHA = "jujuba03";

const char* AP_SSID  = "Doogo";
const char* AP_SENHA = "doogo";

const int PINO_SERVO = 19;

const int ANGULO_ABERTO  = 180;     
const int ANGULO_FECHADO = 0;   

const int TEMPO_DOSE = 400;

Servo servo1;
WebServer server(80);

const char PAGINA[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="pt-BR">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Doogo</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            display: flex;
            justify-content: center;
            align-items: center;
            height: 100vh;
            margin: 0;
            background-color: #2e1730;
        }
        .container {
            text-align: center;
            background-color: #c3a7cb;
            padding: 20px;
            border-radius: 8px;
            box-shadow: 0 4px 8px rgba(0, 0, 0, 0.1);
            width: 90%;
            max-width: 600px;
            border: 2px solid #85708b;
        }
        h1 { color: #2e1730; }
        p  { color: #2e1730; font-size: 1.1em; }
        button {
            background-color: #7b5282;
            color: #F4F0EA;
            padding: 10px 20px;
            border: none;
            border-radius: 5px;
            cursor: pointer;
            font-size: 1em;
            transition: background-color 0.3s ease;
            margin-top: 10px;
            width: 60%;
        }
        button:hover {
            background-color: #d495d5;
            color: #583a52;
        }
        #status {
            margin-top: 15px;
            min-height: 1.2em;
            font-weight: bold;
            color: #2e1730;
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>Doogo</h1>
        <p>Libere comida para o seu pet automaticamente</p>
        <button onclick="enviar('abrir')">Abrir</button><br>
        <button onclick="enviar('fechar')">Fechar</button><br>
        <button onclick="enviar('dose')">Uma dose</button>
        <div id="status"></div>
    </div>

    <script>
        function enviar(acao) {
            const status = document.getElementById('status');
            status.textContent = 'Enviando...';
            fetch('/' + acao)
                .then(r => r.text())
                .then(t => status.textContent = t)
                .catch(() => status.textContent = 'Erro: sem conexao com o Doogo');
        }
    </script>
</body>
</html>
)rawliteral";

void abrir() {
  servo1.write(ANGULO_ABERTO);
}

void fechar() {
  servo1.write(ANGULO_FECHADO);
}

void umaDose() {
  servo1.write(ANGULO_ABERTO);
  delay(TEMPO_DOSE);
  servo1.write(ANGULO_FECHADO);
}

void rotaPrincipal() {
  server.send_P(200, "text/html; charset=utf-8", PAGINA);
}

void rotaAbrir() {
  abrir();
  server.send(200, "text/plain; charset=utf-8", "Aberto");
}

void rotaFechar() {
  fechar();
  server.send(200, "text/plain; charset=utf-8", "Fechado");
}

void rotaDose() {
  umaDose();
  server.send(200, "text/plain; charset=utf-8", "Dose liberada!");
}

void setup() {
  Serial.begin(115200);

  // Servo
  ESP32PWM::allocateTimer(0);
  servo1.setPeriodHertz(50);
  servo1.attach(PINO_SERVO, 500, 2400);
  fechar();                   
  delay(500);

  // Wi-Fi
  if (USAR_AP) {
    WiFi.softAP(AP_SSID, AP_SENHA);
    Serial.print("Rede criada: ");
    Serial.println(AP_SSID);
    Serial.print("Acesse: http://");
    Serial.println(WiFi.softAPIP());   
  } else {
    WiFi.begin(WIFI_SSID, WIFI_SENHA);
    Serial.print("Conectando");
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println();
    Serial.print("Acesse: http://");
    Serial.println(WiFi.localIP());
  }

  server.on("/", rotaPrincipal);
  server.on("/abrir", rotaAbrir);
  server.on("/fechar", rotaFechar);
  server.on("/dose", rotaDose);
  server.begin();
}

void loop() {
  server.handleClient();
}