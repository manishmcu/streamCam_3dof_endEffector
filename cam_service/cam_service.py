import subprocess

cam_driver = "/dev/video2"  # Replace with the correct device path
current_ip = "192.168.1.20" 

# Adjust ffmpeg parameters as needed
ffmpeg_command = f"ffmpeg -f v4l2 -i {cam_driver} -c:v libx264 -b:v 500k -s 320x240 -r 15 -f rtsp rtsp://{current_ip}:8554/mystream"

# Run ./mediamtx in a separate process
mediamtx_process = subprocess.Popen(["./mediamtx"]) 

# Run ffmpeg in a separate process
ffmpeg_process = subprocess.Popen(ffmpeg_command.split()) 

# Wait for both processes to complete (optional)
mediamtx_process.wait()
ffmpeg_process.wait()
