// server info

#ifdef _WIN64
#include "..\src\storage.h"
#elif __linux__
#include "../src/storage.h"
#endif
#include <stdlib.h>

db_settings_t load_db_settings()
{
    db_settings_t db_settings;
    db_settings.n_of_db_endpoints = 2;

    db_settings.db_endpoints = (db_ep_coordinates_t *) malloc(db_settings.n_of_db_endpoints * sizeof(db_ep_coordinates_t));
    strcpy((char *)db_settings.db_endpoints[0].ipv4_addr, "127.0.0.1");
    db_settings.db_endpoints[0].base_port = 2000;
    db_settings.db_endpoints[0].ports = 2;
    strcpy((char *)db_settings.db_endpoints[1].ipv4_addr, "127.0.0.1");
    db_settings.db_endpoints[1].base_port = 2002;
    db_settings.db_endpoints[1].ports = 2;

    return db_settings;
}