#include <Arduino.h>
#include <LoRaMesh.h>

/* Local device ID */
uint16_t localId;

/* Remote device ID */
uint16_t remoteId;

/* Local device Unique ID */
uint32_t localUniqueId;

/* Local device Net */
uint16_t localNet;

/* Remote device ID to communicate with */
uint16_t ID = 10;

/* Payload buffer */
uint8_t bufferPayload[MAX_PAYLOAD_SIZE] = {0};

/* Values read on each analog inputs */
uint16_t AdcIn[2];

/* SoftwareSerial handles */
SoftwareSerial* hSerialCommands = NULL;
SoftwareSerial* hSerialTransparent = NULL;

MeshStatus_Typedef LocalWriteConfig(uint16_t id, uint16_t net,
                                    uint32_t uniqueId) {
    uint8_t crc = 0;
    uint8_t bufferPayload[31];
    uint8_t payloadSize;
    uint8_t i = 0;
    uint8_t command;
    uint16_t idRead = 0;

    /* Asserts parameters */
    if (uniqueId == 0) {
        Serial.println("ERRO UniqueID");
        return MESH_ERROR;
    }

    if (hSerialCommands == NULL) {
        Serial.println("ERRO hSerialCommands");
        return MESH_ERROR;
    }

    /*Creating the payload with
    the passed NET and UniqueID and then,
    call the function with the ID passed*/

    bufferPayload[0] = net & 0xFF;
    bufferPayload[1] = (net >> 8) & 0xFF;
    bufferPayload[2] = uniqueId & 0xFF;
    bufferPayload[3] = (uniqueId >> 8) & 0xFF;
    bufferPayload[4] = (uniqueId >> 16) & 0xFF;
    bufferPayload[5] = (uniqueId >> 24) & 0xFF;

    /* Loads dummy bytes */
    for (i = 6; i < 9; i++) bufferPayload[i] = 0x00;

    PrepareFrameCommand(id, CMD_WRITECONFIG, &bufferPayload[0], i);

    /* Sends packet */
    SendPacket();

    /* Flush serial input buffer */
    // SerialFlush(hSerialCommand);
    // To not make modifications on the original library
    // I copied the content of the SerialFlush function
    while (hSerialCommands->available() > 0) {
        hSerialCommands->read();
    }

    /* Waits for response */
    if (ReceivePacketCommand(&idRead, &command, &bufferPayload[0], &payloadSize,
                             5000) != MESH_OK) {
        Serial.println("Erro de pacote recebido");
        return MESH_ERROR;
    }

    /* Checks command */
    if (command != CMD_REMOTEREAD) {
        Serial.print("Erro! Commando recebido: ");
        Serial.println(command, HEX);
        return MESH_ERROR;
    }

    return MESH_OK;
}

MeshStatus_Typedef SendDatatest() {
    uint8_t i = 0, command, payloadSize;
    uint16_t idRead = 0;
    uint8_t bufferPayload[31];

    /* Asserts parameters */

    if (hSerialTransparent == NULL) {
        Serial.println("ERRO hSerialTransparent");
        return MESH_ERROR;
    }

    /*Creating the payload with
    the passed NET and UniqueID and then,
    call the function with the ID passed*/

    bufferPayload[0] = 0x01;
    bufferPayload[1] = 0x02;
    bufferPayload[2] = 0x03;

    if (PrepareFrameTransp(254, &bufferPayload[0], 3) != MESH_OK) {
        Serial.println("Erro de mensagem enviada");
        return MESH_ERROR;
    }

    /* Sends packet */
    SendPacket();

    return MESH_OK;
}

void setup() {
    // initialize digital pin LED_BUILTIN as an output.
    pinMode(2, OUTPUT);
    digitalWrite(2, HIGH);  // turn the LED on (HIGH is the voltage level)
    // Configurando o módulo LoRaMesh
    delay(1000);
    Serial.begin(9600);  /* Initialize monitor serial */
    Serial1.begin(9600); /* Initialize monitor serial */

    /* Initialize SoftwareSerial */
    // Pinos:
    // D6(12)->Lora TX
    // D5(14)-> Lora RX
    hSerialCommands = SerialCommandsInit(12, 14, 9600);
    // Pinos:
    // D2(4)->Lora TX
    // D7(13)-> Lora RX
    hSerialTransparent = SerialTranspInit(4, 13, 9600);

    /* Gets the local device ID */
    if (LocalRead(&localId, &localNet, &localUniqueId) != MESH_OK)
        Serial.print("Couldn't read local ID\n\n");
    else {
        Serial.print("Local ID: ");
        Serial.println(localId);
        Serial.print("Local NET: ");
        Serial.println(localNet);
        Serial.print("Local Unique ID: ");
        Serial.println(localUniqueId, HEX);
        Serial.print("\n");
    }

    if (LocalRead(&localId, &localNet, &localUniqueId) != MESH_OK)
        Serial.print("Couldn't read local ID\n\n");
    else {
        Serial.print("Local ID: ");
        Serial.println(localId);
        Serial.print("Local NET: ");
        Serial.println(localNet);
        Serial.print("Local Unique ID: ");
        Serial.println(localUniqueId, HEX);
        Serial.print("\n");
    }
    delay(500);
}

void loop() {
    digitalWrite(2, HIGH);  // turn the LED on (HIGH is the voltage level)
    delay(4000);            // wait for a second
    SendDatatest();
    digitalWrite(2, LOW);   // turn the LED off by making the voltage LOW
    delay(4000);            // wait for a second
}