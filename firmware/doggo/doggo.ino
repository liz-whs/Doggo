#include <WiFi.h>
#include <WebServer.h>
#include <ESP32Servo.h>
#include <HX711.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <time.h>

// ================= CONFIGURAÇÕES =================

const bool USAR_AP = false;

const char* WIFI_SSID  = "Doggo";
const char* WIFI_SENHA = "doggo123";

const char* AP_SSID  = "Doggo";
const char* AP_SENHA = "doggo123";

// ----- TELEGRAM -----
const char* BOT_TOKEN = "9294654656:AAHaMsMtbiz2TArFsTlfpdAq8oas6x8JkEI";
const char* CHAT_ID   = "9216754638";

const int   PORCAO_G = 25;                               
const float TEMP_LIMITE = 25.0;                          
const unsigned long INTERVALO_ALERTA_MS = 30UL * 60UL * 1000UL;  
const unsigned long INTERVALO_TELEGRAM_MS = 3000;       
const long FUSO_SEGUNDOS = -3 * 3600;                   

// Servo
const int PINO_SERVO = 19;
const int ANGULO_ABERTO  = 90;
const int ANGULO_FECHADO = 0;
const int TEMPO_DOSE = 400;

// Célula de carga (HX711)
const int PINO_HX_DT  = 16;
const int PINO_HX_SCK = 17;

// Termômetro DS18B20 (resistor de 4,7k entre DATA e 3V3)
const int PINO_TEMP = 25;

// Calibração da balança (ajuste conforme explicado)
float FATOR_CALIBRACAO = 420.0;

// ================= OBJETOS =================

Servo servo1;
WebServer server(80);

HX711 balanca;
OneWire oneWire(PINO_TEMP);
DallasTemperature sensorTemp(&oneWire);

WiFiClientSecure clienteSeguro;
UniversalTelegramBot bot(BOT_TOKEN, clienteSeguro);

unsigned long ultimoTelegram = 0;
bool alertaTempAtivo = false;
unsigned long ultimoAlertaTemp = 0;

String ultimaHora = "--:--";
String ultimaNota = "Nenhuma porção ainda";
String ultimaDataHora = "";
String notificarDosePendente = "";

float pesoGramas = 0;
float tempAgua   = NAN;
unsigned long ultimaLeitura = 0;

// ================= PÁGINA =================

