#include <array>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <windows.h>

namespace {

constexpr DWORD kBaudRate = CBR_19200;
constexpr std::size_t kReceiveBufferSize = 512;

void logMessage(const std::string& message) {
    std::ofstream logFile("saved_communications.txt", std::ios::app);
    if (!logFile) {
        std::cerr << "Warning: could not open saved_communications.txt.\n";
        return;
    }
    logFile << message << '\n';
}

bool readNumber(const char* prompt, double& value) {
    std::cout << prompt;
    if (std::cin >> value) {
        return true;
    }
    std::cerr << "Invalid number.\n";
    return false;
}

std::string makeRequest(double firstNumber, char operation, double secondNumber) {
    std::ostringstream request;
    request << std::setprecision(15)
            << firstNumber << ',' << operation << ',' << secondNumber << '\n';
    return request.str();
}

}  // namespace

int main(int argc, char* argv[]) {
    const std::string portName = argc > 1 ? argv[1] : "COM3";
    const std::string devicePath = "\\\\.\\" + portName;

    HANDLE serialHandle = CreateFileA(
        devicePath.c_str(),
        GENERIC_READ | GENERIC_WRITE,
        0,
        nullptr,
        OPEN_EXISTING,
        0,
        nullptr
    );
    if (serialHandle == INVALID_HANDLE_VALUE) {
        std::cerr << "Could not open serial port " << portName << ".\n";
        return 1;
    }

    DCB serialParameters{};
    serialParameters.DCBlength = sizeof(serialParameters);
    if (!GetCommState(serialHandle, &serialParameters)) {
        std::cerr << "Could not read the serial-port configuration.\n";
        CloseHandle(serialHandle);
        return 1;
    }

    serialParameters.BaudRate = kBaudRate;
    serialParameters.ByteSize = 8;
    serialParameters.StopBits = ONESTOPBIT;
    serialParameters.Parity = NOPARITY;
    if (!SetCommState(serialHandle, &serialParameters)) {
        std::cerr << "Could not configure the serial port.\n";
        CloseHandle(serialHandle);
        return 1;
    }

    COMMTIMEOUTS timeouts{};
    timeouts.ReadIntervalTimeout = 50;
    timeouts.ReadTotalTimeoutConstant = 2000;
    timeouts.ReadTotalTimeoutMultiplier = 10;
    timeouts.WriteTotalTimeoutConstant = 1000;
    timeouts.WriteTotalTimeoutMultiplier = 10;
    if (!SetCommTimeouts(serialHandle, &timeouts)) {
        std::cerr << "Could not configure serial timeouts.\n";
        CloseHandle(serialHandle);
        return 1;
    }

    double firstNumber = 0.0;
    double secondNumber = 0.0;
    char operation = '\0';

    if (!readNumber("Enter the first number: ", firstNumber)) {
        CloseHandle(serialHandle);
        return 1;
    }

    std::cout << "Enter the operation (+, -, *, /): ";
    std::cin >> operation;
    if (operation != '+' && operation != '-' &&
        operation != '*' && operation != '/') {
        std::cerr << "Invalid operation.\n";
        CloseHandle(serialHandle);
        return 1;
    }

    if (!readNumber("Enter the second number: ", secondNumber)) {
        CloseHandle(serialHandle);
        return 1;
    }

    const std::string request = makeRequest(
        firstNumber,
        operation,
        secondNumber
    );
    std::cout << "Sending: " << request;
    logMessage("Sent: " + request.substr(0, request.size() - 1));

    DWORD bytesWritten = 0;
    const bool writeSucceeded = WriteFile(
        serialHandle,
        request.data(),
        static_cast<DWORD>(request.size()),
        &bytesWritten,
        nullptr
    );
    if (!writeSucceeded ||
        bytesWritten != static_cast<DWORD>(request.size())) {
        std::cerr << "Could not send the complete request.\n";
        CloseHandle(serialHandle);
        return 1;
    }

    std::array<char, kReceiveBufferSize> buffer{};
    DWORD bytesRead = 0;
    const bool readSucceeded = ReadFile(
        serialHandle,
        buffer.data(),
        static_cast<DWORD>(buffer.size() - 1),
        &bytesRead,
        nullptr
    );
    if (!readSucceeded) {
        std::cerr << "Could not read from the serial port.\n";
        CloseHandle(serialHandle);
        return 1;
    }
    if (bytesRead == 0) {
        std::cerr << "Timed out while waiting for the microcontroller.\n";
        CloseHandle(serialHandle);
        return 1;
    }

    buffer[bytesRead] = '\0';
    const std::string response(buffer.data(), bytesRead);
    std::cout << "Result: " << response;

    std::string loggedResponse = response;
    while (!loggedResponse.empty() &&
           (loggedResponse.back() == '\n' || loggedResponse.back() == '\r')) {
        loggedResponse.pop_back();
    }
    logMessage("Received: " + loggedResponse);

    CloseHandle(serialHandle);
    return 0;
}
