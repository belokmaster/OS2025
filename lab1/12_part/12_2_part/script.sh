#!/bin/bash
# 12_2.sh - Демонстрация работы утилиты file с разными ключами

# Пример файлов (отредактируйте пути при необходимости)
FILE1="/bin/ls"
FILE2="/etc/passwd"
FILE3="/home/$USER/example.odt"  # если такой файл есть, или замените на существующий

echo "Пример вывода утилиты file без ключей"
file "$FILE1"
echo

echo "Вывод MIME-типа (-i) для $FILE1/"
file -i "$FILE1"
echo

echo "Сокращённый вывод (-b) для $FILE2/"
file -b "$FILE2"
echo

echo "Вывод версии утилиты file (--version)"
file --version

# Если файл $FILE3 существует, покажем его вывод с ключами
if [ -f "$FILE3" ]; then
  echo -e "Вывод для файла $FILE3"
  file "$FILE3"
  echo "С MIME-типом (-i):"
  file -i "$FILE3"
fi

echo -e "\nСкрипт 12_2.sh выполнен."
