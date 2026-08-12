#!/bin/bash

# Configuration
IMAGE_DIR="/home/mattd/camera_images"
LOG_FILE="$IMAGE_DIR/capture.log"
MAX_DISK_USAGE=80  # Percentage

# Create main directory if it doesn't exist
mkdir -p "$IMAGE_DIR"

# Function to log messages
log_message() {
    echo "$(date '+%Y-%m-%d %H:%M:%S') - $1" >> "$LOG_FILE"
}

# Function to capture image
capture_image() {
    local date_dir="$IMAGE_DIR/$(date '+%Y-%m-%d')"
    mkdir -p "$date_dir"  # Create dated subdirectory if it doesn't exist
    local filename="$date_dir/$(date '+%Y%m%d_%H%M%S').jpg"  # Filename with YYYYMMDD_HHMMSS format
    if rpicam-still --width 1920 --height 1080 -o "$filename"; then
        log_message "Captured image: $filename"
    else
        log_message "Failed to capture image"
    fi
}

# Function to archive oldest directory to MP4 if disk usage is over threshold
archive_if_over_disk_usage() {
    local usage=$(df "$IMAGE_DIR" | tail -1 | awk '{print $5}' | sed 's/%//')
    if [ $usage -gt $MAX_DISK_USAGE ]; then
        # Find the oldest dated directory
	 oldest_dir=$(ls -rt camera_images/ | grep -P '\d{4}-\d{2}-\d{2}$' | head -1)

            local dir_name=$(basename "$oldest_dir")
            local video_file="$IMAGE_DIR/${dir_name}.mp4"

            # Compile images into MP4 using ffmpeg
            # Assuming images are named in chronological order which will be processed in order
#            if ffmpeg -framerate 30 -pattern_type glob -i "$oldest_dir/*.jpg" -c:v libx264 -pix_fmt yuv420p "$video_file" -y > /dev/null 2>&1; then
                # Move the MP4 to parent directory (already there) and remove the original directory
                # ls -d camera_images/"$oldest_dir"
                rm -rf camera_images/"$oldest_dir"

                # Update usage after archiving
                usage=$(df "$IMAGE_DIR" | tail -1 | awk '{print $5}' | sed 's/%//')
                log_message "Disk usage reduced to $usage%"
#            else
#                log_message "Failed to create video from $oldest_dir"
#            fi
#        else
#            log_message "No dated directories found to archive"
#        fi
    fi
}

# Main loop
log_message "Starting image capture script"
#while true; do
    capture_image
    archive_if_over_disk_usage
#    sleep 60
#done

