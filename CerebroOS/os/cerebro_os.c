#include "cerebro_os.h"
#include "../kernel/kernel.h"
#include "../drivers/display_driver.h"
#include "../security/permission_manager.h"

void os_init() {
    kernel_init();
    display_init();
    permission_init();
}

void os_run() {
    os_init();
    while(1) {
        kernel_main();
        // Handle system events, update UI, etc.
    }
}