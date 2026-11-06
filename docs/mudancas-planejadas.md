# Doggo — Correções, melhorias e decisões (06/11/2026)

## Escopo aprovado deste PR

O PR é **documental**, com apenas duas alterações de valor no firmware: nome e senha do ponto de acesso (AP) criado pelo ESP32.

- Alterar `AP_SSID` de `Doogo` para `Doggo`.
- Alterar `AP_SENHA` de `doogo` para `doggo123` (8 caracteres).
- **Não utilizar `secrets.h`**, exemplos de secrets ou `.gitignore` relacionado; manter as configurações diretamente em `doggo.ino`.
- **Não alterar** `WIFI_SSID`, `WIFI_SENHA`, `USAR_AP`, `WiFi.begin`, `WiFi.softAP`, rotas, interface, servo ou qualquer outra lógica.
- `USAR_AP = false` permanece como valor original; a rede `Doggo` só será criada ao selecionar `USAR_AP = true`.
- A rede cliente da Ana continuará pendente de configuração e teste físico.
- Retirar acionamento da bomba/fonte/relé do **escopo documental**, preservando o firmware existente. A água será apenas monitorada por sensor.

## Funcionalidades futuras (não implementadas neste PR)

| Item | Pendência |
| :--- | :--- |
| HX711 | Testar montagem física e calibrar a balança. |
| Dosagem | Definir peso-alvo, tolerância e algoritmo de porção depois dos testes físicos. |
| DS18B20 | Implementar leitura da temperatura da água. |
| Telegram — acionamento | Definir chats autorizados e comando, então implementar liberação da ração. |
| Telegram — confirmação | Enviar data e hora após o acionamento; não afirmar peso sem medição. |
| Telegram — temperatura | Notificar se leitura válida for maior que 25 °C. |
| Alertas | Definir frequência, antispam, normalização e tratamento de falhas do sensor, Wi-Fi e envio. |
| NTP e LittleFS | Implementar sincronização e histórico persistente. |
| Wi-Fi | Implementar reconexão automática. |

## Critérios de aceite

1. Comparado à `main`, o firmware possui **somente as duas linhas** `AP_SSID` e `AP_SENHA` alteradas.
2. O ponto de acesso se chama `Doggo` e usa senha `doggo123`.
3. Não há inclusão ou dependência de `secrets.h`.
4. A documentação diferencia as funcionalidades existentes das planejadas.
5. Nenhuma integração Telegram, cálculo por peso ou acionamento de bomba foi implementado.

## Observações

A senha `doggo123` estará visível no repositório público; não deve ser usada em outras redes ou serviços. As credenciais anteriormente presentes no histórico Git não são apagadas por esse PR, então recomenda-se trocá-las. A compilação e o teste físico não foram executados.
