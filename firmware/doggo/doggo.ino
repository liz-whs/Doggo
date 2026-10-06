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
    <title>Doggo</title>
    <style>
        :root {
            --creme: #FFF4E4;
            --papel: #FFFFFF;
            --tinta: #231B15;
            --suave: #7A6A5C;
            --coral: #FF5A36;
            --coral-esc: #E04424;
            --bola: #FFE14D;
            --agua: #3D8BFF;
            --verde: #2DB46B;
            --linha: #F1DFC8;
            --titulo: Bayon, Impact, "Haettenschweiler", "Arial Narrow Bold", "Roboto Condensed", "sans-serif-condensed", sans-serif;
        }
        * { box-sizing: border-box; }
        body {
            margin: 0;
            min-height: 100vh;
            font-family: "Segoe UI", Roboto, "Helvetica Neue", Arial, sans-serif;
            color: var(--tinta);
            background: var(--creme);
            background-image: radial-gradient(var(--linha) 1.5px, transparent 1.5px);
            background-size: 22px 22px;
        }
        .app { max-width: 980px; margin: 0 auto; padding: 20px 22px 40px 16px; }

        header { display: flex; align-items: center; justify-content: space-between; margin-bottom: 20px; }
        .marca { display: flex; align-items: center; gap: 10px; }
        .marca .logo {
            width: 44px; height: 44px; border-radius: 50%;
            background: var(--coral); display: grid; place-items: center;
        }
        .marca span { font-family: var(--titulo); font-size: 30px; letter-spacing: 1px; text-transform: uppercase; line-height: 1; }
        .pilula {
            display: inline-flex; align-items: center; gap: 8px;
            padding: 8px 14px; border-radius: 999px;
            background: var(--papel); border: 2px solid var(--tinta);
            font-size: 13px; font-weight: 700; text-transform: uppercase; letter-spacing: .5px;
        }
        .pilula i { width: 9px; height: 9px; border-radius: 50%; background: var(--verde); }
        .pilula.off i { background: var(--coral); }

        .grade { display: grid; gap: 16px; grid-template-columns: 1fr; }
        @media (min-width: 760px) {
            .grade { grid-template-columns: 1.25fr 1fr; }
            .hero { grid-row: span 2; }
        }

        .cartao {
            background: var(--papel);
            border: 2px solid var(--tinta);
            border-radius: 26px;
            padding: 22px;
            box-shadow: 5px 5px 0 var(--tinta);
        }
        .rotulo {
            font-size: 12px; font-weight: 800; text-transform: uppercase;
            letter-spacing: 1px; color: var(--suave); margin: 0 0 10px;
            display: flex; justify-content: space-between; align-items: center;
        }
        .breve {
            font-size: 10px; background: var(--bola); color: var(--tinta);
            padding: 3px 8px; border-radius: 999px; letter-spacing: .5px;
        }

        .hero { display: flex; flex-direction: column; gap: 18px; }
        .hero-topo { display: flex; align-items: center; gap: 16px; }
        .mascote { flex: 0 0 auto; width: 112px; height: 112px; border-radius: 50%; background: var(--agua); display: flex; align-items: flex-end; justify-content: center; overflow: hidden; }
        .mascote img { display: block; width: 92px; height: auto; margin: 0 0 12px 2px; }
        h1 {
            font-family: var(--titulo); font-weight: 400;
            font-size: clamp(38px, 9vw, 58px); line-height: .92;
            text-transform: uppercase; margin: 0; letter-spacing: .5px;
        }
        h1 mark { background: none; color: var(--coral); }
        .sub { margin: 0; color: var(--suave); font-size: 16px; line-height: 1.45; }

        .tigela { position: relative; height: 92px; margin-top: 4px; }
        .tigela svg { position: absolute; left: 50%; bottom: 0; transform: translateX(-50%); }
        .grao {
            position: absolute; top: -10px; width: 12px; height: 10px;
            border-radius: 50%; background: #B9773A; opacity: 0;
        }
        .caindo .grao { animation: cair .9s cubic-bezier(.5,0,.8,.6) forwards; }
        @keyframes cair {
            0% { opacity: 1; transform: translateY(0) rotate(0); }
            85% { opacity: 1; }
            100% { opacity: 0; transform: translateY(70px) rotate(200deg); }
        }

        button { font: inherit; cursor: pointer; border: 2px solid var(--tinta); }
        button:disabled { opacity: .55; cursor: wait; }
        .cta {
            width: 100%; display: flex; align-items: center; justify-content: space-between;
            padding: 8px 8px 8px 24px; border-radius: 999px;
            background: var(--coral); color: #fff;
            font-size: 18px; font-weight: 800; letter-spacing: .3px;
            box-shadow: 4px 4px 0 var(--tinta); transition: transform .12s, box-shadow .12s, background .2s;
        }
        .cta .seta { width: 46px; height: 46px; border-radius: 50%; background: #fff; display: grid; place-items: center; border: 2px solid var(--tinta); }
        .cta:hover { background: var(--coral-esc); }
        .cta:active, .sec:active { transform: translate(3px,3px); box-shadow: 1px 1px 0 var(--tinta); }
        .duplo { display: grid; grid-template-columns: 1fr 1fr; gap: 10px; }
        .sec {
            padding: 13px; border-radius: 999px; background: var(--papel);
            font-weight: 800; font-size: 15px; box-shadow: 3px 3px 0 var(--tinta);
            transition: transform .12s, box-shadow .12s, background .2s;
        }
        .sec:hover { background: var(--bola); }

        #status {
            min-height: 1.4em; margin: 0; text-align: center;
            font-weight: 700; font-size: 15px;
        }
        #status.ok { color: var(--verde); }
        #status.erro { color: var(--coral-esc); }

        .valor { font-family: var(--titulo); font-size: 46px; line-height: 1; margin: 2px 0 12px; }
        .valor small { font-size: 22px; color: var(--suave); }
        .barra { height: 12px; border-radius: 999px; background: var(--linha); overflow: hidden; border: 2px solid var(--tinta); }
        .barra b { display: block; height: 100%; width: 0; background: var(--coral); }
        .linha { display: flex; justify-content: space-between; align-items: center; gap: 12px; }
        .chave {
            width: 58px; height: 32px; border-radius: 999px; background: var(--linha);
            border: 2px solid var(--tinta); position: relative; flex: 0 0 auto; opacity: .6;
        }
        .chave::after {
            content: ""; position: absolute; top: 50%; left: 3px; transform: translateY(-50%);
            box-sizing: border-box; width: 22px; height: 22px;
            border-radius: 50%; background: #fff; border: 2px solid var(--tinta);
        }
        .nota { margin: 8px 0 0; font-size: 13px; color: var(--suave); }
        .mini { display: grid; gap: 16px; grid-template-columns: 1fr 1fr; }
        .mini .cartao { padding: 18px; }
        .mini .valor { font-size: 36px; margin-bottom: 0; }

        footer { margin-top: 26px; text-align: center; font-size: 12px; font-weight: 700; letter-spacing: 1px; text-transform: uppercase; color: var(--suave); }
    </style>
