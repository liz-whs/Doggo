# Doggo — Decisões da equipe

## Decisões

- O ponto de acesso (AP) criado pelo ESP32 se chama `Doggo` e usa a senha `doggo123` (8 caracteres).
- **Não utilizar `secrets.h`**, exemplos de secrets ou `.gitignore` relacionado: as configurações ficam diretamente em `doggo.ino`.
- `USAR_AP = false` é o valor padrão; a rede `Doggo` só é criada ao selecionar `USAR_AP = true`.
- **Porção de ração:** liberação por tempo, com o servo aberto por 400 ms (`TEMPO_DOSE`), estimada em cerca de 25 g (`PORCAO_G`). A quantidade final é definida pela equipe a partir da medição com o Doggo montado.
- O acionamento de bomba, fonte ou relé está **fora do escopo**. A água é apenas monitorada pelo sensor de temperatura.

## Funcionalidades concluídas

| Item | Situação |
| :--- | :--- |
| HX711 | Concluído: leitura do peso da tigela, com tara automática ao ligar. |
| DS18B20 | Concluído: leitura da temperatura da água. |
| Telegram — acionamento | Concluído: o chat autorizado (`CHAT_ID`) libera ração com `/liberar`. |
| Telegram — confirmação | Concluído: mensagem com data e hora enviada depois de cada liberação, pelo site ou pelo Telegram. |
| Telegram — temperatura | Concluído: alerta quando a leitura válida passa de 25 °C. |
| Alertas | Concluído: repetição no máximo a cada 30 minutos, aviso de volta ao normal (24,5 °C ou menos) e nenhum alerta sem leitura do sensor. |
| NTP | Concluído: data e hora sincronizadas por `pool.ntp.org` (fuso UTC-3). |

## Observações

A senha `doggo123` está visível no repositório público e não deve ser usada em outras redes ou serviços. As credenciais presentes no histórico Git devem ser trocadas.
