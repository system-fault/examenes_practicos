# Control de Disparo de Cohetes

Este proyecto implementa un sistema de control para disparar una secuencia de cohetes utilizando un microcontrolador 8051 y diferentes modos de operación. También incluye mecanismos de seguridad, como la detección de una señal de emergencia y la activación de una guillotina para cortar las mechas de los cohetes.

## Funcionamiento del sistema

El sistema consta de los siguientes componentes:

- Microcontrolador 8051
- Librería `reg552.h` (contiene las definiciones de los registros del microcontrolador)

### Constantes y Variables Globales

En el código se definen las siguientes constantes y variables globales:

- `BASE_TIEMPO_MODO_2` y `BASE_TIEMPO_MODO_1`: Bases de tiempo para diferentes modos de operación.
- `FREQ`: Frecuencia del oscilador.
- `contador`: Contador para controlar los desbordamientos del temporizador.
- `TH_0` y `TL_0`: Variables para almacenar la carga inicial de los registros `TH0` y `TL0` del temporizador.
- `desbordamientos`: Número de desbordamientos por segundo.
- `modo`: Variable para seleccionar el modo de operación.
- `DISPARADO`: Variable de control del sistema disparado.

### Bits de Control y Salidas

Se utilizan los siguientes bits de control y salidas del microcontrolador:

- `marcha`, `botonModo` y `EMERGENCIA`: Bits de entrada para el disparo de la secuencia, la selección de modo y la señal de emergencia, respectivamente.
- `cohete1`, `cohete2`, `cohete3` y `guillotina`: Salidas utilizadas para controlar los disparadores de cohetes y la guillotina.

### Función Principal `main`

La función principal `main` del programa realiza lo siguiente:

1. Llama a la función `inicio` para la inicialización del sistema.
2. En un bucle infinito, verifica las condiciones de inicio y secuencia.
3. Si la señal de marcha está activada y el sistema no está disparado, activa el temporizador y establece la variable `DISPARADO` en 1.

### Función de Inicialización `inicio`

La función `inicio` realiza la configuración inicial del sistema:

1. La variable `modo` se asigna según el valor del botón de selección de modo.
2. Si el modo seleccionado es el modo 2 (8 bits con autorecarga), se configuran los registros del temporizador (`TMOD`, `TL0` y `TH0`) y se calculan los valores iniciales.
3. Si el modo seleccionado es el modo 1 (16 bits), se realiza una configuración similar al modo 2.
4. Se habilitan las interrupciones globales y la interrupción del temporizador.
5. Se inicializan las salidas a cero.

### Función de Interrupción `interrupcionTR0`

La función de interrupción `interrupcionTR0` se ejecuta cada vez que ocurre un desbordamiento del temporizador (`TR0`):

1. Se incrementa el contador de desbordamientos.
2. Si el modo seleccionado es el modo 16 bits,se cargan los valores iniciales en los registros TL0 y TH0.
3. Se ejecutan diferentes acciones dependiendo del valor del contador, como activar los disparadores de cohetes en momentos específicos y reiniciar el sistema después de un tiempo determinado.

Si se activa la señal de emergencia, se detienen los disparadores de cohetes, se activa la guillotina y se espera hasta que la señal de emergencia se desconecte.
Uso del Código
El código proporcionado puede ser utilizado como base para implementar un sistema de control de disparo de cohetes utilizando un microcontrolador 8051. Se recomienda revisar y entender el código antes de realizar cualquier modificación o implementación adicional.

Contribuciones
Las contribuciones son bienvenidas. Si deseas mejorar el código, corregir errores o agregar nuevas características, puedes abrir un pull request y se revisará tu propuesta.
