#include <WiFi.h>
#include <PubSubClient.h>
#define potPin A0

static float miniVolDifference = 0.84 / 868;
int val = 0;
float pa = 0;

static char ssid[] = "RichMan";       // your network SSID (name)
static char pass[] = "richmansheuct"; // your network password
int status = WL_IDLE_STATUS;          // the Wifi radio's status

static char mqttServer[] = "broker.hivemq.com";
static char clientId[] = "amebaClient";
static char publishTopic[] = "patricksheu/outTopic";
String msgStr = "";
char json[25];

WiFiClient wifiClient;
PubSubClient client(wifiClient);

// MQTT重新連線
void reconnect()
{
  // Loop until we're reconnected
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(clientId))
    {
      Serial.println("connected");
      // Once connected, publish an announcement...
      // client.publish(publishTopic, publishPayload);
      // ... and resubscribe
      // client.subscribe(subscribeTopic);
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

double getPressure()
{
  val = analogRead(potPin);
  return pa = (val * miniVolDifference - 0.15) * 1000000;
  // return pa = (val * miniVolDifference * 0.3 - 0.15) * 1000000;
}

void setup()
{
  Serial.begin(9600);
  while (!Serial)
  {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  //連線WiFi
  while (status != WL_CONNECTED)
  {
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass); //連線wifi

    // wait 10 seconds for connection:
    delay(10000);
  }

  client.setServer(mqttServer, 1883); //設定mqtt
  // client.setCallback(callback);

  // Allow the hardware to sort itself out
  delay(1500);
}

void loop()
{
  if (!client.connected())
  {
    reconnect();
  }
  client.loop();

  msgStr = msgStr + "{\"pressure\":" + getPressure() + "}";
  msgStr.toCharArray(json, 30);
  client.publish(publishTopic, json);
  msgStr = "";

  delay(30000);
}
