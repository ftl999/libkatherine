//
// Created by petr on 14.6.18.
//

#include <katherine/device.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

static const uint16_t CONTROL_PORT = 1555;
static const uint16_t DATA_PORT = 1556;
static const uint16_t REMOTE_PORT = 1555;

static const uint32_t CONTROL_TIMEOUT = 100;
static const uint32_t DATA_TIMEOUT = 2000;

#endif /* DOXYGEN_SHOULD_SKIP_THIS */

/**
 * Initialize Katherine device.
 * @param device Katherine device
 * @param addr IP address
 * @return Error code.
 */
int
katherine_device_init(katherine_device_t *device, const char *addr)
{
    int res;

    if ((res = katherine_udp_init(&device->control_socket, CONTROL_PORT, addr, REMOTE_PORT, CONTROL_TIMEOUT))) {
        goto err_control;
    }

    if ((res = katherine_udp_init(&device->data_socket, DATA_PORT, addr, REMOTE_PORT, DATA_TIMEOUT))) {
        goto err_data;
    }

    return 0;

err_data:
    katherine_udp_fini(&device->control_socket);
err_control:
    return res;
}

/**
 * Finalize Katherine device.
 * @param device Device to finalize.
 */
void
katherine_device_fini(katherine_device_t *device)
{
    katherine_udp_fini(&device->data_socket);
    katherine_udp_fini(&device->control_socket);
}
