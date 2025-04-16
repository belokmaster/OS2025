#!/bin/bash
# Скрипт для исследования изменения размера каталога

TEST_DIR="exp_dir"
echo "Создание каталога $TEST_DIR..."
mkdir -p "$TEST_DIR"

# Функция для вывода размера каталога
print_size() {
    echo "stat: $(stat -c %s "$TEST_DIR")"
    echo "du -sh:"; du -sh "$TEST_DIR"
}

echo -e "\nНачальный размер каталога $TEST_DIR:"
print_size

# Создание пустого файла
touch "$TEST_DIR/file_empty"
echo -e "\nПосле создания пустого файла file_empty:"
print_size

# Создание файла с содержимым
echo "Пример содержимого" > "$TEST_DIR/file_content.txt"
echo -e "\nПосле создания файла file_content.txt с содержимым:"
print_size

# Создание подкаталога
mkdir "$TEST_DIR/subdir"
echo -e "\nПосле создания подкаталога subdir:"
print_size

# Создание символической ссылки
ln -s file_content.txt "$TEST_DIR/symlink_file.txt"
echo -e "\nПосле создания символической ссылки symlink_file.txt:"
print_size

# Удаление объектов
rm -f "$TEST_DIR/file_empty"
echo -e "\nПосле удаления файла file_empty:"
print_size

rm -f "$TEST_DIR/file_content.txt" "$TEST_DIR/symlink_file.txt"
echo -e "\nПосле удаления файла file_content.txt и символической ссылки:" 
print_size

rmdir "$TEST_DIR/subdir"
echo -e "\nПосле удаления подкаталога subdir:"
print_size

echo -e "\nИтоговый размер каталога $TEST_DIR:"
print_size

# Удаление каталога
echo -e "\nУдаление каталога $TEST_DIR..."
rmdir "$TEST_DIR" && echo "Каталог удалён успешно." || echo "Ошибка: каталог не пуст."
