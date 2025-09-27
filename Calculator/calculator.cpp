#include <iostream>
#include <limits>

// Function prototypes
double add(double a, double b);
double subtract(double a, double b);
double multiply(double a, double b);
double divide(double a, double b);

int main() {
    double num1, num2;
    char op;
    std::cout << "Basic Calculator\n";
    std::cout << "Enter first number: ";
    std::cin >> num1;
    std::cout << "Enter operator (+, -, *, /): ";
    std::cin >> op;
    std::cout << "Enter second number: ";
    std::cin >> num2;

    double result;
    bool error = false;
    switch (op) {
        case '+':
            result = add(num1, num2);
            break;
        case '-':
            result = subtract(num1, num2);
            break;
        case '*':
            result = multiply(num1, num2);
            break;
        case '/':
            if (num2 == 0) {
                std::cout << "Error: Division by zero!" << std::endl;
                error = true;
            } else {
                result = divide(num1, num2);
            }
            break;
        default:
            std::cout << "Error: Invalid operator!" << std::endl;
            error = true;
    }
    if (!error) {
        std::cout << "Result: " << result << std::endl;
    }
    return 0;
}

double add(double a, double b) {
    return a + b;
}

double subtract(double a, double b) {
    return a - b;
}

double multiply(double a, double b) {
    return a * b;
}

double divide(double a, double b) {
    return a / b;
}
