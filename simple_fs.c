#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

// Constantes de configuración del sistema de archivos
#define MAX_FILES 100                    // Número máximo de archivos que se pueden crear
#define MAX_FILENAME 64                  // Longitud máxima del nombre de un archivo
#define BLOCK_SIZE 512                   // Tamaño de cada bloque en bytes
#define TOTAL_BLOCKS 2048                // Total de bloques disponibles (2048 * 512 = 1 MB simulado)
#define STORAGE_SIZE (BLOCK_SIZE * TOTAL_BLOCKS)  // Tamaño total del almacenamiento simulado
#define MAX_BLOCKS_PER_FILE TOTAL_BLOCKS // Máximo de bloques que puede usar un archivo

/**
 * Estructura que representa una entrada de archivo en el sistema.
 * 
 * Almacena toda la información de control de un archivo: su nombre, tamaños
 * (asignado vs usado), y los índices de los bloques físicos donde se almacenan
 * sus datos. Cada archivo puede ocupar múltiples bloques no necesariamente
 * contiguos.
 */
typedef struct {
    bool used;                          // Indica si esta entrada está en uso
    char name[MAX_FILENAME];            // Nombre del archivo
    size_t allocated_size;              // Tamaño total asignado al crear el archivo
    size_t used_size;                   // Tamaño real de datos escritos en el archivo
    int blocks[MAX_BLOCKS_PER_FILE];    // Arreglo con los índices de los bloques asignados
    size_t block_count;                 // Número de bloques asignados a este archivo
} FileEntry;

/**
 * Estructura principal que representa el sistema de archivos completo.
 * 
 * Contiene la tabla de archivos (directorio), el almacenamiento simulado
 * (donde se guardan los datos), y el mapa de bloques que indica cuáles
 * bloques están ocupados y cuáles están libres.
 */
typedef struct {
    FileEntry files[MAX_FILES];         // Tabla de archivos (directorio raíz)
    size_t file_count;                  // Número de archivos actualmente en el sistema
    unsigned char storage[STORAGE_SIZE]; // Almacenamiento simulado (1 MB de datos)
    bool block_used[TOTAL_BLOCKS];      // Mapa de bloques: true = ocupado, false = libre
} FileSystem;

/**
 * Inicializa el sistema de archivos simulado.
 * 
 * Esta función prepara el sistema de archivos para su uso, estableciendo todos
 * los contadores en cero y limpiando todas las estructuras de datos. Resetea:
 * - El contador de archivos a 0
 * - Todas las entradas de archivos a valores iniciales
 * - Todo el almacenamiento simulado a ceros
 * - El mapa de bloques usados (todos marcados como libres)
 * 
 * @param fs Puntero al sistema de archivos a inicializar
 */
static void fs_init(FileSystem *fs) {
    fs->file_count = 0;
    memset(fs->files, 0, sizeof(fs->files));
    memset(fs->storage, 0, sizeof(fs->storage));
    memset(fs->block_used, 0, sizeof(fs->block_used));
}

/**
 * Busca un archivo en el sistema por su nombre.
 * 
 * Recorre la tabla de archivos buscando una entrada activa cuyo nombre
 * coincida exactamente con el nombre proporcionado. La búsqueda es
 * case-sensitive y requiere coincidencia exacta.
 * 
 * @param fs Puntero al sistema de archivos donde buscar
 * @param name Nombre del archivo a buscar (debe coincidir exactamente)
 * @return Puntero a la entrada del archivo si se encuentra, NULL si no existe
 */
static FileEntry *fs_find(FileSystem *fs, const char *name) {
    for (size_t i = 0; i < fs->file_count; ++i) {
        if (fs->files[i].used && strcmp(fs->files[i].name, name) == 0) {
            return &fs->files[i];
        }
    }
    return NULL;
}

/**
 * Cuenta el número de bloques libres disponibles en el sistema.
 * 
 * Recorre el mapa de bloques y cuenta cuántos bloques están marcados
 * como no utilizados. Esto permite verificar si hay suficiente espacio
 * antes de crear un nuevo archivo.
 * 
 * @param fs Puntero al sistema de archivos
 * @return Número de bloques libres disponibles (0 a TOTAL_BLOCKS)
 */
static size_t fs_free_block_count(FileSystem *fs) {
    size_t count = 0;
    for (size_t i = 0; i < TOTAL_BLOCKS; ++i) {
        if (!fs->block_used[i]) {
            ++count;
        }
    }
    return count;
}