const char PAGINA[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="pt-BR">

<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">

<title>Doggo</title>

<style>

:root {
    --creme: #FFF4E4;
    --papel: #FFFFFF;
    --tinta: #231B15;
    --suave: #7A6A5C;
    --coral: #FF5A36;
    --coral-esc: #E04424;
    --amarelo: #FFE14D;
    --azul: #3D8BFF;
    --verde: #2DB46B;
    --linha: #F1DFC8;
}

* {
    box-sizing: border-box;
}

body {
    margin: 0;
    min-height: 100vh;

    font-family: "Segoe UI", Arial, sans-serif;

    color: var(--tinta);
    background-color: var(--creme);

    background-image:
        radial-gradient(var(--linha) 1.5px, transparent 1.5px);

    background-size: 22px 22px;
}

.app {
    max-width: 950px;
    margin: auto;
    padding: 22px;
}

/* CABECALHO */

header {
    display: flex;
    align-items: center;
    justify-content: space-between;
    margin-bottom: 20px;
}

.marca {
    display: flex;
    align-items: center;
    gap: 10px;
}

.logo {
    width: 46px;
    height: 46px;

    border-radius: 50%;

    background: var(--coral);

    display: grid;
    place-items: center;

    border: 2px solid var(--tinta);
}

.logo svg {
    width: 27px;
    height: 27px;
}

.nome {
    font-size: 29px;
    font-weight: 900;
    letter-spacing: 1px;
    text-transform: uppercase;
}

.conexao {
    display: flex;
    align-items: center;
    gap: 8px;

    padding: 8px 13px;

    background: white;

    border: 2px solid var(--tinta);
    border-radius: 999px;

    font-size: 12px;
    font-weight: 800;
    text-transform: uppercase;
}

.bolinha {
    width: 9px;
    height: 9px;

    background: var(--verde);

    border-radius: 50%;
}

.conexao.erro .bolinha {
    background: var(--coral);
}

/* GRID */

.grade {
    display: grid;
    grid-template-columns: 1fr;
    gap: 16px;
}

@media(min-width: 760px) {

    .grade {
        grid-template-columns: 1.25fr 1fr;
    }

    .principal {
        grid-row: span 2;
    }
}

/* CARDS */

.card {
    background: var(--papel);

    border: 2px solid var(--tinta);
    border-radius: 25px;

    padding: 22px;

    box-shadow: 5px 5px 0 var(--tinta);
}

.rotulo {
    margin: 0 0 10px;

    color: var(--suave);

    font-size: 12px;
    font-weight: 800;

    text-transform: uppercase;
    letter-spacing: 1px;

    display: flex;
    justify-content: space-between;
}

.breve {
    padding: 3px 8px;
    border-radius: 999px;
    background: var(--amarelo);
    color: var(--tinta);
    font-size: 10px;
}

.principal {
    display: flex;
    flex-direction: column;
    gap: 18px;
}

.hero {
    display: flex;
    align-items: center;
    gap: 15px;
}

.mascote {
    width: 105px;
    height: 105px;
    flex: 0 0 auto;
    border-radius: 50%;
    background: var(--azul);
    display: grid;
    place-items: center;
    border: 2px solid var(--tinta);
}

.mascote svg {
    width: 64px;
    height: 64px;
}

h1 {
    margin: 0;
    font-size: clamp(38px, 8vw, 56px);
    line-height: .95;
    text-transform: uppercase;
    font-weight: 900;
}

h1 span {
    color: var(--coral);
}

.sub {
    margin: 0;
    color: var(--suave);
    line-height: 1.5;
}

.tigela {
    position: relative;
    height: 90px;
    display: flex;
    align-items: flex-end;
    justify-content: center;
    overflow: hidden;
}

.tigela svg {
    position: relative;
    z-index: 2;
}

.grao {
    position: absolute;
    top: 0;
    width: 11px;
    height: 9px;
    border-radius: 50%;
    background: #B9773A;
    opacity: 0;
}

.caindo .grao {
    animation: cair .8s forwards;
}

@keyframes cair {
    0% {
        opacity: 1;
        transform: translateY(0) rotate(0);
    }
    85% {
        opacity: 1;
    }
    100% {
        opacity: 0;
        transform: translateY(65px) rotate(180deg);
    }
}

button {
    font: inherit;
    cursor: pointer;
    border: 2px solid var(--tinta);
}

button:disabled {
    opacity: .55;
    cursor: wait;
}

.dose {
    width: 100%;
    display: flex;
    justify-content: space-between;
    align-items: center;
    padding: 8px 8px 8px 23px;
    border-radius: 999px;
    background: var(--coral);
    color: white;
    font-size: 17px;
    font-weight: 800;
    box-shadow: 4px 4px 0 var(--tinta);
}

.dose:hover {
    background: var(--coral-esc);
}

.seta {
    width: 45px;
    height: 45px;
    display: grid;
    place-items: center;
    background: white;
    color: var(--tinta);
    border: 2px solid var(--tinta);
    border-radius: 50%;
}

.duplo {
    display: grid;
    grid-template-columns: 1fr 1fr;
    gap: 10px;
}

.secundario {
    padding: 13px;
    border-radius: 999px;
    background: white;
    font-weight: 800;
    box-shadow: 3px 3px 0 var(--tinta);
}

.secundario:hover {
    background: var(--amarelo);
}

button:active {
    transform: translate(3px, 3px);
    box-shadow: 1px 1px 0 var(--tinta);
}

#status {
    min-height: 22px;
    margin: 0;
    text-align: center;
    font-weight: 700;
}

#status.ok {
    color: var(--verde);
}

#status.erro {
    color: var(--coral-esc);
}

.valor {
    margin: 5px 0;
    font-size: 44px;
    font-weight: 900;
}

