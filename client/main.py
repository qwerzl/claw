import serial
import serial.tools.list_ports
import time
import msgpack

def find_esp_port():
    ports = serial.tools.list_ports.comports()
    for port in ports:
        if "ESP" in port.description:
            return port.device
    return None

def decode_msgpack_data(data):
    try:
        unpacked_data = msgpack.unpackb(bytearray.fromhex(data))
        return unpacked_data
    except Exception as e:
        print(f"Failed to decode msgpack data: {e}")
        return None

def read_from_serial(port, baudrate):
    try:
        ser = serial.Serial(port, baudrate, timeout=1)
        print(f"Connected to {port} at {baudrate} baudrate.")
    except serial.SerialException as e:
        print(f"Error: {e}")
        return

    while True:
        try:
            if ser.in_waiting > 0:
                line = ser.readline().decode('utf-8').rstrip()
                print(f"Received: {line}")

                if line.startswith("STATS |"):
                    hex_data = line.split('|')[1].strip()
                    decoded_data = decode_msgpack_data(hex_data)
                    if decoded_data:
                        print(f"Decoded Data: {decoded_data}")

            time.sleep(0.1)
        except KeyboardInterrupt:
            print("Exiting...")
            break
        except Exception as e:
            print(f"Error: {e}")
            break

    ser.close()

if __name__ == "__main__":
    port = find_esp_port()
    if port is None:
        print("No ESP device found.")
    else:
        baudrate = 9600  # Replace with your actual baud rate
        read_from_serial(port, baudrate)
