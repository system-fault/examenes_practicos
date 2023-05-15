#include <reg552.h>

#define BASE_DE_TIEMPO           50000
#define FREQ_OSCI                11.0592
#define CANAL_REF_TEMPERATURA    0
#define CANAL_SENSOR_TEMPERATURA 1

// Declaracion de funciones

void inicialicizar(void);
unsigned int conversionAD(unsigned char canal);

// Variables globales
unsigned char TL_carga, TH_carga;
unsigned char contador;

sbit marcha = 0xC0; // Activa el control de temperatura de la placa de induccion

void main(void)
{
    inicialicizar();

    while (1) {
        while (marcha == 0) {
            // Paramos el timer
            TR0 = 0;
            // Apagamos las salidas
            P3 = 0xC3;
            // Carga de nuevo valores iniciales TR0
            TL0 = TL_carga;
            TH0 = TH_carga;
            // Reseteamos el contador
            contador = 0;
        }

        if ((marcha == 1) && (TR0 == 0)) {
            // Ponemos en marcha el TR0
            TR0 = 1;
        }
    }
}

/*################################ SFUNCIONES ######################################*/

// Definicion de funciones

// ######INICIALIZAR######//
void inicialicizar(void)
{

    // Variables locales
    unsigned int vi_inicial;

    // Configuramos el TR en modo 01 16bit modo timer T MODO TMOD!!!!
    TMOD = 0x01;

    // Calculo del valor inicial de TR0
    vi_inicial = (0xFFFF - 1) - BASE_DE_TIEMPO * FREQ_OSCI / 12.0;

    // Carga en TL0 y TH0
    TL0 = TL_carga = vi_inicial;
    TH0 = TH_carga = vi_inicial >> 8;

    // Habilitacion interrupciones
    EA  = 1;
    ET0 = 1;

    // Ponemos P3.2;P3.3;P3.4;P3.5 a 0 para evitar problemas
    P3 = 0xC3;
}

// ###### CONVERSION ANALOGICO DIGITAL ######//
unsigned int conversionAD(unsigned char canal)
{
    /*OJO!!!! AL SIMULAR LOS VOLTAJES TIENEN QUE SER REALES, LOS PUERTOS ACEPTAN ENTRE 0 Y 5 VOLTIOS
    SI SE SIMULA SE SIMULAN VOLTAJES ENTRE 0-5 SI SUBES DE 5 VOLTIOS DESBORDAS Y NO FUNCIONA*/

    // Variable local para devolver el valor
    unsigned int resultadoConversion; // Mismo tipo de dato que la funcion unsigned int!!!!

    // Ponemos ADCON al 0
    ADCON = ADCON & 0x00;

    // Configuramos ADCON
    ADCON = ADCON | canal;

    // Iniciamos la conversion
    ADCON = ADCON | 0x8;

    // Esperamos a que la conversion este completada

    while ((ADCON & (0x10)) == 0)
        ;

    // Ponemos ADCI a 0
    ADCON = ADCON & 0xEF;

    // Guardamos en resultadoConversion
    resultadoConversion = (ADCON >> 6) | ADCH << 2;
    // Devolvemos el resultado
    return (resultadoConversion);
}

// ###### INTERRUPCION TR0 ######//
void interrupcionTR0(void) interrupt 1 using 1
{

    // Variables locales
    float referenciaTemperatura, temperatura;                                  // La temperatura es un valor decimal
    unsigned int codigoDigitalReferenciaTemperatura, codigoDigitalTemperatura; // Mismo dato que la funcion de la que recibe el dato
    float error;                                                               // Resultado de operar con decimales

    // Carga de nuevo valores iniciales TR0
    TL0 = TL_carga;
    TH0 = TH_carga;

    contador++;

    if (contador == 200) {

        // Obtenenmos los valores de la referencia de temperatura y la temperatura
        codigoDigitalReferenciaTemperatura = conversionAD(CANAL_REF_TEMPERATURA);
        codigoDigitalTemperatura           = conversionAD(CANAL_SENSOR_TEMPERATURA);
        // Calcula la temperatura y la referencia de temperatura para restar y hacer el error
        referenciaTemperatura = (97.5 / 1023) * codigoDigitalReferenciaTemperatura + 25;
        temperatura           = (97.5 / 1023) * codigoDigitalTemperatura + 25;
        // Obtenemos el error
        error = referenciaTemperatura - temperatura;

        // Segun el signo de error seleccionamos una senal de salida en P3 y ponemos a cero el contador para volver a contar 10 segundos
        if (error > 0) {
            P3 = 0xFF;
        } else if (error <= 0) {
            P3 = 0xC3;
        }
        contador = 0;
    }
}