/**
 * Asigna bloques libres del almacenamiento para un archivo.
 * 
 * Busca bloques libres secuencialmente y los marca como ocupados. Si no
 * se pueden encontrar suficientes bloques, revierte todas las asignaciones
 * realizadas para mantener la consistencia del sistema (operación atómica).
 * 
 * @param fs Puntero al sistema de archivos
 * @param out_blocks Arreglo donde se almacenarán los índices de los bloques asignados
 * @param blocks_needed Número de bloques que se necesitan asignar
 * @return true si se asignaron todos los bloques necesarios, false si no hay suficientes
 */
static bool fs_allocate_blocks(FileSystem *fs, int *out_blocks, size_t blocks_needed) {
    size_t found = 0;
    for (size_t i = 0; i < TOTAL_BLOCKS && found < blocks_needed; ++i) {
        if (!fs->block_used[i]) {
            fs->block_used[i] = true;
            out_blocks[found++] = (int)i;
        }
    }

    if (found < blocks_needed) {
        // Revert allocation
        for (size_t j = 0; j < found; ++j) {
            fs->block_used[out_blocks[j]] = false;
        }
        return false;
    }

    return true;
}

/**
 * Libera los bloques asignados a un archivo y limpia su contenido.
 * 
 * Marca todos los bloques utilizados por el archivo como libres en el mapa
 * de bloques y borra el contenido de esos bloques en el almacenamiento
 * simulado (los llena con ceros). Esto permite que los bloques sean
 * reutilizados por otros archivos.
 * 
 * @param fs Puntero al sistema de archivos
 * @param file Puntero a la entrada del archivo cuyos bloques se liberarán
 */
static void fs_release_blocks(FileSystem *fs, FileEntry *file) {
    for (size_t i = 0; i < file->block_count; ++i) {
        int block_index = file->blocks[i];
        if (block_index >= 0 && block_index < TOTAL_BLOCKS) {
            fs->block_used[block_index] = false;
            memset(&fs->storage[block_index * BLOCK_SIZE], 0, BLOCK_SIZE);
        }
    }
}

/**
 * Crea un nuevo archivo en el sistema de archivos.
 * 
 * Valida que el nombre sea válido, que no exista otro archivo con el mismo
 * nombre, que haya espacio suficiente en bloques, y que no se haya alcanzado
 * el límite máximo de archivos. Si todas las validaciones pasan, asigna los
 * bloques necesarios y crea la entrada del archivo en la tabla.
 * 
 * @param fs Puntero al sistema de archivos
 * @param name Nombre del archivo a crear
 * @param size Tamaño en bytes que se reservará para el archivo
 * @return true si el archivo se creó exitosamente, false en caso de error
 */
static bool cmd_create(FileSystem *fs, const char *name, size_t size) {
    if (fs->file_count >= MAX_FILES) {
        fprintf(stderr, "Error: se alcanzó el número máximo de archivos (%d)\n", MAX_FILES);
        return false;
    }

    if (strlen(name) == 0 || strlen(name) >= MAX_FILENAME) {
        fprintf(stderr, "Error: nombre de archivo inválido\n");
        return false;
    }

    if (fs_find(fs, name)) {
        fprintf(stderr, "Error: el archivo '%s' ya existe\n", name);
        return false;
    }

    size_t blocks_needed = (size + BLOCK_SIZE - 1) / BLOCK_SIZE;
    if (blocks_needed > fs_free_block_count(fs)) {
        fprintf(stderr, "Error: no hay bloques suficientes para crear '%s'\n", name);
        return false;
    }

    FileEntry *entry = &fs->files[fs->file_count];
    memset(entry, 0, sizeof(FileEntry));
    entry->used = true;
    strncpy(entry->name, name, MAX_FILENAME - 1);
    entry->allocated_size = size;
    entry->used_size = 0;
    entry->block_count = blocks_needed;

    if (!fs_allocate_blocks(fs, entry->blocks, blocks_needed)) {
        fprintf(stderr, "Error: no se pudieron asignar bloques para '%s'\n", name);
        memset(entry, 0, sizeof(FileEntry));
        return false;
    }

    if (size > 0) {
        for (size_t i = 0; i < blocks_needed; ++i) {
            size_t block_index = (size_t)entry->blocks[i];
            size_t offset = block_index * BLOCK_SIZE;
            size_t bytes_to_clear = BLOCK_SIZE;
            if (i == blocks_needed - 1) {
                size_t remainder = size % BLOCK_SIZE;
                if (remainder != 0) {
                    bytes_to_clear = remainder;
                }
            }
            memset(&fs->storage[offset], 0, bytes_to_clear);
        }
    }

    fs->file_count++;
    printf("CREATE: archivo '%s' creado (%zu bytes)\n", name, size);
    return true;
}

