// Inclus�o das bibliotecas
#include <proc/pic18f4520.h>
#include "config.h"
#include "lcd.h"
#include "rgb.h"
#include "keypad.h"
#include "serial.h"
#include "ssd.h"
#include "timer.h"
#include "atraso.h"
#include "bombaComb.h"

// Defini��o de alguns comandos do LCD
#define L_ON 0x0F
#define L_OFF 0x08
#define L_CLR 0x01
#define L_L1 0x80
#define L_L2 0xC0

/*

    Perif�ricos utilizados:
    - Teclado
    - Display de LCD 
    - Monitor serial 
    - Displays de 7 segmetos
    - LED RGB
    - Timer

 */

// Fun��o principal
void main(void) {
    char tecla, slot;

    /*
    // Configura��o de algumas portas
    TRISA = 0xC3;
    TRISB = 0x03;
    TRISC = 0x01;
    TRISD = 0x00;
    TRISE = 0x00;
    ADCON1 = 0x06;
    */
    
    // Inicializa��o dos perif�ricos
    lcdInit();
    rgbInit();
    kpInit();
    serial_init();
    ssdInit();
    timerInit();
    
    /*
    // Configura��o da porta serial
    TRISCbits.TRISC7 = 1; // RX
    TRISCbits.TRISC6 = 0; // TX
    PORTB = 0;
    */
     
    // Mensagens iniciais
    lcdCommand(L_L1);
    lcdString("    Bomba de");
    lcdCommand(L_L2);
    lcdString("  Combustivel:");
    serial_tx_str("\r\nPicSimLab-Bomba de combustivel\n\r");
    
    // Atraso para come�ar a rodar o programa principal (+- 5s)
    atraso_ms(5000);
    
    // Looping infinito
    for (;;) {
        
        // Alterna entre as fun��es
        switch (slot) {
            // Tela inicial da bomba, chamada ap�s uma utiliza��o ou na primeira inicializa��o
            case 0:
                telaInicial();
                slot = 1;
                break;
            // Ap�s a tela inicial, espera o usu�rio apertar um bot�o    
            case 1:
                // Recebe a leitura do teclado
                tecla = kpReadKey();

                // Se alguma tecla for apertada
                if (tecla != 0) {
                    // Passa a tecla apertada para a sele��o de combust�vel
                    selecaoCombustivel(tecla);
                    slot = 0;
                } // end if(tecla != 0)
                break;
            // Caso default
            default:
                slot = 0;
                break;
        } // switch(slot)
        
        // Debounce das teclas
        kpDebounce();
        
    } // for(;;)
    
}// end main()
