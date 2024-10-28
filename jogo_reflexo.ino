#include <Wire.h>
#include <LiquidCrystal_I2C.h>

const int numLeds = 9;
int leds[numLeds] = {2, 3, 4, 5, 6, 7, 8, 9, 10}; // Pinos dos LEDs
const int botao = 11; // Pino do botão
int posicaoAtual = 0;
bool estadoBotaoAnterior = LOW; // Armazena o estado anterior do botão
int pontuacao = 0; // Pontuação total do jogador
int pontuacaoMaxima = 0; // Pontuação máxima registrada
int tentativasRestantes = 10; // Número de fases ajustado para 10
int delayAtual = 200; // Delay inicial ajustado para uma velocidade mais rápida

// Configuração do display LCD I2C
LiquidCrystal_I2C lcd(0x27, 16, 2); // Endereço I2C padrão do LCD é 0x27, com dimensões 16x2

void setup() {
    // Configuração dos LEDs como saídas
    for (int i = 0; i < numLeds; i++) {
        pinMode(leds[i], OUTPUT);
        digitalWrite(leds[i], LOW); // Garante que todos os LEDs comecem apagados
    }
    
    // Configuração do botão como entrada
    pinMode(botao, INPUT);
    Serial.begin(9600); // Inicializa o Serial para exibir mensagens
    
    // Inicializa o display LCD
    lcd.init();
    lcd.clear();
    lcd.backlight(); // Liga a luz de fundo do LCD
    atualizarDisplay(); // Exibe as pontuações iniciais
}

void loop() {
    // Espera o botão ser pressionado para começar o jogo
    if (tentativasRestantes == 0) {
        // Se todas as tentativas acabaram, verifica se pontuacao é a nova pontuacaoMaxima
        if (pontuacao > pontuacaoMaxima) {
            pontuacaoMaxima = pontuacao;
        }

        // Apaga todos os LEDs e reseta a posição atual
        for (int i = 0; i < numLeds; i++) {
            digitalWrite(leds[i], LOW);
        }
        posicaoAtual = 0; // Reseta a posição atual para o início

        // Atualiza o display com a maior pontuação
        atualizarDisplay();

        // Espera o botão ser pressionado para reiniciar o jogo
        if (digitalRead(botao) == HIGH && estadoBotaoAnterior == LOW) {
            tentativasRestantes = 10; // Reinicia com 10 tentativas para o novo jogo
            pontuacao = 0;
            delayAtual = 200; // Reinicia o delay para 200 ms no novo jogo
            Serial.println("Novo jogo iniciado!");
            delay(200); // debounce
            atualizarDisplay(); // Atualiza o display com a nova pontuação
        }

        estadoBotaoAnterior = digitalRead(botao);
        return; // Sai do loop até que o botão seja pressionado
    }

    // Lê o estado atual do botão
    bool estadoBotaoAtual = digitalRead(botao);

    // Detecta se o botão foi pressionado (transição de LOW para HIGH)
    if (estadoBotaoAtual == HIGH && estadoBotaoAnterior == LOW) {
        // Calcula a pontuação com base na posição do LED antes de resetar a posição
        int centro = numLeds / 2; // LED central
        int distanciaDoCentro = abs(posicaoAtual - centro); // Distância do LED ao centro
        int pontuacaoGanha = numLeds - distanciaDoCentro; // Maior pontuação no centro, menor nas extremidades

        // Apaga todos os LEDs e reseta a posição atual para o início
        for (int i = 0; i < numLeds; i++) {
            digitalWrite(leds[i], LOW);
        }
        posicaoAtual = 0;

        // Adiciona a pontuação ganha à pontuação total
        int pontuacaoAtual = pontuacao;
        pontuacao += pontuacaoGanha;
        tentativasRestantes--; // Diminui uma tentativa

        // Atualiza o display com a pontuação atual e a pontuação máxima
        atualizarDisplay();

        // Imprime a mensagem formatada no Serial Monitor
        Serial.println("====================");
        Serial.println("Botao pressionado!");
        Serial.print("Posicao: ");
        Serial.println(posicaoAtual + 1); // Ajuste para imprimir 1-9 em vez de 0-8
        Serial.print("Pontuacao atual: ");
        Serial.print(pontuacaoAtual);
        Serial.print(" + ");
        Serial.print(pontuacaoGanha);
        Serial.print(" = ");
        Serial.println(pontuacao);
        Serial.print("Pontuacao maxima: ");
        Serial.println(pontuacaoMaxima);
        Serial.print("Tentativas restantes: ");
        Serial.println(tentativasRestantes);
        Serial.println("====================");

        delay(200); // debounce para evitar leituras indesejadas logo após a transição

        // Reduz o delay a cada tentativa para aumentar a velocidade
        if (tentativasRestantes > 0) {
            delayAtual -= 20; // Diminui o delay em 20 ms para aumentar a velocidade de forma mais gradual
        }
    }

    // Atualiza o estado anterior do botão para o próximo loop
    estadoBotaoAnterior = estadoBotaoAtual;

    // Se atingimos o final da sequência, apagamos todos os LEDs e reiniciamos a posição
    if (posicaoAtual == numLeds) {
        for (int i = 0; i < numLeds; i++) {
            digitalWrite(leds[i], LOW);
        }
        posicaoAtual = 0;
    }

    // Acende o LED atual
    digitalWrite(leds[posicaoAtual], HIGH);

    // Movimento dos LEDs
    delay(delayAtual); // Usamos o delay atual que diminui a cada tentativa

    // Avança para o próximo LED
    posicaoAtual++;
}

// Função para atualizar o display LCD
void atualizarDisplay() {
    lcd.clear();
    lcd.setCursor(0, 0); // Primeira linha
    lcd.print("Maior pont.: ");
    lcd.print(pontuacaoMaxima);

    lcd.setCursor(0, 1); // Segunda linha
    lcd.print("Pont. atual: ");
    lcd.print(pontuacao);
}
