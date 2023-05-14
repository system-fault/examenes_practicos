#include <reg552.h>

#define BASE_TIEMPO 250
#define FREQ_OSC    11.0592
#define DESB_x_SEG  4000

// Declaracion de funciones
void inicializar(void);
unsigned int conversionAD(unsigned char);
void controlPanel(void);

// Variables globales
char verdeIn, ambarIn, rojoIn;
unsigned int contador;
unsigned long int contador10seg;
unsigned char TL_1, TH_1;
int temperatura; // La temperatura tomara valores negativos
unsigned int resultadoConversion;

// Salidas
sbit verde = 0x90;
sbit ambar = 0x91;
sbit rojo  = 0x92;
sbit panel = 0x97;

// Entradas
sbit botoPeaton = 0xC0;

void main(void)
{
    // Inicializa el semaforo
    inicializar();

    while (1) {
        // SECUENCIA VERDE
        while (verdeIn == 1) {
            // Encendemos el verde
            verde = 1;
            // Esperamos a que se levante TF1
            while (TF1 == 0)
                ;
            // Bajamos TF1
            TF1 = 0;
            // incrementamos el contador
            contador++;
            // Aumentamos el contador del panel
            contador10seg++;

            // Controlamos el panel en este punto porque es el timepo mas largo de la secuencia,
            //  es aqui donde podemos tener solapamientos de timepo
            controlPanel();

            // Comprueba si se ha pulsado el boton peaton
            if (botoPeaton == 1) {
                // Cargamos valores para saltar a ambar
                contador = 0;
                verdeIn  = 0;
                ambarIn  = 1;
                verde    = 0;
            }
            // Comprueba si han pasado 10 segundos/ 1seg 4000
            if (contador == 40000) {
                contador = 0;
                verdeIn  = 0;
                ambarIn  = 1;
                verde    = 0;
            }
        }

        controlPanel();

        // SECUENCIA AMBAR
        while (ambarIn == 1) {

            // Encendemos el ambar
            ambar = 1;
            // Esperamos a que se levante TF1
            while (TF1 == 0)
                ;
            // Bajamos TF1
            TF1 = 0;
            // incrementamos el contador
            contador++;
            // Aumentamos el contador del panel
            contador10seg++;

            // Comprueba si han pasado 2 segundos/ 1seg 4000
            if (contador == 8000) {
                contador = 0;
                ambarIn  = 0;
                rojoIn   = 1;
                ambar    = 0;
            }
        }

        controlPanel();

        // SECUANCIA ROJO
        while (rojoIn == 1) {

            // Encendemos el ambar
            rojo = 1;
            // Esperamos a que se levante TF1
            while (TF1 == 0)
                ;
            // Bajamos TF1
            TF1 = 0;
            // incrementamos el contador
            contador++;
            // Aumentamos el contador del panel
            contador10seg++;

            // Compruba si el boton se ha pulsado el boton peaton
            if (botoPeaton == 1) {
                contador = 0; // anade 7seg
            }
            // Comprueba si han pasado 7 segundos/ 1seg 4000
            if (contador == 28000) {
                contador = 0;
                rojoIn   = 0;
                verdeIn  = 1;
                rojo     = 0;
            }
        }

        controlPanel();
    }
}

// Definicion de funciones

void inicializar(void)
{
    // Variables locales
    unsigned char vi_carga;
    // Configuracion del timer 1 modo 2 (10) temporizador 8 bits con auto recarga
    TMOD = 0x20;
    // Calculo de carga inicial del timer 1
    vi_carga = (0xFF - 1) - BASE_TIEMPO * FREQ_OSC / 12.0;
    TL_1     = vi_carga;
    TH_1     = vi_carga;
    // Una vez disparado el timer no hace falta volver a recargarlo
    TL1 = TL_1;
    TH1 = TH_1;
    // Ponemos las salidas a 0
    verde = 0;
    ambar = 0;
    rojo  = 0;

    // Iniciamos el TR1
    TR1 = 1;

    // Seleccionamos verde como primer color
    verdeIn = 1;

    // Pnemos a cero el contador
    contador = 0;

    // Apagamos el panel
    panel = 0;
}

unsigned int conversionAD(unsigned char canal)
{

    // variables locales
    unsigned int resultado;

    // Configuramos ADCON
    ADCON = ADCON & 0x00; // Boramos ADCON
    ADCON = canal;        // Asignamos el canal

    // Iniciamos la conversion AD
    ADCON = ADCON | 0x08;

    // Esperamos a que la conversion este hecha
    while ((ADCON & 0x10) == 0)
        ;
    // Ponemos ADCI a cero
    ADCON = ADCON & 0xEF;

    // Guardamos en la variable el resultado
    resultado = (ADCON >> 6) | ADCH << 2;

    return (resultado);
}

void controlPanel(void)
{

    // Comprobamos si han pasado 10 segundos
    if (contador10seg >= 40000) {
        contador10seg       = 0;
        resultadoConversion = conversionAD(3);
        temperatura         = ((70.0 / 1023) * resultadoConversion - 20.0);
        // Modifica el panel
        if (temperatura < 15) {
            panel = 1;
        }
        if (temperatura >= 15) {
            panel = 0;
        }
    }
}