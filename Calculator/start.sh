#!/bin/bash

# C++ Calculator Server Startup Script

echo "🧮 C++ Calculator Server Setup"
echo "================================="

# Check if the server binary exists
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"
if [ ! -f "calculator_server" ]; then
    echo "📦 Building the calculator server..."
    make clean
    make
    if [ $? -ne 0 ]; then
        echo "❌ Build failed. Please check the error messages above."
        exit 1
    fi
    echo "✅ Build successful!"
fi

echo "🚀 Starting C++ Calculator Server..."
echo "📱 Open http://localhost:8080 in your browser"
echo "🛑 Press Ctrl+C to stop the server"
echo ""

# Start the server
./calculator_server