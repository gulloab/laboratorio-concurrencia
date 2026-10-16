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

## Parte 4: Mini benchmark secuencial vs. paralelo

### Resultados de Ejecución
Al ejecutar el programa procesando un vector de 1,000,000,000 de elementos con la optimización `-O2`, se obtuvieron los siguientes tiempos:

| Ejecución | Tiempo secuencial | Tiempo paralelo | ¿Cuál fue más rápido? |
| :---: | :---: | :---: | :---: |
| 1 | 880 ms | 212 ms | Paralelo |
| 2 | 925 ms | 203 ms | Paralelo |
| 3 | 934 ms | 208 ms | Paralelo |

### Preguntas de Análisis

1. **¿El resultado secuencial y el paralelo son iguales?**  
   Sí, ambos arrojan el mismo resultado matemático (`1000000000`) porque la suma es una operación conmutativa y asociativa. El orden en que se sumen los fragmentos no altera el total.

2. **¿La versión paralela siempre fue más rápida?**  
   Sí, al procesar una cantidad tan grande de datos, la versión paralela distribuye la carga entre los diferentes núcleos del procesador, reduciendo el tiempo a aproximadamente una cuarta parte del tiempo secuencial (bajando de ~900 ms a ~200 ms).

3. **¿Por qué dividir el vector en bloques permite paralelizar el trabajo?**  
   Porque las operaciones de suma sobre partes distintas del vector son completamente independientes (*Data Parallelism*). Ningún hilo necesita acceder o modificar las posiciones de memoria que otro hilo está procesando, lo que evita condiciones de carrera sin necesidad de usar candados (`mutex`) que ralenticen la ejecución.

4. **¿Qué costos adicionales tiene la versión paralela?**  
   La versión paralela tiene el costo adicional (*overhead*) de:
   - Solicitar recursos al sistema operativo para instanciar los hilos.
   - El cambio de contexto (*context switching*) manejado por el planificador.
   - La sincronización final para recolectar y sumar los resultados parciales (`join()`).

5. **¿Qué podría pasar si el vector fuera muy pequeño?**  
   Si el vector fuera pequeño (por ejemplo, 100 elementos), el tiempo que tarda el sistema operativo en crear, administrar y destruir los hilos superaría ampliamente el tiempo que tomaría sumar los elementos de forma directa. En ese caso, la versión paralela terminaría siendo más lenta que la secuencial.

## Parte 5: Cantidad de hilos vs. rendimiento

### Resultados de Ejecución

| Cantidad de hilos | Tiempo obtenido |
| :---: | :---: |
| 1 | 37 ms |
| 2 | 21 ms |
| 4 | 19 ms |
| 8 | 20 ms |
| 16 | 19 ms |

**Conclusión breve:** 
Al aumentar la cantidad de hilos, el tiempo de ejecución disminuye drásticamente al principio (de 37 ms a 19 ms), pero llega a un punto de estancamiento al alcanzar los 4 hilos. Esto demuestra que agregar más hilos no mejora el rendimiento cuando se excede la capacidad física de procesamiento concurrente del sistema.

### Preguntas de Análisis

1. **¿Cuál cantidad de hilos produjo el mejor tiempo?**  
   El mejor tiempo (19 ms) se obtuvo utilizando 4 hilos (el mismo tiempo se repitió con 16 hilos, pero implicando un mayor costo administrativo para el sistema operativo).

2. **¿El tiempo mejoró siempre al aumentar los hilos?**  
   No. El tiempo mejoró claramente al pasar de 1 a 2 y a 4 hilos, pero al subir a 8 hilos el tiempo empeoró ligeramente (20 ms), y con 16 hilos se estancó, comprobando que más hilos no significa necesariamente mayor velocidad.

3. **¿Cuántos núcleos tiene la computadora donde se ejecutó el programa?**  
   *(Nota: Por el comportamiento de tus tiempos, es muy probable que tu procesador tenga 4 núcleos, pero ajusta este número si encontraste un valor distinto en tu consola)*. Mi computadora tiene 4 núcleos.

4. **¿Qué ocurre cuando se usan más hilos que núcleos disponibles?**  
   Los núcleos no pueden ejecutar todos los hilos simultáneamente de forma verdaderamente paralela. El sistema operativo se ve obligado a pausar y alternar la ejecución de los hilos en los mismos núcleos para dar la ilusión de concurrencia.

5. **¿Qué relación tiene esto con el *context switching*?**  
   El *context switching* (cambio de contexto) es el proceso mediante el cual la CPU guarda el estado del hilo actual y carga el estado del siguiente. Cuando hay más hilos que núcleos (como en tu prueba de 8 y 16 hilos), la CPU pasa tiempo haciendo *context switching* en lugar de ejecutar la suma real, impidiendo que el tiempo siga bajando.

6. **¿Por qué la versión con 16 hilos podría no ser la mejor?**  
   Porque a pesar de igualar el tiempo de 19 ms, el sistema gastó energía y recursos adicionales en crear, administrar y realizar constantes cambios de contexto entre 16 hilos distintos, sobrepasando el beneficio matemático de dividir la tarea.

## Parte 6: Ejemplo simple de deadlock

### Preguntas de Análisis

1. **¿El programa termina normalmente?**  
   No. El programa original se queda bloqueado indefinidamente y es necesario forzar su terminación manualmente (con `Ctrl + C`).

2. **¿Qué recurso tomó primero el hilo A?**  
   El hilo A tomó primero el `recurso1`.

3. **¿Qué recurso tomó primero el hilo B?**  
   El hilo B tomó primero el `recurso2`.

4. **¿Por qué ninguno de los dos hilos puede continuar?**  
   Porque ocurre un interbloqueo (*deadlock*). El hilo A necesita el `recurso2` para avanzar, pero está bloqueado por el hilo B. Simultáneamente, el hilo B necesita el `recurso1` para avanzar, pero está bloqueado por el hilo A. Ninguno suelta lo que ya tiene, por lo que ambos se quedan esperando para siempre.

5. **¿Qué significa espera circular?**  
   Es una condición fundamental para que ocurra un deadlock. Significa que existe una cadena cerrada de hilos (A y B) donde cada hilo está esperando un recurso que retiene el siguiente hilo en la cadena (A espera a B, y B espera a A).

6. **¿Cómo se podría evitar este problema?**  
   Existen varias estrategias, pero las más comunes son:
   - Forzar un orden jerárquico estricto de adquisición (ej. todos los hilos deben pedir SIEMPRE el `recurso1` primero y el `recurso2` después).
   - Utilizar mecanismos que aseguren la adquisición de todos los bloqueos al mismo tiempo de forma atómica, como la función `std::lock` o la clase `std::scoped_lock` en C++ moderno.

### Corrección del Deadlock

**Explique por qué `std::scoped_lock` ayuda a evitar este deadlock:**  
`std::scoped_lock` evita el interbloqueo porque está diseñado para bloquear múltiples mutexes de forma atómica y segura mediante un algoritmo interno que previene los bloqueos cruzados (usualmente ordenándolos por sus direcciones de memoria internas). Si no puede adquirir ambos bloqueos de manera segura de una vez, se encarga de ceder los recursos y reintentar, garantizando que nunca se produzca la condición de espera circular.