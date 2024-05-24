#define WIN32_LEAN_AND_MEAN // Эта директива исключает из заголовков Windows неиспользуемые компоненты, ускоряя тем самым компиляцию.

#include <iostream> // Подключает стандартную библиотеку ввода-вывода.
#include <Windows.h> // Подключает базовые компоненты Windows API.
#include <WinSock2.h> // Подключает библиотеку Winsock для работы с сетевыми сокетами.
#include <WS2tcpip.h> // Подключает дополнительные компоненты Winsock для работы с протоколом TCP/IP.

int main() {
    WSADATA wsaData; // Структура для хранения информации о реализации сокетов в операционной системе.
    ADDRINFO* addResult; // Указатель на список структур ADDRINFO, содержащих информацию об адресе сервера.
    ADDRINFO hints; // Структура, содержащая настройки для функции getaddrinfo().
    SOCKET ClientSocket = INVALID_SOCKET; // Сокет для подключения клиентов. Инициализирован как INVALID_SOCKET для проверки на корректность.
    SOCKET ListentSocket = INVALID_SOCKET; // Сокет для прослушивания входящих подключений. Инициализирован как INVALID_SOCKET для проверки на корректность.

    const char* sendBuffer = "Hello from server"; // Буфер для отправки сообщения клиенту.
    char recvBuffer[512]; // Буфер для получения сообщения от клиента.

    int result = WSAStartup(MAKEWORD(2, 2), &wsaData); // Инициализация библиотеки Winsock. Возвращает 0 в случае успеха.

    if (result != 0) { // Если инициализация не удалась, выводится сообщение об ошибке и возвращается код 1.
        std::cout << "WSAStartup failes result" << std::endl;
        return 1;
    }

    ZeroMemory(&hints, sizeof(hints)); // Обнуление памяти, выделенной под структуру hints.
    hints.ai_family = AF_INET; // Установка семейства адресов для IPv4.
    hints.ai_socktype = SOCK_STREAM; // Установка типа сокета для потоковой передачи данных.
    hints.ai_protocol = IPPROTO_TCP; // Установка протокола TCP для передачи данных.
    hints.ai_flags = AI_PASSIVE; // Установка флага AI_PASSIVE для использования адреса сервера.

    result = getaddrinfo(NULL, "666", &hints, &addResult); // Получение информации об адресе сервера с помощью функции getaddrinfo(). Возвращает 0 в случае успеха.
    if (result != 0) { // Если получение информации не удалось, выводится сообщение об ошибке, выполняется очистка библиотеки Winsock с помощью функции WSACleanup() и возвращается код 1.
        std::cout << "Getaddrinfo wit error" << std::endl;
        WSACleanup();
        return 1;
    }

    ListentSocket = socket(addResult->ai_family, addResult->ai_socktype, addResult->ai_protocol); // Создание сокета для прослушивания входящих подключений с помощью функции socket(). Возвращает дескриптор сокета в случае успеха.
    if (ListentSocket == INVALID_SOCKET) { // Если создание сокета не удалось, выводится сообщение об ошибке, выполняется очистка памяти, выделенной под список структур ADDRINFO с помощью функции freeaddrinfo(), и возвращается код 1.
        std::cout << "Socket creation with " << std::endl;
        freeaddrinfo(addResult);
        return 1;
    }

    result = bind(ListentSocket, addResult->ai_addr, (int)addResult->ai_addrlen); // Привязка сокета к адресу сервера с помощью функции bind(). Возвращает 0 в случае успеха.
    if (result == SOCKET_ERROR) { // Если привязка не удалась, выводится сообщение об ошибке, выполняется закрытие сокета с помощью функции closesocket(), очистка памяти и библиотеки Winsock и возвращается код 1.
        std::cout << "Binding connect failed" << std::endl;
        closesocket(ListentSocket);
        freeaddrinfo(addResult);
        WSACleanup();
        return 1;
    }

    result = listen(ListentSocket, SOMAXCONN); // Перевод сокета в режим прослушивания входящих подключений с помощью функции listen(). Возвращает 0 в случае успеха.
    if (result == SOCKET_ERROR) { // Если перевод не удался, выводится сообщение об ошибке, выполняется закрытие сокета, очистка памяти и библиотеки Winsock и возвращается код 1.
        std::cout << "Listening socket failed" << std::endl;
        closesocket(ListentSocket);
        freeaddrinfo(addResult);
        WSACleanup();
        return 1;
    }

    ClientSocket = accept(ListentSocket, NULL, NULL); // Ожидание входящего подключения и получение дескриптора сокета клиента с помощью функции accept(). Возвращает дескриптор сокета в случае успеха.
    if (ClientSocket == INVALID_SOCKET) { // Если получение дескриптора не удалось, выводится сообщение об ошибке, выполняется закрытие сокета, очистка памяти и библиотеки Winsock и возвращается код 1.
        std::cout << "Accepting socket failed" << std::endl;
        closesocket(ListentSocket);
        freeaddrinfo(addResult);
        WSACleanup();
        return 1;
    }

    closesocket(ListentSocket); // Закрытие сокета для прослушивания входящих подключений.

    do { // Цикл обработки сообщений от клиента.
        ZeroMemory(recvBuffer, 512); // Обнуление памяти, выделенной под буфер для получения сообщения от клиента.
        result = recv(ClientSocket, recvBuffer, 512, 0); // Получение сообщения от клиента с помощью функции recv(). Возвращает количество полученных байтов в случае успеха.
        if (result > 0) { // Если сообщение получено, выводится информация о количестве байтов и самом сообщении, а затем отправляется ответное сообщение с помощью функции send().
            std::cout << "Recieved " << result << "bytes" << std::endl;
            std::cout << "Recieved data" << recvBuffer << std::endl;

            result = send(ClientSocket, sendBuffer, (int)strlen(sendBuffer), 0);

            if (result == SOCKET_ERROR) { // Если отправка сообщения не удалась, выводится сообщение об ошибке, выполняется закрытие сокета, очистка памяти и библиотеки Winsock и возвращается код 1.
                std::cout << "Failed to send data back" << std::endl;
                closesocket(ClientSocket);
                freeaddrinfo(addResult);
                WSACleanup();
            }
        }
        else if (result == 0) // Если клиент отключился, выводится сообщение об этом.
            std::cout << "Connection closing..." << std::endl;

        else { // Если произошла ошибка при получении сообщения, выводится сообщение об ошибке, выполняется закрытие сокета, очистка памяти и библиотеки Winsock и возвращается код 1.
            std::cout << "recv failed with error" << std::endl;
            closesocket(ClientSocket);
            freeaddrinfo(addResult);
            WSACleanup();
            return 1;
        }
    } while (result > 0); // Цикл продолжается, пока не будет получено сообщение с количеством байтов, равным 0 (т.е. пока клиент не отключится).

    result = shutdown(ClientSocket, SD_SEND); // Завершение работы сокета клиента с помощью функции shutdown(). Возвращает 0 в случае успеха.
    if (result == SOCKET_ERROR) { // Если завершение не удалось, выводится сообщение об ошибке, выполняется закрытие сокета, очистка памяти и библиотеки Winsock и возвращается код 1.
        closesocket(ClientSocket);
        freeaddrinfo(addResult);
        WSACleanup();
        return 1;
    }

    closesocket(ClientSocket); // Закрытие сокета клиента.
    freeaddrinfo(addResult); // Очистка памяти, выделенной под список структур ADDRINFO.
    WSACleanup(); // Очистка библиотеки Winsock.

    return 0; // Возврат кода 0, обозначающего успешное завершение работы программы.
}
