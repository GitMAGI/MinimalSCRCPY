#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>

#include "custom/log.h"
#include "custom/util.h"

#include "server.h"

int main(int argc, char* argv[]){
#ifdef __WINDOWS__
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
    socket_t srvr_sckt;

    server_init(&srvr);
    if(!server_start(&srvr, serial, local_port, max_size, bit_rate, crop, send_frame_meta)){
        criticalLog("Server Starting failed");
        goto end;
    }
    
    srvr_sckt = server_connect_to(&srvr);

    server_stop(&srvr);
    
    server_destroy(&srvr);

    end:
    getTick(&et);
    infoLog("Application Completed. ETA %s", getETA(st, et));

    return 0;
}