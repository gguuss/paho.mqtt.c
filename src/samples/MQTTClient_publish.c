/*******************************************************************************
 * Copyright (c) 2012, 2017 IBM Corp.
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompany this distribution.
 *
 * The Eclipse Public License is available at
 *   http://www.eclipse.org/legal/epl-v10.html
 * and the Eclipse Distribution License is available at
 *   http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    Ian Craggs - initial contribution
 *******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "MQTTClient.h"

#define ADDRESS     "ssl://mqtt.googleapis.com:8883"
#define CLIENTID    "projects/<project-id>/locations/<region>/registries/<registry-id>/devices/mqtt-c-rsq"
#define TOPIC       "/devices/<device-id>/events"
#define PAYLOAD     "Published message %d"
#define PASSWORD    "<JWT-GOES-HERE>"
#define QOS         1
#define TIMEOUT     10000L
#define USERNAME    "unused"

int main(int argc, char* argv[])
{
    MQTTClient client;
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    MQTTClient_message pubmsg = MQTTClient_message_initializer;
    MQTTClient_deliveryToken token;
    int rc, i;

    MQTTClient_create(&client, ADDRESS, CLIENTID,
        MQTTCLIENT_PERSISTENCE_NONE, NULL);
    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;
    conn_opts.username = USERNAME;
    conn_opts.password = PASSWORD;
    MQTTClient_SSLOptions sslopts = MQTTClient_SSLOptions_initializer;
    sslopts.trustStore = "roots.pem";
    sslopts.privateKey = "rsa_private.pem";
    conn_opts.ssl = &sslopts;

    if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS)
    {
        printf("Failed to connect, return code %d\n", rc);
        exit(EXIT_FAILURE);
    }
    char payload[50];
    for (i=0; i < 10; i++) {
      sprintf(payload, PAYLOAD, i);
      pubmsg.payload = payload;
      pubmsg.payloadlen = strlen(PAYLOAD);
      pubmsg.qos = QOS;
      pubmsg.retained = 0;
      MQTTClient_publishMessage(client, TOPIC, &pubmsg, &token);
      printf("Waiting for up to %d seconds for publication of %s\n"
          "on topic %s for client with ClientID: %s\n",
          (int)(TIMEOUT/1000), PAYLOAD, TOPIC, CLIENTID);
      rc = MQTTClient_waitForCompletion(client, token, TIMEOUT);
      printf("Message with delivery token %d delivered\n", token);
    }
    MQTTClient_disconnect(client, 10000);
    MQTTClient_destroy(&client);
    return rc;
}
