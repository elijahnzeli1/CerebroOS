#ifndef OTA_UPDATE_H
#define OTA_UPDATE_H

#include <stdbool.h>

typedef struct {
    int version;
    const char* url;
    int size;
} UpdateInfo;

bool ota_check_for_update();
bool ota_download_update();
bool ota_apply_update();
UpdateInfo ota_get_latest_update_info();

#endif