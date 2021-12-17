// Inclusão das bibliotecas
#include <proc/pic18f4520.h>
#include "config.h"
#include "io.h"
#include "lcd.h"
#include "rgb.h"
#include "keypad.h"
#include "serial.h"
#include "ssd.h"
#include "timer.h"
#include "atraso.h"

// Definição de alguns comandos do LCD
#define L_ON 0x0F
#define L_OFF 0x08
#define L_CLR 0x01
#define L_L1 0x80
#define L_L2 0xC0

// Função para a configuração da tela inicial da bomba de combustível
void telaInicial(void) {
    
    // O LED RGB acende vermelho para demonstrar que a bomba ainda não está configurada para operar
    rgbColor(1);

    // Mensagens na porta serial das opções de combustíves disponíveis
    serial_tx_str("\r\nCombustiveis disponiveis:\n\r");
    serial_tx_str("\r(d3) - Gasolina R$ 6,2\n\r");
    serial_tx_str("\r(d4) - Etanol R$ 4,9\n\r");
    serial_tx_str("\r(d5) - Diesel R$ 4,7\n\r");
    serial_tx_str("\rEscolha uma opcao...\n\r");
    
    // Opções no LCD
    lcdCommand(L_CLR);
    lcdCommand(L_L1);
    lcdString("(d3)Gaso (d4)Eta");
    lcdCommand(L_L2);
    lcdString("(d5)Dies");

} // end telaInicial(void)

// Função para mostrar uma mensagem de opção inválida
void opcaoInvalida(void) {
    
    // Mostra a mensagem tanto na serial quanto no LCD
    lcdCommand(L_CLR);
    lcdCommand(L_L1);
    lcdString("     Opcao");
    lcdCommand(L_L2);
    lcdString("   Invalida!");
    serial_tx_str("\r\nOpcao invalida!\n\r");

    // Atraso para o usuário ler a mensagem no LCD (+- 2 s)
    atraso_ms(2000);
    
} // end opcaoInvalida(void)

// Função que simula o funcionamento do abastecimento
void abastecimentoBomba(float totalIn, float litrosIn) { 
    char tecla;
    int tempo = 0; 
    float cont = 0;
    
    // O LED RGB acende verde para demonstrar que a bomba já pode operar
    rgbColor(2);

    // Mensagens no LCD e na serial para que o usuário aperte o botão para começar o abastecimento
    lcdCommand(L_CLR);
    lcdCommand(L_L1);
    lcdString("Aperte (d7)");
    lcdCommand(L_L2);
    lcdString("para comecar...");
    serial_tx_str("\r\n\nAperte (d7) no teclado para comecar a abastecer...\n\r");

    // Espera até que a tecla correta seja apertada (d7)
    while ((tecla != 's') && (tecla != 'S')) {
        kpDebounce();
        
        tecla = kpReadKey();
    } // end while((tecla != 's') && (tecla != 'S'))

    // O LED RGB acende azul para demonstrar que a bomba está operando
    rgbColor(4);
    
    // Mensagens no LCD e na serial de que o abastecimento está sendo realizado
    lcdCommand(L_CLR);
    lcdString("Abastecendo...");
    serial_tx_str("\r\nAbastecendo...\n\r");
    
    // Mostra quantos litros já foram abastecidos nos displays de 7 segmentos
    // Se esse valor chegar ao estipulado pelo usuário ou ao calculado, o abastecimento termina
    do {
        timerReset(5000);
        
        // Atualização dos displays
        ssdUpdate();
        
        // Delay para aumentar a contagem de tempos em tempos (+- 1.25 L/s s)
        if (tempo == 10) {
            cont += 0.1;
            tempo = 0;
        } // end if(tempo == 80)
        else {
            tempo++;
        } // end else
        
        // Distribui o valor que já foi abastecido para os displays
        ssdDigit((((int) (cont/100)) % 10), 0); // Centena
        ssdDigit((((int) (cont/10)) % 10), 1); // Dezena
        ssdDigit((((int) (cont/1)) % 10), 2); // Unidade
        ssdDigit((((int) (cont/0.1)) % 10), 3); // 1ª casa decimal
        
        // Espera atingir o tempo mínimo para a multiplexação
        timerWait();
    } while (cont <= litrosIn);
    
    // Mostra no LCD e na serial o preço total a se pagar e quantos litros foram abastecidos
    lcdCommand(L_CLR);
    lcdCommand(L_L1);
    lcdString("Preco: R$ ");
    lcdChar((((int) (totalIn/100)) % 10) + 48); // Centena
    lcdChar((((int) (totalIn/10)) % 10) + 48); // Dezena
    lcdChar((((int) (totalIn/1)) % 10) + 48); // Unidade
    lcdChar(',');
    lcdChar((((int) (totalIn/0.1)) % 10) + 48); // 1ª casa decimal
    lcdCommand(L_L2);
    lcdString("Litros: ");
    lcdChar((((int) (litrosIn/100)) % 10) + 48);
    lcdChar((((int) (litrosIn/10)) % 10) + 48);
    lcdChar((((int) (litrosIn/1)) % 10) + 48);
    lcdChar(',');
    lcdChar((((int) (litrosIn/0.1)) % 10) + 48);
    lcdString(" L");
    
    serial_tx_str("\r\nPreco a se pagar: R$ \r");
    serial_tx((((int) (totalIn/100)) % 10) + 48); // Centena
    serial_tx((((int) (totalIn/10)) % 10) + 48); // Dezena
    serial_tx((((int) (totalIn/1)) % 10) + 48); // Unidade
    serial_tx(',');
    serial_tx((((int) (totalIn/0.1)) % 10) + 48); // 1ª casa decimal
    serial_tx_str("\r\nLitros: \r");
    serial_tx((((int) (litrosIn/100)) % 10) + 48);
    serial_tx((((int) (litrosIn/10)) % 10) + 48);
    serial_tx((((int) (litrosIn/1)) % 10) + 48);
    serial_tx(',');
    serial_tx((((int) (litrosIn/0.1)) % 10) + 48);
    serial_tx_str(" L\n");
    
    // Mostra nos displays a quantidade ,em litros ,abastecida
    // Realiza também um atraso para o usuário ler a mensagem no LCD (+- 3 s)
    tempo = 0;
    while(tempo != 1500){
        timerReset(5000);
        
        // Atualização dos displays
        ssdUpdate();
        
        if(timerEnded()){
            break;
        } // end if(timerEnded())
        
        // Espera atingir o tempo mínimo para a multiplexação
        timerWait();
        
        tempo++;
    } // while(tempo != 1500)
    
    // Desliga os displays
    digitalWrite(DISP_1_PIN, LOW);
    digitalWrite(DISP_2_PIN, LOW);
    digitalWrite(DISP_3_PIN, LOW);
    digitalWrite(DISP_4_PIN, LOW);

    // Mensagem final
    lcdCommand(L_CLR);
    lcdCommand(L_L1);
    lcdString("Obrigado pela");
    lcdCommand(L_L2);
    lcdString("preferencia!");
    serial_tx_str("\r\nObrigado pela preferencia!\n\r");

    // Atraso para o usuário ler a mensagem no LCD (+- 4 s)
    atraso_ms(4000);
    
} // end abastecimentoBomba()

