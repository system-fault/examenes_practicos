#include <reg552.h>

#define BASE_TIEMPO 50000
#define FREQ_OSCI   12.0
#define DESB_x_SEG  20

// Declaracion de funciones
void inicializar(void);

// Variables globales
sbit marcha    = 0xC0;
sbit sentido   = 0xC1;
sbit velocidad = 0xC2;

unsigned char TL_0, TH_0;
unsigned char indice;

void main(void)
{

    inicializar();
    while (1) {

        /*ESTE PROGRAMA DIO PROBLEMAS, NUNCA PARABA, LA INTERRUPCION SE EJECUTABA SIEMPRE Y NUNCA PARABA DE EJECUTARSE EL CODIGO*/
        /*SOLUCION MAS LOGICA!!!!!! PARA EL TR EN EL WHILE Y SE ACTIVA CUANDO MARCHA ON, SE SOLUCINA EL PROBLEMA, SI EL TR ESTA
        ENCENDIDO ES IMPOSIBLE EVITAR QUE LA INTERRUPCION SALTE, POR ESO ES UNA INTERRUPCION*/
        while (marcha == 0) {
            // Apagamos el letrero TR0 y dejamos cargado para cuando marcha ON
            P1  = 0x00;
            TR0 = 0;
            TL0 = TL_0;
            TH0 = TH_0;
        }
        if ((marcha == 1) && (TR0 == 0)) // Si el temporizador esta encendido nos saltamos esta instruccion
        {
            TR0 = 1;
        }
    }
}

// Definicion de funciones

void inicializar(void)
{
    // Variables locales
    unsigned int valor_inicial_TR0;

    // Configuramos TMOD para el TR0 modo 16bits
    TMOD = 0x01;
    // Calculamos la carga inicial de temporizador
    valor_inicial_TR0 = (0xFFFF + 1) - BASE_TIEMPO * FREQ_OSCI / 12.0;
    // Carga de los valores en TR0
    TL_0 = valor_inicial_TR0;
    TH_0 = valor_inicial_TR0 >> 8;
    TL0  = TL_0;
    TH0  = TH_0;

    // Configuracion interrupciones
    EA  = 1; // Interrupciones globales activadas
    ET0 = 1; // Interrupcion TR0 activada

    // Encendemos la primera secuencia segun sea el valor de sentido y fijamos en que posicion del indice estariamos
    if (sentido == 1) {
        P1     = 0x80;
        indice = 1;
    }
    if (sentido == 0) {
        P1     = 0xFF;
        indice = 16;
    }

    /* Ponemos en marcha el TR0  Se añade TR0 al bucle infinito, no es necesario aqui,
    solo tendremos el TR conectado cuando este marcha habilitada */
    // TR0 = 1;
}

void interrupcionTR0(void) interrupt 1 using 1
{
    /*################################ SOLUCIONADO Y EXPLICADO EN EL MAIN ######################################*/
    /*Posible solucion al problema del while, si metemos todo el codigo que se ejecuta en la
    interrupcion en un if,mejor opcion meter codigo en el while*/ /*SOLOCIONADO Y EXPLICADO EN EL MAIN*/ 
    /*###########################################################################################################*/

    // Variables locales **ESTE ARRAY HA DADO PROBLEMAS PONIENDO LA EXPRESION EN BINARIO**
    unsigned char code codigo[18] = {0x80,
                                     0x40,
                                     0x20,
                                     0x10,
                                     0x08,
                                     0x04,
                                     0x02,
                                     0x01,
                                     0xAA,
                                     0x55,
                                     0x80,
                                     0xC0,
                                     0xE0,
                                     0xF0,
                                     0xF8,
                                     0xFC,
                                     0xFE,
                                     0xFF};
    static unsigned char contador;

    unsigned char desbordamientos;

    /* Recarga Carga TR0 SUPER IMPORTANTE, NO DEJA DE FUNCIONAR PERO DA ERRORES DE TIMEPO, VA A SUMAR
       EL CONTADOR ENTERO, PARA DETECTAR ESTE FALLO EN UN SEGUNDO CONTARIA +- EL DOBLE DEL MAXIMO UNOS 130000
       MICRO SEGUNDOS*/
    TL0 = TL_0;
    TH0 = TH_0;

    // Comprobamos velocidad y la fijamos los desbordamientos
    // VELOCIDAD 1/2 SEGUNDO
    if (velocidad == 0) {
        desbordamientos = DESB_x_SEG / 2;
        if (contador > 10) { contador = 0; } /*SI NO SE PONE A CERO Y EL CONTADOR ESTA POR NECIMA DEL VALOR
                                               DE DESBORDAMIENTO SIGUE SUBIENDO HASTA DESBORDAR LA VARIABLE*/
    }
    // VELOCIDAD 1 SEGUNDO
    if (velocidad == 1) {
        desbordamientos = DESB_x_SEG;
    }

    contador++;

    // Verificamos el valor de contador para cambiar el codigo y el sentido de la secuencia
    // Sentido bajada
    if ((contador == desbordamientos) && (sentido == 1)) {

        // Aplicamos la senal de control
        P1 = codigo[indice];
        // Desplazamos el indice de array
        indice++;
        // Reseteamos contador
        contador = 0;
        // Si tenemos el indice desbordado lo reseteamos al primer valor de indice segun su direccion
        if (indice == 18) { indice = 0; }
    }

    // Sentido subida
    if ((contador == desbordamientos) && (sentido == 0)) {

        // Aplicamos la senal de control
        P1 = codigo[indice];
        // Desplazamos el indice de array
        indice--;
        // Reseteamos contador
        contador = 0;
        // Si tenemos el indice desbordado lo reseteamos al primer valor de indice segun su direccion
        if (indice == 0) { indice = 17; }
    }
}