/**
 * Escribe datos en un archivo comenzando desde un offset específico.
 * 
 * Convierte la posición lógica (offset + índice) en una posición física
 * calculando qué bloque contiene el byte y el desplazamiento dentro de ese
 * bloque. Valida que la escritura no exceda el tamaño asignado del archivo.
 * Actualiza el tamaño usado del archivo si se escriben datos más allá del
 * tamaño usado anteriormente.
 * 
 * @param fs Puntero al sistema de archivos
 * @param file Puntero a la entrada del archivo donde escribir
 * @param offset Posición en bytes desde donde comenzar a escribir
 * @param data Puntero a los datos a escribir
 * @param data_len Número de bytes a escribir
 * @return true si la escritura fue exitosa, false si excede el tamaño del archivo
 */
static bool fs_write_data(FileSystem *fs, FileEntry *file, size_t offset, const unsigned char *data, size_t data_len) {
    if (offset + data_len > file->allocated_size) {
        return false;
    }

    for (size_t i = 0; i < data_len; ++i) {
        size_t logical_pos = offset + i;
        size_t block_index = logical_pos / BLOCK_SIZE;
        size_t block_offset = logical_pos % BLOCK_SIZE;

        if (block_index >= file->block_count) {
            return false;
        }

        int disk_block = file->blocks[block_index];
        size_t storage_index = (size_t)disk_block * BLOCK_SIZE + block_offset;
        fs->storage[storage_index] = data[i];
    }

    size_t new_used = offset + data_len;
    if (new_used > file->used_size) {
        file->used_size = new_used;
    }

    return true;
}

/**
 * Procesa el comando WRITE para escribir datos en un archivo.
 * 
 * Busca el archivo por nombre y valida que exista. Luego escribe los datos
 * proporcionados en el archivo comenzando desde el offset especificado.
 * Muestra un mensaje de confirmación con la cantidad de bytes escritos.
 * 
 * @param fs Puntero al sistema de archivos
 * @param name Nombre del archivo donde escribir
 * @param offset Posición en bytes desde donde comenzar la escritura
 * @param payload Cadena de caracteres con los datos a escribir
 * @return true si la escritura fue exitosa, false si el archivo no existe o hay error
 */
static bool cmd_write(FileSystem *fs, const char *name, size_t offset, const char *payload) {
    FileEntry *file = fs_find(fs, name);
    if (!file) {
        fprintf(stderr, "Error: el archivo '%s' no existe\n", name);
        return false;
    }

    size_t len = strlen(payload);
    if (!fs_write_data(fs, file, offset, (const unsigned char *)payload, len)) {
        fprintf(stderr, "Error: la escritura excede el tamaño del archivo '%s'\n", name);
        return false;
    }

    printf("WRITE: se escribieron %zu bytes en '%s'\n", len, name);
    return true;
}

/**
 * Lee datos de un archivo comenzando desde un offset específico.
 * 
 * Convierte la posición lógica en una posición física calculando qué bloque
 * contiene cada byte y su desplazamiento dentro del bloque. Valida que la
 * lectura no exceda el tamaño usado del archivo (no se puede leer más allá
 * de lo que se ha escrito).
 * 
 * @param fs Puntero al sistema de archivos
 * @param file Puntero a la entrada del archivo del cual leer
 * @param offset Posición en bytes desde donde comenzar a leer
 * @param size Número de bytes a leer
 * @param out_buffer Buffer donde se almacenarán los datos leídos (debe tener al menos 'size' bytes)
 * @return true si la lectura fue exitosa, false si excede el contenido del archivo
 */
static bool fs_read_data(FileSystem *fs, FileEntry *file, size_t offset, size_t size, unsigned char *out_buffer) {
    if (offset + size > file->used_size) {
        return false;
    }

    for (size_t i = 0; i < size; ++i) {
        size_t logical_pos = offset + i;
        size_t block_index = logical_pos / BLOCK_SIZE;
        size_t block_offset = logical_pos % BLOCK_SIZE;

        if (block_index >= file->block_count) {
            return false;
        }

        int disk_block = file->blocks[block_index];
        size_t storage_index = (size_t)disk_block * BLOCK_SIZE + block_offset;
        out_buffer[i] = fs->storage[storage_index];
    }

    return true;
}

