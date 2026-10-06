#include <WiFi.h>
#include <WebServer.h>
#include <ESP32Servo.h>


const bool USAR_AP = false;

const char* WIFI_SSID  = "A15 de zize";
const char* WIFI_SENHA = "jujuba03";

const char* AP_SSID  = "Doogo";
const char* AP_SENHA = "doogo";

const int PINO_SERVO = 19;

const int ANGULO_ABERTO  = 90;
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
        <span class="breve">
            Em breve
        </span>
    </p>
    <p class="valor">
        -- <small>g</small>
    </p>
    <div class="barra"></div>
    <p class="nota">
        A balança vai mostrar quanto ainda tem na tigela.
    </p>
</section>

<div class="mini">
    <section class="card">
        <p class="rotulo">
            Água
            <span class="breve">
                Em breve
            </span>
        </p>
        <p class="valor">
            -- <small>°C</small>
        </p>
        <p class="nota">
            Monitoramento futuro.
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
 //ROTAS

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

  server.send(
    200,
    "text/plain; charset=utf-8",
    "Porção liberada!"
  );
}


void setup() {

  Serial.begin(115200);


  // SERVO

  ESP32PWM::allocateTimer(0);

  servo1.setPeriodHertz(50);

  servo1.attach(
    PINO_SERVO,
    500,
    2400
  );

  fechar();

  delay(500);


  // WIFI

  if (USAR_AP) {

    WiFi.softAP(
        AP_SSID,
        AP_SENHA
    );


    Serial.print(
        "Rede criada: "
    );

    Serial.println(
        AP_SSID
    );


    Serial.print(
        "Acesse: http://"
    );

    Serial.println(
        WiFi.softAPIP()
    );

  }

  else {

    WiFi.begin(
        WIFI_SSID,
        WIFI_SENHA
    );


    Serial.print(
        "Conectando"
    );


    while (
        WiFi.status()
        != WL_CONNECTED
    ) {

        delay(500);

        Serial.print(".");

    }


    Serial.println();


    Serial.print(
        "Acesse: http://"
    );

    Serial.println(
        WiFi.localIP()
    );

  }


  // ROTAS

  server.on(
      "/",
      rotaPrincipal
  );

  server.on(
      "/abrir",
      rotaAbrir
  );

  server.on(
      "/fechar",
      rotaFechar
  );

  server.on(
      "/dose",
      rotaDose
  );


  server.begin();


  Serial.println(
      "Servidor iniciado!"
  );
}

void loop() {
  server.handleClient();
}