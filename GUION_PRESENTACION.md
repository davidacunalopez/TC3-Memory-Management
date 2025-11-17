# Guion de Presentación - TC3 Manejo de Memoria
**Duración total: 5 minutos** (2.5 min por persona)

---

## 🎤 PERSONA A (2.5 minutos)

### [0:00 - 0:30] Introducción y Contexto (30 seg)

**Hablar:**
> "Buenos días/tardes. Hoy presentamos nuestro proyecto TC3 - Manejo de Memoria, un simulador que implementa dos componentes fundamentales de los sistemas operativos: la gestión de memoria dinámica y un sistema de archivos simulado."

**Acción:** Mostrar el proyecto en pantalla

---

### [0:30 - 1:30] Gestor de Memoria - Parte 1 (60 seg)

**Hablar:**
> "El primer componente es un gestor de memoria dinámica que simula cómo un sistema operativo asigna y libera memoria. Implementamos tres algoritmos clásicos de asignación:
> 
> - **First-fit**: Asigna el primer bloque libre que satisface la solicitud
> - **Best-fit**: Busca el bloque libre más pequeño que puede satisfacer la solicitud
> - **Worst-fit**: Selecciona el bloque libre más grande disponible
> 
> Cada algoritmo tiene diferentes características en términos de velocidad y fragmentación de memoria."

**Acción:** Mostrar código o diagrama de los algoritmos

---

### [1:30 - 2:00] Gestor de Memoria - Parte 2 (30 seg)

**Hablar:**
> "El gestor soporta las operaciones estándar: ALLOC para asignar memoria, REALLOC para redimensionar bloques, y FREE para liberar memoria. También implementamos características avanzadas como la fusión automática de bloques libres adyacentes y la detección de fugas de memoria al finalizar el programa."

**Acción:** Mostrar ejemplo de ejecución con PRINT

---

### [2:00 - 2:30] Transición (30 seg)

**Hablar:**
> "Ahora, mi compañero les mostrará el segundo componente del proyecto: el sistema de archivos simulado."

**Acción:** Pasar el control a Persona B

---

## 🎤 PERSONA B (2.5 minutos)

### [2:30 - 3:00] Sistema de Archivos - Introducción (30 seg)

**Hablar:**
> "El segundo componente es un sistema de archivos simulado que gestiona archivos en bloques de 512 bytes, simulando un almacenamiento de 1 MB. Este sistema demuestra cómo los sistemas operativos organizan y gestionan el almacenamiento en disco."

**Acción:** Mostrar estructura del sistema de archivos

---

### [3:00 - 4:00] Operaciones del Sistema de Archivos (60 seg)

**Hablar:**
> "Implementamos las operaciones básicas de un sistema de archivos:
> 
> - **CREATE**: Crea archivos y asigna bloques necesarios
> - **WRITE**: Escribe datos desde cualquier offset
> - **READ**: Lee datos desde cualquier posición
> - **DELETE**: Elimina archivos y libera sus bloques
> - **LIST**: Muestra todos los archivos del sistema
> 
> El sistema gestiona automáticamente la asignación de bloques no contiguos, similar a cómo funcionan los sistemas de archivos reales."

**Acción:** Mostrar ejemplo de ejecución con comandos

---

### [4:00 - 4:30] Características Técnicas (30 seg)

**Hablar:**
> "Ambos componentes están implementados en C usando el estándar C11, con validaciones completas de errores y manejo de casos límite. El proyecto incluye documentación completa, archivos de prueba, y un Makefile para facilitar la compilación y ejecución de pruebas."

**Acción:** Mostrar estructura del proyecto o README

---

### [4:30 - 5:00] Conclusión (30 seg)

**Hablar:**
> "Este proyecto demuestra los conceptos fundamentales de gestión de memoria y sistemas de archivos que vimos en el curso. Ambos simuladores funcionan correctamente y están listos para demostración. ¿Hay alguna pregunta?"

**Acción:** Prepararse para preguntas

---

## 📋 NOTAS PARA LOS PRESENTADORES

### Consejos para Persona A:
- Enfócate en explicar los algoritmos de asignación de forma clara
- Muestra un ejemplo visual si es posible (diagrama o ejecución)
- Habla con confianza sobre las diferencias entre los algoritmos

### Consejos para Persona B:
- Enfatiza la similitud con sistemas de archivos reales
- Muestra un ejemplo práctico de uso
- Conecta el concepto con lo aprendido en clase

### Timing:
- **2.5 minutos = 150 segundos exactos**
- Practica con un cronómetro
- Si te adelantas, puedes expandir ejemplos
- Si te atrasas, ve directo a la conclusión

### Material de Apoyo:
- Tener el proyecto compilado y listo para ejecutar
- Tener ejemplos de salida preparados
- Tener el código abierto para mostrar si preguntan

### Puntos Clave a Destacar:
1. ✅ Implementación completa de ambos componentes
2. ✅ Tres algoritmos de asignación funcionando
3. ✅ Manejo de errores y validaciones
4. ✅ Documentación completa
5. ✅ Archivos de prueba incluidos

---

## 🎯 ESTRUCTURA ALTERNATIVA (si prefieren dividir por temas)

### Opción 2: Persona A = Gestor de Memoria, Persona B = Sistema de Archivos

**PERSONA A (2.5 min):**
- [0:00-0:30] Introducción general del proyecto
- [0:30-1:30] Gestor de memoria: algoritmos y operaciones
- [1:30-2:00] Demostración del gestor de memoria
- [2:00-2:30] Transición

**PERSONA B (2.5 min):**
- [2:30-3:00] Introducción al sistema de archivos
- [3:00-4:00] Operaciones y características
- [4:00-4:30] Demostración del sistema de archivos
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

