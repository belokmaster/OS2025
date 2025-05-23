#!/bin/bash
# 12_1.sh - Алгоритм работы утилиты file и анализ базы магии

echo "1. Сначала определяется базовый тип файла (например, с помощью stat)."
echo "2. Затем file считывает первые байты файла и сравнивает их с шаблонами из базы магии."
echo "   Пример правила для ELF-файлов в базе магии:"

grep -R "^0[[:space:]]\+string[[:space:]]\\x7fELF" /usr/share/file/magic* 2>/dev/null | head -n 3

echo "Это правило проверяет, что первые 4 байта содержат: 0x7F 'E' 'L' 'F'."
echo "Пример заголовка ELF-файла (/bin/ls):"
hexdump -C -n 64 /bin/ls

echo -e "\nСкрипт 12_1.sh выполнен."
