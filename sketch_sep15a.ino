#include <ArduinoBLE.h>
#include <Servo.h>
#include <string>  
#include <vector>  

Servo myServo;
int prevAngle = -1;

const char * deviceServiceUuid = "19b10000-e8f2-537e-4f6c-d104768a1214";
const char * deviceServiceCharacteristicUuid = "19b10001-e8f2-537e-4f6c-d104768a1215";
const char * deviceVersionCharacteristicUuid = "19b10001-e8f2-537e-4f6c-d104768a1216";

BLEService servoService(deviceServiceUuid);
BLEStringCharacteristic servoCharacteristic(deviceServiceCharacteristicUuid, BLEWrite | BLENotify, 4);

void setup() {
  myServo.attach(9);
  Serial.begin(9600);

  BLE.setDeviceName("Friyia");
  BLE.setLocalName("Friyia");

  if (!BLE.begin()) {
    Serial.println("- Starting Bluetooth® Low Energy module failed!");
    while (1);
  }

  BLE.setAdvertisedService(servoService);
  servoService.addCharacteristic(servoCharacteristic);
  BLE.addService(servoService);
  servoCharacteristic.writeValue("0");
  BLE.advertise();

  Serial.println("Arduino R4 WiFi BLE (Peripheral Device)");
  Serial.println(" ");
}

void loop() {
  BLEDevice central = BLE.central();
  Serial.println("- Discovering central device...");
  delay(500);

  if (central) {
    Serial.println("* Connected to central device!");
    Serial.print("* Device MAC address: ");
    Serial.println(central.address());
    Serial.println(" ");

    while (central.connected()) {
      if (servoCharacteristic.written()) {
        String newAngle = servoCharacteristic.value();
        int value = newAngle.toInt();

        if (prevAngle != value) {
          myServo.write(value);
          delay(20);
          servoCharacteristic.writeValue(newAngle);
          Serial.println(value);
          Serial.println();
        }
      }
    }
    Serial.println("* Disconnected to central device!");
  }
}