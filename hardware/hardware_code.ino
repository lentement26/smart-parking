#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>
#include <Wire.h> //Gọi thư viện I2C để sử dụng các thư viện I2C

#include <LiquidCrystal_I2C.h> //Thư viện LCD I2C
LiquidCrystal_I2C lcd(0x27,16,2); //Khai báo địa chỉ I2C (0x27 or 0x3F) và LCD 16x02

Servo myservo1;
Servo myservo2;

#define SS_PIN 53  // Slave Select Pin 
#define RST_PIN 5  // Reset Pin 
const int numSensors = 8;
const int sensorPins[numSensors] = {13, 12, 11, 10, 9, 8, 4, 3};  // Chân kết nối các cảm biến hồng ngoại
int parkingSpaces[numSensors];  // Mảng lưu trữ trạng thái của từng vị trí đỗ xe
int sothutu;
int vitri = 15;
int maso = 1;
int UID[4], i;

// Create an instance of MFRC522 
MFRC522 mfrc522(SS_PIN, RST_PIN);
// Create an instance of MIFARE_Key
MFRC522::MIFARE_Key key;          

int blockNum = 4;  // block de luu du lieu

//Create an array of 16 Bytes and fill it with data 
// This is the actual data which is going to be written into the card 
byte blockData [16] = {1,8,7,6,5,4,   23,4,20,24,10,29,  12,13,14,15};
byte blockXoa [16] = {0,8,7,6,5,4,   0,0,0,0,0,0,  0,0,0,0};
// Create another array to read data from Block 
// Legthn of buffer should be 2 Bytes more than the size of Block (16 Bytes) 
byte bufferLen = 18;
byte readBlockData[18];

MFRC522::StatusCode status;

void setup() 
{
  //Initialize serial communications with the PC //
  Serial.begin(9600);
  // Initialize SPI bus
  SPI.begin();
  // Initialize MFRC522 Module 
  mfrc522.PCD_Init();
  // Prepare the key (used both as key A and as key B)
  // using FFFFFFFFFFFFh which is the default at chip delivery from the factory
  for (byte i = 0; i < 6; i++) {
        key.keyByte[i] = 0xFF;
    }
  lcd.init(); //Khởi tạo màn hình LCD
  lcd.backlight(); //Bật đèn màn hình lCD
  for (int i = 0; i < numSensors; i++) {
    pinMode(sensorPins[i], INPUT);  // Cấu hình chân cảm biến là INPUT
    parkingSpaces[i] = 1;  // Khởi tạo tất cả các vị trí đỗ xe là trống (1)
  }
  Serial.println("Scan a MIFARE 1K Tag to write data...");

    
  
    myservo1.attach(6);
    myservo1.write(1); 
    
    myservo2.attach(7);
    myservo2.write(1); 

  
}

