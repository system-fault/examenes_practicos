# Control de Temperatura

## Descripción

Este proyecto consiste en un código de control de temperatura implementado en un microcontrolador. El objetivo es mantener una temperatura estable mediante la retroalimentación de un sensor de temperatura y el control de dispositivos de salida.

## Funcionamiento

El código configura el temporizador TR0 en modo de temporizador de 16 bits (`TMOD = 0x01`). A continuación, se detallan los pasos involucrados en esta configuración:

1. Calcula el valor inicial necesario para generar un retardo de tiempo específico utilizando una fórmula que involucra la frecuencia del oscilador y la base de tiempo deseada.
2. Carga los valores iniciales en los registros TL0 y TH0 del temporizador.
3. Habilita las interrupciones generales (`EA`) y las interrupciones del temporizador 0 (`ET0`).
4. Pone los pines P3.2, P3.3, P3.4 y P3.5 en 0 para evitar problemas.
5. Activa el temporizador TR0 para iniciar el conteo.

Una vez configurado el temporizador, el código entra en un bucle infinito donde se verifica el estado de una variable `marcha`. Mientras `marcha` sea igual a 0, el programa se detiene y espera.

Dentro de la función de interrupción del temporizador TR0, se realiza lo siguiente:

1. Se cargan nuevamente los valores iniciales en los registros TL0 y TH0 del temporizador.
2. Se obtienen los valores digitales de la referencia de temperatura y la temperatura mediante la función `conversionAD()`.
3. Se calculan los valores de temperatura y referencia reales utilizando una fórmula de conversión específica.
4. Se calcula el error como la diferencia entre la referencia de temperatura y la temperatura actual.
5. Dependiendo del signo del error, se establece el valor de salida en los pines P3.2, P3.3, P3.4 y P3.5.
6. El contador se reinicia si ha alcanzado un valor específico (200 en este caso), lo que establece un intervalo de tiempo aproximado de 10 segundos.

En resumen, este código configura un temporizador de 16 bits para generar un retardo de tiempo específico. Luego, realiza conversiones analógico-digitales para obtener valores de temperatura y referencia, y controla los pines de salida en función del error de temperatura detectado. El código se ejecuta en un bucle infinito para mantener el control activo.

## Requisitos y Dependencias

El código está diseñado para ser utilizado en un microcontrolador específico (80c552 NXP )que admite interrupciones y tiene los registros y pines mencionados en el código. Es posible que se requieran bibliotecas o configuraciones adicionales según la plataforma de desarrollo utilizada.

## Contribución

Siéntete libre de contribuir a este proyecto mediante la apertura de problemas o solicitudes de extracción. Cualquier aporte es bienvenido.

## Licencia

Este código se distribuye bajo la licencia MIT. Puedes consultar el archivo LICENSE para más detalles.

