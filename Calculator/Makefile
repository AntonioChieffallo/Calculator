# Makefile for C++ Calculator Server
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
LIBS = -lpthread
TARGET = calculator_server
SOURCE = calculator.cpp

# Default target
all: $(TARGET)

# Build the calculator server
$(TARGET): $(SOURCE)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SOURCE) $(LIBS)

# Install dependencies (Ubuntu/Debian)
install-deps:
	sudo apt-get update
	sudo apt-get install -y build-essential

# Clean build artifacts
clean:
	rm -f $(TARGET)

# Run the server
run: $(TARGET)
	./$(TARGET)

# Build and run
start: $(TARGET)
	@echo "Starting C++ Calculator Server..."
	@echo "Open http://localhost:8080 in your browser"
	./$(TARGET)

# For development - rebuild and run
dev: clean $(TARGET)
	./$(TARGET)

.PHONY: all clean install-deps run start dev