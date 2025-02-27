import serial
import threading
from flask import Flask, request, jsonify, render_template

# Flask app
app = Flask(__name__)

# Serial port configuration
SERIAL_PORT = "/dev/SHELF_ESP"  # Update with your serial port
BAUD_RATE = 115200
ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)

# Shared data
serial_data = []

def read_serial_data():
    while True:
        if ser.in_waiting > 0:
            try:
                # Attempt to read the line, ignoring any invalid UTF-8 characters
                line = ser.readline().decode('utf-8', errors='ignore').strip()
                
                # If there's any content after decoding, append it to the data list
                if line:
                    serial_data.append(line)
                    print(f"Received from serial: {line}")
            except Exception as e:
                print(f"Error reading serial: {e}")


@app.route('/')
def index():
    return render_template('index.html')

@app.route('/send_serial', methods=['POST'])
def send_serial():
    if request.method == 'POST':
        try:
            command = request.json.get("data", "").strip()
            if command:
                ser.write(f"{command}\n".encode('utf-8'))
                # Return a success response with a proper message
                return jsonify({"status": "success", "message": f"Command sent: {command}"}), 200
            else:
                # If no command is provided, return an error
                return jsonify({"status": "error", "message": "No command provided"}), 400
        except Exception as e:
            # Return a JSON error message if something goes wrong
            return jsonify({"status": "error", "message": f"Failed to send command: {str(e)}"}), 500

@app.route('/get_serial_data', methods=['GET'])
def get_serial_data():
    # Return the last 10 lines of serial data, or all data if less than 10
    return jsonify(serial_data[-10:])

# Start the serial reading thread
serial_thread = threading.Thread(target=read_serial_data, daemon=True)
serial_thread.start()

if __name__ == '__main__':
    print(f"Connected to serial port: {SERIAL_PORT} at {BAUD_RATE} baud")
    app.run(host='0.0.0.0', port=12346)