.valor small {
    font-size: 20px;
    color: var(--suave);
}

.barra {
    height: 12px;
    border: 2px solid var(--tinta);
    border-radius: 999px;
    background: var(--linha);
    overflow: hidden;
}

#nivel {
    height: 100%;
    width: 0%;
    background: var(--verde);
    transition: width .5s;
}

.nota {
    margin: 10px 0 0;
    color: var(--suave);
    font-size: 13px;
}

.mini {
    display: grid;
    grid-template-columns: 1fr 1fr;
    gap: 16px;
}

.mini .card {
    padding: 18px;
}

.mini .valor {
    font-size: 32px;
}

footer {
    margin-top: 25px;
    text-align: center;
    color: var(--suave);
    font-size: 11px;
    font-weight: 800;
    letter-spacing: 1px;
    text-transform: uppercase;
}
</style>
</head>
<body>
<div class="app">
<header>
    <div class="marca">
        <div class="logo">
            <svg viewBox="0 0 24 24" fill="white">
                <ellipse cx="12" cy="16" rx="5.2" ry="4.4"/>
                <ellipse cx="5.4" cy="10.4" rx="2.2" ry="2.8"/>
                <ellipse cx="18.6" cy="10.4" rx="2.2" ry="2.8"/>
                <ellipse cx="9" cy="5.6" rx="2.1" ry="2.7"/>
                <ellipse cx="15" cy="5.6" rx="2.1" ry="2.7"/>
            </svg>
        </div>
        <span class="nome">Doggo</span>
    </div>
    <div class="conexao" id="conexao">
        <span class="bolinha"></span>
        <span id="textoConexao">
            Conectado
        </span>
    </div>
</header>

<main class="grade">

<section class="card principal">
    <div class="hero">
        <div class="mascote">
            <svg viewBox="0 0 24 24" fill="white">
                <ellipse cx="12" cy="16" rx="5.2" ry="4.4"/>
                <ellipse cx="5.4" cy="10.4" rx="2.2" ry="2.8"/>
                <ellipse cx="18.6" cy="10.4" rx="2.2" ry="2.8"/>
                <ellipse cx="9" cy="5.6" rx="2.1" ry="2.7"/>
                <ellipse cx="15" cy="5.6" rx="2.1" ry="2.7"/>
            </svg>
        </div>
        <h1>
            Hora de
            <span>comer?</span>
        </h1>
    </div>
    <p class="sub">
        Libere ração para o seu pet de qualquer cômodo da casa.
    </p>


    <div class="tigela" id="tigela">
        <svg width="150" height="52"
             viewBox="0 0 150 52">
            <path
                d="M8 6 H142 L128 44 Q126 50 118 50 H32 Q24 50 22 44 Z"
                fill="#FF5A36"
                stroke="#231B15"
                stroke-width="3"
            />
            <rect
                x="4"
                y="2"
                width="142"
                height="9"
                rx="4.5"
                fill="#FFE14D"
                stroke="#231B15"
                stroke-width="3"
            />
        </svg>
    </div>
    <button
        class="dose"
        id="btnDose"
        onclick="enviar('dose')"
    >
        Liberar uma porção
        <span class="seta">
            →
        </span>
    </button>
    <div class="duplo">
        <button
            class="secundario"
            onclick="enviar('abrir')"
        >
            Abrir
        </button>

        <button
            class="secundario"
            onclick="enviar('fechar')"
        >
            Fechar
        </button>
    </div>
    <p id="status"></p>
</section>

<section class="card">
    <p class="rotulo">
        Ração na tigela
    </p>
    <p class="valor">
        <span id="peso">--</span> <small>g</small>
    </p>
    <div class="barra"><div id="nivel"></div></div>
    <p class="nota">
        A balança mostra quanto ainda tem na tigela.
    </p>
</section>

