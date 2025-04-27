#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <unistd.h>

// Структура сообщения
struct msg_buffer {
    long msg_type;
    char msg_text[100];
};

int main() {
    key_t key = ftok("mailbox", 65); // Генерация ключа
    int msgid = msgget(key, 0666 | IPC_CREAT); // Создание очереди
    
    if (fork() == 0) {
        // Процесс-получатель (ребенок)
        struct msg_buffer message;
        
        printf("Получатель ждет сообщения...\n");
        msgrcv(msgid, &message, sizeof(message), 1, 0); // Блокирующее чтение
        
        printf("Получено сообщение: %s\n", message.msg_text);
        
        // Удаляем очередь (на практике лучше делать в отправителе)
        msgctl(msgid, IPC_RMID, NULL);
        exit(0);
    } else {
        // Процесс-отправитель (родитель)
        sleep(2); // Имитация задержки
        
        struct msg_buffer message;
        message.msg_type = 1;
        strcpy(message.msg_text, "Привет из почтового ящика!");
        
        printf("Отправка сообщения...\n");
        msgsnd(msgid, &message, sizeof(message), 0); // Неблокирующая отправка
        
        sleep(1); // Даем время получателю прочитать
    }
    
    return 0;
}
