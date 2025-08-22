#!/bin/bash

echo "=== The Sound Studio Application Monitor ==="
echo "Checking application status..."

# Check if the application is running
APP_PID=$(pgrep -f "The Sound Studio" | head -1)

if [ -n "$APP_PID" ]; then
    echo "✅ Application is running (PID: $APP_PID)"
    echo "Process information:"
    ps -p $APP_PID -o pid,ppid,user,pcpu,pmem,etime,comm
    echo ""
    echo "Memory usage:"
    ps -p $APP_PID -o pid,rss,vsz,pcpu,pmem
    echo ""
    echo "To stop the application, use: kill $APP_PID"
else
    echo "❌ Application is not running"
    echo "To start the application:"
    echo "  Double-click: The Sound Studio.app"
    echo "  Or run: ./The\ Sound\ Studio.app/Contents/MacOS/The\ Sound\ Studio"
fi

echo ""
echo "=== Log Files ==="
echo "Application logs are available in the logs/ directory:"
ls -la logs/