<div class="mini">
    <section class="card">
        <p class="rotulo">
            Água
        </p>
        <p class="valor">
            <span id="temp">--</span> <small>°C</small>
        </p>
        <p class="nota">
            Temperatura atual.
        </p>
    </section>
    <section class="card">
        <p class="rotulo">
            Última porção
        </p>
        <p
            class="valor"
            id="ultima"
        >
            --:--
        </p>
        <p
            class="nota"
            id="ultimaNota"
        >
            Nenhuma porção ainda
        </p>
    </section>
</div>
</main>
<footer>
    Doggo · alimentador inteligente
</footer>
</div>
<script>
const CAPACIDADE_G = 500; // peso que representa a tigela cheia

function cairRacao() {
    const tigela =
        document.getElementById('tigela');
    tigela
        .querySelectorAll('.grao')
        .forEach(g => g.remove());

    for (let i = 0; i < 8; i++) {
        const grao =
            document.createElement('span');
        grao.className = 'grao';
        grao.style.left =
            (43 + Math.random() * 14) + '%';
        grao.style.animationDelay =
            (i * 0.06) + 's';
        tigela.appendChild(grao);
    }
    tigela.classList.remove('caindo');
    void tigela.offsetWidth;
    tigela.classList.add('caindo');
}

function enviar(acao) {
    const status =
        document.getElementById('status');
    const conexao =
        document.getElementById('conexao');
    const textoConexao =
        document.getElementById('textoConexao');
    const botoes =
        document.querySelectorAll('button');
    status.className = '';
    status.textContent =
        'Enviando...';
    botoes.forEach(
        botao => botao.disabled = true
    );

    fetch('/' + acao)
        .then(resposta => {
            if (!resposta.ok) {
                throw new Error();
            }
            return resposta.text();
        })
        .then(texto => {
            status.textContent = texto;
            status.className = 'ok';
            conexao.className = 'conexao';
            textoConexao.textContent =
                'Conectado';
            if (acao === 'dose') {
                cairRacao();
                const agora =
                    new Date();
                document
                    .getElementById('ultima')
                    .textContent =
                    agora.toLocaleTimeString(
                        'pt-BR',
                        {
                            hour: '2-digit',
                            minute: '2-digit'
                        }
                    );
                document
                    .getElementById('ultimaNota')
                    .textContent =
                    'Liberada por esta página';
                atualizarDados();
            }
        })

        .catch(() => {
            status.textContent =
                'Erro: sem conexão com o Doggo';
            status.className =
                'erro';
            conexao.className =
                'conexao erro';
            textoConexao.textContent =
                'Sem conexão';
        })

        .finally(() => {
            botoes.forEach(
                botao => botao.disabled = false
            );
        });
}

function atualizarDados() {
    fetch('/dados')
        .then(r => r.json())
        .then(d => {
            document.getElementById('peso').textContent =
                d.peso.toFixed(0);
            document.getElementById('temp').textContent =
                d.temp === null ? '--' : d.temp.toFixed(1);

            if (d.ultima) {
                document.getElementById('ultima').textContent = d.ultima;
                document.getElementById('ultimaNota').textContent = d.nota;
            }

            const pct = Math.min(100, (d.peso / CAPACIDADE_G) * 100);
            document.getElementById('nivel').style.width = pct + '%';

            document.getElementById('conexao').className = 'conexao';
            document.getElementById('textoConexao').textContent = 'Conectado';
        })
        .catch(() => {
            document.getElementById('conexao').className = 'conexao erro';
            document.getElementById('textoConexao').textContent = 'Sem conexão';
        });
}

atualizarDados();
setInterval(atualizarDados, 2000);

</script>

</body>
</html>
)rawliteral";


// ================= SERVO =================

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

// ================= SENSORES =================

void lerSensores() {
  if (millis() - ultimaLeitura < 1000) return;   // lê a cada 1 segundo
  ultimaLeitura = millis();

  if (balanca.is_ready()) {
    pesoGramas = balanca.get_units(3);
    if (pesoGramas < 0) pesoGramas = 0;
  }

  sensorTemp.requestTemperatures();
  float t = sensorTemp.getTempCByIndex(0);
  tempAgua = (t == DEVICE_DISCONNECTED_C || t == 85.0) ? NAN : t;
}

