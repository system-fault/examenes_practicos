#include <reg552.h>

#define BASE_DE_TIEMPO 50000
#define FREQ_OSC       11.0592
#define DESB_x_SEG     20

// Declaracion de funciones
void inicializar(void);
unsigned int conversionAD(unsigned char);

// Entradas
sbit marcha = 0x80;

// Salidas
sbit tapaComida        = 0x90; //Control  tapa comida
sbit circutoCalentador = 0x91; //Control circuito calentadorr
sbit luz               = 0x97; //Control luz acuario

// Bombas
// Entradas
sbit bombasON = 0x87;
// salidas
sbit B1 = 0xB0;
sbit B2 = 0xB1;
sbit B3 = 0xB2;

// Variables globales
unsigned char TL_0, TH_0;
unsigned int contador30seg;
unsigned int contador5seg;
unsigned int contador12seg;

void main(void)
{

    // Inicializamos las configuraciones iniciales
    inicializar();

    // BUCLE PRINCIPAL INFINITO

    while (1) {
        while (marcha == 0) {
            // Salidas a 0 preparadas para empezar el ciclo
            tapaComida        = 0;
            luz               = 0;
            circutoCalentador = 0;
            B1                = 0;
            B2                = 0;
            B3                = 0;
            // Carga en el TR0 para poder comenzar a contar cuando marcha on
            TL0 = TL_0;
            TH0 = TH_0;
            // Para poder partir de cero cuando pasamos de paro a marcha es necesario inicializar lo contadores
            // si no seria una pausa, y queremos que comience el ciclo desde el inicio
            contador30seg = 0;
            contador5seg  = 0;
            contador12seg = 0;
        }
        // La luz siempre que esta en marcha esta ON
        luz = 1;
    }
}

// DEFINICION DE FUNCIONES//

// INICIALIZAR

void inicializar(void)
{

    // Variables locales
    unsigned int valorInicialTimer0;

    // Configuramos el timer TR0 16 bits modo 1 (01) 000 0001
    TMOD = TMOD | 0x01;

    // Calculo del valor inicial TR0
    valorInicialTimer0 = (0xFFFF - 1) - BASE_DE_TIEMPO * FREQ_OSC / 12.0;
    // Cargamos valores
    TL_0 = valorInicialTimer0;
    TH_0 = valorInicialTimer0 >> 8;
    TL0  = TL_0;
    TH0  = TH_0;

    // Interrupciones
    EA  = 1; // habilita interrupciones globales
    ET0 = 1; // habilita la interrupcion timer0

    // Salidas
    tapaComida        = 0;
    luz               = 0;
    circutoCalentador = 0;
    B1                = 0;
    B2                = 0;
    B3                = 0;

    // Ponemos en marcha el TR0
    TR0 = 1;
}

// INTERRUPCION
void interrupcionTR0(void) interrupt 1 using 1
{

    // Varibles

    float temperatura;

    // Cargamos valores en el TR0
    TL0 = TL_0;
    TH0 = TH_0;

    // Sumamos un desbordamiento
    contador30seg++;
    contador5seg++;
    // Si tenemos el modo oleaje
    if (bombasON == 1) {
        contador12seg++;
    }

    // Comprobamos que estamos en los 3 primeros segundos del ciclo
    if ((contador30seg >= (0 * DESB_x_SEG)) && (contador30seg <= (3 * DESB_x_SEG))) {
        tapaComida = 1;
    }

    if ((contador30seg >= (3 * DESB_x_SEG)) && (contador30seg <= (30 * DESB_x_SEG))) {
        tapaComida = 0;
    }
    // Termina el ciclo de 30 segundos y reseteamos el contador
    if (contador30seg == (30 * DESB_x_SEG)) { contador30seg = 0; }

    // Control de temperatura

    if (contador5seg == (5 * DESB_x_SEG)) {
        // Reset contador 5seg
        contador5seg = 0;
        // Obtenemos la temperatura
        temperatura = (47.5 / 1023) * conversionAD(0) + 2.5;
        // Si temperatura es 24 o mayor circuito off
        if (temperatura >= 27.4) {
            // Circuito off
            circutoCalentador = 0;
        } else {
            circutoCalentador = 1;
        }
    }

    // Modo oleaje

    // Mientras bombasON = 0
    if (bombasON == 0) {
        contador12seg = 0;
        B1            = 0;
        B2            = 0;
        B3            = 0;
    }

    if (bombasON == 1) {
        // Ciclo 1 B1 ON 6seg
        if ((contador12seg >= (0 * DESB_x_SEG)) && (contador12seg < (6 * DESB_x_SEG))) {
            // bomba 1 on
            B1 = 1;
            B2 = 0;
            B3 = 0;
        }
        // Ciclo 2 B1 OFF B2 Y B3 ON
        if ((contador12seg >= (6 * DESB_x_SEG)) && (contador12seg < (8 * DESB_x_SEG))) {
            // bomba 1 off y bomba 2 y 3 on
            B1 = 0;
            B2 = 1;
            B3 = 1;
        }
        // Ciclo 3 todas on
        if ((contador12seg >= (8 * DESB_x_SEG)) && (contador12seg < (12 * DESB_x_SEG))) {
            // bomba 1 off y bomba 2 y 3 on
            B1 = 1;
            B2 = 1;
            B3 = 1;
        }
        if (contador12seg == (12 * DESB_x_SEG)) {
            // reset contador 12seg
            contador12seg = 0;
        }
    }
}

// CONVERSION ANALOGICA DIGITAL

unsigned int conversionAD(unsigned char canal)
{

    // Inicializamos ADCON
    ADCON = ADCON & 0x00;
    // Configuramos el canal de lectura en este caso el 0 pin P5.0
    ADCON = ADCON | canal;

    // Iniciamos la conversion
    ADCON = ADCON = 0x08;

    // Esperamos a terminar la conversion
    while ((ADCON & 0x10) == 0)
        ;

    // Cuando termina la conversion ponemos ADCI a 0 ADCS se pone a cero solo
    ADCON = ADCON & 0xEF;

    // Devolvemos el valor
    return (ADCON >> 6 | ADCH << 2);
}