// Função para selecionar o combustível que será abastecido
void selecaoCombustivel(char teclaIn) {
    unsigned char buffer[5], i = 0, op;
    float litros = 0, total = 0, price = 0;

    // Dependendo da tecla apertada um tipo de combustível é selecionado
    switch (teclaIn) {
        // Gasolina
        case 'U':
            // Passa o valor do combustível selecionado ao usuário e a uma variável
            price = 6.2;
            lcdCommand(L_CLR);
            lcdCommand(L_L1);
            lcdString("Gasolina");
            lcdCommand(L_L2);
            lcdString("Preco: R$ 6,2");
            break;
        // Etanol
        case 'L':
            price = 4.9;
            lcdCommand(L_CLR);
            lcdCommand(L_L1);
            lcdString("Etanol");
            lcdCommand(L_L2);
            lcdString("Preco: R$ 4,9");
            break;
        // Diesel
        case 'D':
            price = 4.7;
            lcdCommand(L_CLR);
            lcdCommand(L_L1);
            lcdString("Diesel");
            lcdCommand(L_L2);
            lcdString("Preco: R$ 4,7");
            break;
        // Tecla fora das opções
        default:
            opcaoInvalida();
            return;
            break;
    } // end switch(teclaIn)
    
    // O usuário deve escolher se quer abastecer por litro ou por um valor específico
    serial_tx_str("\r\nAbastecer por litro ou real [(L)itro/(R)eal]: \r");

    // Looping até o usuário inserir uma opção
    do {
        op = serial_rx(2000);
    } while (op == 0xA5);

    // Escreve a opção inserida na serial
    serial_tx(op);

    // Se for por litro
    if (op == 'L') {
        serial_tx_str("\r\nLitros (000,0): \r");
    } // end if(op == 'L')
    // Se for por real
    else if (op == 'R') {
        serial_tx_str("\r\nValor (000,0): \r");
    } // end else if(op == 'R')
    // Opção fora das disponíveis
    else {
        opcaoInvalida();
        serial_tx_str("\r\nOpcao Invalida! \n\r");
        return;
    } // end else
    
    // Inserção de quantos litros ou reais pelo usuário
    do {
        // Pega o valor da porta serial
        buffer[i] = serial_rx(2000);

        // Se o dígito é de 0 a 9 ou a vírgula decimal, guarda no buffer e passa para o próximo dígito até os 5 serem enviados 
        if ((((buffer[i] - 48) >= 0) && ((buffer[i] - 48) <= 9) && (i != 3)) || ((buffer[i] == ',') && (i == 3))){
            serial_tx(buffer[i]);
            i++;
        } // end if()
    } while (i != 5);
    i = 0;
    
    // Se o usuário escolheu litro
    if (op == 'L') {
        // Armazena o valor passado na serial em uma variável float usando um typecast
        litros = (float) (0.1 * ((buffer[4] % 48)));
        litros += (float) (1 * ((buffer[2] % 48)));
        litros += (float) (10 * ((buffer[1] % 48)));
        litros += (float) (100 * ((buffer[0] % 48)));
        
        // Calcula o preço a se pagar dependendo do combustível escolhido
        total = (price * litros);
    } // end if(op == 'L')
    else if (op == 'R') {
        // Armazena o valor passado na serial em uma variável float usando um typecast
        total = (float) (0.1 * ((buffer[4] % 48)));
        total += (float) (1 * ((buffer[2] % 48)));
        total += (float) (10 * ((buffer[1] % 48)));
        total += (float) (100 * ((buffer[0] % 48)));
        
        // Calcula qauntos litros serão abastecidos dependendo do combustível escolhido
        litros = (total / price);
    } // end else if(op == 'R')
    else {
        return;
    } // end else
    
    // Realiza o abastecimento
    abastecimentoBomba(total, litros);

} // end selecaoCombustivel()
