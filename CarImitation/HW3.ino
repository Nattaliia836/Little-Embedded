#include <mcp_can.h>
#include <SPI.h>

unsigned long timing;

long unsigned int rxId;
unsigned char len = 0;
unsigned char rxBuf[8];
char msgString[128];                        // Array to store serial string

#define CAN0_INT 2                              // Set INT to pin 2
MCP_CAN CAN0(9);

byte KL_Status = 0x00;    // Set default OFF
byte KEY_Status = 0x00;   // Set default Out Of Range
byte GEAR_Pos = 0x00;     // Set default Unknown
byte MOTOR_Status = 0x00;
bool SYSTEM_Locked = false;
bool Motor_Works = false;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  if (CAN0.begin(MCP_ANY, CAN_500KBPS, MCP_8MHZ) == CAN_OK)
    Serial.println("MCP2515 Initialized Successfully!");
  else
    Serial.println("Error Initializing MCP2515...");

  CAN0.setMode(MCP_NORMAL);
  pinMode(CAN0_INT, INPUT);
}

void doSomething () {

}

void loop() {
  // put your main code here, to run repeatedly:
  if (!digitalRead(CAN0_INT))                        // If CAN0_INT pin is low, read receive buffer
  {
    CAN0.readMsgBuf(&rxId, &len, rxBuf);      // Read data: len = data length, buf = data byte(s)



    if ((rxId & 0x80000000) == 0x80000000)    // Determine if ID is standard (11 bits) or extended (29 bits)
      sprintf(msgString, "Extended ID: 0x%.8lX  DLC: %1d  Data:", (rxId & 0x1FFFFFFF), len);
    else
      sprintf(msgString, "Standard ID: 0x%.3lX       DLC: %1d  Data:", rxId, len);

    //Serial.print(msgString);

    if ((rxId & 0x40000000) == 0x40000000) {  // Determine if message is a remote request frame.
      sprintf(msgString, " REMOTE REQUEST FRAME");
      Serial.print(msgString);
    } else {
      for (byte i = 0; i < len; i++) {

        sprintf(msgString, " 0x%.2X", rxBuf[i]);
        //Serial.print(msgString);
      }
    }

    //Serial.println();


    switch (rxId) {

      // KEY_Status from SMLS
      case 400:
        //BCM
        if (MOTOR_Status == 0x01 && (rxBuf[0] == 0x00)) {
          MOTOR_Status = 0x00;
          GEAR_Pos = 0x01;         // 0x01 == Park
          SYSTEM_Locked = true;

          KL_Status = 0xFF;
          //KlemmenStatus_01[2] = 0xFF ??
        }
        KEY_Status = byte(rxBuf[0]);
        break;

      // KL_Status from SecurityStatus
      case 450:
        //BCM

        if (rxBuf[0] == 0x00) {           // OFF
          KL_Status = 0x00;
          MOTOR_Status = 0x00;
        }
        else if (rxBuf[0] == 0x01) {      // KL_S
          KL_Status = 0x01;
        }
        else if (rxBuf[0] == 0x03) {      // KL_15
          if (KL_Status == 0xFF) {
            if (KEY_Status == 0x02) {
              KL_Status = 0x03;
              SYSTEM_Locked = false;
            }
            else {
              Serial.println("Error! System LOCKED! To unlock the system, put the key inside the car!");
            }
          }
          else if (KEY_Status == 0x01 || KEY_Status == 0x02) {
            KL_Status = 0x00;
          }
          else {
            Serial.println("Error! To activate KL_15 you need to have KEY = 0x01 OR KEY = 0x02. You have KEY = 0x00.");
          }
        }
        else if (rxBuf[0] == 0x0B) {      // KL_50
          if (KL_Status == 0x03 && GEAR_Pos == 0x02) {
            KL_Status = 0x0B;
            MOTOR_Status = 0x01;
            KL_Status = 0x03;
            Motor_Works = true;
          }
          else {
            if (KL_Status == 0xFF) {
              Serial.println("Error! System LOCKED! To unlock the system, put the key inside the car!");
            }
            else {
              Serial.println("Error! To activate KL_50 you need to have KL = 0x03 AND Gear_Pos = 0x02");
              Serial.print("You have KL = ");
              Serial.print(KL_Status);
              Serial.print(", Gear_Pos = ");
              Serial.println(GEAR_Pos);
              Serial.println();
            }
          }
        }
        break;

      // GEAR_Pos from SecurityStatus
      case 460:
        if (SYSTEM_Locked && KEY_Status != 0x02) {
          Serial.print("Error! To change the position you need to have the key in the car");
        } else {
          GEAR_Pos = rxBuf[1];
        }
        break;

      // WK (who knows)
      case 700:
        //some code
        break;
      default:
        Serial.println("Unknown ID");
        break;
    }
  }

  //===============================================================================================
  //BCM
  {
    if (millis() - timing > 100) {
      timing = millis();
    }
    byte data[4] = {0x00, 0x00, KL_Status, 0x00};
    byte KlemmenStatus_01 = CAN0.sendMsgBuf(0x3C0, 0, 4, data);
    if (KlemmenStatus_01 == CAN_OK) {
      Serial.println("KlemmenStatus_01 Sent Successfully!");
    } else {
      Serial.println("Error Sending KlemmenStatus_01...");
    }
  }

  //===============================================================================================
  //GEAR_BOX
  if (KL_Status == 0x03) {
    if (millis() - timing > 250) {
      timing = millis();
    }
    byte data[8] = {0x00, 0x00, GEAR_Pos, 0x00, 0x00, 0x00, 0x00, 0x00};
    byte GearBoxStatus_01 = CAN0.sendMsgBuf(0x501, 0, 8, data);
    if (GearBoxStatus_01 == CAN_OK) {
      Serial.println("GearBoxStatus_01 Sent Successfully!");
    } else {
      Serial.println("Error Sending GearBoxStatus_01...");
    }
  }

  //===============================================================================================
  //MOTOR

  if (KL_Status == 0x03) {
    if (!Motor_Works) {
      if (millis() - timing > 50) {
        timing = millis();
      }
      Motor_Works = false;
    }
    byte data[8] = {0x00, MOTOR_Status, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    byte MotorStatus_01 = CAN0.sendMsgBuf(0x500, 0, 8, data);
    if (MotorStatus_01 == CAN_OK) {
      Serial.println("MotorStatus_01 Sent Successfully!");
    } else {
      Serial.println("Error Sending MotorStatus_01...");
    }
  }
}
