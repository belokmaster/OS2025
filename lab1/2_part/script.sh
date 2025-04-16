#!/bin/bash
# script.sh

# Проверяем, передан ли аргумент
if [ $# -lt 1 ]; then
    echo "Использование: $0 <target_file>"
    exit 1
fi

# Получаем имя файла из аргумента
filename=$1

# Получаем inode файла
inode=$(ls -i "$filename" 2>/dev/null | cut -d ' ' -f 1)

# Если inode не был найден, выводим ошибку
if [ -z "$inode" ]; then
    echo "Не удалось получить inode для файла: $filename"
    exit 1
fi

# Ищем все файлы с таким же inode в пользовательском пространстве (каталоги /home/...)
echo "Ищем жесткие ссылки на файл $filename (inode: $inode)..."

# Используем ls -liR для рекурсивного обхода директорий и фильтруем по inode
ls -liR /home/ 2>/dev/null | awk -v inode="$inode" '
# Обрабатываем строки, содержащие информацию о файле
{
    # Если inode совпадает
    if ($1 == inode) {
        # Печатаем полный путь к файлу, исключая сам целевой файл
        print $NF;
    }
}'
