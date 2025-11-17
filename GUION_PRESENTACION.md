# Guion de Presentación - TC3 Manejo de Memoria
**Duración total: 5 minutos** (2.5 min por persona)

---

## 🎤 PERSONA A (2.5 minutos)

### [0:00 - 0:30] Introducción y Contexto (30 seg)

**Hablar:**
> "Buenos días/tardes. Hoy presentamos nuestro proyecto TC3 - Manejo de Memoria, un simulador de gestión de memoria dinámica que implementa los algoritmos clásicos de asignación de memoria que estudiamos en el curso."

**Acción:** Mostrar el proyecto en pantalla

---

### [0:30 - 1:30] Algoritmos de Asignación (60 seg)

**Hablar:**
> "El proyecto implementa tres algoritmos fundamentales de asignación de memoria:
> 
> - **First-fit**: Asigna el primer bloque libre que puede satisfacer la solicitud. Es rápido pero puede generar más fragmentación.
> 
> - **Best-fit**: Busca el bloque libre más pequeño que puede satisfacer la solicitud. Minimiza el desperdicio pero requiere recorrer toda la lista.
> 
> - **Worst-fit**: Selecciona el bloque libre más grande disponible. Deja bloques grandes libres pero puede generar más fragmentación.
> 
> Cada algoritmo tiene diferentes características en términos de velocidad y eficiencia de uso de memoria."

**Acción:** Mostrar código o diagrama de los algoritmos

---

### [1:30 - 2:00] Operaciones Implementadas (30 seg)

**Hablar:**
> "El gestor soporta las operaciones estándar: ALLOC para asignar memoria, REALLOC para redimensionar bloques, y FREE para liberar memoria. Una característica importante es que tanto en ALLOC como en REALLOC, toda la memoria asignada se rellena con el nombre de la variable, como se especifica en los requisitos."

**Acción:** Mostrar ejemplo de código o ejecución

---

### [2:00 - 2:30] Transición (30 seg)

**Hablar:**
> "Ahora, mi compañero les mostrará las características avanzadas y la demostración práctica del simulador."

**Acción:** Pasar el control a Persona B

---

## 🎤 PERSONA B (2.5 minutos)

### [2:30 - 3:00] Características Avanzadas (30 seg)

**Hablar:**
> "Además de los algoritmos básicos, implementamos características avanzadas como la fusión automática de bloques libres adyacentes para reducir la fragmentación, y la detección de fugas de memoria que reporta variables no liberadas al finalizar el programa."

**Acción:** Mostrar código de fusión de bloques

---

### [3:00 - 4:00] Demostración Práctica (60 seg)

**Hablar:**
> "Vamos a ver una demostración práctica. El programa lee comandos desde un archivo de entrada con el formato especificado: ALLOC, REALLOC, FREE y PRINT. El comando PRINT muestra el estado completo de la memoria, incluyendo variables activas, bloques libres y ocupados, y estadísticas de fragmentación.
> 
> Podemos ejecutar el mismo archivo de entrada con diferentes algoritmos para comparar cómo cada uno gestiona la memoria y la fragmentación resultante."

**Acción:** Ejecutar el programa con `input.txt` y mostrar la salida con PRINT

---

### [4:00 - 4:30] Aspectos Técnicos y Proyecto (30 seg)

**Hablar:**
> "El proyecto está implementado en C usando el estándar C11, con validaciones completas de errores y manejo de casos límite. Incluye documentación completa, archivos de prueba, y un Makefile para facilitar la compilación y ejecución de pruebas con los diferentes algoritmos."

**Acción:** Mostrar estructura del proyecto o README

---

### [4:30 - 5:00] Conclusión (30 seg)

**Hablar:**
> "Este proyecto demuestra los conceptos fundamentales de gestión de memoria dinámica que vimos en el curso, permitiendo comparar el comportamiento de diferentes algoritmos de asignación. El simulador funciona correctamente y está listo para demostración. ¿Hay alguna pregunta?"

**Acción:** Prepararse para preguntas

---

## 📋 NOTAS PARA LOS PRESENTADORES

### Consejos para Persona A:
- Enfócate en explicar los algoritmos de asignación de forma clara
- Destaca las diferencias entre First-fit, Best-fit y Worst-fit
- Muestra un diagrama o código si es posible
- Habla con confianza sobre las características de cada algoritmo

### Consejos para Persona B:
- Enfatiza las características avanzadas (fusión de bloques, detección de fugas)
- Muestra una demostración práctica en vivo si es posible
- Explica cómo el comando PRINT ayuda a visualizar el estado de la memoria
- Conecta el concepto con lo aprendido en clase sobre fragmentación

### Timing:
- **2.5 minutos = 150 segundos exactos**
- Practica con un cronómetro
- Si te adelantas, puedes expandir ejemplos
- Si te atrasas, ve directo a la conclusión

### Material de Apoyo:
- Tener el proyecto compilado y listo para ejecutar
- Tener `input.txt` y `test_memory.txt` listos
- Tener ejemplos de salida con PRINT preparados
- Tener el código abierto para mostrar los algoritmos si preguntan
- Preparar comparación visual de los 3 algoritmos si es posible

### Puntos Clave a Destacar:
1. ✅ Implementación completa del gestor de memoria
2. ✅ Tres algoritmos de asignación funcionando (First-fit, Best-fit, Worst-fit)
3. ✅ Operaciones: ALLOC, REALLOC, FREE, PRINT
4. ✅ Rellenado de memoria con el nombre de la variable (requisito)
5. ✅ Características avanzadas: fusión de bloques, detección de fugas
6. ✅ Manejo de errores y validaciones
7. ✅ Documentación completa
8. ✅ Archivos de prueba incluidos

---

## 🎯 ESTRUCTURA ALTERNATIVA (si prefieren dividir por temas)

### Opción 2: Persona A = Teoría y Algoritmos, Persona B = Implementación y Demostración

**PERSONA A (2.5 min):**
- [0:00-0:30] Introducción general del proyecto
- [0:30-1:30] Explicación de los tres algoritmos (First-fit, Best-fit, Worst-fit)
- [1:30-2:00] Operaciones básicas (ALLOC, REALLOC, FREE)
- [2:00-2:30] Transición

**PERSONA B (2.5 min):**
- [2:30-3:00] Características avanzadas (fusión, detección de fugas)
- [3:00-4:00] Demostración práctica con ejemplos
- [4:00-4:30] Aspectos técnicos y estructura del proyecto
- [4:30-5:00] Conclusión y preguntas

---

## 📝 CHECKLIST PRE-PRESENTACIÓN

- [ ] Proyecto compilado y funcionando
- [ ] Archivos de prueba listos
- [ ] Ejemplos de salida preparados
- [ ] Cronómetro configurado
- [ ] Guion practicado al menos 2 veces
- [ ] Transición entre presentadores ensayada
- [ ] Respuestas a preguntas comunes preparadas

---

**¡Buena suerte con la presentación! 🚀**

