#include <ESP8266WiFi.h>
#include <DHT.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// ===== KHAI BÁO CÁC CHÂN LINH KIỆN =====
#define DHTPIN        D5        // GPIO14
#define RELAY_PIN     D6        // GPIO12
#define SDA_PIN       D2        // GPIO4
#define SCL_PIN       D1        // GPIO5

// ===== THÔNG TIN WIFI =====
const char* ssid = "P402 LX5";
const char* password = "phanquan@123";

// ===== CẢM BIẾN DHT11 =====
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// ===== LCD I2C =====
LiquidCrystal_I2C lcd(0x27, 16, 2);

// ===== THÔNG SỐ NGƯỠNG VÀ TRẠNG THÁI =====
const float thresholdTemp = 35.0;
bool fanState = false;
bool manualFanControl = false;  // true = điều khiển bằng tay

// ===== KHỞI TẠO SERVER =====
WiFiServer server(80);

// ===== BIẾN THỜI GIAN =====
unsigned long lastUpdate = 0;
const unsigned long updateInterval = 3000;

void setup() {
  Serial.begin(115200);
  dht.begin();

  // Khởi tạo LCD I2C với chân tùy chọn
  Wire.begin(SDA_PIN, SCL_PIN);
  lcd.init();
  lcd.backlight();

  // Màn hình chào
  lcd.setCursor(0, 0);
  lcd.print("CHAO MUNG");
  lcd.setCursor(0, 1);
  lcd.print("NHOM 8 KTTTKD");
  delay(3000);
  lcd.clear();

  // Khởi tạo relay
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);

  // Kết nối WiFi
  lcd.setCursor(0, 0);
  lcd.print("Dang ket noi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(200);
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("IP:");
  lcd.setCursor(3, 0);
  lcd.print(WiFi.localIP());
  delay(2000);
  lcd.clear();

  server.begin();
}

void loop() {
  // ===== 1) XỬ LÝ YÊU CẦU TỪ APP =====
  WiFiClient client = server.available();
  if (client) {
    String req = client.readStringUntil('\r');
    client.flush();

    if (req.indexOf("/fan/on") != -1) {
      manualFanControl = true;
      digitalWrite(RELAY_PIN, HIGH);
      fanState = true;
    }
    else if (req.indexOf("/fan/off") != -1) {
      manualFanControl = true;
      digitalWrite(RELAY_PIN, LOW);
      fanState = false;
    }
    else if (req.indexOf("/fan/auto") != -1) {
      manualFanControl = false;
    }

    // Đọc nhiệt độ
    float temp = dht.readTemperature();

    // Gửi phản hồi HTTP
    client.print("HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n");
    client.print("Nhiệt độ: ");
    client.print(isnan(temp) ? 0 : temp, 1);
    client.print("°C");
    client.print("\r\nFan: ");
    client.print(fanState ? "ON" : "OFF");
    if (!isnan(temp) && temp > thresholdTemp)
      client.print("\r\nCANH BAO: Nhiet do cao!");
    client.stop();
  }

  // ===== 2) ĐỌC CẢM BIẾN & ĐIỀU KHIỂN TỰ ĐỘNG + LCD =====
  unsigned long now = millis();
  if (now - lastUpdate >= updateInterval) {
    lastUpdate = now;

    float temp = dht.readTemperature();

    if (!isnan(temp)) {
      // Nếu vượt ngưỡng, ép về chế độ tự động
      if (temp > thresholdTemp) {
        manualFanControl = false;
      }

      // Nếu đang ở chế độ tự động thì bật/tắt quạt
      if (!manualFanControl) {
        bool shouldOn = temp > thresholdTemp;
        if (shouldOn != fanState) {
          digitalWrite(RELAY_PIN, shouldOn ? HIGH : LOW);
          fanState = shouldOn;
        }
      }

      // Hiển thị thông tin trên LCD
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Nhiet Do:");
      lcd.print(temp, 1);
      lcd.print((char)223); // Ký hiệu độ
      lcd.print("C ");
      lcd.setCursor(0, 1);
      lcd.print("FAN: ");
      lcd.print(fanState ? "ON " : "OFF");

      // Nếu nhiệt độ vượt ngưỡng thì hiển thị cảnh báo sau 2s
      if (temp > thresholdTemp) {
        delay(2000);
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("CANH BAO:");
        lcd.setCursor(0, 1);
        lcd.print("Nhiet do cao!");
      }

    } else {
      lcd.clear();
      lcd.print("Loi DHT!");
    }
  }
}
