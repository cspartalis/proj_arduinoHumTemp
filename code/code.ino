#include <dht11.h>                                           // Βιβλιοθηκη για τον αισθητηρα/μοναδα DHT11. (https://github.com/adidax/dht11)
#include <SoftwareSerial.h>                                  // Βιβλιοθηκη για τη σειριακη επικοινωνια με ψηφιακa pins του arduino.
                                  

String mySSID = "myWifi";                                    // Γραφουμε το SSID του δικτου που θα συνδεθουμε.    
String myPASS = "myPassw0rd";                                // Γραφουμε το PASSWORD του δικτου που θα συνδεθουμε.

int rxPin    = 3;                                            // Pin του arduino που συνδεσαμε τo RX pin της μοναδας WiFi.
int txPin    = 2;                                            // Pin του arduino που συνδεσαμε τo TX pin της μοναδας WiFi.
int dht11Pin = 11;                                           // Pin του arduino που συνδεσαμε τo DATA pin του αισθητηρα/μοναδας DHT11.

String ip = "184.106.153.149";                               // IP διευθυνση του ιστοτοπου thingspeak.
float temp_value, hum_value;

dht11 DHT11;

SoftwareSerial esp(txPin, rxPin);                            // Οριζουμε τα pin με τα οποια θα γινεται η σειρικαη επικοινωνια.
                                                             // Προσοχη! Η σειρα των ορισματων ειναι σημαντικη!
void setup() {  
  
  Serial.begin(115200);                                      // Ξεκιναμε την επικοινωνια με τη σειριακη θυρα 115200 (bps)
  Serial.println("Started");
  esp.begin(115200);                                         // Ξεκιναμε τη σειριακη επικοινωνια με τη μοναδα WiFi
  esp.println("AT");                                         // Ελεγχος της μοναδας WiFi με την εντολη ΑΤ
  Serial.println("AT  sent ");
  while(!esp.find("OK")){                                    // Περιμενουμε τη μοναδα WiFi να ανταποκριθει
    esp.println("AT");
    Serial.println("ESP8266 Not Found.");
  }
  Serial.println("OK Command Received");
  esp.println("AT+CWMODE=1");                                // Οριζουμε τη λειτουργια της μοναδας Wifi ως πελατη (client).
  while(!esp.find("OK")){                                    // Περιμενουμε να ρυθμιστει η μοναδα WIFi.
    esp.println("AT+CWMODE=1");
    delay(5000);
    Serial.println("Setting ESP8266 as client");
  }
  Serial.println("Set as client");
  Serial.println("Connecting to the Network ...");
  esp.println("AT+CWJAP=\""+mySSID+"\",\""+myPASS+"\"");     // Συνδεομαστε στο προκαθορισμενο δικτυο.
  while(!esp.find("OK"));                                    // Περιμενουμε να συνδεθουμε στο δικτυο.
  Serial.println("connected to the network.");
  delay(5000);
}
void loop() {
  esp.println("AT+CIPSTART=\"TCP\",\""+ip+"\",80");          // Συνδεομαστε στην ιστοσελιδα thingspeak στη θυρα 80.
  if(esp.find("Error")){                                     // Ελεγχος της συνδεσης.
    Serial.println("AT+CIPSTART Error");
  }
  DHT11.read(dht11Pin);
  temp_value = (float)DHT11.temperature;                                           // Αποθηκευουμε τη μετρηση της θερμοκρασιας.
  hum_value  = (float)DHT11.humidity;                                              // Αποθηκευουμε τη μετρηση της υγρασιας.
  String tsc = "GET https://api.thingspeak.com/update?api_key=AE5SV3XMMCX7CCA1";   // Εντολη Thingspeak. Γραφουμε το κλειδι API που μας ορισε ο ιστοτοπος.                                   
  tsc += "&field1=";
  tsc += String(temp_value);                                                       // Ετοιμαζουμε τη μετρηση της θερμοκρασιας για αποστολη στον ιστοτοπο.
  tsc += "&field2=";
  tsc += String(hum_value);                                                        // Ετοιμαζουμε τη μετρηση της υγρασιας για αποστολη στον ιστοτοπο.
  tsc += "\r\n\r\n"; 
  esp.print("AT+CIPSEND=");                                                        // Δινουμε το πληθος των μετρησεων που θα σταλουν.
  esp.println(tsc.length()+2);
  delay(5000);
  if(esp.find(">")){                                                               // Οι εντολες θα τρεξουν οταν η μοναδα WiFi ειναι ετοιμη.
    esp.print(tsc);                                                                // Στελνουμε τα δεδομενα.
    Serial.println(tsc);
    Serial.println("Data sent.");
    delay(5000);
  }
  Serial.println("Connection Closed.");
  esp.println("AT+CIPCLOSE");                                // Σταματαμε τη συνδεση.
  delay(5000);                                              // Περιμενουμε 5 δευτερόλεπτα μεχρι να στειλουμε τα επομενα δεδομενα.
}
