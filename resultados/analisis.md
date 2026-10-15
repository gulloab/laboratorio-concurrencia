## Parte 1: Creación básica de hilos

**1. ¿Los mensajes aparecen siempre en el mismo orden?**
No, los mensajes no aparecen en el mismo orden.

**2. ¿Por qué podría cambiar el orden de impresión?**
El sistema operativo utiliza un componente llamado planificador (scheduler) que decide en qué momento exacto cada hilo recibe tiempo de procesamiento. Como los hilos se ejecutan concurrentemente, el orden en el que logran acceder al procesador y a la terminal es completamente impredecible.

**3. ¿Qué función cumple join()?**
Su función es pausar la ejecución del hilo principal (`main`) y obligarlo a "esperar" hasta que el hilo secundario termine su tarea. Garantiza que el programa no avance hasta que el hilo haya concluido.

**4. ¿Qué podría pasar si no se llama a join()?**
Aunque visualmente podrías ver impresiones incompletas, el verdadero problema estructural en C++ es que si la función `main` termina mientras aún hay hilos vivos sin un `join()` (o `detach()`), el programa se cerrará de forma abrupta y lanzará un error del sistema (llamando a `std::terminate`).

## Parte 2: Condición de carrera

**Tabla de ejecuciones**

| Ejecución | Valor esperado | Valor obtenido | 
| 
| 1 | 400000 | 155843 | 
| 2 | 400000 | 170116 | 
| 3 | 400000 | 118848 | 
| 4 | 400000 | 160582 | 
| 5 | 400000 | 168533 | 

**1. ¿El valor obtenido siempre coincide con el valor esperado?**
No, cambia cada vez que se ejecuta el programa y no ha estado ni cerca de llegar al valor esperado.

**2. ¿Por qué se pierden incrementos?**
Se pierden incrementos porque varios hilos pueden leer el mismo número exacto al mismo tiempo (por ejemplo, leen 100). Ambos hilos le suman 1 y ambos guardan el resultado como 101. Se realizaron múltiples operaciones, pero el contador solo registró un incremento debido a que se sobreescribieron los datos entre sí.

**3. ¿La operación contador++ es realmente una sola operación a nivel de CPU?**
No, aunque parezca una sola operación al escribir código, a nivel de procesador se divide en tres pasos separados: 
1) Leer el valor actual de la memoria.
2) Modificarlo (sumarle 1).
3) Guardar el nuevo valor en la memoria. 
Como estos pasos pueden ser interrumpidos por otros hilos, la operación no es atómica.

**4. ¿Qué problema de concurrencia se está observando?**
Se está observando una **condición de carrera** (race condition), la cual ocurre cuando dos o más hilos acceden a un recurso compartido al mismo tiempo y al menos uno intenta modificarlo, generando resultados impredecibles.

## Parte 3: Corrección usando mutex

### Resultados de Ejecución
Al ejecutar el programa 5 veces protegiendo la sección crítica con `std::mutex`, se obtuvieron los siguientes resultados:

| Ejecución | Valor esperado | Valor obtenido |
| :---: | :---: | :---: |
| 1 | 400000 | 400000 |
| 2 | 400000 | 400000 |
| 3 | 400000 | 400000 |
| 4 | 400000 | 400000 |
| 5 | 400000 | 400000 |

### Preguntas de Análisis

1. **¿Qué cambió con respecto al programa anterior?**  
   Se agregó un objeto global `std::mutex` y se envolvió la instrucción `contador++` en un bloqueo utilizando `std::lock_guard`. Esto garantiza exclusión mutua, permitiendo que únicamente un hilo modifique el contador a la vez.

2. **¿Qué hace `std::mutex`?**  
   Es un mecanismo de sincronización (Mutual Exclusion) que actúa como un cerrojo. Permite que solo un hilo adquiera la propiedad del recurso a la vez; si otro hilo intenta adquirir el mutex mientras está bloqueado, este se suspende hasta que quede libre.

3. **¿Qué hace `std::lock_guard`?**  
   Es una envoltura RAII para administrar mutexes. Adquiere el bloqueo del mutex al construirse y lo libera automáticamente en su destructor cuando la variable sale fuera del alcance (scope), evitando bloqueos olvidados o fugas de recursos.

4. **¿Por qué ahora el resultado sí debería ser correcto?**  
   Porque la sección crítica (`contador++`) ahora se ejecuta de forma atómica con respecto a otros hilos. Ningún hilo puede interrumpir la secuencia de lectura-modificación-escritura de otro hilo sobre la variable `contador`.

5. **¿Qué desventaja podría tener proteger cada incremento individual con un mutex?**  
   Introduce una penalización sustancial en el rendimiento debido al overhead (costo extra) constante de adquirir y liberar el cerrojo en cada iteración del bucle, provocando mucha contención de hilos y disminuyendo la ganancia de paralelismo.