#include <ESP32Servo.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

#include <DHT.h>
#define DHTTYPE DHT22
#define DHTPIN 14

#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid = "Realme 2 Pro";
const char* password = "D@hruv234";
const char* mqtt_server = "test.mosquitto.org"; 
const char* topic_led = "home/commands"; 
const char* topic_ldr = "esp32/ldr";
const char* topic_dht = "esp32/dht";

const int ledPin = 2;  // Pin where LED is connected
const int ldr = 36;    // Pin where LDR is connected
DHT dht(DHTPIN, DHTTYPE);
const int buzzerPin = 12;  // Buzzer pin
const int motorPin = 13;   // Motor pin
const int rgbPin = 14;     // RGB pin
const int relayPin = 15;   // Relay pin
Servo servo;              // Servo motor
const int servoPin = 16;

WiFiClient espClient;
PubSubClient client(espClient);
AsyncWebServer server(80);  // Create web server on port 80

// HTML and JavaScript code for the front-end
const char* htmlContent = R"rawliteral(
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
)rawliteral";

// Setup Wi-Fi connection
void connectWiFi() {
  Serial.print("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  Serial.println(WiFi.localIP());
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(1000);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nConnected to WiFi!");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nFailed to connect to WiFi. Please check credentials.");
  }
}

// MQTT callback function
void callback(char* topic, byte* message, unsigned int length) {
  String msg;
  for (int i = 0; i < length; i++) {
    msg += (char)message[i];
  }
  Serial.print("Message received on topic: ");
  Serial.println(topic);
  Serial.print("Message: ");
  Serial.println(msg);

  if (String(topic) == topic_led) {
    if (msg == "ledon") {
      digitalWrite(ledPin, HIGH);
      Serial.println("LED turned ON");
    } else if (msg == "ledoff") {
      digitalWrite(ledPin, LOW);
      Serial.println("LED turned OFF");
    }
  }
  else if (String(topic) == "home/buzzer") {
    if (msg == "buzzeron") {
      digitalWrite(buzzerPin, HIGH);
      Serial.println("Buzzer ON");
    } else if (msg == "buzzeroff") {
      digitalWrite(buzzerPin, LOW);
      Serial.println("Buzzer OFF");
    }
  }
  else if (String(topic) == "home/motor") {
    if (msg == "motoron") {
      digitalWrite(motorPin, HIGH);
      Serial.println("Motor ON");
    } else if (msg == "motoroff") {
      digitalWrite(motorPin, LOW);
      Serial.println("Motor OFF");
    }
  }
  else if (String(topic) == "home/rgb") {
    if (msg == "rgbon") {
      digitalWrite(rgbPin, HIGH);
      Serial.println("RGB ON");
    } else if (msg == "rgboff") {
      digitalWrite(rgbPin, LOW);
      Serial.println("RGB OFF");
    }
  }
  else if (String(topic) == "home/relay") {
    if (msg == "relayon") {
      digitalWrite(relayPin, HIGH);
      Serial.println("Relay ON");
    } else if (msg == "relayoff") {
      digitalWrite(relayPin, LOW);
      Serial.println("Relay OFF");
    }
  }
  else if (String(topic) == "home/servo") {
    if (msg == "servoon") {
      servo.write(90);  // Set servo to 90 degrees
      Serial.println("Servo ON");
    } else if (msg == "servooff") {
      servo.write(0);  // Set servo to 0 degrees
      Serial.println("Servo OFF");
    }
  }

  // Read LDR value and send to MQTT
  if (String(topic) == topic_ldr){
    int ldrValue = analogRead(ldr);
    String ldrMessage = String(ldrValue);
    String jsonPayload = "{\"LDR\":" + String(ldrValue) + "}";
    if (client.publish(topic_ldr, jsonPayload.c_str())) {
      Serial.println("LDR Value sent to MQTT broker.");
    } else {
      Serial.println("Failed to send LDR value to MQTT broker.");
    }
  }
  

  // Read DHT values (Temperature & Humidity) and send to MQTT
  if (String(topic) == topic_dht){
    float temp = dht.readTemperature();
    float humidity = dht.readHumidity();
    String tempData = String(temp);
    String humidityData = String(humidity);
    String jsonPayload = "{\"temp\":" + String(temp) + ", \"humidity\":" + String(humidity) + "}";
    if (isnan(temp) || isnan(humidity)) {
      Serial.println("Failed to read from sensor");
    } else {
      if (client.publish(topic_dht, jsonPayload.c_str())) {
        Serial.println("DHT Value sent to MQTT broker.");
      } else {
        Serial.println("Failed to send DHT value to MQTT broker.");
      }
    }
  }
}

// MQTT reconnect function
void reconnectMQTT() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP32Client-";
    clientId += String(WiFi.macAddress());

    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      if (client.subscribe(topic_led)) {
        Serial.println("Subscribed to topic successfully");
      } else {
        Serial.println("Failed to subscribe to topic");
      }
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" trying again in 5 seconds");
      delay(5000);
    }
  }
}


void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  pinMode(ldr, INPUT);
  dht.begin();
  connectWiFi();

  // Setup MQTT
  client.setServer(mqtt_server, 1883);  
  client.setCallback(callback);

  // Serve the HTML page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/html", htmlContent);
  });


  server.on("/dht_values", HTTP_GET, [](AsyncWebServerRequest *request){
    float temp = dht.readTemperature();
    float humidity = dht.readHumidity();
    int ldrValue = analogRead(ldr);


    if (isnan(temp) || isnan(humidity)) {
      String errorResponse = "{\"error\":\"Failed to read from DHT sensor\"}";
      request->send(500, "application/json", errorResponse);
      return;
    }

    String response = "{";
    response += "\"temp\":" + String(temp) + ",";
    response += "\"humidity\":" + String(humidity) + ",";
    response += "\"LDR\":" + String(ldrValue);
    response += "}";
    request->send(200, "application/json", response);
});


  server.begin();
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Wi-Fi disconnected! Reconnecting...");
    connectWiFi();  
  }

  if (!client.connected()) {
    reconnectMQTT(); 
  }

  client.loop();  // Process MQTT messages
}