void loop()
{
  
  /* In ra man led so luong cho trong*/
 int availableSpaces = 0;  // Biến đếm số chỗ trống

  for (int i = 0; i < numSensors; i++) {
    int sensorValue = digitalRead(sensorPins[i]);  // Đọc giá trị từ cảm biến hồng ngoại

    if (sensorValue == LOW && parkingSpaces[i] == 1) {
      parkingSpaces[i] = 0;  // Đã có xe đỗ tại vị trí i, đánh dấu là không trống (0)
    } else if (sensorValue == HIGH && parkingSpaces[i] == 0) {
      parkingSpaces[i] = 1;  // Xe đã di chuyển ra khỏi vị trí i, đánh dấu là trống (1)
    }

    availableSpaces += parkingSpaces[i];
  }

  Serial.print("Available parking spaces: ");
  Serial.println(availableSpaces);  // In ra số chỗ trống còn lại trên Serial Monitor

  delay(1000);  // Đợi 1 giây trước khi đọc giá trị cảm biến lần tiếp theo
  lcd.setCursor (0,0);
  lcd.print("    WELCOME!    ");
  lcd.setCursor (0,1);
  lcd.print("Slot Left: ");
  lcd.print(availableSpaces);
  /*RFID*/
if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
   

  Serial.print("UID của thẻ: ");   
  
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  { 
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");   
    UID[i] = mfrc522.uid.uidByte[i]; // Gán ID thẻ vào UID
    Serial.print(UID[i]);
  }

  Serial.println("   ");
// Read data from the same block 
   Serial.print("\n");
   Serial.println("Reading from Data Block...");
   ReadDataFromBlock(blockNum, readBlockData);
   
   // Print the data read from block 
   Serial.print("\n");
   Serial.print("Data in Block:");
   Serial.print(blockNum);
   Serial.print(" --> ");
   for (int j=0 ; j<16 ; j++)
   {
     Serial.print(readBlockData[j]);
   }
   Serial.print("\n");

  
  Serial.print("\n");
  Serial.println("**Card Detected**");
  // Print UID of the Card 
  Serial.print(F("Card UID:"));
  for (byte i = 0; i < mfrc522.uid.size; i++)
  {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
  }
  Serial.print("\n");
  // Print type of card (for example, MIFARE 1K) 
  Serial.print(F("PICC type: "));
  MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
  Serial.println(mfrc522.PICC_GetTypeName(piccType));

  /*ĐI RA*/
 
  if(( readBlockData[0] == 1)&&(readBlockData[1] == 8) && (readBlockData[2] == 7) && (readBlockData[3] == 6) && (readBlockData[4] == 5))
  {
      sothutu = readBlockData[12] + readBlockData[13] + readBlockData[14] + readBlockData[15];
      Serial.print("Thời gian: ");
      Serial.print(readBlockData[10]);
      Serial.print(":");
      Serial.println(readBlockData[11]);
      Serial.print("Ngày: ");
      Serial.print(readBlockData[6]);
      Serial.print("/");
      Serial.print(readBlockData[7]);
      Serial.print("/");
      Serial.print(readBlockData[8]);
      Serial.println(readBlockData[9]);
      Serial.print("Số thứ tự: ");
      Serial.println(sothutu);
      Serial.print("CỬA MỞ ĐI RA");
      
      
//XOÁ THẺ
     Serial.print("\n");
     Serial.println("Writing to Data Block...");
     WriteDataToBlock(blockNum, blockXoa);
    
     myservo2.write(90); // servo quay - mở rào chắn
     Serial.println(" OPEN ");  
     delay(5000);
     Serial.println(" CLOSE ");  
     myservo2.write(1);
     
  }

  else if ( ( readBlockData[0] == 0)&& (readBlockData[1] == 8) && (readBlockData[2] == 7) && (readBlockData[3] == 6) && (readBlockData[4] == 5) )
  {
    //KHI ĐI VÀO
    
    if(vitri >= 12)
    {
      if(maso <= 255)
      { 
          blockData[vitri] = maso;
          maso++;
      }
      else
      {   
          maso = 1;
          vitri--;
          blockData[vitri] = maso;
      }
    }
    else
    {
       Serial.print(" Bãi đỗ xe đã đầy xe");
    }

     // Call 'WriteDataToBlock' function, which will write data to the block
   Serial.print("\n");
   Serial.println("Writing to Data Block...");
   WriteDataToBlock(blockNum, blockData);
   Serial.print("CỬA MỞ ĐI VÀO");
   
   myservo1.write(90); // servo quay - mở rào chắn
   Serial.println(" OPEN ");  
   delay(5000);
   Serial.println(" CLOSE ");  
   myservo1.write(1);
   return 0;
   
  }

  else
  {
   Serial.println("SAI THẺ");
  }
}
 mfrc522.PICC_HaltA();  // Hàm thủ tục
 mfrc522.PCD_StopCrypto1(); // Hàm thủ tục

}


// ham viet du lieu vao the
void WriteDataToBlock(int blockNum, byte blockData[]) 
{

  for (byte i = 0; i < 6; i++)
  {
    key.keyByte[i] = 0xFF;
  }
  /* Authenticating the desired data block for write access using Key A */
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, blockNum, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK)
  {
    Serial.print("Authentication failed for Write: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }
  else
  {
    Serial.println("Authentication success");
  }

  
  /* Write data to the block */
  status = mfrc522.MIFARE_Write(blockNum, blockData, 16);
  if (status != MFRC522::STATUS_OK)
  {
    Serial.print("Writing to Block failed: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }
  else
  {
    Serial.println("Data was written into Block successfully");
  }
  
}

//ham doc du lieu tu the
void ReadDataFromBlock(int blockNum, byte readBlockData[]) 
{

  for (byte i = 0; i < 6; i++)
  {
   key.keyByte[i] = 0xFF;
  }
  /* Authenticating the desired data block for Read access using Key A */
  byte status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, blockNum, &key, &(mfrc522.uid));

  if (status != MFRC522::STATUS_OK)
  {
     Serial.print("Authentication failed for Read: ");
     Serial.println(mfrc522.GetStatusCodeName(status));
     return;
  }
  else
  {
    Serial.println("Authentication success");
  }

  /* Reading data from the Block */
  status = mfrc522.MIFARE_Read(blockNum, readBlockData, &bufferLen);
  if (status != MFRC522::STATUS_OK)
  {
    Serial.print("Reading failed: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }
  else
  {
    Serial.println("Block was read successfully");  
   
  }

}
