#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <DHT.h>

int n;
int D3;
int sensor_value;
int readD2;
int Pin_D0 = 16;
int Pin_D4 = 2;


Adafruit_MPU6050 mpu; //MPU6050

#define minval -10
#define maxval 10
int Buzzer1 = 15;

int Buzzer2 = 12;   //Heartsensor
int Signal;
int Threshold = 550;

int Buzzer = 13;
const int sensor = A1; //MQ06 sensor and Heart sensor bcause only one analog pin
int sensor_limit = 2000;
#define DHTPIN   D3
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

int analogRead1() {
  digitalWrite(Pin_D0, HIGH); // Turn D1 On
  digitalWrite(Pin_D4, LOW); // Turn D2 Off
  return analogRead(0);
}

int analogRead2() {
  digitalWrite(Pin_D0, LOW); //  Turn D1 On
  digitalWrite(Pin_D4, HIGH); // Turn D2 Off
  return analogRead(0);
}

void setup()
{
  Serial.begin(9600);        //Temperature sensor
  dht.begin();


  pinMode(Buzzer2, OUTPUT);     //heartbeat!
  pinMode(Pin_D4, OUTPUT);
  pinMode(Pin_D0, OUTPUT);
  delay(1000);

  pinMode(Buzzer, OUTPUT); //MQ06 sensor
  pinMode(sensor, INPUT);

  pinMode(Buzzer1, OUTPUT);  //MPU6050

  while (!Serial)
    delay(10); // will pause Zero, Leonardo, etc until serial console opens
  Serial.println("Adafruit MPU6050 test!");
  // Try to initialize!
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  Serial.println("MPU6050 Found!");

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  Serial.print("Accelerometer range set to: ");
  switch (mpu.getAccelerometerRange()) {
    case MPU6050_RANGE_2_G:
      Serial.println("+-2G");
      break;
    case MPU6050_RANGE_4_G:
      Serial.println("+-4G");
      break;
    case MPU6050_RANGE_8_G:
      Serial.println("+-8G");
      break;
    case MPU6050_RANGE_16_G:
      Serial.println("+-16G");
      break;
  }
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  Serial.print("Gyro range set to: ");
  switch (mpu.getGyroRange()) {
    case MPU6050_RANGE_250_DEG:
      Serial.println("+- 250 deg/s");
      break;
    case MPU6050_RANGE_500_DEG:
      Serial.println("+- 500 deg/s");
      break;
    case MPU6050_RANGE_1000_DEG:
      Serial.println("+- 1000 deg/s");
      break;
    case MPU6050_RANGE_2000_DEG:
      Serial.println("+- 2000 deg/s");
      break;
  }

  mpu.setFilterBandwidth(MPU6050_BAND_5_HZ);
  Serial.print("Filter bandwidth set to: ");
  switch (mpu.getFilterBandwidth()) {
    case MPU6050_BAND_260_HZ:
      Serial.println("260 Hz");
      break;
    case MPU6050_BAND_184_HZ:
      Serial.println("184 Hz");
      break;
    case MPU6050_BAND_94_HZ:
      Serial.println("94 Hz");
      break;
    case MPU6050_BAND_44_HZ:
      Serial.println("44 Hz");
      break;
    case MPU6050_BAND_21_HZ:
      Serial.println("21 Hz");
      break;
    case MPU6050_BAND_10_HZ:
      Serial.println("10 Hz");
      break;
    case MPU6050_BAND_5_HZ:
      Serial.println("5 Hz");
      break;
  }

  Serial.println("");
  delay(100);

}

void loop()
{
  
   temp();
   heart();
   gas();
   quake();
  delay(1000);
}

void temp() {
  float h = dht.readHumidity();                                 // Read Humidity
  float t = dht.readTemperature();                              // Read temperature

  if (isnan(h) || isnan(t))                                     // Checking sensor working
  {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
  Serial.print("Humidity: ");
  Serial.print(h);
  String fireHumid = String(h) + String("%");                   //Humidity integer to string conversion

  Serial.print("%  Temperature: ");
  Serial.print(t);
  Serial.println("°C ");
  String fireTemp = String(t) + String("°C");                  //Temperature integer to string conversion
  delay(1000);
}

void heart() {
  Signal = analogRead2();  // Read the PulseSensor's value

  Serial.println(Signal);                    // Send the Signal value to Serial Plotter.

  if (Signal > Threshold) {                        // If the signal is above "550", then "turn-on" Arduino's on-Board LED.
    digitalWrite(Buzzer2, HIGH);
  } else {
    digitalWrite(Buzzer2, LOW);               //  Else, the sigal must be below "550", so "turn-off" this LED.
  }
  delay(1000);
}

void gas() {
  sensor_value = analogRead1();
  delay(200);
  Serial.print("Gas Level: ");
  Serial.println(sensor_value);
  if (sensor_value > sensor_limit)  // Checks if it has reached the threshold value
  {
    digitalWrite(Buzzer, HIGH);
    delay(300);
    digitalWrite(Buzzer, LOW);
  }
  else
  {
    digitalWrite(Buzzer, LOW);
  }
  delay(100);
}

void quake() {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
  float a_x;
  float a_y;
  float a_z;
  float A=(g.gyro.x > maxval || g.gyro.x < minval && g.gyro.y > maxval || g.gyro.y  < minval && g.gyro.z > maxval || g.gyro.z < minval);
  if (A)
  { 
    delay(10000);
    if (A==(g.gyro.x > maxval || g.gyro.x < minval && g.gyro.y > maxval || g.gyro.y  < minval && g.gyro.z > maxval || g.gyro.z < minval))
    {
    digitalWrite(Buzzer1, HIGH);
    delay(300);
    digitalWrite(Buzzer1, HIGH);
    delay(300);
    Serial.print("Motion Detected");
    delay (1000);
    }
  }

  else {
    digitalWrite(Buzzer1, LOW);
    a_x=a.acceleration.x;
    a_y=a.acceleration.y;
    a_z=a.acceleration.z;
    Serial.print("Acceleration X: ");
    Serial.print(a.acceleration.x);
    Serial.print("\n Y: ");
    Serial.print(a.acceleration.y);
    Serial.print("\n Z: ");
    Serial.print(a.acceleration.z);
    Serial.println(" m/s^2");

    Serial.print("Rotation X: ");
    Serial.print(g.gyro.x);
    Serial.print("\n Y: ");
    Serial.print(g.gyro.y);
    Serial.print("\n Z: ");
    Serial.print(g.gyro.z);
    Serial.println(" rad/s");

    Serial.print("\nTemperature: ");
    Serial.print(temp.temperature);
    Serial.println("°C");
  }
  delay(1000);
}
