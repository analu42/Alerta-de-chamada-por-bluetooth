# Avisador de chamadas do WhatsApp com Arduino
(Feito com Claude)

Quando alguém liga pelo WhatsApp, o nome de quem está ligando aparece num display LCD 16x2 e um buzzer toca. O projeto usa um **Arduino UNO**, um módulo Bluetooth **HC-05** e um **app Android** feito no **MIT App Inventor**.

## Como funciona

```
Celular Android                                      Arduino UNO
┌─────────────────────────────┐   Bluetooth (HC-05)  ┌──────────────────────────┐
│ App (App Inventor)          │ ───────────────────▶ │ lê a linha "CALL:Nome"   │
│ lê a notificação de chamada │    "CALL:Nome\n"     │ mostra no LCD 16x2       │
│ do WhatsApp                 │                      │ e toca o buzzer          │
└─────────────────────────────┘                      └──────────────────────────┘
```

1. O WhatsApp mostra uma notificação de chamada (título = nome do contato, texto = tipo de chamada).
2. O app, usando uma extensão de leitura de notificações, detecta que é uma chamada **recebida**.
3. O app conecta ao HC-05 e envia `CALL:` + nome + quebra de linha.
4. O Arduino lê a linha, remove os acentos, escreve no LCD e toca o buzzer por 20 segundos.

> **Não existe API do WhatsApp para isso.** O projeto lê a **notificação** que o Android mostra, e não a chamada em si.

## Componentes

| Item | Observação |
|---|---|
| Arduino UNO + cabo USB | |
| Módulo Bluetooth HC-05 (placa-base com pinos EN, +5V, GND, TX, RX, STATE) | O HC-05 funciona só com **Android** (o iOS não aceita esse tipo de Bluetooth) |
| LCD 16x2 de 16 pinos (sem placa I2C) | Pinos: GND, VDD, VO, RS, RW, E, D0–D7, BLA, BLK |
| Buzzer | |
| Resistores: 1 kΩ, 2 kΩ (divisor de tensão) e 220 Ω (luz de fundo do LCD) | |
| Protoboard e jumpers | |
| Celular Android | Com Bluetooth |

## 1. Montagem do circuito

Monte com o Arduino **desligado**. Use os trilhos da protoboard para distribuir 5 V e GND.

### HC-05

| HC-05 | Arduino |
|---|---|
| +5V | 5 V |
| GND | GND |
| TX | pino 10 |
| RX | pino 11, **passando por um divisor de tensão** |
| EN | sem ligar |
| STATE | sem ligar |

Divisor de tensão para o RX (o RX do HC-05 trabalha com 3,3 V): `pino 11 → resistor de 1 kΩ → RX do HC-05`, e do ponto entre o resistor e o RX, um resistor de 2 kΩ para o GND.

### Buzzer

| Buzzer | Arduino |
|---|---|
| positivo (+) | pino 8 |
| negativo (−) | GND |

### LCD 16x2

| LCD | Arduino / ligação |
|---|---|
| GND (VSS) | GND |
| VDD | 5 V |
| VO (contraste) | GND |
| RS | pino 7 |
| RW | GND |
| E | pino 6 |
| D0 a D3 | sem ligar |
| D4 | pino 5 |
| D5 | pino 4 |
| D6 | pino 3 |
| D7 | pino 2 |
| BLA (luz de fundo +) | 5 V, com resistor de 220 Ω |
| BLK (luz de fundo −) | GND |

**Contraste:** ligar o VO direto no GND dá contraste máximo e funcionou neste projeto. Se o texto ficar escondido atrás de quadrados escuros, o ideal é um **potenciômetro de 10 kΩ** (pontas em 5 V e GND, pino do meio no VO). Um resistor sozinho entre VO e GND **não** resolve, porque o VO quase não consome corrente. Com o VO solto, o display pode apagar por completo.

## 2. Código do Arduino

O código está em [`aviso_chamada/aviso_chamada.ino`](aviso_chamada/aviso_chamada.ino).

1. Abra o arquivo na **Arduino IDE** (as bibliotecas `SoftwareSerial` e `LiquidCrystal` já vêm instaladas).
2. Escolha a placa **Arduino Uno** e a porta, e clique em Enviar.
3. O LCD deve mostrar `Aguardando...`.

O Arduino espera linhas no formato `CALL:Nome` terminadas em `\n`. Letras acentuadas em UTF-8 são convertidas para a letra simples (o LCD não mostra acentos). O alarme dura 20 s (valor `20000` no código) e depois a tela volta ao estado de espera.

