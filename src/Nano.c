#include <nano.h>
#include <stdio.h>
#include <string.h>

/*
-----------------------
 -> Nano Networking  <- 
-----------------------
*/

void initNanoNet()
{
    if (enet_initialize() != 0) {
        printf("Could not initialize ENet.\n");
        exit(-1);
    }
    printf("Succesfully initiated ENet.\n");
}

void exitNanoNet(ENetHost* host)
{
    enet_host_destroy(host);
    enet_deinitialize();
    printf("Succesfully deinitialized ENet.\n");
}

ENetAddress NNet_adress(const char* host, enet_uint16 port)
{
    ENetAddress address;
    if (host == NULL) address.host = ENET_HOST_ANY;
    else enet_address_set_host(&address, host);
    address.port = port;
    return address;
}

ENetPeer* NNet_server(ENetHost* client, ENetAddress* address, size_t channels)
{
    ENetPeer* server = enet_host_connect(client, address, channels, 0);
    if (server == NULL) {
        printf("Could not connect to server.\n");
        exit(-1);
    }
    return server;
}

ENetHost* NNet_host_create(ENetAddress* address, size_t clients, size_t channels)
{
    if (address == NULL) clients = 1;
    ENetHost* host = enet_host_create(address, clients, channels, 0, 0);
    if (host == NULL) {
        printf("Could not create client.\n");
        exit(-1);
    }
    return host;
}

int NNet_connect(ENetHost* client, ENetPeer* server, ENetEvent* event, unsigned int timeout)
{
    if (enet_host_service(client, event, timeout) > 0 && event->type == ENET_EVENT_TYPE_CONNECT) {
        printf("Connection to server succeeded.\n");
        return 1;
    } else {
        enet_peer_reset(server);
        printf("Could not connect to server.\n");
    }
    return 0;
}

unsigned int NNet_read(ENetPacket* packet, void* buffer)
{
    unsigned int length = packet->dataLength;
    memcpy(buffer, packet->data, length);
    enet_packet_destroy(packet);
    return length;
}

void NNet_send(ENetPeer* server, ENetPacket* packet, void* buffer, size_t size, enet_uint8 channel)
{
    packet = enet_packet_create(buffer, size, ENET_PACKET_FLAG_RELIABLE);
    enet_peer_send(server, channel, packet);
}

void NNet_broadcast(ENetHost* host, ENetPacket* packet, void* buffer, size_t size, enet_uint8 channel)
{
    packet = enet_packet_create(buffer, size, ENET_PACKET_FLAG_RELIABLE);
    enet_host_broadcast(host, channel, packet);
}

void NNet_disconnect(ENetHost* client, ENetPeer* server, ENetEvent* event, unsigned int timeout)
{
    enet_peer_disconnect(server, 0);
    while (enet_host_service(client, event, timeout) > 0) {
        if (event->type == ENET_EVENT_TYPE_RECEIVE) {
            enet_packet_destroy(event->packet);
            break;
        }
        else if (event->type == ENET_EVENT_TYPE_DISCONNECT) {
            printf("Disconnection succeeded.\n");
            return;
        }
    }
    enet_peer_reset(server);
}