</head>
<body>
<div class="app">
    <header>
        <div class="marca">
            <div class="logo">
                <svg width="26" height="26" viewBox="0 0 24 24" fill="#fff" aria-hidden="true"><ellipse cx="12" cy="16" rx="5.2" ry="4.4"/><ellipse cx="5.4" cy="10.4" rx="2.2" ry="2.8"/><ellipse cx="18.6" cy="10.4" rx="2.2" ry="2.8"/><ellipse cx="9" cy="5.6" rx="2.1" ry="2.7"/><ellipse cx="15" cy="5.6" rx="2.1" ry="2.7"/></svg>
            </div>
            <span>Doggo</span>
        </div>
        <div class="pilula" id="conexao"><i></i><span>Conectado</span></div>
    </header>

    <main class="grade">
        <section class="cartao hero">
            <div class="hero-topo">
                <div class="mascote" aria-hidden="true">
                    <img src="data:image/webp;base64,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" alt="" width="104" height="84">
                </div>
                <h1>Hora de <mark>comer</mark>?</h1>
            </div>
            <p class="sub">Libere ração para o seu pet de qualquer cômodo da casa.</p>

            <div class="tigela" id="tigela" aria-hidden="true">
                <svg width="150" height="52" viewBox="0 0 150 52">
                    <path d="M8 6 H142 L128 44 Q126 50 118 50 H32 Q24 50 22 44 Z" fill="#FF5A36" stroke="#231B15" stroke-width="3"/>
                    <rect x="4" y="2" width="142" height="9" rx="4.5" fill="#FFE14D" stroke="#231B15" stroke-width="3"/>
                </svg>
            </div>

            <button class="cta" id="btnDose" onclick="enviar('dose')">
                Liberar uma porção
                <span class="seta"><svg width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="#231B15" stroke-width="2.6" stroke-linecap="round" stroke-linejoin="round"><path d="M5 12h14M13 6l6 6-6 6"/></svg></span>
            </button>
            <div class="duplo">
                <button class="sec" onclick="enviar('abrir')">Abrir</button>
                <button class="sec" onclick="enviar('fechar')">Fechar</button>
            </div>
            <p id="status" role="status"></p>
        </section>

        <section class="cartao">
            <p class="rotulo">Ração na tigela <span class="breve">Em breve</span></p>
            <p class="valor">-- <small>g</small></p>
            <div class="barra"><b></b></div>
            <p class="nota">A balança vai mostrar quanto ainda tem na tigela.</p>
        </section>

        <div class="mini">
            <section class="cartao">
                <p class="rotulo">Água <span class="breve">Em breve</span></p>
                <p class="valor">-- <small>°C</small></p>
                <div class="linha" style="margin-top:12px">
                    <span class="nota" style="margin:0">Fonte</span>
                    <span class="chave" aria-label="Fonte de água (em breve)"></span>
                </div>
            </section>
            <section class="cartao">
                <p class="rotulo">Última porção</p>
                <p class="valor" id="ultima">--:--</p>
                <p class="nota" id="ultimaNota">Nenhuma porção ainda</p>
            </section>
        </div>
    </main>

    <footer>Doggo · alimentador inteligente</footer>
