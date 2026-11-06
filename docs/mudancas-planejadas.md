# Doggo — Correções, melhorias e decisões (06/11/2026)

## Escopo aprovado deste PR

Este PR é **documental**, com uma exceção mínima no firmware: retirar credenciais Wi-Fi reais do código versionado, passar a carregá-las de `secrets.h` local e corrigir o nome do AP de `Doogo` para **`Doggo`**. A lógica Wi-Fi, servidor, rotas, interface e acionamento do servo permanecem inalterados.

- Adicionar `#include "secrets.h"` no firmware, sem alterar `WiFi.begin`, `WiFi.softAP` ou `USAR_AP`.
- Manter as declarações de `WIFI_SSID`, `WIFI_SENHA` e `AP_SENHA` somente em `firmware/doggo/secrets.h` **local**, ignorado pelo Git; distribuir apenas `secrets.example.h` com valores fictícios.
- O AP passa a se chamar **Doggo**, e a senha configurada localmente precisa ter **pelo menos 8 caracteres**.
- Não inserir credenciais da rede da Ana nem tokens do Telegram no repositório.
- Remover acionamento de bomba/fonte/relé do escopo e dos planos de implementação. O sistema apenas monitorará a temperatura da água.

## Melhorias futuras (não implementadas neste PR)

| Item | Pendência / decisão futura |
| :--- | :--- |
| Balança HX711 | Testar a montagem física, calibrar e então definir peso-alvo, tolerância e cálculo da porção. |
| DS18B20 | Integrar leitura da temperatura da água. |
| Telegram — acionamento | Permitir liberação de ração pelo bot, após definir comando e chats autorizados. |
| Telegram — confirmação | Enviar mensagem com data/hora **após** a liberação; distinguir acionamento do servo de comprovação do peso entregue. |
| Telegram — temperatura | Alertar se a temperatura medida for **maior que 25 °C**. |
| Alertas e falhas | Definir regra anti-spam, repetição, normalização, sensor indisponível, queda de Wi-Fi e falha de envio. |
| Rede | Configurar privadamente a rede da Ana durante testes físicos. |
| Logs e NTP | Implementar sincronização de data/hora e registro persistente em LittleFS. |
| Conexão | Implementar reconexão automática Wi-Fi. |

## Critérios de aceite para este PR

1. O firmware não contém SSID/senhas reais versionados; `secrets.h` está no `.gitignore`.
2. Há `secrets.example.h` com valores de exemplo e instruções de configuração.
3. `AP_SSID` é `Doggo`.
4. A lógica de conexão, rotas, servo, HTML/CSS/JS e demais comandos do firmware não foi modificada.
5. O README separa claramente o que está implementado do que segue pendente.
6. Não há implementação de bomba, Telegram, alerta térmico ou cálculo da porção neste PR.

## Limites e segurança

Para compilar, cada integrante deve copiar `secrets.example.h` para `secrets.h` e preencher os próprios dados locais. Sem esse arquivo o projeto não compila. A inclusão no `.gitignore` não apaga credenciais de commits anteriores: **é necessário trocar as senhas anteriormente expostas**. Compilação e testes físicos ainda não foram realizados.
