import serial
import csv
import time

# --- CONFIGURACION ---
COM_PORT = 'COM3'      # Revisa que sea tu puerto activo
BAUD_RATE = 9600     
FILE_NAME = f"captura_MQ136_BME280_{time.strftime('%Y%m%d_%H%M%S')}.csv"

ser = None 

try:
    # Abrimos el canal de comunicacion. Al hacerlo, el ESP32 se reiniciara 
    # automaticamente, asegurando un inicio limpio de los datos.
    ser = serial.Serial(COM_PORT, BAUD_RATE, timeout=1)
    
    print(f"[{time.strftime('%H:%M:%S')}] Conectado exitosamente a {COM_PORT}.")
    print(f"Guardando datos en el archivo: {FILE_NAME}")
    print("Presiona Ctrl+C en esta consola para detener la captura de forma segura.\n")

    with open(FILE_NAME, mode='w', newline='') as file:
        writer = csv.writer(file)
        # La cabecera la enviara el propio ESP32 al reiniciarse, 
        # asi que no necesitamos escribirla a mano aqui.

        while True:
            if ser.in_waiting > 0:
                linea = ser.readline().decode('utf-8', errors='ignore').strip()
                
                if linea:
                    print(linea) 
                    datos = linea.split(',')
                    writer.writerow(datos)
                    file.flush() 

except serial.SerialException as e:
    print(f"\n[ERROR] No se pudo abrir el puerto {COM_PORT}.")
    print("Asegurate de que el Monitor Serie de Arduino IDE este CERRADO.")
    print(f"Detalle: {e}")

except KeyboardInterrupt:
    print("\n[INFO] Captura detenida. Archivo CSV guardado correctamente.")

finally:
    if ser is not None and ser.is_open:
        ser.close()
        print("Puerto serial liberado.")