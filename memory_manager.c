#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_VARIABLES 100
#define MAX_NAME_LENGTH 50
#define MEMORY_SIZE 10000  // Tamaño del bloque de memoria principal

// Estructura para representar un bloque de memoria asignado
typedef struct MemoryBlock {
    char variable_name[MAX_NAME_LENGTH];
    void* address;
    size_t size;
    bool is_free;
    struct MemoryBlock* next;
} MemoryBlock;

// Estructura para gestionar las variables
typedef struct Variable {
    char name[MAX_NAME_LENGTH];
    void* address;
    size_t size;
} Variable;

// Estructura principal del gestor de memoria
typedef struct MemoryManager {
    void* memory_pool;           // Bloque grande de memoria solicitado al SO
    size_t pool_size;             // Tamaño del bloque
    MemoryBlock* blocks;          // Lista de bloques asignados
    Variable* variables;          // Tabla de variables
    int variable_count;           // Número de variables activas
    int allocation_algorithm;     // 0: First-fit, 1: Best-fit, 2: Worst-fit
} MemoryManager;

// Funciones de inicialización
MemoryManager* init_memory_manager(size_t pool_size, int algorithm) {
    MemoryManager* mm = (MemoryManager*)malloc(sizeof(MemoryManager));
    if (!mm) {
        fprintf(stderr, "Error: No se pudo asignar memoria para el gestor\n");
        return NULL;
    }
    
    mm->memory_pool = malloc(pool_size);
    if (!mm->memory_pool) {
        fprintf(stderr, "Error: No se pudo asignar el bloque de memoria principal\n");
        free(mm);
        return NULL;
    }
    
    mm->pool_size = pool_size;
    mm->blocks = NULL;
    mm->variables = (Variable*)calloc(MAX_VARIABLES, sizeof(Variable));
    mm->variable_count = 0;
    mm->allocation_algorithm = algorithm;
    
    // Inicializar el bloque libre principal
    MemoryBlock* free_block = (MemoryBlock*)malloc(sizeof(MemoryBlock));
    strcpy(free_block->variable_name, "");
    free_block->address = mm->memory_pool;
    free_block->size = pool_size;
    free_block->is_free = true;
    free_block->next = NULL;
    mm->blocks = free_block;
    
    return mm;
}

void destroy_memory_manager(MemoryManager* mm) {
    if (!mm) return;
    
    // Liberar todos los bloques
    MemoryBlock* current = mm->blocks;
    while (current) {
        MemoryBlock* next = current->next;
        free(current);
        current = next;
    }
    
    free(mm->variables);
    free(mm->memory_pool);
    free(mm);
}

// Función para encontrar una variable
Variable* find_variable(MemoryManager* mm, const char* name) {
    for (int i = 0; i < mm->variable_count; i++) {
        if (strcmp(mm->variables[i].name, name) == 0) {
            return &mm->variables[i];
        }
    }
    return NULL;
}