/**
 * Procesa el comando READ para leer datos de un archivo.
 * 
 * Busca el archivo por nombre y valida que exista. Lee la cantidad especificada
 * de bytes desde el offset dado y los muestra en la salida estándar como una
 * cadena de caracteres. Si se solicita leer 0 bytes, muestra una cadena vacía.
 * 
 * @param fs Puntero al sistema de archivos
 * @param name Nombre del archivo del cual leer
 * @param offset Posición en bytes desde donde comenzar la lectura
 * @param size Número de bytes a leer
 * @return true si la lectura fue exitosa, false si el archivo no existe o hay error
 */
static bool cmd_read(FileSystem *fs, const char *name, size_t offset, size_t size) {
    FileEntry *file = fs_find(fs, name);
    if (!file) {
        fprintf(stderr, "Error: el archivo '%s' no existe\n", name);
        return false;
    }

    if (size == 0) {
        printf("READ: \"\"\n");
        return true;
    }

    unsigned char *buffer = (unsigned char *)malloc(size + 1);
    if (!buffer) {
        fprintf(stderr, "Error: no se pudo reservar memoria temporal\n");
        return false;
    }

    bool ok = fs_read_data(fs, file, offset, size, buffer);
    if (!ok) {
        fprintf(stderr, "Error: la lectura excede el contenido del archivo '%s'\n", name);
        free(buffer);
        return false;
    }

    buffer[size] = '\0';
    printf("READ: \"%s\"\n", buffer);
    free(buffer);
    return true;
}

/**
 * Elimina un archivo del sistema de archivos.
 * 
 * Busca el archivo por nombre, libera todos los bloques que ocupaba,
 * elimina su entrada de la tabla de archivos (reorganizando las entradas
 * para mantener la tabla compacta), y muestra un mensaje de confirmación.
 * 
 * @param fs Puntero al sistema de archivos
 * @param name Nombre del archivo a eliminar
 * @return true si el archivo se eliminó exitosamente, false si no existe
 */
static bool cmd_delete(FileSystem *fs, const char *name) {
    FileEntry *file = fs_find(fs, name);
    if (!file) {
        fprintf(stderr, "Error: el archivo '%s' no existe\n", name);
        return false;
    }

    fs_release_blocks(fs, file);

    size_t index = (size_t)(file - fs->files);
    for (size_t i = index; i + 1 < fs->file_count; ++i) {
        fs->files[i] = fs->files[i + 1];
    }

    memset(&fs->files[fs->file_count - 1], 0, sizeof(FileEntry));
    fs->file_count--;

    printf("DELETE: archivo '%s' eliminado\n", name);
    return true;
}

/**
 * Lista todos los archivos almacenados en el sistema.
 * 
 * Recorre la tabla de archivos y muestra el nombre y tamaño asignado de cada
 * archivo activo. Si no hay archivos, muestra un mensaje indicando que no hay
 * archivos en el sistema.
 * 
 * @param fs Puntero al sistema de archivos
 */
static void cmd_list(FileSystem *fs) {
    if (fs->file_count == 0) {
        printf("(no hay archivos)\n");
        return;
    }

    for (size_t i = 0; i < fs->file_count; ++i) {
        FileEntry *file = &fs->files[i];
        if (file->used) {
            printf("%s - %zu bytes\n", file->name, file->allocated_size);
        }
    }
}

/**
 * Elimina espacios en blanco al inicio de una cadena.
 * 
 * Avanza el puntero de la cadena hasta encontrar el primer carácter que no
 * sea un espacio en blanco (espacios, tabs, etc.). No modifica la memoria
 * original, solo retorna un puntero a la primera posición no-espacio.
 * 
 * @param s Cadena de caracteres a procesar
 * @return Puntero a la primera posición de la cadena que no es espacio en blanco
 */
static char *ltrim(char *s) {
    while (*s && isspace((unsigned char)*s)) {
        ++s;
    }
    return s;
}

/**
 * Elimina espacios en blanco al final de una cadena.
 * 
 * Recorre la cadena desde el final hacia el inicio, reemplazando con el
 * carácter nulo todos los espacios en blanco encontrados al final. Modifica
 * la cadena original acortándola.
 * 
 * @param s Cadena de caracteres a procesar (se modifica in-place)
 */
static void rtrim(char *s) {
    size_t len = strlen(s);
    while (len > 0 && isspace((unsigned char)s[len - 1])) {
        s[len - 1] = '\0';
        len--;
    }
}

/**
 * Elimina comillas dobles al inicio y final de una cadena.
 * 
 * Si la cadena comienza y termina con comillas dobles, las elimina moviendo
 * el contenido hacia la izquierda y ajustando el terminador nulo. Útil para
 * procesar datos que vienen entre comillas en los comandos.
 * 
 * @param s Cadena de caracteres a procesar (se modifica in-place)
 */
