import json
import paho.mqtt.client as mqtt
from paho.mqtt.enums import CallbackAPIVersion
import pyqtgraph as pg
from pyqtgraph.Qt import QtWidgets, QtCore

def on_connect(client, userdata, flags, reason_code, properties):
    print(f"Connected with result code {reason_code}")
    # Subscribing in on_connect() means that if we lose the connection and
    # reconnect then subscriptions will be renewed.
    client.subscribe("tesis/sensores/ukf")


def on_message(client, userdata, msg):
    try:
        # Decodificar bytes a texto y parsear el JSON
        payload_str = msg.payload.decode('utf-8')
        datos = json.loads(payload_str)
        
        # Extraer las variables usando las mismas claves que definiste en el ESP32
        val_ukf = datos["UKF"]
        val_raw = datos["raw"]
        val_temp = datos["temp"]
        val_hum = datos["hum"]
        
        # (Aquí es donde guardarás los valores en las listas de PyQtGraph)
        print(f"UKF: {val_ukf} | Crudo: {val_raw} | Temp: {val_temp}")
        
    except json.JSONDecodeError:
        print("Error: El paquete recibido no es un JSON válido.")

mqttc = mqtt.Client(CallbackAPIVersion.VERSION2)
mqttc.on_connect = on_connect
mqttc.on_message = on_message

mqttc.connect("192.168.68.65", 1883, 60)

mqttc.loop_start()

# Always start by initializing Qt (only once per application)
app = QtWidgets.QApplication([])

# Define a top-level widget to hold everything
w = QtWidgets.QWidget()
w.setWindowTitle('PyQtGraph example')

# Create some widgets to be placed inside
btn = QtWidgets.QPushButton('press me')
text = QtWidgets.QLineEdit('enter text')
listWidget = QtWidgets.QListWidget()
plot = pg.PlotWidget()

# Create a grid layout to manage the widgets size and position
layout = QtWidgets.QGridLayout()
w.setLayout(layout)

# Add widgets to the layout in their proper positions
layout.addWidget(btn, 0, 0)  # button goes in upper-left
layout.addWidget(text, 1, 0)  # text edit goes in middle-left
layout.addWidget(listWidget, 2, 0)  # list widget goes in bottom-left
layout.addWidget(plot, 0, 1, 3, 1)  # plot goes on right side, spanning 3 rows
# Display the widget as a new window
w.show()

# Start the Qt event loop
app.exec()