// ================= DATA/HORA E TELEGRAM =================

const char* TECLADO = "[[\"/liberar\"],[\"/status\",\"/ultima\"],[\"/peso\",\"/temperatura\"],[\"/dieta\"]]";

const char TEXTO_DIETA[] = R"rawliteral(🐶 CLASSIFICAÇÃO DOS CACHORROS POR PORTE E QUANTIDADE DE RAÇÃO

📌 Cálculo utilizado:

1. RER = 70 × Peso(kg)^0,75
2. MER = RER × 1,6
3. Quantidade de ração (g) = MER ÷ 3,8

Considerando uma ração com densidade energética média de 3.800 kcal/kg.

🐾 PORTE MINI / TOY
• Peso: até 4 kg
• Quantidade: aproximadamente 40 a 90 g por dia.

🐾 PORTE PEQUENO
• Peso: 4 a 10 kg
• Quantidade: aproximadamente 90 a 165 g por dia.

🐾 PORTE MÉDIO
• Peso: 10 a 25 kg
• Quantidade: aproximadamente 165 a 325 g por dia.

🐾 PORTE GRANDE
• Peso: 25 a 45 kg
• Quantidade: aproximadamente 325 a 500 g por dia.

🐾 PORTE GIGANTE
• Peso: acima de 45 kg
• Quantidade: aproximadamente 500 g ou mais por dia.

⚠️ OBSERVAÇÕES:

* Os valores são estimativas para cães adultos saudáveis.
* A quantidade pode variar conforme idade, castração, atividade física e condição corporal.
* Filhotes, idosos e cães com necessidades especiais precisam de ajustes.
* Recomenda-se dividir a quantidade diária em duas refeições.

Importante: os valores são referências médias e não substituem a orientação de um médico-veterinário.)rawliteral";

String textoMenu() {
  String s = "🐶 Olá! Eu sou o Doggo, seu alimentador inteligente.\n\n";
  s += "📋 Comandos disponíveis:\n\n";
  s += "🍽️ /liberar - libera uma porção de ração (cerca de " + String(PORCAO_G) + " g)\n";
  s += "📊 /status - resumo com peso, temperatura e última porção\n";
  s += "⚖️ /peso - quanto de ração tem na tigela\n";
  s += "🌡️ /temperatura - temperatura da água\n";
  s += "🕒 /ultima - data e hora da última liberação\n";
  s += "🐕 /dieta - ração indicada por porte do cachorro\n";
  s += "📋 /menu - mostra esta mensagem de novo\n\n";
  s += "🔔 Aviso automático: se a água passar de " + String(TEMP_LIMITE, 0) + " °C, eu te alerto!";
  return s;
}

String dataHoraAgora() {
  struct tm t;
  if (!getLocalTime(&t, 300)) return "hora indisponível";
  char buf[24];
  strftime(buf, sizeof(buf), "%d/%m/%Y %H:%M:%S", &t);
  return String(buf);
}

// Guarda quando e como a ração foi liberada e marca para avisar no Telegram
void registrarDose(String origem) {
  struct tm t;
  if (getLocalTime(&t, 300)) {
    char h[8], d[12], c[24];
    strftime(h, sizeof(h), "%H:%M", &t);
    strftime(d, sizeof(d), "%d/%m/%Y", &t);
    strftime(c, sizeof(c), "%d/%m/%Y %H:%M:%S", &t);
    ultimaHora = String(h);
    ultimaNota = String(d) + " · via " + origem;
    ultimaDataHora = String(c);
  } else {
    ultimaHora = "--:--";
    ultimaNota = "Liberada via " + origem + " (sem hora)";
    ultimaDataHora = "hora indisponível";
  }
  notificarDosePendente = origem;
}

void enviarMenu(String chat, String texto) {
  bot.sendMessageWithReplyKeyboard(chat, texto, "", TECLADO, true);
}

