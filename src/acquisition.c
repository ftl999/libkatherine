//
// Created by petr on 29.5.18.
//

#include <stdlib.h>
#include <string.h>
#include <katherine/acquisition.h>
#include <katherine/command_interface.h>
#include "katherine/MultiplattformTypes.h"

#ifndef DOXYGEN_SHOULD_SKIP_THIS

PACKED(
typedef struct md {
    uint64_t : 44;
    uint8_t header : 4;
}) md_t;

PACKED(
typedef struct md_time_offset {
    uint32_t offset : 32; // 31..0
    uint16_t : 12; // 43..32
}) md_time_offset_t;

PACKED(
typedef struct md_frame_finished {
    uint64_t n_sent : 44; // 0..43
}) md_frame_finished_t;

PACKED(
typedef struct md_time_lsb {
    uint32_t lsb : 32; // 31..0
    uint16_t : 12; // 43..32
}) md_time_lsb_t;

PACKED(
typedef struct md_time_msb {
    uint16_t msb : 16; // 15..0
    uint32_t : 24; // 43..16
}) md_time_msb_t;

PACKED(
typedef struct md_lost_px {
    uint64_t n_lost : 44; // 43..0
}) md_lost_px_t;

#define DEFINE_PMD_MAP(SUFFIX) \
    static inline void\
    pmd_##SUFFIX##_map(katherine_px_##SUFFIX##_t *dst, const pmd_##SUFFIX##_t *src, const katherine_acquisition_t *acq)

#define DEFINE_PMD_PAIR(NAME) \
    dst->NAME = src->NAME

#define DEFINE_PMD_PAIR_TOA \
    dst->toa = src->toa + acq->last_toa_offset

#define DEFINE_PMD_PAIR_COORD \
    {\
        dst->coord.x = src->coord_x;\
        dst->coord.y = src->coord_y;\
    }

PACKED(
typedef struct pmd_f_toa_tot {
    uint16_t ftoa : 4; // 0..3
    uint16_t tot : 10; // 13..4
    uint16_t toa : 14; // 27..14
    uint16_t coord_x : 8; // 35..28
    uint16_t coord_y : 8; // 43..36
    uint16_t : 4; // 44..47
}) pmd_f_toa_tot_t;

DEFINE_PMD_MAP(f_toa_tot)
{
    DEFINE_PMD_PAIR_COORD;
    DEFINE_PMD_PAIR_TOA;
    DEFINE_PMD_PAIR(ftoa);
    DEFINE_PMD_PAIR(tot);
}

PACKED(
typedef struct pmd_toa_tot {
    uint16_t hit_count : 4; // 0..3
    uint16_t tot : 10; // 13..4
    uint16_t toa : 14; // 27..14
    uint16_t coord_x : 8; // 35..28
    uint16_t coord_y : 8; // 43..36
    uint16_t : 4; // 44..47
}) pmd_toa_tot_t;

DEFINE_PMD_MAP(toa_tot)
{
    DEFINE_PMD_PAIR_COORD;
    DEFINE_PMD_PAIR_TOA;
    DEFINE_PMD_PAIR(hit_count);
    DEFINE_PMD_PAIR(tot);
}

PACKED(
typedef struct pmd_f_toa_only {
    uint16_t ftoa : 4; // 0..3
    uint16_t : 10; // 13..4
    uint16_t toa : 14; // 27..14
    uint16_t coord_x : 8; // 35..28
    uint16_t coord_y : 8; // 43..36
    uint16_t : 4; // 44..47
}) pmd_f_toa_only_t;

DEFINE_PMD_MAP(f_toa_only)
{
    DEFINE_PMD_PAIR_COORD;
    DEFINE_PMD_PAIR_TOA;
    DEFINE_PMD_PAIR(ftoa);
}

PACKED(
typedef struct pmd_toa_only {
    uint16_t hit_count : 4; // 0..3
    uint16_t : 10; // 13..4
    uint16_t toa : 14; // 27..14
    uint16_t coord_x : 8; // 35..28
    uint16_t coord_y : 8; // 43..36
    uint16_t : 4; // 44..47
}) pmd_toa_only_t;

