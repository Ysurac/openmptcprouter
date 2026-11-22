// SPDX-License-Identifier: GPL-2.0
/*
 * Teltonika RUTX STM32F0 I/O expander ("R2EC", compatible "tlt,stm32v1").
 *
 * Based on the r2ec driver by Jokubas Maciulaitis from the Teltonika GPL
 * sources RUTX_R_00.02.06.1, as adapted for Linux 5.4 by Felix Matouschek
 * (github.com/0xFelix/teltonika-rutx-openwrt).
 */

#include <linux/gpio/driver.h>
#include <linux/i2c.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/unaligned.h>

#define R2EC_PROTO_VERSION	0x02
#define R2EC_NGPIO		37

#define CMD_GPIO		0x06
#define CMD_BOOT		0xfe

#define BOOT_START_APP		0x03
#define BOOT_STATE		0xfd

#define NO_IMAGE_FOUND		0x17
#define APP_STARTED		0xfc
#define STATUS_ACK		0x7d

#define GPIO_VALUE_SET_LOW	0x00
#define GPIO_VALUE_SET_HIGH	0x01
#define GPIO_VALUE_GET		0x02
#define GPIO_MODE_SET_OUTPUT	0x04
#define GPIO_MODE_SET_INPUT	0x05

#define GPIO_STATE_HIGH		0x1e
#define GPIO_STATE_LOW		0x9f

struct r2ec {
	struct gpio_chip chip;
	struct i2c_client *client;
	struct mutex lock;
};

static u8 r2ec_crc8(const u8 *data, size_t len)
{
	u8 crc = 0xff;
	int i;

	while (len--) {
		crc ^= *data++;
		for (i = 0; i < 8; i++)
			crc = (crc & 0x80) ? (crc ^ 0xd5) << 1 : crc << 1;
	}

	return crc;
}

/*
 * Request: version, le16 length (command + data + crc), command, data, crc8.
 * Replies used here are a single status byte.
 */
static int r2ec_xfer(struct i2c_client *client, u8 cmd, const u8 *data,
		     size_t len, u8 *reply)
{
	u8 buf[7];
	int ret;

	buf[0] = R2EC_PROTO_VERSION;
	put_unaligned_le16(len + 2, &buf[1]);
	buf[3] = cmd;
	memcpy(&buf[4], data, len);
	buf[4 + len] = r2ec_crc8(buf, 4 + len);

	ret = i2c_master_send(client, buf, len + 5);
	if (ret < 0)
		return ret;

	if (!reply)
		return 0;

	ret = i2c_master_recv(client, reply, 1);
	return ret < 0 ? ret : 0;
}

/* The STM32 may still sit in its bootloader: start the application */
static int r2ec_start(struct i2c_client *client)
{
	u8 arg = BOOT_STATE, reply;
	int ret;

	ret = r2ec_xfer(client, CMD_BOOT, &arg, 1, &reply);
	if (ret)
		return ret;

	if (reply == APP_STARTED || reply == NO_IMAGE_FOUND)
		return 0;

	arg = BOOT_START_APP;
	ret = r2ec_xfer(client, CMD_BOOT, &arg, 1, &reply);
	if (ret)
		return ret;

	if (reply != STATUS_ACK) {
		dev_err(&client->dev, "start application: reply 0x%02x\n", reply);
		return -EIO;
	}

	return 0;
}

static int r2ec_gpio(struct gpio_chip *chip, unsigned int offset, u8 op,
		     u8 *reply)
{
	struct r2ec *r2ec = gpiochip_get_data(chip);
	u8 data[2] = { offset, op };
	int ret;

	mutex_lock(&r2ec->lock);
	ret = r2ec_xfer(r2ec->client, CMD_GPIO, data, sizeof(data), reply);
	mutex_unlock(&r2ec->lock);

	return ret;
}

static int r2ec_get(struct gpio_chip *chip, unsigned int offset)
{
	u8 state;
	int ret;

	ret = r2ec_gpio(chip, offset, GPIO_VALUE_GET, &state);
	if (ret)
		return ret;

	switch (state) {
	case GPIO_STATE_HIGH:
		return 1;
	case GPIO_STATE_LOW:
		return 0;
	}

	return -EIO;
}

static int r2ec_set(struct gpio_chip *chip, unsigned int offset, int value)
{
	return r2ec_gpio(chip, offset,
			 value ? GPIO_VALUE_SET_HIGH : GPIO_VALUE_SET_LOW, NULL);
}

static int r2ec_direction_input(struct gpio_chip *chip, unsigned int offset)
{
	return r2ec_gpio(chip, offset, GPIO_MODE_SET_INPUT, NULL);
}

static int r2ec_direction_output(struct gpio_chip *chip, unsigned int offset,
				 int value)
{
	int ret;

	ret = r2ec_gpio(chip, offset, GPIO_MODE_SET_OUTPUT, NULL);
	return ret ?: r2ec_set(chip, offset, value);
}

/*
 * shortcut: no irqchip and no version/reset sysfs files from the Teltonika
 * driver; add a gpio_irq_chip when a DTS uses stm32_io as interrupt parent.
 */
static int r2ec_probe(struct i2c_client *client)
{
	struct r2ec *r2ec;
	int ret;

	ret = r2ec_start(client);
	if (ret)
		return dev_err_probe(&client->dev, ret, "STM32 not ready\n");

	r2ec = devm_kzalloc(&client->dev, sizeof(*r2ec), GFP_KERNEL);
	if (!r2ec)
		return -ENOMEM;

	ret = devm_mutex_init(&client->dev, &r2ec->lock);
	if (ret)
		return ret;

	r2ec->client = client;
	r2ec->chip.label = client->name;
	r2ec->chip.parent = &client->dev;
	r2ec->chip.owner = THIS_MODULE;
	r2ec->chip.base = -1;
	r2ec->chip.ngpio = R2EC_NGPIO;
	r2ec->chip.can_sleep = true;
	r2ec->chip.get = r2ec_get;
	r2ec->chip.set = r2ec_set;
	r2ec->chip.direction_input = r2ec_direction_input;
	r2ec->chip.direction_output = r2ec_direction_output;

	return devm_gpiochip_add_data(&client->dev, &r2ec->chip, r2ec);
}

static const struct i2c_device_id r2ec_id[] = {
	{ "stm32v1" },
	{ }
};
MODULE_DEVICE_TABLE(i2c, r2ec_id);

static const struct of_device_id r2ec_of_match[] = {
	{ .compatible = "tlt,stm32v1" },
	{ }
};
MODULE_DEVICE_TABLE(of, r2ec_of_match);

static struct i2c_driver r2ec_driver = {
	.driver = {
		.name = "r2ec",
		.of_match_table = r2ec_of_match,
	},
	.probe = r2ec_probe,
	.id_table = r2ec_id,
};
module_i2c_driver(r2ec_driver);

MODULE_AUTHOR("Jokubas Maciulaitis <jokubas.maciulaitis@teltonika.lt>");
MODULE_DESCRIPTION("Teltonika RUTX STM32F0 (R2EC) I2C GPIO expander");
MODULE_LICENSE("GPL v2");