## 3. Testar o Arduino sem o app

1. Nas configurações de Bluetooth do celular, **pareie o HC-05** (senha `1234` ou `0000`).
2. Instale o app **Serial Bluetooth Terminal**, conecte ao HC-05 e envie `CALL:Maria` com quebra de linha no final.
3. O LCD deve mostrar `Chamada WhatsApp` / `Maria`, e o buzzer deve tocar uma vez por segundo.
4. Feche o app de terminal depois do teste: **o HC-05 aceita só uma conexão por vez**.

**LED do HC-05:**
- pisca rápido (cerca de 2 vezes por segundo): ligado, esperando conexão
- dois piscos e uma pausa: conectado ao celular
- um pisco a cada 2 segundos: modo de comandos AT (não é o desejado; desligue e ligue de novo sem apertar o botão do módulo)

## 4. App Android (MIT App Inventor)

Crie um projeto em <https://ai2.appinventor.mit.edu>.

### 4.1 Extensões

| Extensão | Para que serve | Onde achar |
|---|---|---|
| **Notification Listener** (Taifun) | Lê as notificações do aparelho | `puravidaapps.com/notificationlistener.php` |
| **Itoo** | Faz o app responder a notificações em segundo plano | Post "Open Source • Background Tasks: Itoo" no fórum do App Inventor, ou o gerador **Itoo Manifest** (`theitoo.github.io`) |

**Itoo gerado no Itoo Manifest:**
- Tipo de serviço em primeiro plano: **somente Connected Device** (não marque Phone Call nem Remote Messaging).
- Permissões: deixe **Bluetooth Connect** marcada; as outras (Bluetooth Advertise e Scan) desmarcadas.
- Deixe **Special Use** em branco.

**Importar:** na tela **Designer**, Palette → seção **Extension** → **Import extension** → escolha o arquivo `.aix`. Arraste cada extensão para o celular desenhado (elas ficam em "Non-visible components").

### 4.2 Componentes (Designer)

| Componente | Função |
|---|---|
| `ListPicker1` | Escolher o HC-05 pareado |
| `Label1` | Mostrar mensagens de status |
| `Button1` | Botão "Ver último" (depuração) |
| `BluetoothClient1` | Comunicação com o HC-05 |
| `TaifunNotificationListener1` | Leitura das notificações |
| `Itoo1` | Execução em segundo plano |

Em `Itoo1`, marque a propriedade **ConnectedDeviceUsage**. Em `TaifunNotificationListener1`, o `PackageNamesFilter` pode ficar vazio, ou conter `com.whatsapp` para ler só o WhatsApp.

### 4.3 Blocos

**Iniciar o serviço de notificações**
```
when Screen1.Initialize
do  call TaifunNotificationListener1.StartService
```

**Listar os dispositivos Bluetooth pareados**
```
when ListPicker1.BeforePicking
do  set ListPicker1.Elements to BluetoothClient1.AddressesAndNames
```

**Guardar o endereço do HC-05** (o app aberto **não conecta**: quem conecta é o procedimento em segundo plano)
```
when ListPicker1.AfterPicking
do  call Itoo1.StoreProperty  name: "hc-05"  value: ListPicker1.Selection
    set Label1.Text to "Endereço salvo"
```

**Procedimento de inicialização, vazio** (o nome precisa ser exatamente este)
```
to OnCreate
do  (vazio)
```

**Procedimento chamado pelo Itoo a cada notificação** (nome e parâmetros exatos: `OnNotificationReceived (packageName, title, text)`)
```
to OnNotificationReceived (packageName, title, text)
do  call Itoo1.StoreProperty  name: "ultimo"
        value: join( packageName , " | " , title , " | " , text )
    if   contains(text, "Appel")  and  contains(text, "entrant")
    then if   not BluetoothClient1.IsConnected
         then evaluate but ignore result:
                  call BluetoothClient1.Connect
                       address: call Itoo1.FetchProperty  name: "hc-05"  valueIfTagNotThere: ""
         if   BluetoothClient1.IsConnected
         then call BluetoothClient1.SendText
                  text: join( "CALL:" , title , "\n" )
```

**Botão de depuração: mostra o texto da última notificação lida**
```
when Button1.Click
do  set Label1.Text to
        call Itoo1.FetchProperty  name: "ultimo"  valueIfTagNotThere: "nada ainda"
```

