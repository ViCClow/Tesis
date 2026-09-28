import json
import paho.mqtt.client as mqtt
from paho.mqtt.enums import CallbackAPIVersion
import pyqtgraph as pg
from pyqtgraph.Qt import QtWidgets, QtCore

max_puntos = 300    # Cantidad de puntos a mostrar en el eje X
datos_raw = []      
datos_ukf = []
datos_ref = []      # Para el sensor DFRobot comercial

def on_connect(client, userdata, flags, reason_code, properties):
    print(f"Connected with result code {reason_code}")
    # Subscribing in on_connect() means that if we lose the connection and
    # reconnect then subscriptions will be renewed.
    client.subscribe("tesis/sensores/ukf")


def on_message(client, userdata, msg):
    try:
        payload_str = msg.payload.decode('utf-8')
        datos = json.loads(payload_str)
        
        # ==========================================
        # 2. GUARDAR DATOS EN LAS LISTAS
        # ==========================================
        datos_raw.append(datos["raw"])
        datos_ukf.append(datos["UKF"])
        datos_ref.append(datos["ref"]) 
        
        # Mantener el tamaño de la gráfica constante (ventana móvil)
        if len(datos_raw) > max_puntos:
            datos_raw.pop(0)
            datos_ukf.pop(0)
            datos_ref.pop(0)
            
    except json.JSONDecodeError:
        print("Error: El paquete recibido no es un JSON válido.")
    except KeyError as e:
        print(f"Error: Falta la clave {e} en el JSON")

mqttc = mqtt.Client(CallbackAPIVersion.VERSION2)
mqttc.on_connect = on_connect
mqttc.on_message = on_message

mqttc.connect_async("192.168.68.65", 1883, 60)

mqttc.loop_start()

# Always start by initializing Qt (only once per application)
app = QtWidgets.QApplication([])

# Define a top-level widget to hold everything
w = QtWidgets.QWidget()
w.setWindowTitle('Monitor de sensores')

# Create some widgets to be placed inside
btn = QtWidgets.QPushButton('press me')
text = QtWidgets.QLineEdit('enter text')
listWidget = QtWidgets.QListWidget()
plot = pg.PlotWidget(title = "Concentración de gas (ppm) - UKF vs Raw vs Ref")

# Configurar aspecto visual de la gráfica
plot.addLegend()
plot.showGrid(x=True, y=True)

# Crear curvas para cada conjunto de datos
curva_raw = plot.plot(pen='r', name='MQ136 (Crudo)')    # Rojo
curva_ukf = plot.plot(pen='g', name='UKF (Procesado)')  # Verde
curva_ref = plot.plot(pen='c', name='DFRobot (Ref)')    # Cyan

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

def actualizar_grafica():
    # Esta función inyecta los datos históricos a las curvas para redibujarlas
    curva_raw.setData(datos_raw)
    curva_ukf.setData(datos_ukf)
    curva_ref.setData(datos_ref)

# Configurar el reloj interno para llamar a la función cada 50 milisegundos
timer = QtCore.QTimer()
timer.timeout.connect(actualizar_grafica)
timer.start(50)

# Start the Qt event loop
app.exec()