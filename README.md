### Protocolos e Pilha TCP/IP Utilizados:
* **Camada de Aplicação:** HTTP / NTP (Network Time Protocol para carimbo de hora).
* **Camada de Transporte:** TCP (porta 80 para escuta de requisições).
* **Camada de Rede:** IP (Endereçamento IPv4 estático/dinâmico).
* **Camada de Enlace:** Wi-Fi IEEE 802.11 b/g/n (Armazenamento de Endereço MAC).

---

## 🛠️ Especificação de Hardware

### Sensores (Entradas):
1. **Célula de Carga 1kg + Módulo HX711:** Balança digital de precisão para medição contínua da massa de ração disponível na tigela.
2. **Sensor DS18B20 (À prova d'água):** Leitura da temperatura da água no reservatório da fonte.

### Atuadores (Saídas):
1. **Servomotor SG90 (9g):** Atuador mecânico para abertura/fechamento controlado do dispenser de ração (liberação por peso alvo).
2. **Módulo Relé 5V (1 Canal):** Atuador chaveador para acionamento/interrupção da fonte de água elétrica.

---

## 💻 Endpoints da API (Rotas do Servidor)

O ESP32 responde aos seguintes rotas HTTP:

| Método | Endpoint | Descrição | Exemplo de Retorno / Parâmetro |
| :--- | :--- | :--- | :--- |
| `GET` | `/` | Retorna a interface web principal de monitoramento. | HTML/CSS/JS embarcado |
| `GET` | `/status` | Exibe o IP, MAC Address, estado da rede e dados dos sensores. | JSON: `{ "ip": "192.168.1.100", "peso_g": 120.5, "temp_agua": 22.4 }` |
| `POST` | `/alimentar` | Inicia a dosagem automática via servomotor até atingir o peso informado. | Query: `?gramas=150` |
| `POST` | `/fonte` | Lida/desliga o relé da fonte de água. | Query: `?status=1` ou `?status=0` |
| `GET` | `/logs` | Retorna o histórico de logs persistido na memória Flash (`LittleFS`). | Arquivo de texto puro / JSON |

---

## 📜 Log Persistente e Gerenciamento de Erros

Em conformidade com os requisitos da disciplina:
* O ESP32 realiza **reconexão automática** em caso de queda na rede Wi-Fi.
* Os eventos de sistema são carimbados com **data e hora reais** sincronizadas via servidor NTP público (`pool.ntp.org`).
* Os logs são categorizados em **`[INFO]`**, **`[AVISO]`** e **`[ERRO]`** e gravados na memória Flash interna utilizando a biblioteca **LittleFS** para garantir persistência após reinicializações.

### Exemplo de Log Gravado:
```text
[2026-09-10 10:15:02] [INFO] Sistema inicializado. IP: 192.168.1.105 | MAC: AA:BB:CC:11:22:33
[2026-09-10 10:15:03] [INFO] Sincronização NTP realizada com sucesso.
[2026-09-10 10:20:15] [INFO] Dosagem solicitada: 100g. Servo acionado.
[2026-09-10 10:20:22] [INFO] Dosagem concluída. Peso medido pelo HX711: 101.2g. Servo fechado.
[2026-09-10 11:00:00] [AVISO] Temperatura da água elevada: 28.5°C.
[2026-09-10 11:45:12] [ERRO] Conexão Wi-Fi perdida. Iniciando rotina de reconexão...
[2026-09-10 11:45:18] [INFO] Wi-Fi reestabelecido com sucesso.
