#include <stdint.h>
#include <stdbool.h>

#define STRING_NA(a) do { (a)[0] = 'N'; (a)[1] = '/'; (a)[2] = 'A'; (a)[3] = 0; } while(0)
#define STRING_NA_LENGTH 3

#define PARTITION_SIZE 0xA0
#define SECTION_SIZE 0x10

#if defined(RUTX_PLATFORM)
#include "rutx.h"
#elif defined(TRB1_PLATFORM)
#include "trb1.h"
#elif defined(TRB2_PLATFORM)
#include "trb2.h"
#elif defined(RUT2_PLATFORM)
#include "rut2.h"
#elif defined(RUT300_PLATFORM) || defined(RUT360_PLATFORM)
#include "rut3.h"
#else
#error Platform not recognized!
#endif

#if !(defined(MAC_OFFSET) && defined(MAC_LENGTH))
#undef MAC_OFFSET
#undef MAC_LENGTH
#define MAC_OFFSET 0
#define MAC_LENGTH STRING_NA_LENGTH
#else
#define MAC_ENABLED
#endif

#if !(defined(NAME_OFFSET) && defined(NAME_LENGTH))
#undef NAME_OFFSET
#undef NAME_LENGTH
#define NAME_OFFSET 0
#define NAME_LENGTH STRING_NA_LENGTH
#else
#define NAME_ENABLED
#endif

#if !(defined(WPS_OFFSET) && defined(WPS_LENGTH))
#undef WPS_OFFSET
#undef WPS_LENGTH
#define WPS_OFFSET 0
#define WPS_LENGTH STRING_NA_LENGTH
#else
#define WPS_ENABLED
#endif

#if !(defined(SERIAL_OFFSET) && defined(SERIAL_LENGTH))
#undef SERIAL_OFFSET
#undef SERIAL_LENGTH
#define SERIAL_OFFSET 0
#define SERIAL_LENGTH STRING_NA_LENGTH
#else
#define SERIAL_ENABLED
#endif

#if !(defined(BATCH_OFFSET) && defined(BATCH_LENGTH))
#undef BATCH_OFFSET
#undef BATCH_LENGTH
#define BATCH_OFFSET 0
#define BATCH_LENGTH STRING_NA_LENGTH
#else
#define BATCH_ENABLED
#endif

#if !(defined(HWVER_OFFSET) && defined(HWVER_LENGTH))
#undef HWVER_OFFSET
#undef HWVER_LENGTH
#define HWVER_OFFSET 0
#define HWVER_LENGTH STRING_NA_LENGTH
#else
#define HWVER_ENABLED
#endif

#if !(defined(SIMPIN_OFFSET) && defined(SIMPIN_LENGTH))
#undef SIMPIN_OFFSET
#undef SIMPIN_LENGTH
#define SIMPIN_OFFSET 0
#define SIMPIN_LENGTH STRING_NA_LENGTH
#else
#define SIMPIN_ENABLED
#endif

#if !(defined(WIFI_OFFSET) && defined(WIFI_LENGTH))
#undef WIFI_OFFSET
#undef WIFI_LENGTH
#define WIFI_OFFSET 0
#define WIFI_LENGTH STRING_NA_LENGTH
#else
#define WIFI_ENABLED
#endif

/*
 * mnfinfo_get_*() functions return ptr to static memory, which is not to be free()d!
 * the returned memory ptr is safely accessible throughout the using program's runtime
 * bad: free(mnfinfo_get_mac());
 *
 * returns NULL on /dev/mtdX reading failure, "N/A" if the particular device
 *  doesn't support the field, or a dummy string if the mtdblock space contains garbage
*/
char* mnfinfo_get_mac(void);
char* mnfinfo_get_name(void);
char* mnfinfo_get_maceth(void);
char* mnfinfo_get_sn(void);
char* mnfinfo_get_hwver(void);
char* mnfinfo_get_batch(void);
char* mnfinfo_get_wps(void);
char* mnfinfo_get_wifi_pw(void);
char* mnfinfo_get_sim_pin(uint8_t sim_id);

// returns true on success
bool mnfinfo_set_sim_pin(uint8_t sim_id, const char *pin);
