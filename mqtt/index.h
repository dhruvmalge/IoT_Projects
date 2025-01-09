<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>ESP32 MQTT Control</title>
  <script src="https://cdn.jsdelivr.net/npm/mqtt/dist/mqtt.min.js"></script>
  <style>
    body {
      font-family: Arial, sans-serif;
      text-align: center;
      padding-top: 50px;
      background-color: #f0f0f0;
    }
    h1 {
      color: #333;
    }
    button {
      padding: 10px 20px;
      font-size: 18px;
      cursor: pointer;
      margin: 15px;
      background-color: #007bff;
      color: white;
      border: none;
      border-radius: 5px;
      transition: background-color 0.3s ease;
    }
    button:hover {
      background-color: #0056b3;
    }
    #ldrValue {
      font-size: 24px;
      margin-top: 20px;
      color: #555;
    }
    .status {
      font-size: 18px;
      color: #777;
      margin-top: 10px;
    }
    .status.success {
      color: green;
    }
    .status.error {
      color: red;
    }
  </style>
</head>
<body>

  <h1>ESP32 LED Control</h1>

  <div>
    <button onclick="sendMessage('ON')">Turn LED ON</button>
    <button onclick="sendMessage('OFF')">Turn LED OFF</button>
  </div>

  <div id="ldrValue">LDR Value: Loading...</div>

  <div class="status" id="status">Connecting to MQTT...</div>

  <script>
    // MQTT broker details
    const mqttServer = "wss://broker.emqx.io:8084/mqtt"; // WebSocket URL for MQTT broker
    const topic_led = "esp32/led";  // Topic to control the LED
    const topic_ldr = "esp32/ldr";  // Topic to receive LDR value

    // Create MQTT client instance
    const client = mqtt.connect(mqttServer);

    // Connect to the MQTT broker
    client.on('connect', () => {
      document.getElementById('status').textContent = 'Connected to MQTT Broker';
      document.getElementById('status').classList.remove('error');
      document.getElementById('status').classList.add('success');
      console.log('Connected to MQTT broker');
      
      // Subscribe to the LDR value topic
      client.subscribe(topic_ldr, (err) => {
        if (err) {
          console.log('Subscription failed: ', err);
        } else {
          console.log('Subscribed to topic: ', topic_ldr);
        }
      });
    });

    // Handle incoming messages
    client.on('message', (topic, message) => {
      if (topic === topic_ldr) {
        const ldrValue = message.toString();
        document.getElementById('ldrValue').textContent = `LDR Value: ${ldrValue}`;
      }
    });

    // Function to send control messages to the ESP32 (LED control)
    function sendMessage(message) {
      client.publish(topic_led, message, (err) => {
        if (err) {
          console.log("Failed to send message: ", err);
          document.getElementById('status').textContent = "Failed to send message";
          document.getElementById('status').classList.remove('success');
          document.getElementById('status').classList.add('error');
        } else {
          console.log(`Message sent: ${message}`);
        }
      });
    }

    // Handling connection errors
    client.on('error', (error) => {
      console.log('MQTT Connection Error: ', error);
      document.getElementById('status').textContent = 'Error connecting to MQTT Broker';
      document.getElementById('status').classList.remove('success');
      document.getElementById('status').classList.add('error');
    });

  </script>

</body>
</html>