DEFINE_PMD_MAP(toa_only)
{
    DEFINE_PMD_PAIR_COORD;
    DEFINE_PMD_PAIR_TOA;
    DEFINE_PMD_PAIR(hit_count);
}

PACKED(
typedef struct pmd_f_event_itot {
    uint16_t hit_count : 4; // 0..3
    uint16_t event_count : 10; // 13..4
    uint16_t integral_tot : 14; // 27..14
    uint16_t coord_x : 8; // 35..28
    uint16_t coord_y : 8; // 43..36
    uint16_t : 4; // 44..47
}) pmd_f_event_itot_t;

DEFINE_PMD_MAP(f_event_itot)
{
    DEFINE_PMD_PAIR_COORD;
    DEFINE_PMD_PAIR(hit_count);
    DEFINE_PMD_PAIR(event_count);
    DEFINE_PMD_PAIR(integral_tot);
}

PACKED(
typedef struct pmd_event_itot {
    uint16_t : 4; // 0..3
    uint16_t event_count : 10; // 13..4
    uint16_t integral_tot : 14; // 27..14
    uint16_t coord_x : 8; // 35..28
    uint16_t coord_y : 8; // 43..36
    uint16_t : 4; // 44..47
}) pmd_event_itot_t;

DEFINE_PMD_MAP(event_itot)
{
    DEFINE_PMD_PAIR_COORD;
    DEFINE_PMD_PAIR(event_count);
    DEFINE_PMD_PAIR(integral_tot);
}

#undef DEFINE_PMD_MAP
#undef DEFINE_PMD_PAIR
#undef DEFINE_PMD_PAIR_COORD
#undef DEFINE_PMD_PAIR_TOA

static inline void
flush_buffer(katherine_acquisition_t *acq)
{
    acq->handlers.pixels_received(acq->pixel_buffer, acq->pixel_buffer_valid);

    acq->current_frame_info.received_pixels += acq->pixel_buffer_valid;
    acq->pixel_buffer_valid = 0;
}

static inline void
handle_new_frame(katherine_acquisition_t *acq, const void *data)
{
    memset(&acq->current_frame_info, 0, sizeof(katherine_frame_info_t));
    acq->handlers.frame_started(acq->completed_frames);
}

static inline void
handle_timestamp_offset_driven_mode(katherine_acquisition_t *acq, const void *data)
{
    const md_time_offset_t *md = data;

    acq->last_toa_offset = 16384 * md->offset;
}

static inline void
handle_current_frame_finished(katherine_acquisition_t *acq, const void *data)
{
    const md_frame_finished_t *md = data;

    flush_buffer(acq);

    acq->current_frame_info.sent_pixels = md->n_sent;
    acq->handlers.frame_ended(acq->completed_frames, true, &acq->current_frame_info);

    ++acq->completed_frames;

    if (acq->completed_frames == acq->requested_frames) {
        acq->state = ACQUISITION_SUCCEEDED;
    }
}

static inline void
handle_frame_start_timestamp_lsb(katherine_acquisition_t *acq, const void *data)
{
    const md_time_lsb_t *md = data;

    acq->current_frame_info.start_time.b.lsb = md->lsb;
}

static inline void
handle_frame_start_timestamp_msb(katherine_acquisition_t *acq, const void *data)
{
    const md_time_msb_t *md = data;

    acq->current_frame_info.start_time.b.msb = md->msb;
}

static inline void
handle_frame_end_timestamp_lsb(katherine_acquisition_t *acq, const void *data)
{
    const md_time_lsb_t *md = data;

    acq->current_frame_info.end_time.b.lsb = md->lsb;
}

static inline void
handle_frame_end_timestamp_msb(katherine_acquisition_t *acq, const void *data)
{
    const md_time_msb_t *md = data;

    acq->current_frame_info.end_time.b.msb = md->msb;
}