</div>

<script>
    function cairRacao() {
        const t = document.getElementById('tigela');
        t.querySelectorAll('.grao').forEach(g => g.remove());
        for (let i = 0; i < 9; i++) {
            const g = document.createElement('span');
            g.className = 'grao';
            g.style.left = (42 + Math.random() * 16) + '%';
            g.style.animationDelay = (i * 0.07) + 's';
            t.appendChild(g);
        }
        t.classList.remove('caindo'); void t.offsetWidth; t.classList.add('caindo');
    }

    function enviar(acao) {
        const status = document.getElementById('status');
        const conexao = document.getElementById('conexao');
        const botoes = document.querySelectorAll('button');
        status.className = '';
        status.textContent = 'Enviando...';
        botoes.forEach(b => b.disabled = true);
        fetch('/' + acao)
            .then(r => r.text())
            .then(t => {
                status.textContent = t;
                status.className = 'ok';
                conexao.className = 'pilula';
                conexao.lastElementChild.textContent = 'Conectado';
                if (acao === 'dose') {
                    cairRacao();
                    const agora = new Date();
                    document.getElementById('ultima').textContent =
                        agora.toLocaleTimeString('pt-BR', { hour: '2-digit', minute: '2-digit' });
                    document.getElementById('ultimaNota').textContent = 'Liberada por esta página';
                }
            })
            .catch(() => {
                status.textContent = 'Erro: sem conexão com o Doggo';
                status.className = 'erro';
                conexao.className = 'pilula off';
                conexao.lastElementChild.textContent = 'Sem conexão';
            })
            .finally(() => botoes.forEach(b => b.disabled = false));
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
  server.send(200, "text/plain; charset=utf-8", "Porção liberada!");
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