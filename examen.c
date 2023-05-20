#include <REG552.H>

#define BaseTiempo                50000
#define FrecOsc                   11.0592
#define DesbordamientosPorSegudo  20
#define Canal_lectura_temperatura 5

// variables globales
unsigned char TL_0, TH_0;
unsigned char contadorLlenado, contadorTemperatura;
bit primerLlenado;
unsigned int codigoTemperatura;
float temperatura;
bit mantenimientoOut;

// entradas
sbit marcha              = 0x90;
sbit S1                  = 0x91;
sbit S2                  = 0x92;
sbit control_temperatura = 0x97;
sbit mantenimiento       = 0xB2; // interrupcion externa 0 logica negada un cero lo trataremos como un uno


    // salidas
    sbit LED_M              = 0xC0;
sbit ValvulaVaciado         = 0xC1;
sbit BombaHidraulica        = 0xC2;
sbit LED_T                  = 0xC7;
sbit resistenciaCalentadora = 0xC6;

void inicializar(void);
unsigned int conversionAD(unsigned char canal);

void main(void)
{
    inicializar();
    while (1) {

        while ((marcha == 0) || (mantenimientoOut == 1)) {
            TR0                 = 0;
            contadorLlenado     = 0;
            contadorTemperatura = 0;
            BombaHidraulica     = 0;
            ValvulaVaciado      = 0;
            TL0                 = TL_0;
            TH0                 = TH_0;
            primerLlenado       = 0;
            // Si acabamos de hacer un manteniemiento ponemos a cero el bit de registro de entrada al modo mantenimiento
            if (mantenimientoOut == 1) { mantenimientoOut = 0; }
        }
        while ((marcha == 1)&&(!mantenimientoOut)) {

            // Encendemos el led
            LED_M = 1;

            /*### CONTROL DE TEMPERATURA ###*/
            /* Si la bomba hidraulica esta encendida estamos en un ciclo de llenado y el control de temperatura tiene que estar apagado
            O si esta la bomba apagada y el control de temperatura apagado reinicia el control de temperatura y para el TR0  */
            if ((BombaHidraulica)) {
                LED_T                  = 0;
                contadorTemperatura    = 0;
                resistenciaCalentadora = 0;
            }

            if (!BombaHidraulica && !control_temperatura) {
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
                BombaHidraulica = 1;
                ValvulaVaciado  = 0;
            }

            /* Comprobamos el contador o si S2 esta activado para parar
                el cliclo y dejarlo preparada para comenzar de nuevo*/
            if ((contadorLlenado == (10 * DesbordamientosPorSegudo)) || (S2 == 1)) {
                ValvulaVaciado  = 1;
                BombaHidraulica = 0;
                // Paramos el TR0, lo recargamos y a la espera de empezar un nuevo ciclo
                TR0             = 0;
                TL0             = TL_0;
                TH0             = TH_0;
                contadorLlenado = 0;
                /*Activamos el primer llenado. Esto no cambia hasta que marcha para
                y vuelva al reposo. nunca habra un primer llenado mas en el ciclo*/
                primerLlenado = 1;
            }

            /*### CONTROL DE TEMPERATURA ###*/

            // Si el control de temperatura esta On y no estamos en proceso de llenado ejecutamos
            if ((control_temperatura == 1) && (!BombaHidraulica)) {

                // Encendemos el led y TR0
                LED_T = 1;
                TR0   = 1;

                if (contadorTemperatura == (10 * DesbordamientosPorSegudo)) {

                    // Reset al contador
                    contadorTemperatura = 0;

                    // Lectura de temperatura
                    codigoTemperatura = conversionAD(Canal_lectura_temperatura);
                    // calculamos la temperatura
                    temperatura = (130.0 / 1023) * codigoTemperatura - 30.0;

                    if (temperatura <= 30) {
                        resistenciaCalentadora = 1;
                    }
                    if (temperatura > 30) {
                        resistenciaCalentadora = 0;
                    }
                }
            }
        }
    }
}

void inicializar(void)
{
    // varoables locales
    unsigned int vi;
    // configuramos tmod timer 0 16 bits
    TMOD = 0x01;
    // Calculamos el valor inicial de del TR0
    vi = (0xFFFF + 1) - BaseTiempo * FrecOsc / 12.0;
    // Guardamos los valores iniciales en las variables
    TL_0 = vi;
    TH_0 = vi >> 8;
    // Cargamos lo valores de TR0
    TL0 = TL_0;
    TH0 = TH_0;
    // Habilitamos las interrupciones
    EA  = 1;
    ET0 = 1;
    EX0 = 1;
    IT0 = 0;
    // desactivamos los LEDs
    LED_T = 0;
    LED_M = 0;
    // Cerramos la valvula y apagamos la bombahidraulica
    ValvulaVaciado  = 0;
    BombaHidraulica = 0;
    // Ponemos a cero la variable de primer llenado
    primerLlenado = 0;
    // Ponemos a cero el control de entrad a mantenimiento
    mantenimientoOut = 0;
}

void interrupcion(void) interrupt 1 using 2
{

    // Recargamos el TR0 al entrar en la interrupcion
    TL0 = TL_0;
    TH0 = TH_0;

    // Si estamos llenando el tanque sumamos al contador de llenado
    if (BombaHidraulica)
        contadorLlenado = contadorLlenado + 1; // contador++;

    // Si tenemos el control de temperatura on y no estamos llenando sumamos al contador de temperatura
    if ((control_temperatura == 1) && (!BombaHidraulica)) {
        contadorTemperatura++;
    }
}

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

void interrupcionMantenimiento(void) interrupt 0 using 3
{

    // Apagamos todas las salidas
    BombaHidraulica        = 0; // apagada
    ValvulaVaciado         = 0; // Cerrada
    LED_M                  = 0;
    LED_T                  = 0;
    resistenciaCalentadora = 0;
    TR0 = 0;

    // Activamos el bit que registra la entrada al modo mantenimiento
    mantenimientoOut = 1;
    while ((mantenimiento == 0)&&(marcha))
        ;
}