static void strip_quotes(char *s) {
    size_t len = strlen(s);
    if (len >= 2 && s[0] == '"' && s[len - 1] == '"') {
        memmove(s, s + 1, len - 2);
        s[len - 2] = '\0';
    }
}

/**
 * Procesa una línea de comando y ejecuta la operación correspondiente.
 * 
 * Parsea la línea de entrada, identifica el comando (CREATE, WRITE, READ,
 * DELETE, LIST), extrae los parámetros necesarios y llama a la función
 * correspondiente. Ignora líneas vacías y comentarios (que comienzan con #).
 * Maneja el formato de cada comando y valida que tenga los parámetros
 * correctos antes de ejecutarlo.
 * 
 * @param fs Puntero al sistema de archivos
 * @param line Línea de texto con el comando a procesar (se modifica durante el parsing)
 * @return true si el comando se procesó exitosamente, false si hubo un error
 */
static bool process_command(FileSystem *fs, char *line) {
    rtrim(line);
    char *trimmed = ltrim(line);

    if (*trimmed == '\0' || *trimmed == '#') {
        return true;
    }

    char *command = strtok(trimmed, " \t");
    if (!command) {
        return true;
    }

    if (strcmp(command, "CREATE") == 0) {
        char *name = strtok(NULL, " \t");
        char *size_str = strtok(NULL, " \t");
        if (!name || !size_str) {
            fprintf(stderr, "Error: formato de CREATE inválido\n");
            return false;
        }
        size_t size = (size_t)strtoull(size_str, NULL, 10);
        return cmd_create(fs, name, size);
    }

    if (strcmp(command, "WRITE") == 0) {
        char *name = strtok(NULL, " \t");
        char *offset_str = strtok(NULL, " \t");
        char *payload = strtok(NULL, "");
        if (!name || !offset_str || !payload) {
            fprintf(stderr, "Error: formato de WRITE inválido\n");
            return false;
        }

        payload = ltrim(payload);
        rtrim(payload);
        strip_quotes(payload);

        size_t offset = (size_t)strtoull(offset_str, NULL, 10);
        return cmd_write(fs, name, offset, payload);
    }

    if (strcmp(command, "READ") == 0) {
        char *name = strtok(NULL, " \t");
        char *offset_str = strtok(NULL, " \t");
        char *size_str = strtok(NULL, " \t");
        if (!name || !offset_str || !size_str) {
            fprintf(stderr, "Error: formato de READ inválido\n");
            return false;
        }
        size_t offset = (size_t)strtoull(offset_str, NULL, 10);
        size_t size = (size_t)strtoull(size_str, NULL, 10);
        return cmd_read(fs, name, offset, size);
    }

    if (strcmp(command, "DELETE") == 0) {
        char *name = strtok(NULL, " \t");
        if (!name) {
            fprintf(stderr, "Error: formato de DELETE inválido\n");
            return false;
        }
        return cmd_delete(fs, name);
    }

    if (strcmp(command, "LIST") == 0) {
        cmd_list(fs);
        return true;
    }

    fprintf(stderr, "Error: comando desconocido '%s'\n", command);
    return false;
}

/**
 * Función principal del programa.
 * 
 * Inicializa el sistema de archivos simulado y procesa comandos desde la
 * entrada estándar o desde un archivo si se proporciona como argumento.
 * Lee línea por línea hasta el final del archivo o entrada, procesando cada
 * comando. Al finalizar, cierra el archivo si fue abierto.
 * 
 * Uso: simple_fs [archivo_comandos]
 *   - Sin argumentos: lee comandos desde stdin
 *   - Con un argumento: lee comandos desde el archivo especificado
 * 
 * @param argc Número de argumentos de la línea de comandos
 * @param argv Arreglo de argumentos (argv[0] es el nombre del programa)
 * @return EXIT_SUCCESS si todo fue correcto, EXIT_FAILURE en caso de error
 */
int main(int argc, char *argv[]) {
    FileSystem fs;
    fs_init(&fs);

    FILE *input = stdin;
    if (argc == 2) {
        input = fopen(argv[1], "r");
        if (!input) {
            fprintf(stderr, "Error: no se pudo abrir el archivo '%s'\n", argv[1]);
            return EXIT_FAILURE;
        }
    } else if (argc > 2) {
        fprintf(stderr, "Uso: %s [archivo_comandos]\n", argv[0]);
        return EXIT_FAILURE;
    }

    char line[1024];
    while (fgets(line, sizeof(line), input)) {
        process_command(&fs, line);
    }

    if (input != stdin) {
        fclose(input);
    }

    return EXIT_SUCCESS;
}



