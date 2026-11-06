# Doggo

Sistema IoT embarcado em **ESP32** para alimentação e monitoramento da água de pets. O dispositivo funciona como servidor HTTP local e permite controlar o dispenser de ração por uma página web. A evolução planejada inclui pesagem por célula de carga, monitoramento da temperatura da água e integração com **Telegram**.

> **Estado do projeto:** o protótipo físico está montado e a liberação temporizada de ração pela página web funciona no firmware existente. Leitura da balança, cálculo da porção, leitura da temperatura, alertas e Telegram **ainda são pendências**. O acionamento elétrico de bomba/fonte de água foi **retirado do escopo**.

![Interface web do Doggo](docs/interface-web.png)

## Sumário

1. [Funcionalidades e progresso](#funcionalidades-e-progresso)
2. [Hardware e ligações](#hardware-e-ligações)
3. [Rede Wi-Fi e credenciais](#rede-wi-fi-e-credenciais)
4. [Telegram — escopo futuro](#telegram--escopo-futuro)
5. [Protocolos e API](#protocolos-e-api)
6. [Registros de eventos](#registros-de-eventos)
7. [Pendências](#pendências)
8. [Estrutura do repositório](#estrutura-do-repositório)
9. [Como executar](#como-executar)

## Funcionalidades e progresso

| Recurso | Situação |
| :--- | :--- |
| Página de controle servida pelo ESP32 | Implementado |
| Abrir, fechar e liberar ração pelo servomotor | Implementado; dose temporizada |
| Indicador visual de conexão e hora da última porção | Implementado na interface, não persistido no dispositivo |
| Wi-Fi cliente e modo ponto de acesso (AP) | Implementado; configuração diretamente no firmware |
| Nome do AP `Doggo`, senha `doggo123` | Atualizado |
| Leitura de peso HX711 e calibração | Pendente |
| Cálculo da quantidade de ração por peso | Pendente de testes da montagem |
| Leitura DS18B20 | Pendente |
| Alerta de temperatura da água acima de 25 °C | Pendente |
| Comando para liberar ração pelo Telegram | Pendente |
| Mensagem Telegram com data e hora após liberação | Pendente |
| NTP, logs persistentes LittleFS e reconexão automática | Pendente |
| Acionamento de bomba, fonte ou relé | **Fora do escopo** |

Atualmente, o comando `/dose` abre o servo por **400 ms** e o fecha. Isso **não equivale** à dosagem por peso. O horário da última porção mostrado no navegador é uma informação local à interface, não uma confirmação persistida ou enviada ao Telegram.

## Hardware e ligações

| Componente | Função | Pinos previstos |
| :--- | :--- | :--- |
| ESP32 | Controlador com servidor web e Wi-Fi | — |
| Servomotor SG90 (9 g) | Abertura e fechamento do dispenser | Sinal GPIO 19, alimentação 5V |
| Célula de carga de 1 kg + HX711 | Medição da ração na tigela | DT GPIO 16, SCK GPIO 17, alimentação 3V3 |
| DS18B20 à prova d'água | Medição da temperatura da água | DATA GPIO 25, alimentação 3V3, resistor 4,7 kΩ para 3V3 |

Consulte o [guia de conexões](docs/guia-de-conexoes.md). Mantenha alimentação apropriada para o servomotor e GND comum. **Na versão atual do código, somente o servo está integrado ao firmware.** A montagem da balança e do sensor precisa de testes antes de implementar suas leituras.

A bomba, o relé e o acionamento da fonte não fazem mais parte do escopo do Doggo. O sistema deve **monitorar a temperatura** da água, sem ligá-la ou desligá-la eletricamente.

## Rede Wi-Fi e credenciais

O firmware continua com os dois modos existentes e com as credenciais definidas diretamente em `firmware/doggo/doggo.ino`, sem arquivo `secrets.h`:

- `USAR_AP = false` (valor atual): ESP32 tenta conectar-se à rede Wi-Fi cliente configurada nas variáveis `WIFI_SSID` e `WIFI_SENHA`. Os valores dessas variáveis **não foram alterados neste PR**.
- `USAR_AP = true`: ESP32 cria o ponto de acesso **`Doggo`**, com senha **`doggo123`** (8 caracteres).

O nome e a senha acima são do **ponto de acesso criado pelo ESP32**, não da rede Wi-Fi cliente. Para usar o AP, é necessário mudar `USAR_AP` para `true`; isso **não foi feito neste PR**, para preservar o comportamento existente.

A rede da Ana ainda será configurada e testada posteriormente. Não foram implementadas alterações na lógica de conexão ou reconexão automática.

**Observação de segurança:** a senha do AP está escrita no firmware conforme decidido pela equipe. Como o repositório é público, essa senha também ficará pública; não a reutilize em redes ou serviços pessoais. Recomenda-se também trocar credenciais reais anteriormente expostas no histórico do repositório.

## Telegram — escopo futuro

A integração Telegram **ainda não existe no firmware**. Os recursos abaixo descrevem o comportamento desejado para versões futuras.

### Comando de alimentação

Um chat autorizado deverá solicitar a liberação de ração ao bot. O comando exato, a lista de chats autorizados e a forma de autenticação serão definidos posteriormente. O bot somente deve acionar o dispositivo após validar a solicitação.

### Confirmação de liberação

Após a execução do acionamento, o sistema deverá enviar uma mensagem com **data e hora** da liberação e identificar o resultado. O registro deve ocorrer após o evento, e não quando o pedido chegar. Até que a balança esteja integrada, **não é possível afirmar que uma quantidade específica de ração foi efetivamente entregue**.

Exemplo ilustrativo de mensagem futura (não funcional atualmente):

```text
Doggo — liberação de ração
Data: 06/11/2026
Hora: 10:37
Status: comando de acionamento executado
```

### Alerta de temperatura

Quando a medição válida indicar temperatura da água **estritamente maior que 25 °C**, o sistema deverá notificar o chat autorizado via Telegram.

A equipe ainda precisará definir o intervalo de leitura, regra contra mensagens repetidas (anti-spam), condição de retorno à normalidade e tratamento de falhas de sensor, Wi-Fi e envio ao Telegram. A regra não deve tratar ausência de leitura como temperatura normal.

O token do bot deverá ser protegido fora do controle de versão. A integração deve usar comunicação segura com a API do Telegram.

## Protocolos e API

| Camada | Tecnologia | Situação |
| :--- | :--- | :--- |
| Aplicação | HTTP, interface web | Implementado |
| Transporte | TCP, porta 80 | Implementado |
| Rede | IPv4 e Wi-Fi IEEE 802.11 b/g/n | Implementado |
| Sincronização de relógio | NTP | Pendente |
| Mensagens e comandos | API do Telegram sobre HTTPS | Pendente |

### Rotas existentes

| Método | Rota | Comportamento |
| :--- | :--- | :--- |
| `GET` | `/` | Página HTML de controle |
| `GET` | `/abrir` | Abre o servo (90°) |
| `GET` | `/fechar` | Fecha o servo (0°) |
| `GET` | `/dose` | Abre por 400 ms e fecha; responde `Porção liberada!` |

### Rotas planejadas (não implementadas)

- `GET /status`: retornar estado da rede e leituras válidas dos sensores.
- `POST /alimentar`: solicitar dosagem controlada por peso, **após definir e testar o cálculo**.
- `GET /logs`: consultar os eventos persistidos no dispositivo.

A antiga rota planejada `/fonte` foi removida do escopo. Rotas que alteram o estado do dispositivo devem ter acesso protegido; a atual `GET /dose` também merece revisão futura por acionar um dispositivo com uma requisição GET.

## Registros de eventos

A sincronização NTP, o armazenamento em LittleFS e os logs classificados como `[INFO]`, `[AVISO]` e `[ERRO]` são **planejados, ainda não implementados**.

Eventos previstos: início do sistema, conexão e queda do Wi-Fi, liberação de ração, falha do servo ou dos sensores, temperatura acima do limite e resultado dos envios ao Telegram.

Exemplos ilustrativos (não são logs extraídos do dispositivo):

```text
[2026-11-06 10:35:00] [INFO] Solicitação de alimentação recebida.
[2026-11-06 10:35:02] [INFO] Comando do servo finalizado.
[2026-11-06 10:35:03] [INFO] Notificação de alimentação enviada ao Telegram.
[2026-11-06 10:40:00] [AVISO] Temperatura da água: 25.8 °C (limite: 25 °C).
```

Não armazenar credenciais ou tokens em logs.

## Pendências

- [ ] Testar a montagem física e calibrar a célula de carga HX711.
- [ ] Definir, **após os testes físicos**, peso-alvo, tolerância e estratégia de cálculo da porção.
- [ ] Integrar leitura da temperatura com DS18B20.
- [ ] Criar alerta Telegram para temperatura **acima de 25 °C**.
- [ ] Definir políticas anti-spam, repetição e tratamento de falhas dos alertas.
- [ ] Definir chats autorizados e comando do Telegram.
- [ ] Implementar liberação de ração via Telegram.
- [ ] Enviar confirmação com data/hora **após** liberação de ração.
- [ ] Sincronizar data/hora por NTP e persistir logs com LittleFS.
- [ ] Implementar reconexão Wi-Fi automática.
- [ ] Configurar e testar privadamente a rede da Ana.
- [ ] Testar compilação e funcionamento físico com a configuração atual do AP.

Consulte [mudanças planejadas e decisões da equipe](docs/mudancas-planejadas.md) para o escopo deste PR e os critérios de aceite.

## Estrutura do repositório

```text
Doggo/
├── README.md
├── docs/
│   ├── guia-de-conexoes.md
│   ├── interface-web.png
│   └── mudancas-planejadas.md
└── firmware/
    └── doggo/
        └── doggo.ino
```

## Como executar

1. Instale a [Arduino IDE](https://www.arduino.cc/en/software) e o pacote de placas ESP32 da Espressif.
2. Instale a biblioteca `ESP32Servo`.
3. Abra `firmware/doggo/doggo.ino` e escolha o modo de rede por `USAR_AP`:
   - `false` (atual): conecta à rede cliente definida no firmware;
   - `true`: cria o ponto de acesso **Doggo**, senha **doggo123**.
4. Faça as ligações seguindo o [guia de conexões](docs/guia-de-conexoes.md), selecione placa e porta e envie o firmware.
5. Abra o Monitor Serial a **115200 baud** e verifique o endereço exibido após `Acesse: http://`.
6. Abra esse endereço em um navegador conectado à rede correspondente.

O firmware existente oferece apenas o controle web temporizado do servo. Balança, temperatura, mensagens Telegram, alertas e cálculo da porção não passam a funcionar por causa desta atualização documental.
