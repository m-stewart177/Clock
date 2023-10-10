//
// Created by mike on 04/10/23.
//

#include <WiFi.h>
#include <WiFiManager.h>

#include <Arduino_JSON.h>

#include <Arduino.h>

void InitialiseWiFi()
{
    WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP
    // it is a good practice to make sure your code sets wifi mode how you want it.
    //WiFiManager, Local intialization. Once its business is done, there is no need to keep it around
    WiFiManager wm;

    // reset settings - wipe stored credentials for testing
    // these are stored by the esp library
//    wm.resetSettings();

    // Automatically connect using saved credentials,
    // if connection fails, it starts an access point with the specified name ( "AutoConnectAP"),
    // if empty will auto generate SSID, if password is blank it will be anonymous AP (wm.autoConnect())
    // then goes into a blocking loop awaiting configuration and will return success result

    bool res;
    // res = wm.autoConnect(); // auto generated AP name from chipid
    // res = wm.autoConnect("AutoConnectAP"); // anonymous ap
    res = wm.autoConnect("ESP_Clock", "ESP_Clock"); // password protected ap

    if (!res)
    {
        Serial.println("Failed to connect");
        // ESP.restart();
    }
    else
    {
        //if you get here you have connected to the WiFi
        Serial.println("connected...yeey :)");
    }
}

WiFiClient ClientConnect(char* server, int port)
{
    WiFiClient client;
    if (client.connect(server, port))
    {
        Serial.print("Connected to server: ");
        Serial.println(client.remoteIP());
        return client;
    }

    throw "Failed to connect to server";
}

void SendGetRequest(String server, String const& path, WiFiClient& client)
{
    client.println("GET " +path + " HTTP/1.1");
    client.println("Host: " + server);
    client.println("Connection: close");
    client.println("Accept: application/json");
    client.println();
}

void WaitForResponse(WiFiClient& client)
{
    while (!client.available())
        ;
}

uint16_t GetStatusCode(WiFiClient& client)
{
    char c = client.read();
    String statusLine;
    while (c != '\n')
    {
        statusLine += c;
        c = client.read();
    }
    int codeStart = statusLine.indexOf(' ');
    return statusLine.substring(codeStart + 1, codeStart + 4).toInt();
}

String GetJsonString(WiFiClient& client)
{
    String json;
    char c = client.read();
    while (c != '{')
        c = client.read();

    while (client.available())
    {
        json += c;
        c = client.read();
    }
    json += c;

    return json;
}

JSONVar GetJson(char* server, int port, String pathQuery)
{
    JSONVar result;
    WiFiClient client;
    try
    {
        client = ClientConnect(server, port);
    }
    catch (String& e)
    {
        Serial.println(e);
        return result;
    }

    SendGetRequest(server, pathQuery, client);
    WaitForResponse(client);
    uint16_t statusCode = GetStatusCode(client);
    if (statusCode < 200 || statusCode >= 300)
    {
        client.stop();
        return result;
    }

    String json = GetJsonString(client);

    result = JSON.parse(json);
    return result;
}
