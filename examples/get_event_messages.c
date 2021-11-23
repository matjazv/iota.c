// Copyright 2021 IOTA Stiftung
// SPDX-License-Identifier: Apache-2.0

#include <string.h>

#include "client/api/events/node_event.h"
#include "client/api/events/sub_milestone_latest.h"
#include "client/api/events/sub_milestones_confirmed.h"

void process_event_data(event_client_event_t *event);

void callback(event_client_event_t *event) {
  switch (event->event_id) {
    case NODE_EVENT_ERROR:
      printf("Node event network error : %s\n", (char *)event->data);
      break;
    case NODE_EVENT_CONNECTED:
      printf("Node event network connected\n");
      /* Making subscriptions in the on_connect() callback means that if the
       * connection drops and is automatically resumed by the client, then the
       * subscriptions will be recreated when the client reconnects. */
      event_subscribe(event->client, NULL, "milestones/latest", 1);
      event_subscribe(event->client, NULL, "milestones/confirmed", 1);
      break;
    case NODE_EVENT_DISCONNECTED:
      printf("Node event network disconnected\n");
      break;
    case NODE_EVENT_SUBSCRIBED:
      printf("Subscribed topic, granted qos = %d\n", event->qos);
      break;
    case NODE_EVENT_UNSUBSCRIBED:
      printf("Unsubscribed topic\n");
      break;
    case NODE_EVENT_PUBLISHED:
      // To Do : Handle publish callback
      break;
    case NODE_EVENT_DATA:
      printf("Message arrived\nTopic : %s\n", event->topic);
      process_event_data(event);
      break;
    default:
      break;
  }
}

void process_event_data(event_client_event_t *event) {
  if (!strcmp(event->topic, "milestones/latest")) {
    milestone_latest_t res = {};
    parse_milestone_latest((char *)event->data, &res);
    printf("Index :%u\nTimestamp : %lu\n", res.index, res.timestamp);
  } else if (!strcmp(event->topic, "milestones/confirmed")) {
    milestone_confirmed_t res = {};
    parse_milestones_confirmed((char *)event->data, &res);
    printf("Index :%u\nTimestamp : %lu\n", res.index, res.timestamp);
  }
}

int main(void) {
  event_client_config_t config = {
      .host = "mqtt.lb-0.h.chrysalis-devnet.iota.cafe", .port = 1883, .client_id = "iota_test_1234", .keepalive = 60};
  event_client_handle_t client = event_init(&config);
  event_register_cb(client, &callback);
  event_start(client);
  event_destroy(client);
  return 0;
}