#include <reg552.h>

#define BASE_TIEMPO_MODO_2 250     // Base de tiempo para el modo 8 bits con auto recarga
#define BASE_TIEMPO_MODO_1 50000   // Base de tiempo para el modo 16 bits
#define FREQ               11.0592 // Frecuencia del oscilador

// Declaracion de funciones
void inicio(void); // Funcion de inicializacion, en esta fase se hace la seleccion de modo

unsigned int contador;        // Contador para controlar los flags up del TR0
unsigned char TH_0, TL_0;     // Variables para almacenar la carga inicial de TL0 y TH0
unsigned int desbordamientos; // Desbordamientos por segundo
char modo;                    // Variable para seleccion de modo
char DISPARADO;               // Variable de control de sistema disparado

sbit marcha     = 0x90; // Disparo de la secuenciade lanzamiento
sbit botonModo  = 0x91; // Boton selleccion modo
sbit EMERGENCIA = 0x97; // Seta de emergencia

sbit cohete1    = 0xC0; // Disparador cohete 1 tronco
sbit cohete2    = 0xC1; // Disparador cohete 2 hojas
sbit cohete3    = 0xC2; // Disparador cohete 3 sol
sbit guillotina = 0xC7; // Guillotina para cortar mechas en caso de emergencia

// Funcion principal
void main(void)
{

    inicio(); // Llamada a la funcion de inicio

    // Bucle infinito
    while (1) {

        // Mientras marcha, emergencia y disparado a cero, se espera y mantienen las variables inicializadas
        // Tambien se ponen a cero todas la salidas
        while ((!marcha) && (!EMERGENCIA) && (!DISPARADO)) {
            TR0        = 0;
            TL0        = TL_0;
            TH0        = TH_0;
            cohete1    = 0;
            cohete2    = 0;
            cohete3    = 0;
            guillotina = 0;
            contador   = 0;
            DISPARADO  = 0;
            // Si tenemos el modo 16 bits activado carga valores iniciales
            if (modo == 1) {
                TL0 = TL_0;
                TH0 = TH_0;
            }
        }

        // Si marcha esta ON y no esta el sistema disparado entra y comienza la secuencia
        if (marcha && !DISPARADO) {

            TR0       = 1;
            DISPARADO = 1; // PUNTO UNICO EN QUE SE PONE A 1 LA VARIABLE //

            // Si tenemos el modo 16 bits activado carga valores iniciales
            if (modo == 1) {
                TL0 = TL_0;
                TH0 = TH_0;
            }
        }
    }
}

/*############ FUNCIONES ############*/

// INICIO//Configuracion inicial
void inicio(void)
{
    // Variable modo toma el valor de la entrada de modo
    modo = botonModo;

    // Si seleccionamos el modo 2 8 bits con autorecarga
    if (modo == 0) {

        // Varible para almacenar valores iniciales del timer
        unsigned char vi;
        // Configuramos TMOD
        TMOD = 0x02;
        // Calculamos valores iniciales
        vi = (0xFF + 1) - BASE_TIEMPO_MODO_2 * FREQ / 12.0;

        TL_0 = vi; // Cargamos vi
        TH_0 = vi; // En el modo 2 TH toma el valor de TL para poder recargar automaticamente
                   // Carga en SFR
        TL0 = TL_0;
        TH0 = TH_0;

        // Desbordamientos por segundo en modo 2
        desbordamientos = 4000;
    }

    // Si seleccionamos el modo 1 16 bits
    if (modo == 1) {

        // Varible para almacenar valores iniciales del timer
        unsigned int vi;
        // Configuramos TMOD
        TMOD = 0x01;
        // Calculamos valores iniciales
        vi = (0xFFFF + 1) - BASE_TIEMPO_MODO_1 * FREQ / 12.0;

        TL_0 = vi;      // Cargamos vi en TL
        TH_0 = vi >> 8; // En el modo 16 bits despalzamos la variable vi 8 posiciones y lo asignamos TH0
                        // Carga en SFR
        TL0 = TL_0;
        TH0 = TH_0;

        // Desbordamientos por segundo en modo 1
        desbordamientos = 20;
    }

    // Habilitacion de la interrupciones globales y TR0
    EA  = 1;
    ET0 = 1;

    // Se ponen a cero todas la salidas
    cohete1    = 0;
    cohete2    = 0;
    cohete3    = 0;
    guillotina = 0;
}

// INTERRUPCION//Interrupcion del TR0
void interrupcionTR0(void) interrupt 1 using 1
{
    // Sumamos un desbordamiento
    contador++;

    // Si tenemos el modo 16 bits activado carga valores iniciales
    if (modo == 1) {
        TL0 = TL_0;
        TH0 = TH_0;
    }

    // Si ha comenzado la secuencia en la primera pasada por la interrupcion lanza el primer cohete
    if ((contador == (1)) && (EMERGENCIA == 0)) {
        cohete1 = 1;
    }

    // En el segundo 5 lanzamos el segundo cohete
    if (contador == (5 * desbordamientos) && (EMERGENCIA == 0) && (DISPARADO == 1)) {
        cohete2 = 1;
    }

    // En el segundo 8 lanzamos el sol
    if (contador == (8 * desbordamientos) && (EMERGENCIA == 0) && (DISPARADO == 1)) {
        cohete3 = 1;
    }

    // En el segundo 12 reiniciamos el sistema y que preparado para un nuevo lanzamiento si no hay emergencia
    if (contador == (12 * desbordamientos) && (EMERGENCIA == 0) && (DISPARADO == 1)) {
        cohete1   = 0;
        cohete2   = 0;
        cohete3   = 0;
        contador  = 0;
        DISPARADO = 0;
    }

    // Si se activa la señal de emergencia
    if (EMERGENCIA == 1) {

        guillotina = 1; // Activa la guillotina
        cohete1    = 0; // Se detiene el lanzador 1
        cohete2    = 0; // Se detiene el lanzador 2
        cohete3    = 0; // Se detiene el lanzador 3
        contador   = 0; // Se reinicia el contador
        DISPARADO  = 0; // Se resetea la variable para poder volver a lanzar

        // Mientras no se desconecte la señal de emergencia no saldra por seguridad de este bucle
        while (EMERGENCIA) {
            guillotina = 0; // Desconecta la guillotina de las mechas
            // Si tenemos el modo 16 bits activado carga valores iniciales
            if (modo == 1) {
                TL0 = TL_0;
                TH0 = TH_0;
            }
        }
    }
}
