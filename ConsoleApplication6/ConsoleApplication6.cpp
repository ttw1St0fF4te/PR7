#define WIN32_LEAN_AND_MEAN // Эта директива используется для исключения редко используемых сервисов из заголовков Windows, что делает их компиляцию более быстрой.

#include <iostream> // Включает стандартную библиотеку ввода/вывода.
#include <Windows.h> // Включает API Windows.
#include <WinSock2.h> // Включает библиотеку Winsock (версия 2), которая используется для сетевого программирования.
#include <WS2tcpip.h> // Включает функции TCP/IP библиотеки Winsock.

using namespace std; // Использует стандартное пространство имен.

int main() {
    WSADATA wsaData; // Структура, которая используется функцией WSAStartup для получения сведений об реализации Windows Sockets.
    ADDRINFO* addResult; // Указатель на связанный список структур ADDRINFO, который содержит ответные сведения от вызова функции getaddrinfo.
    ADDRINFO hints; // Структура, которая используется функцией getaddrinfo для задания критериев запроса.
    SOCKET ConnectSocket = INVALID_SOCKET; // Дескриптор сокета, который используется для установления соединения с сервером. Он инициализирован как INVALID_SOCKET, чтобы указать, что он в настоящее время не ассоциирован с сокетом.

    const char* sendBuffer = "Hello from Client"; // Буфер, который содержит сообщение, которое будет отправлено на сервер.
    const char* sendBuffer2 = "Hello from client 2"; // Буфер, который содержит второе сообщение, которое будет отправлено на сервер.
    char recvBuffer[512]; // Буфер, который используется для получения сообщений от сервера.

    int result = WSAStartup(MAKEWORD(2,2), &wsaData); // Инициализирует библиотеку Winsock. Функция возвращает значение, которое указывает, была ли инициализация успешной.

    if (result != 0) { // Если инициализация не была успешной, вывести сообщение об ошибке и вернуть 1.
        std::cout << "WSAStartup failes result" << std::endl;
        return 1;
    }

    ZeroMemory(&hints, sizeof(hints)); // Инициализирует структуру hints нулем.
    hints.ai_family = AF_INET; // Указывает, что запрос должен возвращать IPv4-адреса.
    hints.ai_socktype = SOCK_STREAM; // Указывает, что запрос должен возвращать потоковые сокеты (т.е. TCP-сокеты).
    hints.ai_protocol = IPPROTO_TCP; // Указывает, что запрос должен возвращать TCP-сокеты.

    result = getaddrinfo("localhost", "666", &hints,&addResult); // Запрашивает у DNS IP-адрес и номер порта сервера. Функция возвращает значение, которое указывает, был ли запрос успешным.
    if (result != 0) { // Если запрос не был успешным, вывести сообщение об ошибке, очистить библиотеку Winsock и вернуть 1.
        std::cout << "Getaddrinfo wit error" << std::endl;
        WSACleanup();
        return 1;
    }

    ConnectSocket = socket(addResult->ai_family, addResult->ai_socktype, addResult->ai_protocol); // Создает сокет. Функция возвращает дескриптор сокета, который используется для идентификации сокета.
    if (ConnectSocket == INVALID_SOCKET) { // Если создание сокета не было успешным, вывести сообщение об ошибке, очистить библиотеку Winsock и вернуть 1.
        std::cout << "Socket creation with " << std::endl;
        WSACleanup();
        return 1;
    }

    result = connect(ConnectSocket, addResult->ai_addr, (int)addResult->ai_addrlen); // Устанавливает соединение с сервером. Функция возвращает значение, которое указывает, было ли соединение успешным.
    if (result == SOCKET_ERROR) { // Если соединение не было успешным, вывести сообщение об ошибке, закрыть сокет, очистить память и библиотеку Winsock, и вернуть 1.
        std::cout << "Unable to connect to server" << std::endl;
        closesocket(ConnectSocket);
        freeaddrinfo(addResult);
        WSACleanup();
        return 1;
    }

    result = send(ConnectSocket, sendBuffer, (int)strlen(sendBuffer), 0); // Отправляет сообщение на сервер. Функция возвращает значение, которое указывает, была ли отправка успешной.
    if (result == SOCKET_ERROR) { // Если отправка не была успешной, вывести сообщение об ошибке, закрыть сокет, очистить память и библиотеку Winsock, и вернуть 1.
        cout << "Send failed, error: " << result << endl;
        closesocket(ConnectSocket);
        freeaddrinfo(addResult);
        WSACleanup();
        return 1;
    }

    std::cout << "Data sent succsesfully" << result << std::endl; // Вывести сообщение об успешной отправке и количестве отправленных байтов.

    result = send(ConnectSocket, sendBuffer2, (int)strlen(sendBuffer2), 0); // Отправляет второе сообщение на сервер.
    if (result == SOCKET_ERROR) { // Если отправка не была успешной, вывести сообщение об ошибке, закрыть сокет, очистить память и библиотеку Winsock, и вернуть 1.
        cout << "Send failed, error: " << result << endl;
        closesocket(ConnectSocket);
        freeaddrinfo(addResult);
        WSACleanup();
        return 1;
    }

    std::cout << "Data sent succsesfully" <<result<< std::endl; // Вывести сообщение об успешной отправке и количестве отправленных байтов.

    result = shutdown(ConnectSocket, SD_SEND); // Закрывает отправляющую часть соединения. Функция возвращает значение, которое указывает, было ли закрытие успешным.
    if (result == SOCKET_ERROR) { // Если закрытие не было успешным, вывести сообщение об ошибке, очистить память и библиотеку Winsock, и вернуть 1.
        cout << "shutdown error <3" << endl;
        freeaddrinfo(addResult);
        WSACleanup();//очситка WSAStartup
        return 1;
    }

    do { // Получает сообщения от сервера, пока соединение не будет закрыто.
        ZeroMemory(recvBuffer, 512); // Инициализирует буфер получения нулем.
        result = recv(ConnectSocket, recvBuffer, 512, 0); // Получает сообщение от сервера. Функция возвращает значение, которое указывает, была ли получение успешной.
        if (result > 0) { // Если получение было успешным, вывести количество полученных байтов и полученные данные.
            cout << "Received " << result << "bytes" << endl;
            cout << "Received data " << recvBuffer << endl;
        }
        else if (result == 0) // Если соединение было закрыто, вывести сообщение.
            cout << "Connection closed" << endl;
        else { // Если получение не было успешным, вывести сообщение об ошибке, закрыть сокет и выйти из цикла.
            cout << "Received failed " << endl;
            closesocket(ConnectSocket);
        }
    } while (result > 0);

    result = shutdown(ConnectSocket, SD_SEND); // Закрывает отправляющую часть соединения еще раз. Это необязательно, но это не причиняет вреда.
    if (result == SOCKET_ERROR) { // Если закрытие не было успешным, вывести сообщение об ошибке, очистить память и библиотеку Winsock, и вернуть 1.
        cout << "shutdown  error " << endl;
        freeaddrinfo(addResult);
        WSACleanup();
        return 1;
    }

    closesocket(ConnectSocket); // Закрывает сокет.
    freeaddrinfo(addResult); // Очищает память.
    WSACleanup(); // Очищает библиотеку Winsock.
    return 0; // Возвращает 0, чтобы указать, что программа выполнена успешно.
}
