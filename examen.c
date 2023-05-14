#include <reg552.h>

//#define BASE_DE_TIEMPO 250//
#define FREQ_OCS       11.0592
#define DESB_x_SEG     4000

// Declaracion de funciones
void inicializar(void);

// Salidas P4
// Cohetes
sbit cohete1 = 0xC0;
sbit cohete2 = 0xC1;
sbit cohete3 = 0xC2;
// Seguridad
sbit guillotina = 0xC7;

// Entradas P1
sbit disparo    = 0x90;
sbit EMERGENCIA = 0x97;

// Variables globales
unsigned long int contador;
unsigned char TL_0, TH_0;
unsigned char SEGURIDAD; // VARIABLE PARA CONTROLAR EL DISPARO DE SETA DE SEGURIDAD
unsigned char DISPARADO;
unsigned char MODO;


void main(void)
{

    // Inicializa
    inicializar();

    while (1) {

        while ((disparo == 0) && (DISPARADO == 0) && (EMERGENCIA == 0)) {
            // Todas las salidas a cero listas
            cohete1    = 0;
            cohete2    = 0;
            cohete3    = 0;
            guillotina = 0;
            // contador para el ciclo lo mantenemos en cero
            contador = 0;
        }
        if (disparo) {
            DISPARADO = 1;
        }
    }
}

/*################# FUNCIONES ######################*/

// CONFIGURACION//

void inicializar(void)
{

    // Variables locales
    unsigned char valor_carga_inical;



    // Configuracion timer 0  modo 2 (10) 8bits con autorecarga
    TMOD = TMOD | 0x02;

    // Calculo y carga del temporizador
    valor_carga_inical = (0xFF - 1) - BASE_DE_TIEMPO * FREQ_OCS / 12.0;
    TL_0               = valor_carga_inical;
    TH_0               = valor_carga_inical;
    TL0                = TL_0;
    TH0                = TH_0;

    // Interruciones
    EA  = 1; // Interrpciones globales
    ET0 = 1; // Interrupcion timer0

    // Salidas a cero por seguridad
    cohete1    = 0;
    cohete2    = 0;
    cohete3    = 0;
    guillotina = 0;

    // Control disparo de seguridad
    SEGURIDAD = 0;

    // TR0 START
    TR0 = 1;
}
////////////////////////////////////////////////////////////////////////////////

// INTERRUPCION TR0//

void interrupcionTR0(void) interrupt 1 using 3
{

    // Variables locales

    // Sumamos un desbrodamiento SI ESTAMOS EN MARCHA
    if (DISPARADO == 1) {
        contador++;
    }

    if ((contador > 0) && (contador < (5 * DESB_x_SEG)) && (EMERGENCIA == 0)) {
        // Disparamos el primer choete
        cohete1   = 1;
        DISPARADO = 1;
    }

    if (contador == (5 * DESB_x_SEG) && (EMERGENCIA == 0)) {
        cohete2   = 1;
        DISPARADO = 1;
    }

    if (contador == (8 * DESB_x_SEG) && (EMERGENCIA == 0)) {
        cohete3   = 1;
        DISPARADO = 1;
    }

    if (contador == (48000) && (EMERGENCIA == 0)) {
        cohete1   = 0;
        cohete2   = 0;
        cohete3   = 0;
        DISPARADO = 0;
        contador  = 0;
    }

    if (EMERGENCIA == 1) {

        // Corte de mechas
        guillotina = 1;

        // Desactivacion pines de disparo
        cohete1 = 0;
        cohete2 = 0;
        cohete3 = 0;

        while (EMERGENCIA == 1) {
            // mientras se espera el reset de emergencia se inicializan los parameetros necesarios para volver al modo espera
            DISPARADO  = 0;
            contador   = 0;
            guillotina = 0;
        }
    }
}