#!/bin/bash
# 12_3.sh - Экспериментальное добавление собственного типа файла в базу утилиты file

# 1. Создание тестового файла с уникальной сигнатурой.
CUSTOM_FILE="custom.bin"
echo -n $'\x43\x54\x4d' > "$CUSTOM_FILE"   # Записываем байты: 0x43 0x54 0x4d, что соответствует "CTM"
echo " Дополнительные данные" >> "$CUSTOM_FILE"
echo "Создан файл $CUSTOM_FILE с сигнатурой 'CTM'"

# 2. Создание локального файла магии.
CUSTOM_MAGIC="my_magic"
cat > "$CUSTOM_MAGIC" <<EOF
0   string  CTM  Custom File Type: MyCTM File
!:mime  application/x-myctm
EOF
echo "Создан локальный файл магии ($CUSTOM_MAGIC):"
cat "$CUSTOM_MAGIC"
echo

# 3. Компиляция новой базы магии.
echo "Компиляция новой базы магии..."
file -C -m "$CUSTOM_MAGIC" > /dev/null 2>&1
if [ $? -eq 0 ]; then
   echo "Новая база магии скомпилирована успешно (файл ${CUSTOM_MAGIC}.mgc создан)."
else
   echo "Ошибка при компиляции новой базы магии." && exit 1
fi

# 4. Определение типа файла с использованием новой базы магии.
echo "Определение типа файла $CUSTOM_FILE с использованием новой базы магии:"
file -m "${CUSTOM_MAGIC}.mgc" "$CUSTOM_FILE"

echo -e "\nСкрипт 12_3.sh выполнен."
