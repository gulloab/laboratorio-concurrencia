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