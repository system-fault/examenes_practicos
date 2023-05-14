#include <reg552.h>

#define BASE_DE_TIEMPO_MODO_1 50000
#define BASE_DE_TIEMPO_MODO_2 250
#define FREQ_OCS              11.0592


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
sbit seleccionModo = 0x91;
sbit EMERGENCIA = 0x97;

// Variables globales
unsigned long int contador;
unsigned char TL_0, TH_0;
unsigned char DISPARADO;
//unsigned char MODO; // MODO = 0 (MODO 1 16 bits) ; MODO = 1 (MODO 2 8 bit con autorecarga)
unsigned long int desbordamientos;

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
        if (disparo && !EMERGENCIA) {
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

    // Seleccion de modo por defecto MODO1
    //MODO = seleccionModo;


    /*
    ################ MODO 1 ###############33
    if (MODO == 0) {
        // Configuracion timer 0  modo 2 (10) 16bits
        TMOD = TMOD | 0x01;

        // Calculo y carga del temporizador
        valor_carga_inical = (0xFFFF - 1) - BASE_DE_TIEMPO_MODO_1 * FREQ_OCS / 12.0;
        TL_0               = valor_carga_inical;
        TH_0               = valor_carga_inical >> 8;
        TL0                = TL_0;
        TH0                = TH_0;

        // Se fijan unos los desbordamientos por segundo
        DESB_x_SEG = 20;
    }

    */
    /*################ MODO 2 ###############33*/
    //if (MODO == 1) {
        // Configuracion timer 0  modo 2 (10) 8bits con autorecarga
        TMOD = TMOD | 0x02;

        // Calculo y carga del temporizador
        valor_carga_inical = (0xFF - 1) - BASE_DE_TIEMPO_MODO_2 * FREQ_OCS / 12.0;
        TL_0               = valor_carga_inical;
        TH_0               = valor_carga_inical;
        TL0                = TL_0;
        TH0                = TH_0;

        // Se fijan unos los desbordamientos por segundo
        desbordamientos = 4000;
   // }
    // Interruciones
    EA  = 1; // Interrpciones globales
    ET0 = 1; // Interrupcion timer0

    // Salidas a cero por seguridad
    cohete1    = 0;
    cohete2    = 0;
    cohete3    = 0;
    guillotina = 0;


    // TR0 START
    TR0 = 1;
}
////////////////////////////////////////////////////////////////////////////////

// INTERRUPCION TR0//

void interrupcionTR0(void) interrupt 1 using 3
{

    // Variables locales

    // Si estamos en MODO 1 recargamos el TR0
    //if (MODO == 0) {
    //    TL0 = TL_0;
    //    TH0 = TH_0;
   // }

    // Sumamos un desbrodamiento SI ESTAMOS EN MARCHA
    if (DISPARADO == 1) {
        contador++;
    }

    if ((contador > 0) && (contador < (5 * desbordamientos)) && (EMERGENCIA == 0)) {
        // Disparamos el primer choete
        cohete1   = 1;
        DISPARADO = 1;
    }

    if (contador == (5 * desbordamientos) && (EMERGENCIA == 0)) {
        cohete2   = 1;
        DISPARADO = 1;
    }

    if (contador == (8 * desbordamientos) && (EMERGENCIA == 0)) {
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