static inline void
handle_lost_pixel_count(katherine_acquisition_t *acq, const void *data)
{
    const md_lost_px_t *md = data;

    acq->current_frame_info.lost_pixels += md->n_lost;
}

static inline void
handle_aborted_measurement(katherine_acquisition_t *acq, const void *data)
{
    acq->state = ACQUISITION_ABORTED;

    flush_buffer(acq);
    acq->handlers.frame_ended(acq->completed_frames, false, &acq->current_frame_info);
}

static inline void
handle_unknown_msg(katherine_acquisition_t *acq, const void *data)
{
    ++acq->dropped_measurement_data;
}

#endif /* DOXYGEN_SHOULD_SKIP_THIS */

/**
 * Initialize acquisition.
 * @param acq Acquisition to initialize
 * @param device Katherine device
 * @param md_buffer_size Size of the measurement data buffer in bytes
 * @param pixel_buffer_size Size of the pixel buffer in bytes
 * @return Error code.
 */
int
katherine_acquisition_init(katherine_acquisition_t *acq, katherine_device_t *device, size_t md_buffer_size, size_t pixel_buffer_size)
{
    acq->device = device;
    acq->state = ACQUISITION_NOT_STARTED;

    acq->md_buffer_size = md_buffer_size;
    acq->md_buffer = malloc(acq->md_buffer_size);
    if (!acq->md_buffer) goto err_datagram_buffer;

    acq->pixel_buffer_size = pixel_buffer_size;
    acq->pixel_buffer = malloc(acq->pixel_buffer_size);
    acq->pixel_buffer_valid = 0;
    if (!acq->pixel_buffer) goto err_pixel_buffer;

    return 0;

err_pixel_buffer:
    free(acq->md_buffer);
err_datagram_buffer:
    return 1;
}

/**
 * Finalize acquisition
 * @param acq Acquisition to finalize
 */
void
katherine_acquisition_fini(katherine_acquisition_t *acq)
{
    free(acq->md_buffer);
    free(acq->pixel_buffer);
}

/** TEST 

static inline void handle_measurement_data_f_toa_tot(katherine_acquisition_t *acq, const void *data)
{
	static const int PIXEL_SIZE = sizeof(katherine_px_f_toa_tot_t);
        const md_t *md = ((const md_t*)data);\
        if (md->header == 0x4) {
            if (acq->pixel_buffer_valid == acq->pixel_buffer_max_valid) {
                flush_buffer(acq);
            }
            
            pmd_f_toa_tot_map((katherine_px_f_toa_tot_t *) ((char*)acq->pixel_buffer) + acq->pixel_buffer_valid, (pmd_f_toa_tot_t *) md, acq);
            ++acq->pixel_buffer_valid;
        } else {
            switch (md->header) {
            case 0x5: handle_timestamp_offset_driven_mode(acq, data); break;
            case 0x7: handle_new_frame(acq, data); break;
            case 0x8: handle_frame_start_timestamp_lsb(acq, data); break;
            case 0x9: handle_frame_start_timestamp_msb(acq, data); break;
            case 0xA: handle_frame_end_timestamp_lsb(acq, data); break;
            case 0xB: handle_frame_end_timestamp_msb(acq, data); break;
            case 0xC: handle_current_frame_finished(acq, data); break;
            case 0xD: handle_lost_pixel_count(acq, data); break;
            case 0xE: handle_aborted_measurement(acq, data); break;
            default:  handle_unknown_msg(acq, data); break;
            }
        }
}

static int acquisition_read_f_toa_tot(katherine_acquisition_t *acq)
{
	static const int TRIES = 64; 
	static const int PIXEL_SIZE = sizeof(katherine_px_f_toa_tot_t);
    if (katherine_udp_mutex_lock(&acq->device->data_socket) != 0) return 1;
    \
    int tries = TRIES;
    int res;
    size_t i;
    size_t received;
    acq->pixel_buffer_valid = 0;
    acq->pixel_buffer_max_valid = acq->pixel_buffer_size / PIXEL_SIZE;

    while (acq->state == ACQUISITION_RUNNING) {
        received = acq->md_buffer_size;
        res = katherine_udp_recv(&acq->device->data_socket, acq->md_buffer, &received);
        
        if (res) {
            if (--tries == 0) {
                acq->state = ACQUISITION_TIMED_OUT;
            }
            
            continue;
        }
        
        tries = TRIES;
        
        const char *it = ((char*)acq->md_buffer);
        for (i = 0; i < received; i += KATHERINE_MD_SIZE, it += KATHERINE_MD_SIZE) {
            handle_measurement_data_f_toa_tot(acq, it);
        }
    }
    
    (void) katherine_udp_mutex_unlock(&acq->device->data_socket);
    return acq->state == ACQUISITION_SUCCEEDED;
}

/*******/

