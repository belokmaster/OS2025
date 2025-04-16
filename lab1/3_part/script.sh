#!/bin/bash
# script.sh
#
# Описание: Поиск и подсчет всех полноименных символьных ссылок на заданный файл,
# размещённых в указанном каталоге и его подкаталогах.
#
# Использование:
#   ./script.sh <целевой_файл> <каталог_поиска>
#
if [ $# -ne 2 ]; then
  echo "Использование: $0 <целевой_файл> <каталог_поиска>"
  exit 1
fi

TARGET_FILE="$1"
SEARCH_DIR="$2"

# Получаем абсолютный путь к целевому файлу
TARGET_ABS=$(readlink -f "$TARGET_FILE")
if [ -z "$TARGET_ABS" ]; then
  echo "Ошибка: не удалось определить абсолютный путь для файла '$TARGET_FILE'"
  exit 1
fi

echo "Ищем символические ссылки, указывающие на: $TARGET_ABS"
echo "Начинаем поиск в: $SEARCH_DIR"

count=0

# Ищем все символьные ссылки в указанном каталоге
while IFS= read -r symlink; do
  link_target=$(readlink "$symlink")
  
  # Проверяем, что ссылка указывает на абсолютный путь
  if [[ "$link_target" == /* ]] && [ "$link_target" = "$TARGET_ABS" ]; then
    echo "$symlink"
    ((count++))
  fi

done < <(find "$SEARCH_DIR" -type l 2>/dev/null)

echo "Общее количество символических ссылок: $count"
