# Semáforo con Control de Temperatura y Botón Peatón

## Explicacion del sistema

Este código implementa un semáforo con control de temperatura y un botón peatón utilizando el microcontrolador 8052 y el lenguaje de programación C. El objetivo es controlar el ciclo del semáforo y adaptarlo según la temperatura medida, además de permitir la intervención manual mediante el botón peatón.

El programa principal se estructura en tres secuencias principales: verde, ámbar y rojo. Cada secuencia se ejecuta durante un tiempo determinado y luego pasa a la siguiente secuencia siguiendo una lógica específica.

El control del tiempo se realiza mediante el uso del temporizador 1 del microcontrolador, configurado en modo 2 (temporizador de 8 bits con recarga automática). Se utiliza una interrupción generada por este temporizador para mantener el conteo del tiempo transcurrido.

La función inicializar() se encarga de realizar la configuración inicial del semáforo y del temporizador. Configura los registros necesarios, como TMOD (modo del temporizador), carga los valores iniciales del temporizador y establece los pines de salida del semáforo en estado apagado.

La función conversionAD(unsigned char canal) realiza la conversión analógico-digital (AD) utilizando el canal especificado como argumento. Esta función configura el ADCON (registro del convertidor analógico-digital) para seleccionar el canal de conversión, inicia la conversión y espera hasta que se complete. Luego, devuelve el resultado de la conversión en un valor entero sin signo.

La función controlPanel() se encarga de controlar el panel en base a la temperatura medida. Cada vez que han transcurrido 10 segundos (40000 unidades de tiempo), se realiza una conversión AD para obtener el valor de temperatura. Luego, se compara ese valor con un umbral de temperatura (en este caso, 15 grados) y se enciende o apaga el panel según corresponda.

En el bucle principal main(), se implementan las tres secuencias del semáforo (verde, ámbar, rojo) utilizando bucles while anidados. Cada secuencia enciende el LED correspondiente, incrementa el contador de tiempo, controla el panel y verifica si se ha pulsado el botón peatón. Al cumplirse ciertas condiciones, se pasa a la siguiente secuencia o se reinicia el ciclo.

El código hace uso de variables globales para controlar el estado de las secuencias, los contadores de tiempo, los valores de temperatura y los resultados de las conversiones AD. También se definen variables locales para almacenar valores temporales durante la ejecución de las funciones.

Además, se utilizan variables de tipo sbit para representar los pines de entrada y salida del microcontrolador. Estas variables se utilizan para controlar los LEDs del semáforo y el botón peatón en el programa principal.

En resumen, este código implementa un semáforo que se adapta a la temperatura medida y permite la intervención manual mediante un botón peatón. La secuencia del semáforo y el control de tiempo se gestionan utilizando interrupciones y variables globales, mientras que las funciones auxiliares se encargan de realizar conversiones AD y controlar el panel según la temperatura.

## Como funciona

El código implementa un sistema de semáforo que controla el cambio de luces de verde, ámbar y rojo, junto con una función de control de temperatura y un botón peatón. Aquí se explica cómo se realiza cada parte del código:

Inicialización:

El sistema se inicializa configurando los registros y variables necesarias.
Se establecen las definiciones de los pines correspondientes a las salidas del semáforo (verde, ámbar, rojo, panel) y la entrada del botón peatón (botoPeaton).
Se declara una serie de variables globales, como verdeIn, ambarIn, rojoIn para indicar el estado actual del semáforo, contador para contar el tiempo transcurrido, contador10seg para contar los 10 segundos, TL_1 y TH_1 para la configuración del temporizador y temperatura para almacenar la temperatura medida.
Función main():

En esta función principal, se realiza el bucle principal del sistema.
Se llama a la función "inicializar()" para configurar el sistema y los periféricos.
A continuación, se ejecuta un bucle while(1) infinito para mantener el funcionamiento continuo del semáforo.
Secuencia Verde:

El bucle while (verdeIn == 1) representa la secuencia de luces verdes.
Se enciende la luz verde (verde = 1).
Se espera hasta que se levante la bandera TF1 del temporizador.
Se baja la bandera TF1.
Se incrementa el contador.
Se aumenta el contador10seg.
Se llama a la función "controlPanel()" para controlar el panel y verificar el botón peatón.
Si se pulsa el botón peatón (botoPeaton == 1) o han pasado 10 segundos (contador == 40000), se cambia a la secuencia de ámbar.
Control del Panel:

La función "controlPanel()" se encarga de controlar el panel que muestra la temperatura.
Se verifica si han pasado 10 segundos (contador10seg >= 40000) desde la última vez que se actualizó el panel.
Se llama a la función "conversionAD()" para realizar una conversión analógica a digital y obtener la temperatura.
Se actualiza la variable "temperatura" con el valor calculado.
Se compara la temperatura con un umbral (15 en este caso).
Se enciende o apaga el panel (panel = 1 o panel = 0) según el valor de la temperatura.
Otras secuencias:

Después de la secuencia de verde, se repite un patrón similar para las secuencias de ámbar y rojo.
En cada secuencia, se enciende la luz correspondiente, se espera el tiempo necesario y se verifica si se debe cambiar a la siguiente secuencia.
Esta es una descripción general del funcionamiento del código del semáforo con control de temperatura y botón peatón. Cada función y sección desempeña un papel importante en el control y la lógica del sistema.
