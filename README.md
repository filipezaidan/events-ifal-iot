# 🏷️ Eventos IFAL IOT - TOSI


## 📋 Características

- ✅ Leitura de cartões RFID/NFC
- 🖥️ Display OLED para feedback visual
- 📦 Código modular organizado em arquivos separados
- 🔧 Fácil de configurar e usar
- 📱 Interface serial para debug

## 🛠️ Hardware Necessário

| Componente | Modelo | Quantidade |
|------------|--------|------------|
| Microcontrolador | ESP8266 NodeMCU | 1 |
| Módulo RFID | MFRC522 | 1 |
| Display OLED | SSD1306 128x64 | 1 |
| Jumpers | - | Vários |
| Protoboard | - | 1 |

## 🔌 Esquema de Ligação

### MFRC522 → ESP8266 NodeMCU
| MFRC522 | NodeMCU Pin | GPIO |
|---------|-------------|------|
| SDA/SS  | D8          | GPIO15 |
| SCK     | D5          | GPIO14 |
| MOSI    | D7          | GPIO13 |
| MISO    | D6          | GPIO12 |
| IRQ     | -           | -      |
| GND     | GND         | GND    |
| RST     | D3          | GPIO0  |
| 3.3V    | 3V3         | 3.3V   |

### Display OLED SSD1306 → ESP8266 NodeMCU
| OLED | NodeMCU Pin | GPIO |
|------|-------------|------|
| VCC  | 3V3         | 3.3V |
| GND  | GND         | GND  |
| SDA  | D2          | GPIO4 |
| SCL  | D1          | GPIO5 |

## 📦 Instalação

### 1. Clone o repositório
```bash
git clone https://github.com/filipezaidan/events-ifal-iot.git
cd RFID-Arduino-System
```

### 2. Instale as bibliotecas necessárias
Abra o Arduino IDE e instale as seguintes bibliotecas através do Library Manager:

**Bibliotecas obrigatórias:**
- MFRC522v2
- Adafruit SSD1306  
- Adafruit GFX Library

**Para ESP8266 (caso não tenha):**
- ESP8266WiFi (geralmente já incluída)
- Wire (geralmente já incluída)

**Configuração do Arduino IDE para ESP8266:**
1. Arquivo → Preferências → URLs adicionais do gerenciador de placas
2. Adicione: `http://arduino.esp8266.com/stable/package_esp8266com_index.json`
3. Ferramentas → Placa → Gerenciador de placas
4. Instale "esp8266 by ESP8266 Community"
5. Selecione: Ferramentas → Placa → ESP8266 Boards → NodeMCU 1.0

### 3. Carregue o código
1. Abra o arquivo `src/main.ino` no Arduino IDE
2. Certifique-se de que todos os arquivos (.h e .cpp) estão na mesma pasta
3. Selecione sua placa e porta serial
4. Faça o upload do código

## 🚀 Como Usar

1. **Conecte o hardware** seguindo o esquema de ligação do NodeMCU
2. **Carregue o código** no Arduino
3. **Abra o Serial Monitor** (115200 baud)
4. **Aproxime um cartão RFID** do leitor
5. **Veja o UID** no display e no Serial Monitor

## 📁 Estrutura do Código

```
src/
├── main.ino              # Arquivo principal
├── rfid_handler.h/.cpp   # Funções do RFID
├── display_handler.h/.cpp # Funções do display
└── icons.h               # Ícones e bitmaps
```

### Principais Funções

#### RFID Handler
- `initRFID()`: Inicializa o módulo RFID
- `readRFIDCard()`: Lê e retorna o UID do cartão

#### Display Handler
- `initDisplay()`: Inicializa o display OLED
- `displayUID(String uid)`: Mostra o UID na tela
- `displayMessage(String msg)`: Mostra mensagem personalizada

## 🔧 Configuração Avançada

### Alterar Pin do RFID
Edite o arquivo `rfid_handler.cpp`:
```cpp
// Altere o pin 15 (D8) para o desejado
MFRC522DriverPinSimple ss_pin(15); // GPIO15 = D8 no NodeMCU
```

### Alterar Endereço I2C do Display
Edite o arquivo `display_handler.cpp`:
```cpp
// Altere 0x3C para 0x3D se necessário
display.begin(SSD1306_SWITCHCAPVCC, 0x3C)
```

## 🤝 Contribuições

Contribuições são bem-vindas! Por favor:

1. Faça um Fork do projeto
2. Crie uma branch para sua feature (`git checkout -b feature/AmazingFeature`)
3. Commit suas mudanças (`git commit -m 'Add some AmazingFeature'`)
4. Push para a branch (`git push origin feature/AmazingFeature`)
5. Abra um Pull Request

## 📝 Licença

Este projeto está licenciado sob a Licença MIT - veja o arquivo [LICENSE](LICENSE) para detalhes.

## 🆘 Suporte

Se você encontrar algum problema:

1. Verifique as [Issues](https://github.com/filipezaidan/events-ifal-iot/issues) existentes
2. Crie uma nova Issue se necessário
3. Forneça detalhes sobre o erro e sua configuração

## 📸 Screenshots

![Sistema em funcionamento](docs/images/project_photo.jpg)
![Esquema de ligação](docs/images/wiring_diagram.png)

## 🔄 Changelog

Veja [CHANGELOG.md](CHANGELOG.md) para um histórico completo das mudanças.

---

⭐ Se este projeto foi útil para você, considere dar uma estrela!

📧 Contato: [seu-email@exemplo.com]