#define DEFINE_ACQ_IMPL(SUFFIX) \
    static inline void\
    handle_measurement_data_##SUFFIX(katherine_acquisition_t *acq, const void *data)\
    {\
        static const int PIXEL_SIZE = sizeof(katherine_px_##SUFFIX##_t);\
        const md_t *md = ((const md_t*)data);\
        \
        if (md->header == 0x4) {\
            if (acq->pixel_buffer_valid == acq->pixel_buffer_max_valid) {\
                flush_buffer(acq);\
            }\
            \
            pmd_##SUFFIX##_map((katherine_px_##SUFFIX##_t *) ((char*)acq->pixel_buffer) + acq->pixel_buffer_valid, (pmd_##SUFFIX##_t *) md, acq);\
            ++acq->pixel_buffer_valid;\
        } else {\
            switch (md->header) {\
            case 0x5: handle_timestamp_offset_driven_mode(acq, data); break;\
            case 0x7: handle_new_frame(acq, data); break;\
            case 0x8: handle_frame_start_timestamp_lsb(acq, data); break;\
            case 0x9: handle_frame_start_timestamp_msb(acq, data); break;\
            case 0xA: handle_frame_end_timestamp_lsb(acq, data); break;\
            case 0xB: handle_frame_end_timestamp_msb(acq, data); break;\
            case 0xC: handle_current_frame_finished(acq, data); break;\
            case 0xD: handle_lost_pixel_count(acq, data); break;\
            case 0xE: handle_aborted_measurement(acq, data); break;\
            default:  handle_unknown_msg(acq, data); break;\
            }\
        }\
    }\
    \
    static int\
    acquisition_read_##SUFFIX(katherine_acquisition_t *acq)\
    {\
        static const int TRIES = 64;\
        static const int PIXEL_SIZE = sizeof(katherine_px_##SUFFIX##_t);\
        \
        if (katherine_udp_mutex_lock(&acq->device->data_socket) != 0) return 1;\
        \
        int tries = TRIES;\
        int res;\
        \
        size_t i;\
        size_t received;\
        \
        acq->pixel_buffer_valid = 0;\
        acq->pixel_buffer_max_valid = acq->pixel_buffer_size / PIXEL_SIZE;\
        \
        while (acq->state == ACQUISITION_RUNNING) {\
            received = acq->md_buffer_size;\
            res = katherine_udp_recv(&acq->device->data_socket, acq->md_buffer, &received);\
            \
            if (res) {\
                if (--tries == 0) {\
                    acq->state = ACQUISITION_TIMED_OUT;\
                }\
                \
                continue;\
            }\
            \
            tries = TRIES;\
            \
            const /*void*/char *it = ((char*)acq->md_buffer);\
            for (i = 0; i < received; i += KATHERINE_MD_SIZE, it += KATHERINE_MD_SIZE) {\
                handle_measurement_data_##SUFFIX(acq, it);\
            }\
        }\
        \
        (void) katherine_udp_mutex_unlock(&acq->device->data_socket);\
        return acq->state == ACQUISITION_SUCCEEDED;\
    }

DEFINE_ACQ_IMPL(f_toa_tot);
DEFINE_ACQ_IMPL(toa_tot);
DEFINE_ACQ_IMPL(f_toa_only);
DEFINE_ACQ_IMPL(toa_only);
DEFINE_ACQ_IMPL(f_event_itot);
DEFINE_ACQ_IMPL(event_itot);

#undef DEFINE_ACQ_IMPL

/**
 * Read measurement data from acquisition.
 * @param acq Acquisition
 * @return Error code.
 */
int
katherine_acquisition_read(katherine_acquisition_t *acq)
{
    switch (acq->acq_mode) {
    case ACQUISITION_MODE_TOA_TOT:
        if (acq->fast_vco_enabled) {
            return acquisition_read_f_toa_tot(acq);
        } else {
            return acquisition_read_toa_tot(acq);
        }

    case ACQUISITION_MODE_ONLY_TOA:
        if (acq->fast_vco_enabled) {
            return acquisition_read_f_toa_only(acq);
        } else {
            return acquisition_read_toa_only(acq);
        }

    case ACQUISITION_MODE_EVENT_ITOT:
        if (acq->fast_vco_enabled) {
            return acquisition_read_f_event_itot(acq);
        } else {
            return acquisition_read_event_itot(acq);
        }

    default:
        return 1;
    }
}

/**
 * Set detector configuration and begin acquisition.
 * @param acq Acquisition
 * @param config Configuration
 * @param readout_mode Readout mode
 * @return Error code.
 */
int
katherine_acquisition_begin(katherine_acquisition_t *acq, const katherine_config_t *config, char readout_mode)
{
    int res;

    res = katherine_configure(acq->device, config);
    if (res) goto err;

    acq->readout_mode = readout_mode;
    acq->state = ACQUISITION_RUNNING;

    acq->completed_frames = 0;
    acq->requested_frames = config->no_frames;
    acq->dropped_measurement_data = 0;
    acq->acq_mode = config->acq_mode;
    acq->fast_vco_enabled = config->fast_vco_enabled;

    acq->pixel_buffer_valid = 0;
    acq->pixel_buffer_max_valid = 0;
    acq->last_toa_offset = 0;

    res = katherine_udp_mutex_lock(&acq->device->control_socket);
    if (res) goto err;

    res = katherine_cmd_start_acquisition(&acq->device->control_socket, readout_mode);
    if (res) goto err_cmd;

    (void) katherine_udp_mutex_unlock(&acq->device->control_socket);
    return 0;

err_cmd:
    (void) katherine_udp_mutex_unlock(&acq->device->control_socket);
err:
    return res;
}

/**
 * Abort acquisition (not recommended).
 * @param acq Acquisition
 * @return Error code.
 */
int
katherine_acquisition_abort(katherine_acquisition_t *acq)
{
    int res;

    res = katherine_udp_mutex_lock(&acq->device->control_socket);
    if (res) goto err;

    res = katherine_cmd_stop_acquisition(&acq->device->control_socket, acq->readout_mode);
    if (res) goto err;

    res = katherine_cmd_wait_ack(&acq->device->control_socket);
    if (res) goto err;

    (void) katherine_udp_mutex_unlock(&acq->device->control_socket);
    return 0;

err:
    (void) katherine_udp_mutex_unlock(&acq->device->control_socket);
    return res;
}

/**
 * Get human-readable description of acquisition status.
 * @param status Status to describe
 * @return Null-terminated string.
 */
const char*
katherine_str_acquisition_status(char status)
{
    switch (status) {
    case ACQUISITION_NOT_STARTED:   return "not started";
    case ACQUISITION_SUCCEEDED:     return "succeeded";
    case ACQUISITION_ABORTED:       return "aborted";
    case ACQUISITION_RUNNING:       return "running";
    case ACQUISITION_TIMED_OUT:     return "timed out";
    default:                        return "unknown";
    }
}
