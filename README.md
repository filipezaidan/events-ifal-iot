# 🏷️ Eventos IFAL IOT - TOSI

Sistema de identificação de convidados utilizando RFID com ESP8266, display OLED e integração com Google Apps Script para verificação e exibição de informações na tela.

---

## 📋 Funcionalidades

* ✅ Leitura de cartões RFID/NFC (MFRC522)
* 📡 Conexão Wi-Fi automática e reconexão em caso de queda
* 🌐 Requisições seguras HTTPS com suporte a redirecionamentos
* 🖥️ Exibição de mensagens, informações e imagens bitmap no display OLED (SSD1306)
* ❌ Feedback visual de acesso negado ou erro de rede
* ✅ Exibição do nome, horário de entrada/saída e imagem do convidado autorizado

---

## 🛠️ Requisitos de Hardware

| Componente       | Modelo             | Quantidade |
| ---------------- | ------------------ | ---------- |
| Microcontrolador | ESP8266 NodeMCU    | 1          |
| Módulo RFID      | MFRC522            | 1          |
| Display OLED     | SSD1306 128x64 I2C | 1          |
| Jumpers          | -                  | Vários     |
| Protoboard       | -                  | 1          |

---

## 🔌 Esquema de Ligações

### 🔷 RFID MFRC522 <-> ESP8266 (NodeMCU)

| MFRC522 | ESP8266       |
| ------- | ------------- |
| SDA     | D8 (GPIO15)   |
| SCK     | D5 (GPIO14)   |
| MOSI    | D7 (GPIO13)   |
| MISO    | D6 (GPIO12)   |
| IRQ     | - (não usado) |
| GND     | GND           |
| RST     | D4 (GPIO2)    |
| 3.3V    | 3.3V          |

> ⚠️ **Importante:** o MFRC522 **não deve** ser alimentado com 5V. Use sempre 3.3V para evitar danos.

---

### 🔷 Display OLED SSD1306 <-> ESP8266 (I2C)

| OLED | ESP8266    |
| ---- | ---------- |
| VCC  | 3.3V       |
| GND  | GND        |
| SCL  | D1 (GPIO5) |
| SDA  | D2 (GPIO4) |

---

## 📶 Configuração Wi-Fi

No código, altere as credenciais para sua rede:

```cpp
const char *ssid = "NOME_DA_REDE";
const char *password = "SENHA_DA_REDE";
```

---

## 🔗 Integração com Google Apps Script

* O código realiza uma requisição HTTPS a uma URL do Google Apps Script contendo o parâmetro `rfid` com o UID lido.
* Exemplo de URL chamada:

```
https://script.google.com/macros/s/SEU_SCRIPT_ID/exec?rfid=XXXXXXXX
```

* O script deve retornar um JSON contendo os campos:

  * `Nome do convidado`
  * `Timestamp`
  * `Entrada`
  * `Saida`
  * `ImagemBitmap` (formato `{0x00, 0xFF, ...}` de 1024 bytes para 128x64)

---

## 🧪 Exibição OLED

A tela mostrará:

* Mensagem de inicialização
* Status de conexão Wi-Fi
* Aguardando cartão
* UID do cartão
* Resultado da verificação (acesso liberado ou negado)
* Dados do convidado (nome, entrada, saída, imagem opcional)

---

## 🐞 Debug

* Todas as etapas do processo são exibidas via `Serial Monitor` a 115200 baud.
* Inclui mensagens de conexão, leitura de cartão, resposta da API e erros de JSON ou rede.

---

## 📜 Licença

Este projeto é de uso acadêmico e experimental no contexto de eventos do IFAL - Instituto Federal de Alagoas.

---

Se quiser, posso gerar um diagrama visual das ligações também. Deseja isso?
