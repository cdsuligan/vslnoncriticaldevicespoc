#include "dweet_ip.h"

//https://freeboard.io/board/6elHGj //add this to server setup

bool getIp(char *PubIP)
{
  WiFiClient client;
  if (!client.connect("api.ipify.org", 80)) {
    Serial.println(F("Failed to connect with 'api.ipify.org' !"));
    return false;
  }
  else {
    int timeout = millis() + 5000;
    client.print(F("GET /?format=json HTTP/1.1\r\nHost: api.ipify.org\r\n\r\n"));
    while (client.available() == 0) {
      if (timeout - millis() < 0) {
        Serial.println("Client Timeout!");
        client.stop();
        return false;
      }
    }
    String line;
    while(client.available())
    {
      line = client.readStringUntil('\n');
      Serial.println(line);
    }
	  strcpy(PubIP, line.c_str());
	  PubIP[line.length()] = '\0'; //add the null terminator at the end of the string
    return true;
  }
}




bool dweet(char *pubIP, char *mac, char *locIP)
{

  WiFiClient client;
  if (!client.connect("dweet.io", 80)) 
  {
    Serial.println("dweet failed");
    return false;
  }  
  else 
  {
    Serial.println(String("dweeting IP as ") + mac + " Public[" + pubIP + "]" + " Local[" + locIP + "]" );
    
    client.println(String("GET /dweet/for/") + mac + "?PublicIP=" + pubIP + "&LocalIP=" + locIP + " HTTP/1.1");
    client.println( "Host: dweet.io" );
    client.println( "Connection: close");
    client.println("");
    client.println("");
    
    delay(1000);
  
    // Read all the lines of the reply from server and print them to Serial
    while(client.available())
    {
      String line = client.readStringUntil('\r');
      Serial.print(line);
    }
    return true;
  }
}
