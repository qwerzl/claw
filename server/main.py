import paho.mqtt.client as mqtt
import msgpack
from utils.gpt import get_gpt_response
from utils.printer.label import print_markdown
import zmq

context = zmq.Context()

# MQTT settings
MQTT_BROKER = "127.0.0.1"
MQTT_PORT = 1883
MQTT_TOPIC = "out"

data = []

# Function to decode MsgPack data after stripping "STATS |" prefix
def decode_msgpack_data(payload):
    try:
        # Convert the payload to a string and check for the "STATS |" prefix
        message = payload.decode('utf-8')

        if message.startswith("STATS |"):
            hex_data = message.split('|')[1].strip()  # Get the MsgPack part
            unpacked_data = msgpack.unpackb(bytearray.fromhex(hex_data))  # Decode MsgPack data
            return unpacked_data
        elif "EM Off" in message:
            return 0
        # elif "EM On" in message:
        #     return 1
        else:
            print(message)
            print("Invalid message format, skipping...")
            return None
    except Exception as e:
        print(f"Failed to decode msgpack data: {e}")
        return None

# Callback when the client receives a message from the broker
def on_message(client, userdata, message):
    print(f"Received message on topic {message.topic}")

    payload = message.payload
    decoded_data = decode_msgpack_data(payload)

    if decoded_data == 0:
        print("-------------------------")
        print(data)
        response = get_gpt_response(data).choices[0].message.content
        print(response)

        # First line of response is the movie choice
        choice = response.split("\n")[0]
        if "Alice" in choice:
            socket.send(b"0")
        elif "Inception" in choice:
            socket.send(b"1")
        elif "Sisyphus" in choice:
            socket.send(b"2")
        elif "Vertigo" in choice:
            socket.send(b"3")

        print_markdown(response.split("\n", 1)[1])
        data.clear()
    elif decoded_data:
        data.append(decoded_data)
        print("Decoded data:", data)

# Set up MQTT client
def setup_mqtt_client():
    _client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)

    _client.on_message = on_message
    # _client.on_log = lambda c, u, level, buf: print(f"MQTT log: {buf}")

    try:
        _client.connect(MQTT_BROKER, MQTT_PORT)
        _client.subscribe(MQTT_TOPIC)
        print(f"Connected to MQTT broker at {MQTT_BROKER} on topic '{MQTT_TOPIC}'")

        return _client
    except Exception as e:
        print(f"Failed to connect to MQTT broker: {e}")
        return None

# Main loop for MQTT client
def start_mqtt_loop(client):
    try:
        client.loop_forever()  # Blocking call, processes network traffic and dispatches callbacks
    except KeyboardInterrupt:
        print("Exiting...")
    except Exception as e:
        print(f"Error: {e}")
    finally:
        client.disconnect()

print("Connecting to zmq server…")
socket = context.socket(zmq.REQ)
socket.connect("tcp://localhost:5555")

# Initialize MQTT client and start listening
client = setup_mqtt_client()
if client:
    start_mqtt_loop(client)