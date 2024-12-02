# Nombre del ejecutable
TARGET = p3

# Compilador y opciones
CC = gcc
CFLAGS = -Wall -g

# Archivos fuente
SRCS = p3.c command_handler.c list.c shell_utilities.c

# Regla por defecto
all: $(TARGET)

# Regla para compilar el ejecutable
$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) -o $(TARGET)

# Regla para ejecutar con valgrind
valgrind: $(TARGET)
	valgrind --leak-check=full --show-leak-kinds=all ./$(TARGET)

# Regla para limpiar archivos generados
clean:
	rm -f $(TARGET)

.PHONY: all clean valgrind
