# Control Acuario

Este código se encarga de controlar un sistema de alimentación automatizado para un acuario. Utiliza un microcontrolador para gestionar diferentes dispositivos y realizar acciones como abrir y cerrar la tapa de comida, controlar la iluminación, regular la temperatura y activar las bombas en modo oleaje.

El código implementa funciones para inicializar el sistema, manejar interrupciones y realizar conversiones analógicas a digitales. A través de la programación del microcontrolador, se logra un control preciso y automatizado del acuario, brindando un entorno adecuado para los organismos que lo habitan.

## Funcionalidades principales

- Control de la tapa de comida para proporcionar alimento a los peces en momentos específicos.
- Gestión de la iluminación del acuario para simular condiciones diurnas y nocturnas.
- Control de la temperatura a través de un circuito calentador, evitando fluctuaciones bruscas.
- Activación de las bombas en modo oleaje para crear corrientes en el agua del acuario.

## Configuración y uso

Para utilizar este código en tu sistema de control de acuario, sigue los siguientes pasos:

1. Realiza las conexiones adecuadas del microcontrolador a los dispositivos del acuario, como las salidas para las bombas, la tapa de comida, la iluminación y el circuito calentador.
2. Ajusta las constantes definidas en el código, como `BASE_DE_TIEMPO`, `FREQ_OSC` y `DESB_x_SEG`, según tus necesidades.
3. Compila y carga el código en el microcontrolador.
4. Alimenta y enciende el sistema. El código se encargará de controlar automáticamente las diferentes funciones del acuario.

Recuerda realizar pruebas exhaustivas y verificar que el sistema funcione correctamente antes de utilizarlo con organismos vivos en el acuario.

¡Disfruta de un control automatizado y eficiente para tu acuario con este código!

# Descripción detallada del código

El código "Control Acuario" está diseñado para controlar de manera automatizada un sistema de alimentación para un acuario. A continuación, se detallan las principales partes y funcionalidades del código:

## Definición de constantes y pines
En esta sección del código, se definen las constantes que serán utilizadas a lo largo del programa, como BASE_DE_TIEMPO, FREQ_OSC y DESB_x_SEG. Además, se establecen los pines correspondientes a las entradas y salidas utilizadas en el sistema, tales como marcha, tapaComida, circutoCalentador, luz, bombasON, B1, B2 y B3.

## Declaración de funciones
En esta sección, se declaran las funciones inicializar() y conversionAD(), que se encargan de inicializar el sistema y realizar la conversión analógica a digital, respectivamente.

### Función principal main()
La función main() es el punto de entrada del programa. Dentro de un bucle principal infinito, verifica el estado de la señal marcha para determinar si el sistema está en funcionamiento o detenido. Dependiendo de este estado, se realizan diferentes acciones, como establecer las salidas en sus estados iniciales, cargar valores en el temporizador TR0 y reiniciar los contadores. Además, se enciende la luz del acuario cuando el sistema está en funcionamiento.

### Función inicializar()
La función inicializar() se encarga de realizar las configuraciones iniciales del sistema. En primer lugar, configura el temporizador TR0 en modo 16 bits y modo 1. A continuación, calcula los valores iniciales para el temporizador utilizando la constante BASE_DE_TIEMPO y la frecuencia del oscilador FREQ_OSC. Luego, habilita las interrupciones globales y la interrupción del temporizador ET0. Por último, configura las salidas en sus estados iniciales y pone en marcha el temporizador TR0.

### Función de interrupción interrupcionTR0()
La función interrupcionTR0() se ejecuta cuando ocurre una interrupción del temporizador TR0. En esta función se realizan diversas acciones en función de los valores de los contadores y las señales de entrada. Se controla la apertura y cierre de la tapa de comida en intervalos de tiempo específicos. Además, se realiza el control de temperatura del circuito calentador cada 5 segundos, obteniendo la temperatura mediante la función conversionAD(). Si la temperatura supera un umbral determinado, se apaga el circuito calentador; de lo contrario, se mantiene encendido. También se implementa el modo oleaje para las bombas, donde se activan y desactivan en ciclos determinados por el contador de 12 segundos.

### Función conversionAD()
La función conversionAD() se encarga de realizar la conversión analógica a digital (ADC). Primero, se inicializa el registro ADCON para configurar el canal de lectura del ADC. Luego, se inicia la conversión y se espera hasta que

esta finalice. Una vez completada, se obtiene el valor digitalizado y se devuelve.
