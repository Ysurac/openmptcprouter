#include <common.h>

#ifdef CONFIG_MNFINFO_SUPPORT

#include <mnf_info.h>
#include <spi_flash.h>

#define MNF_INFO_SIZE 0x20000

static struct spi_flash *flash = NULL;
static char mnf_cache[MNF_INFO_SIZE];

static int flash_probe(void)
{
	struct spi_flash *new_flash;

	if (flash)
		return 0;

	new_flash = spi_flash_probe(CONFIG_SF_DEFAULT_BUS, CONFIG_SF_DEFAULT_CS, 
                                    CONFIG_SF_DEFAULT_SPEED, CONFIG_SF_DEFAULT_MODE);

	if (!new_flash)
		return 1;

	flash = new_flash;
	return 0;
}

extern int mnf_flash_read(const mnf_field_t *field, char *result)
{
        int ret;

        ret = flash_probe();
        if (ret)
                return ret;

	return spi_flash_read(flash, CONFIG_MNF_INFO_START + field->offset, field->length, result);
}

extern int mnf_flash_write_init(void)
{
        int ret;

        ret = flash_probe();
        if (ret)
                return ret;

	return spi_flash_read(flash, CONFIG_MNF_INFO_START, MNF_INFO_SIZE, mnf_cache);
}

extern int mnf_flash_write(const mnf_field_t *field, const char *buf)
{
        memcpy(mnf_cache + field->offset, buf, field->length);
        return 0;
}

extern int mnf_flash_write_finalize(void)
{
        int ret;
        
        ret = spi_flash_erase(flash, CONFIG_MNF_INFO_START, MNF_INFO_SIZE);
        if (ret)
                return ret;

        return spi_flash_write(flash, CONFIG_MNF_INFO_START, MNF_INFO_SIZE, mnf_cache);
}

#endif