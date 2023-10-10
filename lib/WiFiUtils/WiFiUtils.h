//
// Created by mike on 04/10/23.
//
#pragma once

#include <Arduino_JSON.h>

void InitialiseWiFi();

JSONVar GetJson(char* server, int port, String pathQuery);
