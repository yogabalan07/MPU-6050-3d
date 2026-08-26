#include <Wire.h>

#define MPU_ADDR 0x68

float roll = 0;
float pitch = 0;

float gyroXOffset = 0;
float gyroYOffset = 0;

unsigned long previousTime;

void setup() {

  Serial.begin(115200);
  Wire.begin();

  // Wake MPU6050
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x6B);
  Wire.write(0x00);
  Wire.endTransmission();

  // Accelerometer ±2g
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x1C);
  Wire.write(0x00);
  Wire.endTransmission();

  // Gyroscope ±250°/s
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x1B);
  Wire.write(0x00);
  Wire.endTransmission();

  delay(1000);

  calibrateGyro();

  previousTime = micros();

  Serial.println("MPU6050_READY");
}


void loop() {

  int16_t AcX, AcY, AcZ;
  int16_t GyX, GyY, GyZ;

  readMPU(AcX, AcY, AcZ, GyX, GyY, GyZ);

  // Convert accelerometer
  float ax = AcX / 16384.0;
  float ay = AcY / 16384.0;
  float az = AcZ / 16384.0;

  // Accelerometer angles
  float accRoll = atan2(ay, az) * 180.0 / PI;

  float accPitch =
    atan2(
      -ax,
      sqrt(ay * ay + az * az)
    ) * 180.0 / PI;


  // Gyroscope
  float gyroX =
    (GyX - gyroXOffset) / 131.0;

  float gyroY =
    (GyY - gyroYOffset) / 131.0;


  // Time
  unsigned long currentTime = micros();

  float dt =
    (currentTime - previousTime)
    / 1000000.0;

  previousTime = currentTime;


  // Integrate gyro
  roll += gyroX * dt;
  pitch += gyroY * dt;


  // Complementary filter
  roll =
    0.98 * roll +
    0.02 * accRoll;

  pitch =
    0.98 * pitch +
    0.02 * accPitch;


  // Send data to PC

  Serial.print("R:");
  Serial.print(roll, 2);

  Serial.print(",P:");
  Serial.println(pitch, 2);

  delay(10);
}


// ==============================
// READ MPU6050
// ==============================

void readMPU(
  int16_t &AcX,
  int16_t &AcY,
  int16_t &AcZ,
  int16_t &GyX,
  int16_t &GyY,
  int16_t &GyZ
) {

  Wire.beginTransmission(MPU_ADDR);

  Wire.write(0x3B);

  Wire.endTransmission(false);

  Wire.requestFrom(MPU_ADDR, 14, true);

  AcX = Wire.read() << 8 | Wire.read();
  AcY = Wire.read() << 8 | Wire.read();
  AcZ = Wire.read() << 8 | Wire.read();

  // Temperature
  Wire.read();
  Wire.read();

  GyX = Wire.read() << 8 | Wire.read();
  GyY = Wire.read() << 8 | Wire.read();
  GyZ = Wire.read() << 8 | Wire.read();
}


// ==============================
// GYRO CALIBRATION
// ==============================

void calibrateGyro() {

  long gxSum = 0;
  long gySum = 0;

  Serial.println("CALIBRATING...");
  Serial.println("KEEP MPU6050 STILL!");

  delay(2000);

  for (int i = 0; i < 1000; i++) {

    int16_t ax, ay, az;
    int16_t gx, gy, gz;

    readMPU(
      ax, ay, az,
      gx, gy, gz
    );

    gxSum += gx;
    gySum += gy;

    delay(2);
  }

  gyroXOffset =
    gxSum / 1000.0;

  gyroYOffset =
    gySum / 1000.0;

  Serial.println("CALIBRATION COMPLETE");
}
