# Определяем компилятор и флаги компиляции
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -D_GNU_SOURCE -I./pkg/cJSON -I./internal/http -I./internal/linked_list -I./cmd

# Указываем каталоги с исходными кодами
SRC_DIR = internal/header internal/service internal/util internal/http internal/linked_list cmd

# Указываем целевой исполняемый файл
TARGET = server  # Исполняемый файл находится в корне проекта

# Находим все .c файлы исходя из структуры директорий
SOURCES = $(wildcard internal/*/*.c) \
          cmd/main.c

# Создаем список объектных файлов
OBJECTS = $(SOURCES:.c=.o)  # Замена .c на .o для формирования объектных файлов

# Правило для компиляции целевого исполняемого файла
$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^
	@rm -f $(OBJECTS)

# Правило для компиляции исходных файлов в объектные
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Правило для очистки объектных файлов и исполняемого файла
clean:
	rm -f $(TARGET) $(OBJECTS)

# Правило для очистки всех файлов
.PHONY: all clean

# Назначаем цель по умолчанию
all: $(TARGET)
