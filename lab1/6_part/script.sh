#!/bin/bash

# 1. Анализ заголовка бинарного файла (/bin/ls)
BINARY_FILE="/bin/ls"
echo "Анализ заголовка бинарного файла: $BINARY_FILE"
echo "Использование od:"
od -A x -t x1z -N 64 "$BINARY_FILE" | head -n4

echo -e "\nИспользование hexdump:\n"
hexdump -C -n 64 "$BINARY_FILE" | head -n4

# 2. Анализ заголовка текстового файла
TEXT_FILE="text.txt"
echo -e "Создание текстового файла: $TEXT_FILE"
echo "HELLO, TEXT!" > "$TEXT_FILE"

echo "Использование od:"
od -A x -t x1z -N 64 "$TEXT_FILE" | head -n4

echo -e "\nИспользование hexdump:"
hexdump -C -n 64 "$TEXT_FILE" | head -n4

# 3. Анализ символической ссылки
SYMLINK="link_text.txt"
ln -sf "$TEXT_FILE" "$SYMLINK"
echo -e "Анализ символической ссылки: $SYMLINK\n"
echo "Использование od:"
od -A x -t x1z -N 64 "$SYMLINK" | head -n4

echo -e "Использование hexdump:\n"
hexdump -C -n 64 "$SYMLINK" | head -n4

# 4. Анализ содержимого каталога
DIR="."
echo -e "Анализ содержимого каталога: $DIR\n"
ls -l "$DIR"

echo "Использование od:\n"
od -A x -t x1z -N 128 "$DIR" | head -n4

echo -e "Использование hexdump:\n"
hexdump -C -n 128 "$DIR" | head -n4

# 5. Изменение содержимого каталога: создание и удаление файла
TEMP_FILE="$DIR/temp_file.txt"
echo -e "Создание временного файла: $TEMP_FILE"
touch "$TEMP_FILE"
ls -l "$DIR"

echo "Содержимое каталога после создания файла:\n"
od -A x -t x1z -N 128 "$DIR" | head -n4

rm "$TEMP_FILE"
echo -e "Удаление временного файла: $TEMP_FILE"
ls -l "$DIR"

echo "Содержимое каталога после удаления файла:\n"
od -A x -t x1z -N 128 "$DIR" | head -n4

# Очистка: удаление созданных файлов и символической ссылки (с проверкой)
echo -e "Очистка временных файлов"
[[ -f "$TEXT_FILE" ]] && rm "$TEXT_FILE"
[[ -L "$SYMLINK" ]] && rm "$SYMLINK"

echo "Очистка завершена.\n"