// Función para encontrar un bloque libre que pueda satisfacer el tamaño requerido
MemoryBlock* find_free_block(MemoryManager* mm, size_t size) {
    MemoryBlock* current = mm->blocks;
    while (current) {
        if (current->is_free && current->size >= size) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

// Algoritmo First-fit: encuentra el primer bloque libre que pueda satisfacer la solicitud
MemoryBlock* first_fit(MemoryManager* mm, size_t size) {
    return find_free_block(mm, size);
}

// Algoritmo Best-fit: encuentra el bloque libre más pequeño que pueda satisfacer la solicitud
MemoryBlock* best_fit(MemoryManager* mm, size_t size) {
    MemoryBlock* best = NULL;
    MemoryBlock* current = mm->blocks;
    
    while (current) {
        if (current->is_free && current->size >= size) {
            if (!best || current->size < best->size) {
                best = current;
            }
        }
        current = current->next;
    }
    
    return best;
}

// Algoritmo Worst-fit: encuentra el bloque libre más grande
MemoryBlock* worst_fit(MemoryManager* mm, size_t size) {
    MemoryBlock* worst = NULL;
    MemoryBlock* current = mm->blocks;
    
    while (current) {
        if (current->is_free && current->size >= size) {
            if (!worst || current->size > worst->size) {
                worst = current;
            }
        }
        current = current->next;
    }
    
    return worst;
}

// Función para seleccionar el algoritmo según la configuración
MemoryBlock* select_block(MemoryManager* mm, size_t size) {
    switch (mm->allocation_algorithm) {
        case 0: return first_fit(mm, size);
        case 1: return best_fit(mm, size);
        case 2: return worst_fit(mm, size);
        default: return first_fit(mm, size);
    }
}

// Función para dividir un bloque si es necesario
void split_block(MemoryBlock* block, size_t size) {
    if (block->size > size) {
        MemoryBlock* new_block = (MemoryBlock*)malloc(sizeof(MemoryBlock));
        new_block->address = (char*)block->address + size;
        new_block->size = block->size - size;
        new_block->is_free = true;
        strcpy(new_block->variable_name, "");
        new_block->next = block->next;
        block->next = new_block;
        block->size = size;
    }
}

// Función para fusionar bloques libres adyacentes
void merge_free_blocks(MemoryManager* mm) {
    MemoryBlock* current = mm->blocks;
    while (current && current->next) {
        if (current->is_free && current->next->is_free) {
            // Verificar si son adyacentes
            void* end_current = (char*)current->address + current->size;
            if (end_current == current->next->address) {
                // Fusionar bloques
                current->size += current->next->size;
                MemoryBlock* to_remove = current->next;
                current->next = to_remove->next;
                free(to_remove);
            } else {
                current = current->next;
            }
        } else {
            current = current->next;
        }
    }
}

// Función ALLOC
bool alloc_memory(MemoryManager* mm, const char* var_name, size_t size) {
    // Verificar si la variable ya existe
    if (find_variable(mm, var_name)) {
        fprintf(stderr, "Error: La variable '%s' ya existe\n", var_name);
        return false;
    }
    
    // Seleccionar bloque según el algoritmo
    MemoryBlock* block = select_block(mm, size);
    if (!block) {
        fprintf(stderr, "Error: No hay suficiente memoria para asignar %zu bytes a '%s'\n", size, var_name);
        return false;
    }
    
    // Asignar el bloque
    block->is_free = false;
    strcpy(block->variable_name, var_name);
    
    // Dividir el bloque si es necesario
    split_block(block, size);
    
    // Agregar a la tabla de variables
    if (mm->variable_count >= MAX_VARIABLES) {
        fprintf(stderr, "Error: Se alcanzó el límite de variables\n");
        return false;
    }
    
    Variable* var = &mm->variables[mm->variable_count];
    strcpy(var->name, var_name);
    var->address = block->address;
    var->size = size;
    mm->variable_count++;
    
    // Llenar toda la memoria con el nombre de la variable (repetido)
    size_t name_len = strlen(var_name);
    if (name_len > 0) {
        for (size_t i = 0; i < size; i++) {
            ((char*)block->address)[i] = var_name[i % name_len];
        }
    } else {
        memset(block->address, 0, size);
    }
    
    printf("ALLOC: Variable '%s' asignada con %zu bytes\n", var_name, size);
    return true;
}

// Función REALLOC
bool realloc_memory(MemoryManager* mm, const char* var_name, size_t new_size) {
    Variable* var = find_variable(mm, var_name);
    if (!var) {
        fprintf(stderr, "Error: La variable '%s' no existe\n", var_name);
        return false;
    }
    
    // Buscar el bloque asociado
    MemoryBlock* block = mm->blocks;
    while (block) {
        if (block->address == var->address && !block->is_free) {
            break;
        }
        block = block->next;
    }
    
    if (!block) {
        fprintf(stderr, "Error: No se encontró el bloque para '%s'\n", var_name);
        return false;
    }
    
    size_t old_size = var->size;
    
    if (new_size <= old_size) {
        // Reducir el tamaño
        if (block->size > new_size) {
            // Crear un nuevo bloque libre con el espacio sobrante
            MemoryBlock* free_block = (MemoryBlock*)malloc(sizeof(MemoryBlock));
            free_block->address = (char*)block->address + new_size;
            free_block->size = block->size - new_size;
            free_block->is_free = true;
            strcpy(free_block->variable_name, "");
            free_block->next = block->next;
            block->next = free_block;
            block->size = new_size;
            merge_free_blocks(mm);
        }
        var->size = new_size;
        printf("REALLOC: Variable '%s' redimensionada de %zu a %zu bytes\n", var_name, old_size, new_size);
        return true;
    } else {
        // Intentar expandir el bloque
        // Verificar si hay espacio libre después del bloque
        MemoryBlock* next = block->next;
        size_t available = block->size;
        
        if (next && next->is_free) {
            void* end_current = (char*)block->address + block->size;
            if (end_current == next->address) {
                available += next->size;
            }
        }
        
        if (available >= new_size) {
            // Expandir en el lugar
            if (next && next->is_free) {
                void* end_current = (char*)block->address + block->size;
                if (end_current == next->address) {
                    size_t needed = new_size - block->size;
                    if (needed <= next->size) {
                        block->size = new_size;
                        next->size -= needed;
                        if (next->size == 0) {
                            MemoryBlock* to_remove = next;
                            block->next = next->next;
                            free(to_remove);
                        } else {
                            next->address = (char*)next->address + needed;
                        }
                        var->size = new_size;
                        // Llenar toda la nueva memoria con el nombre (repetido)
                        size_t name_len = strlen(var_name);
                        if (name_len > 0) {
                            for (size_t i = old_size; i < new_size; i++) {
                                ((char*)block->address)[i] = var_name[i % name_len];
                            }
                        } else {
                            memset((char*)block->address + old_size, 0, new_size - old_size);
                        }
                        printf("REALLOC: Variable '%s' expandida de %zu a %zu bytes\n", var_name, old_size, new_size);
                        return true;
                    }
                }
            }
        }
        
        // No se puede expandir en el lugar, intentar reasignar
        // Liberar el bloque actual
        block->is_free = true;
        strcpy(block->variable_name, "");
        merge_free_blocks(mm);
        
        // Intentar asignar uno nuevo
        MemoryBlock* new_block = select_block(mm, new_size);
        if (!new_block) {
            fprintf(stderr, "Error: No hay suficiente memoria para expandir '%s'\n", var_name);
            // Intentar restaurar
            block->is_free = false;
            strcpy(block->variable_name, var_name);
            return false;
        }
        
        // Copiar datos
        void* old_addr = var->address;
        size_t copy_size = old_size < new_size ? old_size : new_size;
        memcpy(new_block->address, old_addr, copy_size);
        
        // Asignar nuevo bloque
        new_block->is_free = false;
        strcpy(new_block->variable_name, var_name);
        split_block(new_block, new_size);
        
        // Actualizar variable
        var->address = new_block->address;
        var->size = new_size;
        
        // Llenar toda la nueva memoria con el nombre (repetido)
        size_t name_len2 = strlen(var_name);
        if (name_len2 > 0) {
            for (size_t i = copy_size; i < new_size; i++) {
                ((char*)new_block->address)[i] = var_name[i % name_len2];
            }
        } else {
            memset((char*)new_block->address + copy_size, 0, new_size - copy_size);
        }
        
        printf("REALLOC: Variable '%s' reasignada de %zu a %zu bytes\n", var_name, old_size, new_size);
        return true;
    }
}

// Función FREE
bool free_memory(MemoryManager* mm, const char* var_name) {
    Variable* var = find_variable(mm, var_name);
    if (!var) {
        fprintf(stderr, "Error: La variable '%s' no existe\n", var_name);
        return false;
    }
    
    // Buscar el bloque asociado
    MemoryBlock* block = mm->blocks;
    while (block) {
        if (block->address == var->address && !block->is_free) {
            break;
        }
        block = block->next;
    }
    
    if (!block) {
        fprintf(stderr, "Error: No se encontró el bloque para '%s'\n", var_name);
        return false;
    }
    
    // Liberar el bloque
    block->is_free = true;
    strcpy(block->variable_name, "");
    
    // Fusionar bloques libres adyacentes
    merge_free_blocks(mm);
    
    // Eliminar de la tabla de variables
    for (int i = 0; i < mm->variable_count; i++) {
        if (strcmp(mm->variables[i].name, var_name) == 0) {
            // Mover las variables restantes
            for (int j = i; j < mm->variable_count - 1; j++) {
                mm->variables[j] = mm->variables[j + 1];
            }
            mm->variable_count--;
            break;
        }
    }
    
    printf("FREE: Variable '%s' liberada\n", var_name);
    return true;
}

// Función PRINT
void print_memory_state(MemoryManager* mm) {
    printf("\n=== Estado de la Memoria ===\n");
    printf("Variables activas: %d\n", mm->variable_count);
    printf("\nVariables asignadas:\n");
    for (int i = 0; i < mm->variable_count; i++) {
        printf("  - %s: %zu bytes en dirección %p\n", 
               mm->variables[i].name, 
               mm->variables[i].size, 
               mm->variables[i].address);
    }
    
    printf("\nBloques de memoria:\n");
    MemoryBlock* current = mm->blocks;
    int block_num = 1;
    while (current) {
        printf("  Bloque %d: %s [%zu bytes] en %p - %s\n",
               block_num++,
               current->is_free ? "LIBRE" : current->variable_name,
               current->size,
               current->address,
               current->is_free ? "(libre)" : "(ocupado)");
        current = current->next;
    }
    
    // Calcular estadísticas
    size_t total_free = 0;
    size_t total_used = 0;
    int free_blocks = 0;
    int used_blocks = 0;
    
    current = mm->blocks;
    while (current) {
        if (current->is_free) {
            total_free += current->size;
            free_blocks++;
        } else {
            total_used += current->size;
            used_blocks++;
        }
        current = current->next;
    }
    
    printf("\nEstadísticas:\n");
    printf("  Memoria total: %zu bytes\n", mm->pool_size);
    printf("  Memoria libre: %zu bytes (%d bloques)\n", total_free, free_blocks);
    printf("  Memoria usada: %zu bytes (%d bloques)\n", total_used, used_blocks);
    printf("  Fragmentación: %d bloques libres\n", free_blocks);
    printf("===========================\n\n");
}

// Función para procesar una línea del archivo
bool process_line(MemoryManager* mm, char* line) {
    // Eliminar salto de línea
    line[strcspn(line, "\r\n")] = 0;
    
    // Ignorar líneas vacías y comentarios
    if (line[0] == '\0' || line[0] == '#') {
        return true;
    }
    
    // Eliminar espacios al inicio
    while (*line == ' ' || *line == '\t') {
        line++;
    }
    
    char command[20];
    char var_name[MAX_NAME_LENGTH];
    size_t size;
    
    if (sscanf(line, "%s", command) != 1) {
        return true;
    }
    
    if (strcmp(command, "ALLOC") == 0) {
        if (sscanf(line, "%s %s %zu", command, var_name, &size) == 3) {
            return alloc_memory(mm, var_name, size);
        } else {
            fprintf(stderr, "Error: Formato incorrecto para ALLOC\n");
            return false;
        }
    } else if (strcmp(command, "REALLOC") == 0) {
        if (sscanf(line, "%s %s %zu", command, var_name, &size) == 3) {
            return realloc_memory(mm, var_name, size);
        } else {
            fprintf(stderr, "Error: Formato incorrecto para REALLOC\n");
            return false;
        }
    } else if (strcmp(command, "FREE") == 0) {
        if (sscanf(line, "%s %s", command, var_name) == 2) {
            return free_memory(mm, var_name);
        } else {
            fprintf(stderr, "Error: Formato incorrecto para FREE\n");
            return false;
        }
    } else if (strcmp(command, "PRINT") == 0) {
        print_memory_state(mm);
        return true;
    } else {
        fprintf(stderr, "Error: Comando desconocido '%s'\n", command);
        return false;
    }
}

// Función principal
int main(int argc, char* argv[]) {
    if (argc < 2 || argc > 3) {
        fprintf(stderr, "Uso: %s <archivo_entrada> [algoritmo]\n", argv[0]);
        fprintf(stderr, "Algoritmos: 0=First-fit, 1=Best-fit, 2=Worst-fit\n");
        fprintf(stderr, "Por defecto se usa First-fit\n");
        return 1;
    }
    
    int algorithm = 0; // Por defecto First-fit
    if (argc == 3) {
        algorithm = atoi(argv[2]);
        if (algorithm < 0 || algorithm > 2) {
            fprintf(stderr, "Error: Algoritmo inválido. Use 0, 1 o 2\n");
            return 1;
        }
    }
    
    const char* algorithm_names[] = {"First-fit", "Best-fit", "Worst-fit"};
    printf("Algoritmo seleccionado: %s\n\n", algorithm_names[algorithm]);
    
    // Inicializar el gestor de memoria
    MemoryManager* mm = init_memory_manager(MEMORY_SIZE, algorithm);
    if (!mm) {
        return 1;
    }
    
    // Abrir el archivo de entrada
    FILE* file = fopen(argv[1], "r");
    if (!file) {
        fprintf(stderr, "Error: No se pudo abrir el archivo '%s'\n", argv[1]);
        destroy_memory_manager(mm);
        return 1;
    }
    
    // Procesar el archivo línea por línea
    char line[256];
    int line_num = 0;
    while (fgets(line, sizeof(line), file)) {
        line_num++;
        if (!process_line(mm, line)) {
            fprintf(stderr, "Error en la línea %d\n", line_num);
        }
    }
    
    fclose(file);
    destroy_memory_manager(mm);
    
    return 0;
}

