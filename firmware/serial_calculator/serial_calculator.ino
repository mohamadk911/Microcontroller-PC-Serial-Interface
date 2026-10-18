#include <stdlib.h>

namespace {

constexpr unsigned long kBaudRate = 19200;

bool parseNumber(const String& text, double& value) {
  if (text.length() == 0) {
    return false;
  }

  char* endPointer = nullptr;
  value = strtod(text.c_str(), &endPointer);
  return endPointer != text.c_str() && *endPointer == '\0';
}

void processRequest(String request) {
  request.trim();

  const int firstComma = request.indexOf(',');
  const int secondComma = request.indexOf(',', firstComma + 1);
  if (firstComma <= 0 || secondComma <= firstComma + 1 ||
      request.indexOf(',', secondComma + 1) != -1) {
    Serial.println("Error: invalid input format");
    return;
  }

  const String firstText = request.substring(0, firstComma);
  const String operation = request.substring(firstComma + 1, secondComma);
  const String secondText = request.substring(secondComma + 1);

  double firstNumber = 0.0;
  double secondNumber = 0.0;
  if (!parseNumber(firstText, firstNumber) ||
      !parseNumber(secondText, secondNumber)) {
    Serial.println("Error: invalid number");
    return;
  }

  double result = 0.0;
  if (operation == "+") {
    result = firstNumber + secondNumber;
  } else if (operation == "-") {
    result = firstNumber - secondNumber;
  } else if (operation == "*") {
    result = firstNumber * secondNumber;
  } else if (operation == "/") {
    if (secondNumber == 0.0) {
      Serial.println("Error: division by zero");
      return;
    }
    result = firstNumber / secondNumber;
  } else {
    Serial.println("Error: invalid operation");
    return;
  }

  Serial.println(result, 6);
}

}  // namespace

void setup() {
  Serial.begin(kBaudRate);
  Serial.setTimeout(2000);
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  if (Serial.available() > 0) {
    processRequest(Serial.readStringUntil('\n'));
  }
}