> O nome usado em `StoreProperty` e em `FetchProperty` (`hc-05`) precisa ser **idêntico**.
>
> O bloco `Connect` devolve um valor, por isso vai dentro de `evaluate but ignore result` quando é usado como ação.
>
> Não use `Label` nem variáveis globais **dentro** de `OnNotificationReceived`: esse procedimento roda em segundo plano.

### 4.4 Texto que identifica uma chamada

A notificação de chamada do WhatsApp tem o **nome do contato no título** e, no texto, uma frase que **depende do idioma do celular**. Neste projeto, com o celular em francês, o texto era `Appel vocal entrant`, e a chamada perdida era `Appel vocal manqué`. Por isso a condição procura `Appel` **e** `entrant` (e assim ignora chamadas perdidas e mensagens comuns).

**Adapte as palavras ao idioma do seu celular.** Para descobrir o texto exato, receba uma chamada de teste e toque em "Ver último" no app.

### 4.5 Gerar e instalar o app

1. No App Inventor: **Build → Android App (.apk)**. O Companion não serve, porque o app precisa rodar de verdade em segundo plano.
2. Instale o APK no celular. Dica: baixe pelo computador e passe o arquivo para o celular.
3. **Google Play Protect:** o Android pode bloquear a instalação ("app bloqueado para proteger seu aparelho"), porque o app usa acesso a notificações e foi instalado fora da Play Store. Solução: Play Store → foto do perfil → **Play Protect** → configurações → desligue a verificação de apps, instale o APK e religue depois. Não dá para desligar durante uma ligação.
4. Abra o app e ative o **acesso a notificações** para ele (Configurações → procure "acesso a notificações"). Se a chave estiver bloqueada, vá em Configurações → Aplicativos → seu app → menu de três pontinhos → **Permitir configurações restritas**.
5. Depois de reinstalar o app, **desligue e ligue de novo** a chave do acesso a notificações.
6. Desative a **otimização de bateria** do app (Configurações → Aplicativos → seu app → Bateria → Sem restrições).
7. Aceite a permissão de dispositivos próximos (Bluetooth) quando o Android pedir.

## 5. Como usar

1. Ligue o Arduino (cabo USB no computador, num carregador ou num power bank).
2. Abra o app e escolha o **HC-05** na lista. O Label mostra "Endereço salvo". Não conecte o HC-05 por nenhum outro app.
3. Pode minimizar ou fechar o app.
4. Quando alguém ligar pelo WhatsApp, o LCD mostra o nome e o buzzer toca por 20 s.
5. Se quiser ver o que a última notificação continha, abra o app e toque em **Ver último**.

## 6. Problemas encontrados e soluções

| Sintoma | Causa e solução |
|---|---|
| LCD mostra só quadrados brancos | Normal antes de o código rodar. Depois do upload deve aparecer o texto |
| Texto do LCD escondido atrás dos quadrados | Contraste alto demais: use um potenciômetro de 10 kΩ no VO |
| LCD em branco, sem quadrados | VO solto: ligue no GND (ou no potenciômetro) |
| Buzzer toca sozinho ao ligar o Arduino | O programa de outro projeto ainda está gravado na placa, ou o positivo do buzzer está no 5 V em vez do pino 8 |
| Nomes aparecem com `?` no LCD | O texto não chegou em UTF-8, ou tem emoji/letras fora do português |
| Label fica em "Falha" ao conectar | O HC-05 aceita uma conexão por vez: feche o app de terminal. Confira se ele está pareado |
| Evento `NotificationReceived` não dispara com o Itoo no projeto | Use os procedimentos de segundo plano `OnCreate` e `OnNotificationReceived`, como descrito acima |
| Play Protect bloqueia a instalação | Veja o passo 4.5 |
| Botão de teste parou de enviar | É esperado: o app aberto não conecta mais, só guarda o endereço |

## 7. Limitações e ideias

- Só funciona com **Android** e Bluetooth clássico (HC-05). Para iPhone seria preciso outro caminho (por exemplo, um ESP32 com BLE).
- O texto que identifica a chamada depende do **idioma** do celular.
- O app lê notificações do aparelho. Use o `PackageNamesFilter` com `com.whatsapp` para ler só as do WhatsApp e mantenha o app apenas para uso pessoal.
- Ideias: parar o buzzer quando a chamada for atendida, avisar também chamadas perdidas ou mensagens, usar um LCD I2C (menos fios).

## Créditos

- Extensão **Notification Listener**: Taifun (Pura Vida Apps).
- Extensão **Itoo**: Kumaraswamy B G e colaboradores (fórum do MIT App Inventor).
- Plataforma: [MIT App Inventor](https://appinventor.mit.edu).
