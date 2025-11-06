# TC3 - Manejo de Memoria

## Descripción

Este proyecto implementa un simulador de gestión de memoria dinámica en C que permite realizar operaciones de asignación, reasignación y liberación de memoria utilizando los algoritmos **First-fit**, **Best-fit** y **Worst-fit**.

## Características

- ✅ Implementación de operaciones de memoria dinámica (ALLOC, REALLOC, FREE)
- ✅ Tres algoritmos de asignación de memoria:
  - **First-fit**: Asigna el primer bloque libre que puede satisfacer la solicitud
  - **Best-fit**: Asigna el bloque libre más pequeño que puede satisfacer la solicitud
  - **Worst-fit**: Asigna el bloque libre más grande disponible
- ✅ Simulación de fragmentación de memoria
- ✅ Detección de fugas de memoria
- ✅ Visualización del estado de la memoria con el comando PRINT

## Requisitos

- Compilador GCC (o compatible con C11)
- Sistema operativo Linux/Unix/Windows (con MinGW)

## Compilación

### Usando Makefile (recomendado)

```bash
make
```

### Compilación manual

```bash
gcc -Wall -Wextra -std=c11 -g -o memory_manager memory_manager.c
```

## Uso

### Sintaxis

```bash
./memory_manager <archivo_entrada> [algoritmo]
```

### Parámetros

- `<archivo_entrada>`: Ruta al archivo que contiene las operaciones de memoria
- `[algoritmo]`: (Opcional) Algoritmo de asignación a utilizar:
  - `0` = First-fit (por defecto)
  - `1` = Best-fit
  - `2` = Worst-fit

### Ejemplos

```bash
# Usar First-fit (por defecto)
./memory_manager input.txt

# Usar Best-fit
./memory_manager input.txt 1

# Usar Worst-fit
./memory_manager input.txt 2
```

## Formato del Archivo de Entrada

El archivo de entrada debe contener una secuencia de operaciones, una por línea:

- `ALLOC <variable_nombre> <tamaño>`: Asigna un bloque de memoria de `<tamaño>` bytes y lo asocia a `<variable_nombre>`
- `REALLOC <variable_nombre> <nuevo_tamaño>`: Reasigna el bloque de memoria de `<variable_nombre>` a un nuevo tamaño
- `FREE <variable_nombre>`: Libera el bloque de memoria asociado a `<variable_nombre>`
- `PRINT`: Muestra el estado actual de las asignaciones de memoria
- `#`: Líneas que comienzan con `#` son comentarios y serán ignoradas

### Ejemplo de Archivo de Entrada

```
# Simulación de fragmentación
ALLOC A 100
ALLOC B 200
FREE A
ALLOC C 50
PRINT

# Simulación de fuga de memoria
ALLOC D 300
ALLOC E 400
# No liberamos D
FREE E
PRINT
```

## Ejecución de Pruebas

Para ejecutar las pruebas con todos los algoritmos:

```bash
make test
```

## Estructura del Código

- `memory_manager.c`: Código principal del simulador
- `input.txt`: Archivo de entrada de ejemplo
- `Makefile`: Archivo para compilación automatizada
- `README.md`: Esta documentación

## Funcionalidades Implementadas

### 1. Gestión de Memoria Dinámica

El programa solicita un bloque grande de memoria al sistema operativo (10,000 bytes por defecto) y gestiona las asignaciones dentro de ese bloque.

### 2. Algoritmos de Asignación

- **First-fit**: Encuentra el primer bloque libre que pueda satisfacer la solicitud
- **Best-fit**: Encuentra el bloque libre más pequeño que pueda satisfacer la solicitud
- **Worst-fit**: Encuentra el bloque libre más grande disponible

### 3. Operaciones Implementadas

- **ALLOC**: Asigna memoria y llena el bloque con el nombre de la variable
- **REALLOC**: Reasigna memoria, intentando expandir en el lugar cuando es posible
- **FREE**: Libera memoria y fusiona bloques libres adyacentes
- **PRINT**: Muestra el estado completo de la memoria

### 4. Características Adicionales

- Fragmentación de memoria: El programa muestra cómo se fragmenta la memoria cuando se asignan y liberan bloques
- Fugas de memoria: Se puede simular dejando variables sin liberar
- Fusión automática: Los bloques libres adyacentes se fusionan automáticamente

## Limpieza

Para eliminar los archivos compilados:

```bash
make clean
```

## Notas

- El programa inicializa un bloque de memoria de 10,000 bytes
- Las variables pueden tener nombres de hasta 50 caracteres
- Se puede manejar hasta 100 variables simultáneamente
- La memoria se llena con el nombre de la variable para facilitar la visualización

## Autor

Implementado como parte de la tarea TC3 - Manejo de Memoria del curso Principios de Sistemas Operativos.
