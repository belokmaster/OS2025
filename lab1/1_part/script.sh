#!/bin/sh

# chmod +x script.sh 
#./script.sh output.txt

# Проверяем, что указан ровно один аргумент — имя выходного файла
if [ "$#" -ne 1 ]; then
  echo "Использование: $0 имя_выходного_файла"
  exit 1
fi

OUTFILE="$1"
# Очищаем (или создаём) файл для результатов
> "$OUTFILE"

# Сопоставление символа типа файла и ключа в found_types
declare -A type_map=(
  ["-"]="regular"    # Обычный файл
  [b]="block"        # Блочное устройство
  [c]="char"         # Символьное устройство
  [d]="directory"    # Директория
  [l]="symlink"      # Символическая ссылка
  [p]="fifo"         # FIFO
  [s]="socket"       # Сокет
)

# Массив, отслеживающий, нашли ли уже конкретный тип
declare -A found_types=(
  [regular]=0
  [block]=0
  [char]=0
  [directory]=0
  [symlink]=0
  [fifo]=0
  [socket]=0
)

# Функция, чтобы проверить, все ли типы найдены
all_found() {
  for val in "${found_types[@]}"; do
    if [ "$val" -eq 0 ]; then
      return 1
    fi
  done
  return 0
}

current_dir=""

# Рекурсивный вывод ls, читаем строчно через процесс-подстановку
while IFS= read -r line; do
  
  # Если строка заканчивается двоеточием, значит это путь к директории
  if [[ "$line" =~ :$ ]]; then
    current_dir="${line%:}"
    # Убираем завершающий слэш, если есть
    current_dir="${current_dir%/}"
    continue
  fi

  # Пропускаем пустые строки
  [ -z "$line" ] && continue

  # Первый символ строки — это тип файла в выводе "ls -l"
  file_char="${line:0:1}"
  
  # Если символ есть в type_map, значит у нас интересующий тип
  if [[ -n "${type_map[$file_char]}" ]]; then
    file_type="${type_map[$file_char]}"
    
    # Проверяем, не находили ли мы этот тип ранее
    if [ "${found_types[$file_type]}" -eq 0 ]; then
      filename="$(awk '{print $NF}' <<< "$line")"
      echo "$file_char $current_dir/$filename" >> "$OUTFILE"
      # Отмечаем, что теперь этот тип найден
      found_types[$file_type]=1
    fi
  fi

  # Если все типы уже найдены, выходим из цикла
  if all_found; then
    break
  fi

done < <(ls -lR / 2>/dev/null)

echo "Результаты записаны в файл: $OUTFILE"