void notificarDose() {
  if (notificarDosePendente == "") return;

  String origem = notificarDosePendente;
  notificarDosePendente = "";

  String msg = "✅ Ração liberada!\n\n";
  msg += "📅 " + ultimaDataHora + "\n";
  msg += "📲 Acionada por: " + origem + "\n";
  msg += "🍖 Porção: cerca de " + String(PORCAO_G) + " g";
  bot.sendMessage(CHAT_ID, msg, "");
}

String textoStatus() {
  String s = "📊 Status do Doggo\n\n";
  s += "⚖️ Ração na tigela: " + String(pesoGramas, 0) + " g\n";
  s += "🌡️ Água: " + (isnan(tempAgua) ? String("sem leitura") : String(tempAgua, 1) + " °C") + "\n";
  s += "🍽️ Última porção: " + (ultimaDataHora == "" ? String("nenhuma ainda") : ultimaDataHora) + "\n";
  s += "🕒 Agora: " + dataHoraAgora();
  return s;
}

void verificarTemperatura() {
  if (isnan(tempAgua)) return;

  if (tempAgua > TEMP_LIMITE) {
    if (!alertaTempAtivo || millis() - ultimoAlertaTemp >= INTERVALO_ALERTA_MS) {
      alertaTempAtivo = true;
      ultimoAlertaTemp = millis();

      String msg = "🌡️🔥 ALERTA: a água está quente!\n\n";
      msg += "Temperatura: " + String(tempAgua, 1) + " °C\n";
      msg += "Limite: " + String(TEMP_LIMITE, 0) + " °C\n";
      msg += "📅 " + dataHoraAgora();
      bot.sendMessage(CHAT_ID, msg, "");
    }
  } else if (alertaTempAtivo && tempAgua <= TEMP_LIMITE - 0.5) {
    alertaTempAtivo = false;

    String msg = "✅ A temperatura da água voltou ao normal: " + String(tempAgua, 1) + " °C\n";
    msg += "📅 " + dataHoraAgora();
    bot.sendMessage(CHAT_ID, msg, "");
  }
}

void tratarMensagens(int n) {
  for (int i = 0; i < n; i++) {
    String chat  = bot.messages[i].chat_id;
    String texto = bot.messages[i].text;
    texto.trim();

    int at = texto.indexOf('@');
    if (at > 0) texto = texto.substring(0, at);

    // Responde para qualquer um: serve para descobrir o seu chat ID
    if (texto == "/id") {
      bot.sendMessage(chat, "Seu chat ID é: " + chat, "");
      continue;
    }

    if (chat != String(CHAT_ID)) {
      bot.sendMessage(chat, "⛔ Acesso negado.", "");
      continue;
    }

    if (texto == "/start" || texto == "/menu") {
      enviarMenu(chat, textoMenu());

    } else if (texto == "/liberar" || texto == "/racao") {
      umaDose();
      registrarDose("Telegram");
      notificarDose();

    } else if (texto == "/dieta" || texto == "/porte" || texto == "/racaoindicada") {
      bot.sendMessage(chat, String(TEXTO_DIETA), "");

    } else if (texto == "/peso") {
      bot.sendMessage(chat, "⚖️ Ração na tigela: " + String(pesoGramas, 0) + " g", "");

    } else if (texto == "/temperatura") {
      if (isnan(tempAgua)) {
        bot.sendMessage(chat, "🌡️ Sem leitura de temperatura no momento.", "");
      } else {
        bot.sendMessage(chat, "🌡️ Água: " + String(tempAgua, 1) + " °C", "");
      }

    } else if (texto == "/status") {
      bot.sendMessage(chat, textoStatus(), "");

    } else if (texto == "/ultima") {
      if (ultimaDataHora == "") {
        bot.sendMessage(chat, "Nenhuma porção liberada ainda.", "");
      } else {
        bot.sendMessage(chat, "🍽️ Última porção liberada em:\n" + ultimaDataHora + "\n(" + ultimaNota + ")", "");
      }

    } else {
      bot.sendMessage(chat, "Não entendi 🤔 Use /menu para ver as opções.", "");
    }
  }
}

