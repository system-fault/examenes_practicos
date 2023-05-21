/*############################################################################
  #*                                                                        *#
  #* ## Autor: David                                                        *#
  #* ## Proyecto: Control Deposito                                          *#
  #* ## Fecha: Mayo de 2023                                                 *#
  #* ## Lenguaje: C                                                         *#
  #* ## Utilidad: Control de llenado y temperatura de un deposito           *#
  #*                                                                        *#
############################################################################*/

#include <REG552.H> // Proporciona acceso a los registros del microcontrolador

#define BASE_DE_TIEMPO            50000   // Base de tiempo para TR0 16bits
#define FREQ_OSC                  11.0592 // Frecuencia del oscilador
#define DESB_x_SEG                20      // Desbordamientos del TR0 por segundo
#define CANAL_LECTURA_TEMPERATURA 5       // Canal de lectura señal analogica

// variables globales
unsigned char TL_0, TH_0;                           // Guarda valores iniciales de TL y TH
unsigned char contadorLlenado, contadorTemperatura; // Contadores para calcular tiempos
bit primerLlenado;                                  // Bit que controla si cuando ponemos en marcha es el primer llenado de un ciclo
unsigned int codigoTemperatura;                     // Guarda el resultado de la conversion analogico-digital
float temperatura;                                  // Guarda el calculo de la temperatura
bit mantenimientoOut;                               /* Variable que controla si acabamos de salir de un mantenimiento y obliga a pasar
                                                       por el reinicio de las condiciones iniciales*/

// entradas
sbit marcha             = 0x90; // P1.0
sbit S1                 = 0x91; // P1.1 sensor
sbit S2                 = 0x92; // P1.2 sensor
sbit controlTemperatura = 0x97; // P1.7
sbit mantenimiento      = 0xB2; // interrupcion externa 0, logica negada

// salidas
sbit LED_M                  = 0xC0; // 4.0 led puesta en marcha
sbit valvulaVaciado         = 0xC1; // 4.1
sbit bombaHidraulica        = 0xC2; // P4.2
sbit LED_T                  = 0xC7; // P4.7 led control temperatura activada
sbit resistenciaCalentadora = 0xC6; // P4.6

// Declaracion de funciones
void inicializar(void);
unsigned int conversionAD(unsigned char canal);

// FUNCION MAIN
void main(void)
{
    // Inicializa los valores iniciales
    inicializar();

    // Bucle principal
    while (1) {

        // Mientras marcha a off o se acaba de salir de un mantenimiento
        while ((marcha == 0) || (mantenimientoOut == 1)) {
            // Variables y salidas valores iniciales
            TR0                 = 0;
            contadorLlenado     = 0;
            contadorTemperatura = 0;
            bombaHidraulica     = 0;
            valvulaVaciado      = 0;
            temperatura         = 0;
            // Carga valores iniciales TR0
            TL0 = TL_0;
            TH0 = TH_0;
            // Bit de control de primer llenado 0 (no se ha realizado) 1 (ya se ha llenado el tanque una vez)
            primerLlenado = 0;
            // Si acabamos de hacer un manteniemiento ponemos a cero el bit de registro de entrada al modo mantenimiento
            if (mantenimientoOut == 1) { mantenimientoOut = 0; }
        }

        /* Mientras marcha on y el bit de mantenimeinto a cero(solo pasando por la racerga
        de valores iniciales se puede resetear este bit)*/
        while ((marcha == 1) && (!mantenimientoOut)) {

            // Encendemos el led
            LED_M = 1;

            // Si la bomba hidraulica esta encendida estamos en un ciclo de llenado y el control de temperatura tiene que estar apagado
            if ((bombaHidraulica)) {
                LED_T                  = 0;
                contadorTemperatura    = 0;
                resistenciaCalentadora = 0;
            }
            /* Si esta la bomba apagada y el control de temperatura apagado, reinicia el control de temperatura y para el TR0,
            solo entra si hemos realizado el primer llenado*/
            else if (!bombaHidraulica && !controlTemperatura && primerLlenado) {
                TR0                    = 0;
                LED_T                  = 0;
                contadorTemperatura    = 0;
                resistenciaCalentadora = 0;
                TL0                    = TL_0;
                TH0                    = TH_0;
            }

            // Si detectamos que es el primer llenado o el S1 se desactiva ponemos el proceso en marcha
            if ((primerLlenado == 0) || (S1 == 0)) {
                // Encendemos el TR0, la bomba y la valvula cerrada
                TR0             = 1;
                bombaHidraulica = 1;
                valvulaVaciado  = 0;
                /*Activamos el primer llenado. Esto no cambia hasta que marcha para
                y vuelva al reposo. nunca habra un primer llenado mas en el ciclo, si no se pasa por el bloque de paro*/
                if (!primerLlenado) { primerLlenado = 1; }
            }

            /* Comprobamos el contador ha llegado a 10 seg o si S2 se activa, para parar
                el cliclo y dejarlo preparado para comenzar de nuevo*/
            if (((contadorLlenado == (10 * DESB_x_SEG)) || (S2 == 1)) && bombaHidraulica) {
                valvulaVaciado  = 1;
                bombaHidraulica = 0;
                // Paramos el TR0 y lo recargamos,a la espera de empezar un nuevo ciclo
                TR0             = 0;
                TL0             = TL_0;
                TH0             = TH_0;
                contadorLlenado = 0;
            }

            /*### CONTROL DE TEMPERATURA ###*/

            // Si el control de temperatura esta On y no estamos en proceso de llenado ejecutamos el control de temperatura
            if ((controlTemperatura == 1) && (!bombaHidraulica)) {

                // Encendemos el led y TR0
                LED_T = 1;
                TR0   = 1;

                // Si han pasado diez segundos ejecuta la lectura de la temperatura
                if (contadorTemperatura == (10 * DESB_x_SEG)) {

                    // Reset al contador
                    contadorTemperatura = 0;

                    // Lectura de temperatura
                    codigoTemperatura = conversionAD(CANAL_LECTURA_TEMPERATURA);
                    // calculamos la temperatura
                    temperatura = (130.0 / 1023) * codigoTemperatura - 30.0;
                    // Si la temperatura es <= que 30 se enciende la resistancia
                    if (temperatura <= 30) {
                        resistenciaCalentadora = 1;
                    }
                    // Si la temperatura es > que 30 se apaga la resistancia
                    if (temperatura > 30) {
                        resistenciaCalentadora = 0;
                    }
                }
            }
        }
    }
}

