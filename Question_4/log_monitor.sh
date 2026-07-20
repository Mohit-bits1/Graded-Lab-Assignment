#!/bin/bash

LOG_FILE="app.log"
REPORT_FILE="error_report.txt"

# Check if the log file exists before running
if [ ! -f "$LOG_FILE" ]; then
    touch "$LOG_FILE"
fi

echo "=== Real-Time Log Monitoring Started ==="
echo "Monitoring '$LOG_FILE' for ERROR entries..."
echo "Press [Ctrl + C] to stop monitoring."
echo "----------------------------------------"

# 1. tail -f streams newly added log entries in real time.
# 2. grep filters and extracts only lines containing 'ERROR'.
# 3. tee reads from standard input and writes both to the screen and the report file.
# The '--line-buffered' flags ensure text updates instantly without trailing delays.
tail -f "$LOG_FILE" | grep --line-buffered "ERROR" | tee -a "$REPORT_FILE"
