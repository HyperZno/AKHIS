

/*
Yapılması gerekenler:

1- Telefonunuzun internetini paylaşıma açın.
2-Aşağıdaki Ağ adı (ssid) ve şifresni(password) açtığınız internetiniz bilgileri ile değiştirin.
3-Seri port Haberleşme hızı 9600 ayarlayin
3-NodeMCU-32S  kartını seçin. 
4-Kodu derleyip ve yüleyin.
5-Yükleme tamamlandıktan sonra seri portta EKG  cihazın IP adresi görünecektir. Önek: 192.168.1.104  gibi
6- IP adresini kpyalayın ve  google  arama çubuğunda yapıştırın. Önceki adımları doğru yapmış iseniz bir arayüz çıkacaktır.

7- Kandaki oksijen ölçümü için parmağınızı hafifçe sensörün üzerine koyun.

8-AD8232 nin proplerini doğru renk ve yere yerleştirin.
Yeşil Prop --> Sağ Bacağa
Kırmızı Prop --> Sağ Kola
Sarı Prop --> Sol kola

Gelecek şeklinde plastik kısım kaldırılıp yapıştırın.
*/


#include <Wire.h>
#include "MAX30100_PulseOximeter.h"
#include "LordRobotics32.h"


const char* ssid = "EKG CIHAZI";




/*------- AD8232 ECG Sensor  ---------*/

const int AD8232AnaloPin=39;
int ADCValue = 0;  // ADC değeri için değişken


void ReadAndViewAD8232(void){
    ADCValue = analogRead(AD8232AnaloPin);  // ADC değerini oku
    Serial.println(ADCValue);  // Seri monitöre yazdır
  
}




/*------- Oximetre ---------*/

#define REPORTING_PERIOD_MS 1000


float BPM,SpO2;

// Create a PulseOximeter object
PulseOximeter pox;


// Time at which the last beat occurred
uint32_t tsLastReport = 0;



void onBeatDetected(){ Serial.println("Beat detected!");}




void InitMAX30100(void){
    Serial.println("Initializing pulse oximeter...");

    if (!pox.begin()) {
        Serial.println("FAILED to initialize!");
        for (;;);
    } else {
        Serial.println("MAX30100 Initialization SUCCESS.");
    }

    // Configure sensor to use 7.6mA for LED drive
    pox.setIRLedCurrent(MAX30100_LED_CURR_7_6MA);

    // Register a callback routine
    pox.setOnBeatDetectedCallback(onBeatDetected);
}




void ReadAndPrintMAX30100(void){
    // Update the sensor readings
    pox.update();

    // Report the readings every REPORTING_PERIOD_MS milliseconds
    if (millis() - tsLastReport > REPORTING_PERIOD_MS) {
     BPM = pox.getHeartRate();
     SpO2 = pox.getSpO2();
    
     Serial.print("Heart rate:");
     Serial.print(BPM);
     Serial.print("bpm / SpO2:");
     Serial.print(SpO2);
     Serial.println("%");

        tsLastReport = millis();
    }

}













void setup(){
  Serial.begin(115200);
  LR_InitWebServer();
  delay(500);
  InitMAX30100();
}









void loop(){
	
  server.handleClient();// Handle Web Server Clients
  webSocket.loop();  // Handle WebSocket Clients	
	
  ReadAndViewAD8232();     // ADC değerini oku
  ReadAndPrintMAX30100();  // Oksimetreyi oku
  
  msgPackageForSend.HeartADCVal = ADCValue; 
  msgPackageForSend.Spo2Val  = SpO2; 
  msgPackageForSend.BpmVal= BPM; 
  msgPackageForSend.Msg =""; 
  msgPackageForSend.ChartValName ="EKG";   

  LR_BroadcastMsg(); /* Her 500 ms de bir msaj paketini  yayınlanır*/

  delay(100); 
}












