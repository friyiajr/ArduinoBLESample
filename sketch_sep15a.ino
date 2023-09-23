#include <ArduinoBLE.h>
#include <Servo.h>
#include <string>  
#include <vector>  

Servo myServo;
int prevAngle = -1;

const char * deviceServiceUuid = "19b10000-e8f2-537e-4f6c-d104768a1214";
const char * deviceServiceRequestCharacteristicUuid = "19b10001-e8f2-537e-4f6c-d104768a1215";
const char * deviceServiceResponseCharacteristicUuid = "19b10001-e8f2-537e-4f6c-d104768a1216";

BLEService servoService(deviceServiceUuid);
BLEStringCharacteristic servoRequestCharacteristic(deviceServiceRequestCharacteristicUuid, BLEWrite, 4);
BLEStringCharacteristic servoResponseCharacteristic(deviceServiceResponseCharacteristicUuid, BLENotify, 4);

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
  servoService.addCharacteristic(servoRequestCharacteristic);
  servoService.addCharacteristic(servoResponseCharacteristic);
  BLE.addService(servoService);
  servoResponseCharacteristic.writeValue("0");

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
      if (servoRequestCharacteristic.written()) {
        String newAngle = servoRequestCharacteristic.value();
        int value = newAngle.toInt();

        if (prevAngle != value) {
          myServo.write(value);
          delay(20);
          servoResponseCharacteristic.setValue(newAngle);
          Serial.println(value);
          Serial.println();
        }
      }
    }
    Serial.println("* Disconnected to central device!");
  }
}