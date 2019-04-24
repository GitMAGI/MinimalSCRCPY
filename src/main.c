#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>

#include "custom/log.h"
#include "custom/util.h"

#include "server.h"
#include "net.h"

int main(int argc, char* argv[]){
#if defined _WIN64 || defined _WIN32
    // disable buffering, we want logs immediately
    // even line buffering (setvbuf() with mode _IOLBF) is not sufficient
    setbuf(stdout, NULL);
    setbuf(stderr, NULL);
#endif
    struct timeval st, et;
    getTick(&st);

	infoLog("Application starting ...");

    uint16_t local_port = DEFAULT_LOCAL_PORT;
    uint16_t max_size = DEFAULT_MAX_SIZE;
    uint32_t bit_rate = DEFAULT_BIT_RATE;
    const char *serial = NULL;
    const char *crop = NULL;
    bool send_frame_meta = true;

    struct server srvr;    

    debugLog("Server Initiating ...");
    server_init(&srvr);
    debugLog("Server Initiated");

    debugLog("Server Starting ...");
    if(!server_start(&srvr, serial, local_port, max_size, bit_rate, crop, send_frame_meta)){
        criticalLog("Server Starting failed");
        goto end;
    }
    debugLog("Server Started");   

    socket_t srvr_sckt = server_connect_to(&srvr);
    if(srvr_sckt == INVALID_SOCKET){
        criticalLog("Unable to correctly connect to the server");
        goto end;
    }

    // 1st Packet of 64 B is the Device name (NUL terminating string)
    // 2nd Packet of 2 B unisigned big endian is the width of the device
    // 3rd Packet of 2 B unisigned big endian is the height of the device
    // Next packages (size of 512) are the H.264 stream
 
    char *buf = (char*) malloc(sizeof(char) * 512);
    ssize_t size = 0;
    uint16_t delay = 2; 

    size = net_recv(srvr_sckt, buf, 64);
    if(size<=0){
        criticalLog("Socket Devece Name failed");
        goto end;
    }
    char *deviceName = (char*) malloc(sizeof(char)*size);
    memcpy(deviceName, buf, size);
    debugLog("Device Name: %s", deviceName);

    size = net_recv(srvr_sckt, buf, 2);
    if(size<=0){
        criticalLog("Socket Devece Screen Width failed");
        goto end;
    }
    uint16_t screenWidth = 0;
    memcpy(&screenWidth, buf, size);
    screenWidth = __builtin_bswap16(screenWidth);
    debugLog("Device Screen Width: %d", screenWidth);

    size = net_recv(srvr_sckt, buf, 2);
    if(size<=0){
        criticalLog("Socket Devece Screen Height failed");
        goto end;
    }
    uint16_t screenHeight = 0;
    memcpy(&screenHeight, buf, size);
    screenHeight = __builtin_bswap16(screenHeight);
    debugLog("Device Screen Height: %d", screenHeight);

    int fake = 0;
    do
    { 
        int packet_size = net_recv(srvr_sckt, buf, sizeof(buf));
        if(packet_size == -1){
            criticalLog("Socket Receiving data failed");
            break;
        }
        //debugLog("Got packet of size %d B", packet_size);

        #if defined _WIN64 || defined _WIN32
            Sleep(delay);
        #else
            Sleep(delay);
        #endif
        fake++;
    }
    while(fake < 100); 

    /*
    uint32_t counter = 0;
    uint16_t delay = 10; 
    do{
        #if defined _WIN64 || defined _WIN32
            Sleep(delay);
        #else
            Sleep(delay);
        #endif
        counter++;
    }while(counter < 10000);
    */

    debugLog("Server Stopping ...");
    server_stop(&srvr);
    debugLog("Server Stopped");

    debugLog("Server Destroying ...");
    server_destroy(&srvr);
    debugLog("Server Destroyed");

    end:
    getTick(&et);
    infoLog("Application Completed. ETA %s", getETA(st, et));

    return 0;
}