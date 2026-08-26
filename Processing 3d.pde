import processing.serial.*;

Serial myPort;

float roll = 0;
float pitch = 0;

void setup() {

  size(1000, 700, P3D);

  println(Serial.list());

  // Your Arduino is COM26
  myPort = new Serial(this, "COM26", 115200);

  myPort.bufferUntil('\n');
}

void draw() {

  background(30);

  lights();

  // -------------------------
  // Text
  // -------------------------

  fill(255);
  textSize(30);

  text("MPU6050 Virtual 3D", 30, 45);

  textSize(20);

  text(
    "Roll  : " + nf(roll, 1, 2) + "°",
    30,
    85
  );

  text(
    "Pitch : " + nf(pitch, 1, 2) + "°",
    30,
    120
  );


  // -------------------------
  // 3D BOARD
  // -------------------------

  pushMatrix();

  translate(
    width / 2,
    height / 2
  );

  // MPU6050 rotation
  rotateX(radians(pitch));
  rotateZ(radians(-roll));


  // Main board
  fill(40, 150, 220);

  box(
    300,
    50,
    200
  );


  // MPU6050 chip
  pushMatrix();

  translate(0, -35, 0);

  fill(40);

  box(
    80,
    15,
    80
  );

  popMatrix();


  // Front indicator
  pushMatrix();

  translate(0, 0, -105);

  fill(255, 50, 50);

  box(
    120,
    25,
    10
  );

  popMatrix();


  // Top indicator
  pushMatrix();

  translate(0, -30, 0);

  fill(50, 255, 100);

  box(
    100,
    10,
    60
  );

  popMatrix();


  popMatrix();


  // Instructions

  fill(255);

  textSize(18);

  text(
    "Tilt the physical MPU6050",
    30,
    height - 50
  );
}


// -------------------------
// Serial data
// -------------------------

void serialEvent(Serial port) {

  String data =
    port.readStringUntil('\n');

  if (data == null) {
    return;
  }

  data = trim(data);

  println(data);

  if (data.startsWith("R:")) {

    int comma =
      data.indexOf(",P:");

    if (comma > 0) {

      String rollValue =
        data.substring(2, comma);

      String pitchValue =
        data.substring(comma + 3);

      try {

        roll =
          Float.parseFloat(rollValue);

        pitch =
          Float.parseFloat(pitchValue);

      } catch (Exception e) {

        println("Invalid data: " + data);
      }
    }
  }
}
