Exactamente, cuando el bucle while contiene código dentro, el compilador ejecutará ese código antes de evaluar nuevamente la condición para determinar si debe continuar o salir del bucle. En ese caso, la verificación de la condición ocurre al final de cada iteración del bucle.

Por otro lado, cuando el bucle while está vacío, el compilador simplemente comprueba la condición y, si se cumple, pasa inmediatamente a la siguiente instrucción fuera del bucle. No hay código adicional que se ejecute dentro del bucle antes de realizar la verificación de la condición nuevamente.

Es por eso que, en el código que compartiste, cuando el bucle while está vacío, puede haber una mayor probabilidad de que la interrupción se active rápidamente, ya que no hay ningún código que ralentice la ejecución o proporcione un tiempo de espera dentro del bucle.