void iniciarTelegram() {
  if (USAR_AP) {
    Serial.println("Telegram desativado no modo AP (sem internet).");
    return;
  }

  clienteSeguro.setInsecure();
  configTime(FUSO_SEGUNDOS, 0, "pool.ntp.org", "time.google.com");

  Serial.print("Sincronizando hora");
  struct tm t;
  int tentativas = 0;
  while (!getLocalTime(&t, 500) && tentativas < 20) {
    Serial.print(".");
    tentativas++;
  }
  Serial.println();
  Serial.println("Hora: " + dataHoraAgora());

  String msg = textoMenu();   // mensagem inicial: lista de comandos
  if (bot.sendMessageWithReplyKeyboard(CHAT_ID, msg, "", TECLADO, true)) {
    Serial.println("Telegram conectado.");
  } else {
    Serial.println("Telegram: nao consegui enviar. Confira BOT_TOKEN e CHAT_ID (envie /id para o bot).");
  }
}

void loopTelegram() {
  if (USAR_AP || WiFi.status() != WL_CONNECTED) return;

  notificarDose();
  verificarTemperatura();

  if (millis() - ultimoTelegram < INTERVALO_TELEGRAM_MS) return;
  ultimoTelegram = millis();

  int n = bot.getUpdates(bot.last_message_received + 1);
  while (n) {
    tratarMensagens(n);
    n = bot.getUpdates(bot.last_message_received + 1);
  }
}

// ================= ROTAS =================

void rotaPrincipal() {
  server.send_P(
    200,
    "text/html; charset=utf-8",
    PAGINA
  );
}

void rotaAbrir() {
  abrir();

  server.send(
    200,
    "text/plain; charset=utf-8",
    "Aberto"
  );
}

void rotaFechar() {
  fechar();

  server.send(
    200,
    "text/plain; charset=utf-8",
    "Fechado"
  );
}

void rotaDose() {
  umaDose();
  registrarDose("Site");

  server.send(
    200,
    "text/plain; charset=utf-8",
    "Porção liberada!"
  );
}

void rotaDados() {
  String json = "{";
  json += "\"peso\":" + String(pesoGramas, 1) + ",";
  json += "\"temp\":" + (isnan(tempAgua) ? String("null") : String(tempAgua, 1)) + ",";
  json += "\"ultima\":\"" + ultimaHora + "\",";
  json += "\"nota\":\"" + ultimaNota + "\"";
  json += "}";

  server.send(200, "application/json", json);
}

// ================= SETUP =================

void setup() {

  Serial.begin(115200);

  // SERVO
  ESP32PWM::allocateTimer(0);
  servo1.setPeriodHertz(50);
  servo1.attach(PINO_SERVO, 500, 2400);
  fechar();
  delay(500);

  // CELULA DE CARGA
  balanca.begin(PINO_HX_DT, PINO_HX_SCK);
  balanca.set_scale(FATOR_CALIBRACAO);
  if (balanca.wait_ready_timeout(2000)) {
    balanca.tare();   // zera com a tigela vazia
    Serial.println("Balanca pronta (tara feita).");
  } else {
    Serial.println("HX711 nao encontrado! Confira DT e SCK.");
  }

  // TEMPERATURA
  sensorTemp.begin();
  sensorTemp.setResolution(10);   // leitura mais rapida (~190 ms)

  // WIFI
  if (USAR_AP) {

    WiFi.softAP(AP_SSID, AP_SENHA);

    Serial.print("Rede criada: ");
    Serial.println(AP_SSID);

    Serial.print("Acesse: http://");
    Serial.println(WiFi.softAPIP());

  } else {

    WiFi.mode(WIFI_STA);
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

  // ROTAS
  server.on("/", rotaPrincipal);
  server.on("/abrir", rotaAbrir);
  server.on("/fechar", rotaFechar);
  server.on("/dose", rotaDose);
  server.on("/dados", rotaDados);

  server.begin();

  Serial.println("Servidor iniciado!");

  iniciarTelegram();
}

// ================= LOOP =================

void loop() {
  server.handleClient();
  lerSensores();
  loopTelegram();
}
