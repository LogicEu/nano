#include <nano.h>
#include <stdio.h>
#include <stdlib.h>

/*
----------------------------
 -> Nano Networking Host  <- 
----------------------------
*/

NNetHost* NNetHost_create(const char* host, enet_uint16 port, unsigned int clients, unsigned int channels, unsigned int buffer_size, unsigned int timeout)
{
    NNetHost* client = (NNetHost*)malloc(sizeof(NNetHost));
    client->size = buffer_size;
    client->buffer = malloc(client->size);
    client->server = NULL;
    client->packet = NULL;
    client->address = NNet_adress(host, port);
    if (host == NULL) client->host = NNet_host_create(&client->address, clients, channels);
    else {
        client->host = NNet_host_create(NULL, clients, channels);
        client->server = NNet_server(client->host, &client->address, channels);
        client->connected = NNet_connect(client->host, client->server, &client->event, timeout);
    }
    return client;
}

void NNetHost_free(NNetHost* host)
{
    free(host->buffer);
    free(host);
}

unsigned int NNetHost_read(NNetHost* client, unsigned int timeout)
{
    unsigned int buffer_size = 0;
    while (enet_host_service(client->host, &client->event, timeout) > 0) {
        if (client->event.type == ENET_EVENT_TYPE_RECEIVE) {
            buffer_size = NNet_read(client->event.packet, client->buffer);
        }
        else if (client->event.type == ENET_EVENT_TYPE_DISCONNECT) {
            client->connected = 0;
            printf("You have been disconnected.\n");
            break;
        }
        else break;
    }
    return buffer_size;
}
