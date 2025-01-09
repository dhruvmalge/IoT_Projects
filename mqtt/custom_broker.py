import paho.mqtt.client as mqtt
import time

# Define the broker connection parameters
BROKER = "localhost"
PORT = 1883
TOPIC_SUBSCRIBE = "test/python/rcv"
TOPIC_PUBLISH = "test/python"

# Flag to check if connected to the broker
connected = False

# The callback for when the client receives a CONNACK response from the server.
def on_connect(client, userdata, flags, rc):
    global connected
    if rc == 0:
        print("Connected to broker")
        connected = True
        client.subscribe(TOPIC_SUBSCRIBE)  # Subscribe to topic after connection
    else:
        print(f"Connection failed with result code {rc}")

# The callback for when a PUBLISH message is received from the server.
def on_message(client, userdata, msg):
    print(f"Received message on {msg.topic}: {msg.payload.decode()}")

# Initialize MQTT client
client = mqtt.Client()

# Assign the callbacks
client.on_connect = on_connect
client.on_message = on_message

# Connect to the MQTT broker
client.connect(BROKER, PORT, 60)

# Start the loop in a non-blocking way
client.loop_start()

# Wait for connection to establish
while not connected:
    time.sleep(0.2)

try:
    # Send messages to the broker
    while True:
        message = input("Enter a message to publish: ")
        client.publish(TOPIC_PUBLISH, message)
except KeyboardInterrupt:
    print("Disconnected from broker")
finally:
    # Gracefully disconnect and stop the loop
    client.disconnect()
    client.loop_stop()
