/*
    C File subq1.c
    Written by: Asharudin 'aviezab' Achzab
    First written date: 2019 05 25
    This file is granted without any warranty
*/

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <json-c/json.h>
#include <nats/nats.h>
#include "../src/natshighsend.h"

int sendmsgq1(char *pesan, char *topic);

static void onMsg(natsConnection *nc, natsSubscription *sub, natsMsg *msg, void *closure){
  struct  json_object *parsed_json;
  struct  json_object *dest;
  parsed_json = json_tokener_parse(msg);
	json_object_object_get_ex(parsed_json, "dest", &dest);
  printf("Dest/ Received msg: %s - %s - %.*s\n -", dest, natsMsg_GetSubject(msg), 
	                              natsMsg_GetDataLength(msg),
                                      natsMsg_GetData(msg));
  sendmsgq1(msg, dest); 
  sleep(2);
  natsMsg_Destroy(msg);
}

int main(int argc, char **argv){
  natsConnection *conn = NULL;
  natsOptions *opts = NULL; 
  natsSubscription *sub = NULL;
  natsStatus stat = NATS_OK;

  const char * subj = "topic";
  //const char * name = argv[1];
  
  stat = natsOptions_Create(&opts);
  if (stat == NATS_OK) stat = natsOptions_SetURL(opts, "nats://localhost:4222");
  if (stat == NATS_OK) stat = natsConnection_Connect(&conn, opts);
  if (stat == NATS_OK) stat = natsConnection_Subscribe(&sub, conn, subj, onMsg, NULL);
  if (stat == NATS_OK) stat = natsSubscription_SetPendingLimits(sub, -1, -1);

  printf("status: %s\n", natsStatus_GetText(stat));

  while (1);

  natsSubscription_Destroy(sub);
  natsConnection_Destroy(conn);
  natsOptions_Destroy(opts);
  nats_Close();
  return 0;
}