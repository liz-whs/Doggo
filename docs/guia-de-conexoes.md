# Guia de conexões

Ligações do hardware do Doggo ao ESP32: alimentador (célula de carga, HX711 e servo) e sensor de temperatura da água.

> 3V3 e 5V devem permanecer separados. Use uma alimentação adequada para o servo.

## 1. Célula de carga → HX711

Soldagem dos fios da célula de carga no módulo.

| Fio da célula de carga | Terminal do HX711 |
| :--- | :--- |
| Vermelho | E+ |
| Preto | E− |
| Branco | A− |
| Verde | A+ |

## 2. HX711 → ESP32

Alimentação e comunicação.

| HX711 (cor do fio) | ESP32 |
| :--- | :--- |
| VCC (vermelho) | 3V3 |
| GND (marrom, conforme o guia original) | GND |
| DT (branco) | GPIO 16 |
| SCK (roxo) | GPIO 17 |

## 3. Servomotor SG90 → ESP32

Pinos confirmados na montagem.

| Servo (cor do fio) | ESP32 |
| :--- | :--- |
| Sinal (laranja / amarelo) | GPIO 19 |
| Positivo (vermelho / azul) | 5V |
| Negativo (marrom / verde) | GND |

## 4. DS18B20 → ESP32

Adicionado após o teste conjunto do Doggo.

| DS18B20 (cor do fio) | ESP32 |
| :--- | :--- |
| Vermelho (alimentação) | 3V3 |
| Preto (terra) | GND |
| Amarelo (dados) | GPIO 25 |
| Resistor de 4,7 kΩ | Entre 3V3 e DATA |

## Resumo dos pinos do ESP32

| Pino | Uso |
| :--- | :--- |
| GPIO 16 | HX711 DT |
| GPIO 17 | HX711 SCK |
| GPIO 19 | Sinal do servo SG90 |
| GPIO 25 | Dados do DS18B20 |
| 3V3 | HX711 e DS18B20 |
| 5V | Servo SG90 |
| GND | Terra comum |

Na versão atual do firmware, apenas o servo (GPIO 19) está em uso. Os pinos do HX711 e do DS18B20 são os da montagem e serão usados na integração dos sensores.
