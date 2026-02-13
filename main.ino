// Proje: Akıllı Trafik Lambası (Smart Traffic Light)
// Yazarlar: Hüseyin YILMAZ & Emre
// Kurum: Kayseri Üniversitesi & BTK Akademi

#define TRIG_PIN 9   // Sensörün TRIG bacağı Arduino'nun 9. pinine bağlı
#define ECHO_PIN 10  // Sensörün ECHO bacağı Arduino'nun 10. pinine bağlı
#define SENSOR_LED 7 // Sensör aktifken veya ölçüm yapılırken yanan LED
#define RED_LED    6 // Kırmızı LED
#define YELLOW_LED 5 // Sarı LED
#define GREEN_LED  4 // Yeşil LED

#define MESAFE_ESIK 20 // Sensörün tepki vereceği mesafe sınırı (cm)

unsigned long t0 = 0; // Zaman sayacı
int durum = 0; // Sistemin durumunu tutar (0:Bekleme, 1:Algılandı, vb.)

long mesafeOlc() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(5);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // Timeout ile kilitlemeyi önlemek için 35000 kullanıldı
  long sure = pulseIn(ECHO_PIN, HIGH, 35000); 
  long mesafe = sure * 0.034 / 2;
  return mesafe;
}

void setup() {
  Serial.begin(9600);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  
  pinMode(SENSOR_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);

  digitalWrite(SENSOR_LED, HIGH); // Başlangıçta sensör aktif
  digitalWrite(RED_LED, HIGH);    // Başlangıçta kırmızı ışık yanar
  digitalWrite(YELLOW_LED, LOW);
  digitalWrite(GREEN_LED, LOW);

  Serial.println("Sistem basladi...");
}

void loop() {
  long mesafe = mesafeOlc();
  // Ölçülen mesafe 0'dan büyük ve eşik değerden küçükse engel var
  bool engelVar = (mesafe > 0 && mesafe < MESAFE_ESIK);

  Serial.print("Mesafe: ");
  if (mesafe == -1 || mesafe == 0) {
    Serial.println("Okunamadi");
  } else {
    Serial.print(mesafe);
    Serial.println(" cm");
  }

  switch (durum) {
    case 0: // Bekleme Modu (Kırmızı Yanıyor)
      digitalWrite(SENSOR_LED, HIGH);
      digitalWrite(RED_LED, HIGH);
      digitalWrite(YELLOW_LED, LOW);
      digitalWrite(GREEN_LED, LOW);

      if (engelVar) {
        durum = 1;
        t0 = millis();
        digitalWrite(SENSOR_LED, LOW); // Algılama başladı, sensör LED sönüyor
      }
      break;

    case 1: // Engel algılandı, kırmızıda 2 sn bekleme
      if (millis() - t0 >= 2000) {
        durum = 2;
        t0 = millis(); // Sarı ışık için sayaç sıfırla
        digitalWrite(RED_LED, LOW);
        digitalWrite(YELLOW_LED, HIGH);
      }
      break;

    case 2: // Sarı ışıkta 2 sn bekleme
      if (millis() - t0 >= 2000) {
        durum = 3;
        digitalWrite(YELLOW_LED, LOW);
        digitalWrite(GREEN_LED, HIGH);
      }
      break;

    case 3: // Yeşil ışık (Engel kalkana kadar bekle)
      if (!engelVar) { // Engel kalkarsa
        durum = 4;
        t0 = millis(); // 5 sn ek süre için sayaç başlat
      }
      break;

    case 4: // Engel kalktıktan sonra 5 sn daha yeşil yak
      if (millis() - t0 >= 5000) {
        durum = 5;
        t0 = millis();
        digitalWrite(GREEN_LED, LOW);
        digitalWrite(YELLOW_LED, HIGH);
      }
      break;

    case 5: // Sarıdan tekrar kırmızıya dönüş
      if (millis() - t0 >= 2000) {
        durum = 0; // Başa dön
        digitalWrite(YELLOW_LED, LOW);
        digitalWrite(RED_LED, HIGH);
      }
      break;
  }
  
  delay(200); // Stabilite için küçük gecikme
}