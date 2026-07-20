#!/bin/bash

# Define directories and files
SUBMISSIONS_DIR="./submissions"
BACKUP_DIR="./backup"
REPORT_FILE="system_report.txt"
ERROR_LOG="error_log.txt"

# Initialize counters
total_processed=0
total_duplicates=0
total_backed_up=0

# Clear previous logs/reports and redirect errors to error_log.txt
echo "--- Error Log ---" > "$ERROR_LOG"
mkdir -p "$BACKUP_DIR" 2>> "$ERROR_LOG"

# Temporary file to track MD5 hashes of unique files
hash_list=$(mktemp -t hash_list)

# Check if submissions directory exists
if [ ! -d "$SUBMISSIONS_DIR" ]; then
    echo "Error: Submissions directory '$SUBMISSIONS_DIR' not found." >> "$ERROR_LOG"
    exit 1
fi

# Loop through files in the submissions directory
for file in "$SUBMISSIONS_DIR"/*; do
    # Check if it is a regular file
    if [ -f "$file" ]; then
        ((total_processed++))
        
        # macOS specific: Use 'md5 -q' to get just the hash string
        file_hash=$(md5 -q "$file" 2>> "$ERROR_LOG")
        
        # Check if the hash already exists in our tracked list
        if grep -q "$file_hash" "$hash_list" 2>> "$ERROR_LOG"; then
            ((total_duplicates++))
        else
            # If unique, add hash to list and copy to backup directory
            echo "$file_hash" >> "$hash_list"
            cp "$file" "$BACKUP_DIR/" 2>> "$ERROR_LOG"
            
            if [ $? -eq 0 ]; then
                ((total_backed_up++))
            fi
        fi
    fi
done

# Clean up temporary hash file
rm "$hash_list"

# Generate the final report
{
    echo "========================================="
    echo "       SUBMISSION PROCESSING REPORT      "
    echo "========================================="
    echo "Total files processed : $total_processed"
    echo "Duplicate files found : $total_duplicates"
    echo "Unique files backed up: $total_backed_up"
    echo "========================================="
} > "$REPORT_FILE"

echo "Processing complete. Report generated in '$REPORT_FILE'."
