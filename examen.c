#include <REG552.H>

#define BaseTiempo               50000
#define FrecOsc                  11.0592
#define DesbordamientosPorSegudo 20
#define Canal_lectura_temperatura 5

// variables globales
unsigned char TL_0, TH_0;
unsigned char contador;

// entradas
sbit marcha = 0x90;
sbit S1     = 0x91;
sbit S2     = 0x92;
sbit control_temperatura = 0x97;

// salidas
sbit LED_M           = 0xC0;
sbit ValvulaVaciado  = 0xC1;
sbit BombaHidraulica = 0xC2;
sbit LED_T = 0xC7;

void inicializar(void);

void main(void)
{
		inicializar();
    while (1) {
        while (marcha == 0) {
            TR0             = 0;
            contador        = 0;
            BombaHidraulica = 0;
            ValvulaVaciado  = 0;
            TL0             = TL_0;
            TH0             = TH_0;
        }
        if ((marcha == 1) && ((S2 == 0) || (S1 == 0))) {
            TR0   = 1;
            LED_M = 1;
            if ((S2 == 1)&&(S1 == 1)){
                TR0 = 0;
            }
            
        }
    }
}

void inicializar(void)
{
    // varoables locales
    unsigned int vi;
    TMOD            = 0x01;
    vi              = (0xFFFF + 1) - BaseTiempo * FrecOsc / 12.0;
    TL_0            = vi;
    TH_0            = vi >> 8;
    TL0             = TL_0;
    TH0             = TH_0;
    EA              = 1;
    ET0             = 1;
    LED_M           = 0;
    ValvulaVaciado  = 0;
    BombaHidraulica = 0;
		
}

void interrupcion(void) interrupt 1 using 2
{

    //Variables locales
    unsigned int codigoDigital;
    float temperatura;

    TL0      = TL_0;
    TH0      = TH_0;
    contador = contador + 1;
    if ((contador == 1) && (S2 == 0)) {
        ValvulaVaciado  = 0;
        BombaHidraulica = 1;
        LED_T = 0;
    }
    if ((contador == (10 * DesbordamientosPorSegudo)) || (S2 == 1)) {
        ValvulaVaciado  = 1;
        BombaHidraulica = 0;
        contador        = 0;
    }

    if ((control_temperatura == 1)&&(!BombaHidraulica)){

        LED_T = 1;

        if (contador == (10 * DesbordamientosPorSegudo)){
            
            //Lectura de temperatura



        }
    }
}

unsigned int conversionAD (void){

    ADCON = ADCON & 0x00; //Borramos ADCON
    ADCON = ADCON | Canal_lectura_temperatura;

    ADCON = ADCON | 0x08;

    while ((ADCON && 0x10) == 0 );

    ADCON = ADCON & 0xEF;

    return(ADCON >> 6 | ADCH << 2);
     
}



