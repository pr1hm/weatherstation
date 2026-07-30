from flask import Flask, render_template_string, jsonify
import serial
import json
import threading
import time

app = Flask(__name__)

# Replace 'COM3' with the actual COM port of your ESP32
# Ensure the baud rate matches the Serial.begin(9600) in Arduino
try:
    ser = serial.Serial('COM3', 9600, timeout=1)
except Exception as e:
    print(f"Error opening serial port: {e}")
    ser = None

latest_data = {"temperature": "--", "humidity": "--"}

def read_serial():
    global latest_data
    while ser and ser.is_open:
        try:
            line = ser.readline().decode('utf-8').strip()
            if line.startswith("{") and line.endswith("}"):
                latest_data = json.loads(line)
        except json.JSONDecodeError:
            pass # Ignore malformed JSON strings during startup
        except Exception as e:
            time.sleep(1)

# The HTML from your Arduino code
html_template = """
<!DOCTYPE html><html><head>
<meta charset='UTF-8'>
<meta name='viewport' content='width=device-width, initial-scale=1'>
<title>ESP32 Live Weather</title>
<style>
  body { background:#111; color:white; text-align:center; font-family: Arial; }
  h1 { font-size: 32px; margin-top: 30px; }
  p { font-size: 26px; }
</style>
<script>
  function updateData(){
    fetch('/data').then(res => res.json()).then(data => {
      if(data.temperature) document.getElementById('temp').innerHTML = data.temperature;
      if(data.humidity) document.getElementById('hum').innerHTML = data.humidity;
    });
  }
  setInterval(updateData, 1000);
</script>
</head><body>
<h1>ESP32 Live Weather Station</h1>
<p>Temperature: <span id='temp'>--</span> &deg;C</p>
<p>Humidity: <span id='hum'>--</span> %</p>
</body></html>
"""

@app.route('/')
def index():
    return render_template_string(html_template)

@app.route('/data')
def data():
    return jsonify(latest_data)

if __name__ == '__main__':
    if ser:
        # Run the serial reading loop in the background
        threading.Thread(target=read_serial, daemon=True).start()
    app.run(debug=True, use_reloader=False, port=5000)