/*##################### FUNCIONES #####################*/
// Definiciones

// INICIALIZAR
void inicializar(void)
{
    // variables locales
    unsigned int vi;
    // configuramos tmod timer 0 16 bits
    TMOD = 0x01;
    // Calculamos el valor inicial de del TR0
    vi = (0xFFFF + 1) - BASE_DE_TIEMPO * FREQ_OSC / 12.0;
    // Guardamos los valores iniciales en las variables
    TL_0 = vi;
    TH_0 = vi >> 8;
    // Cargamos lo valores de TR0
    TL0 = TL_0;
    TH0 = TH_0;
    // Habilitamos las interrupciones
    EA  = 1; // Globales
    ET0 = 1; // timer 0
    EX0 = 1; // externa 0 pin3.2
    IT0 = 0; // externa por nivel
    // desactivamos los LEDs
    LED_T = 0;
    LED_M = 0;
    // Cerramos la valvula y apagamos la bombaHidraulica
    valvulaVaciado  = 0;
    bombaHidraulica = 0;
    // Ponemos a cero la variable de primer llenado
    primerLlenado = 0;
    // Ponemos a cero el control de entrad a mantenimiento
    mantenimientoOut = 0;
}
/*##########################################################################*/

// INTERRUPCION TIMER 0
void interrupcionTR0(void) interrupt 1 using 2
{

    // Recargamos el TR0 al entrar en la interrupcion
    TL0 = TL_0;
    TH0 = TH_0;

    // Si estamos llenando el tanque sumamos al contador de llenado
    if (bombaHidraulica)
        contadorLlenado = contadorLlenado + 1; // contador++;

    // Si tenemos el control de temperatura on y no estamos llenando sumamos al contador de temperatura
    if ((controlTemperatura == 1) && (!bombaHidraulica)) {
        contadorTemperatura++;
    }
}
/*##########################################################################*/

// CONVERSION ANALOGICO DIGITAL
unsigned int conversionAD(unsigned char canal)
{

    ADCON = ADCON & 0x00;  // Borramos ADCON
    ADCON = ADCON | canal; // Cargamos el canal

    ADCON = ADCON | 0x08; // Ininciamos la conversion

    while ((ADCON & 0x10) == 0) // Esperamos a que este completa la conversion
        ;

    ADCON = ADCON & 0xEF; // poneoms a 0 ADCI

    return (ADCON >> 6 | ADCH << 2); // devolvemos el valor
}
/*##########################################################################*/

// INTERRUPCION EXTERNA P3.2
void interrupcionMantenimiento(void) interrupt 0 using 3
{

    // Apagamos todas las salidas
    bombaHidraulica        = 0;    // apagada
    valvulaVaciado         = 0;    // Cerrada
    LED_M                  = 0;    // off
    LED_T                  = 0;    // off
    resistenciaCalentadora = 0;    // off
    TR0                    = 0;    // off
    contadorLlenado        = 0;    // reset
    contadorTemperatura    = 0;    // reset
    TL0                    = TL_0; // valor inicial
    TH0                    = TH_0; // valor inicial

    // Activamos el bit que registra la entrada al modo mantenimiento
    mantenimientoOut = 1;
    // Espera a que mantnimiento este a nivel alto para salir de la interrupcion
    while (!mantenimiento)
        ;
}
/*##########################################################################*/