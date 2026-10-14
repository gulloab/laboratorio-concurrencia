## Parte 1: Creación básica de hilos

**1. ¿Los mensajes aparecen siempre en el mismo orden?**
No, los mensajes no aparecen en el mismo orden.

**2. ¿Por qué podría cambiar el orden de impresión?**
El sistema operativo utiliza un componente llamado planificador (scheduler) que decide en qué momento exacto cada hilo recibe tiempo de procesamiento. Como los hilos se ejecutan concurrentemente, el orden en el que logran acceder al procesador y a la terminal es completamente impredecible.

**3. ¿Qué función cumple join()?**
Su función es pausar la ejecución del hilo principal (`main`) y obligarlo a "esperar" hasta que el hilo secundario termine su tarea. Garantiza que el programa no avance hasta que el hilo haya concluido.

**4. ¿Qué podría pasar si no se llama a join()?**
Aunque visualmente podrías ver impresiones incompletas, el verdadero problema estructural en C++ es que si la función `main` termina mientras aún hay hilos vivos sin un `join()` (o `detach()`), el programa se cerrará de forma abrupta y lanzará un error del sistema (llamando a `std::terminate`).