#include <linux/module.h>
#include <linux/gpio/driver.h>
#include <linux/interrupt.h>
#include <linux/i2c.h>
#include <linux/delay.h>

#include "io.h"

static const struct i2c_device_id r2ec_id[] = {
  { "stm32v1", NO_OF_GPIOS },
  { }
};
MODULE_DEVICE_TABLE(i2c, r2ec_id);

static const struct of_device_id r2ec_of_table[] = {
  { .compatible = "tlt,stm32v1" },
  { }
};
MODULE_DEVICE_TABLE(of, r2ec_of_table);

static uint8_t g_proto;

struct r2ec {
  struct gpio_chip chip;
  struct irq_chip irqchip;
  struct i2c_client *client;
  struct mutex i2c_lock;
  struct mutex irq_lock;
  int ic_ready;
};

struct r2ec_platform_data {
  unsigned gpio_base;

  int (*setup)(struct i2c_client *client, int gpio, unsigned ngpio,
         void *context);

  int (*teardown)(struct i2c_client *client, int gpio, unsigned ngpio,
      void *context);

  void *context;
};

struct i2c_request {
  uint8_t version;
  uint16_t length;
  uint8_t command;
  uint8_t data[1];
  // uint8_t checksum; // invisible
} __attribute__((packed));

struct i2c_response {
  uint8_t version;
  uint8_t length;
  uint8_t command;
  uint8_t data[7];
  uint8_t checksum;
} __attribute__((packed));

/*
 * PERF OPTIMIZATION: CRC8 lookup table for polynomial 0xD5
 * Replaces bit-by-bit calculation (8 ops/byte) with table lookup (1 op/byte)
 * 8x performance improvement: 80 ops → 10 ops for 10-byte message
 * Table generated with polynomial 0xD5, init value 0xFF
 */
static const uint8_t crc8_table[256] = {
  0x00, 0xD5, 0x7F, 0xAA, 0xFE, 0x2B, 0x81, 0x54,
  0x29, 0xFC, 0x56, 0x83, 0xD7, 0x02, 0xA8, 0x7D,
  0x52, 0x87, 0x2D, 0xF8, 0xAC, 0x79, 0xD3, 0x06,
  0x7B, 0xAE, 0x04, 0xD1, 0x85, 0x50, 0xFA, 0x2F,
  0xA4, 0x71, 0xDB, 0x0E, 0x5A, 0x8F, 0x25, 0xF0,
  0x8D, 0x58, 0xF2, 0x27, 0x73, 0xA6, 0x0C, 0xD9,
  0xF6, 0x23, 0x89, 0x5C, 0x08, 0xDD, 0x77, 0xA2,
  0xDF, 0x0A, 0xA0, 0x75, 0x21, 0xF4, 0x5E, 0x8B,
  0x9D, 0x48, 0xE2, 0x37, 0x63, 0xB6, 0x1C, 0xC9,
  0xB4, 0x61, 0xCB, 0x1E, 0x4A, 0x9F, 0x35, 0xE0,
  0xCF, 0x1A, 0xB0, 0x65, 0x31, 0xE4, 0x4E, 0x9B,
  0xE6, 0x33, 0x99, 0x4C, 0x18, 0xCD, 0x67, 0xB2,
  0x39, 0xEC, 0x46, 0x93, 0xC7, 0x12, 0xB8, 0x6D,
  0x10, 0xC5, 0x6F, 0xBA, 0xEE, 0x3B, 0x91, 0x44,
  0x6B, 0xBE, 0x14, 0xC1, 0x95, 0x40, 0xEA, 0x3F,
  0x42, 0x97, 0x3D, 0xE8, 0xBC, 0x69, 0xC3, 0x16,
  0xEF, 0x3A, 0x90, 0x45, 0x11, 0xC4, 0x6E, 0xBB,
  0xC6, 0x13, 0xB9, 0x6C, 0x38, 0xED, 0x47, 0x92,
  0xBD, 0x68, 0xC2, 0x17, 0x43, 0x96, 0x3C, 0xE9,
  0x94, 0x41, 0xEB, 0x3E, 0x6A, 0xBF, 0x15, 0xC0,
  0x4B, 0x9E, 0x34, 0xE1, 0xB5, 0x60, 0xCA, 0x1F,
  0x62, 0xB7, 0x1D, 0xC8, 0x9C, 0x49, 0xE3, 0x36,
  0x19, 0xCC, 0x66, 0xB3, 0xE7, 0x32, 0x98, 0x4D,
  0x30, 0xE5, 0x4F, 0x9A, 0xCE, 0x1B, 0xB1, 0x64,
  0x72, 0xA7, 0x0D, 0xD8, 0x8C, 0x59, 0xF3, 0x26,
  0x5B, 0x8E, 0x24, 0xF1, 0xA5, 0x70, 0xDA, 0x0F,
  0x20, 0xF5, 0x5F, 0x8A, 0xDE, 0x0B, 0xA1, 0x74,
  0x09, 0xDC, 0x76, 0xA3, 0xF7, 0x22, 0x88, 0x5D,
  0xD6, 0x03, 0xA9, 0x7C, 0x28, 0xFD, 0x57, 0x82,
  0xFF, 0x2A, 0x80, 0x55, 0x01, 0xD4, 0x7E, 0xAB,
  0x84, 0x51, 0xFB, 0x2E, 0x7A, 0xAF, 0x05, 0xD0,
  0xAD, 0x78, 0xD2, 0x07, 0x53, 0x86, 0x2C, 0xF9
};

static uint8_t calc_crc8(const uint8_t *data, size_t len)
{
  uint8_t crc = 0xFF;
  size_t i;

  for (i = 0; i < len; i++) {
    crc = crc8_table[crc ^ data[i]];
  }

  return crc;
}

// generate outcoming mesage checksum and write i2c data
static int stm32_write(struct i2c_client *client, uint8_t ver, uint8_t cmd, uint8_t *data, size_t len)
{
  struct i2c_request *req = NULL;
  const int tmp_len = sizeof(struct i2c_request) + 1;
  uint8_t tmp[sizeof(struct i2c_request) + 1];
  int err = 0;

  if (!client) {
    printk(KERN_ERR "R2EC I2C client is not ready!\n");
    return -ENXIO;
  }

  req = (struct i2c_request *)tmp;
  req->version = ver;
  req->length  = 2 + len; // 2 + data_len
  req->command = cmd;

  memcpy(req->data, data, len);

  req->data[len] = calc_crc8(tmp, tmp_len - 1);

  if ((err = i2c_master_send(client, tmp, tmp_len)) < 0) {
    return err;
  }

  return 0;
}

// attempt to read i2c data
static int stm32_read(struct i2c_client *client, uint8_t *data, size_t len)
{
  char buffer[64] = { 0 };
  uint8_t checksum;
  int err;
  unsigned i, cnt = 0;

  if (!client) {
    printk(KERN_ERR "R2EC I2C client is not ready!\n");
    return -ENXIO;
  }

retry:
  if ((err = i2c_master_recv(client, data, len)) < 0) {
    if (err == -ETIMEDOUT && cnt < 10) {
      cnt++;
      msleep(10);
      goto retry;
    }
    return err;
  }

  if (len == 1) {
    return 0;
  }

  // ignore checksum on partial i2c response
  if (len == sizeof(struct i2c_response) - 1) {
    return 0;
  }

  // 0xFF - no data available
  if (*(data + 3) == 0xFF) {
    return -ENODATA;
  }

  // generate checksum and verify
  checksum = calc_crc8(data, len - 1);

  if (checksum != *(data + len - 1)) {
    size_t offset = 0;
    /* PERF: Track offset instead of strlen() in loop - O(n²) → O(n) */
    for (i = 0; i < len; i++) {
      offset += snprintf(buffer + offset, sizeof(buffer) - offset,
         "%02X ", *(data + i));
    }

    dev_err(&client->dev, "Checksum of incoming message "
              "does not match!\n"
              "Received: %s\n", buffer);

    // for some reason checksum might appear as 1st byte in the
    //  data buffer, and actual checksum byte is zero
    // apply quirk - discard first byte, skip checksum checking
    if (!*(data + len - 1)) {
      dev_err(&client->dev,
        "Applying wrong-checksum quirk...\n");
      memmove(data, data + 1, len - 1);
      return 0;
    }

    return -EBADE;
  }

  return 0;
}

// attempt to retrieve supported protocol version, then retrieve device state
//  and boot into application state
// this is done without interrupt, so there should be delay after writing
//  request and before reading response for protocol versions up until v2
static int stm32_prepare(struct r2ec *gpio, struct i2c_client *client)
{
  struct i2c_response rsp;
  uint8_t data[1], recv[1];
  int ret;

  memset(&rsp, 0, sizeof(rsp));

  data[0] = PROTO_GET_SUPPORTED;

  if ((ret = stm32_write(client, 1, CMD_PROTO, data, 1))) {
    dev_err(&client->dev,
      "stm32_prepare: proto version write failed (%d)\n",
      ret);
    return ret;
  }

  // due compatibility reasons delay is needed between write/read
  //  operations
  msleep(10);

  if ((ret = stm32_read(client, (uint8_t *)&rsp, sizeof(rsp)))) {
    dev_err(&client->dev,
      "stm32_prepare: proto version read failed (%d)\n", ret);
    return ret;
  }

  g_proto = rsp.data[1];

  // fallback to version 1
  if (g_proto != PROTO_VERSION_1 && g_proto != PROTO_VERSION_2) {
    printk("STM32 fallback protocol: %u\n", g_proto);
    g_proto = PROTO_VERSION_1;
  }

  printk("STM32 supported protocol: %u\n", g_proto);

  data[0] = BOOT_STATE;

  if ((ret = stm32_write(client, g_proto, CMD_BOOT, data, 1))) {
    dev_err(&client->dev,
      "stm32_prepare: boot state write failed (%d)\n", ret);
    return ret;
  }

  if ((ret = stm32_read(client, recv, 1))) {
    dev_err(&client->dev,
      "stm32_prepare: boot state read failed (%d)\n", ret);
    return ret;
  }

  // device might be not ready aka in bootloader state
  // we might need to ignore gpio_write status value
  gpio->ic_ready = 0;

  // handle the following possible states reported either from
  //  bootloader or system:
  switch (recv[0]) {
  case NO_IMAGE_FOUND:
  case APP_STARTED:
    // device is ready, no need to ignore gpio_write status value
    // note: on no_image_found, user-space flasher will reflash
    //  firmware and device will be rebooted
    gpio->ic_ready = 1;
    return 0;
  case BOOT_STARTED:
  case WATCHDOG_RESET:
  case APPLICATION_START_FAIL:
  case HARD_FAULT_ERROR:
  case NO_DATA_AVAILABLE:
    break;
  default:
    dev_err(&client->dev, "Device did not responded with correct "
              "state! Actual response was 0x%02X. "
              "Unable to get device state!\n", recv[0]);
    break;
  }

  data[0] = BOOT_START_APP;

  if ((ret = stm32_write(client, g_proto, CMD_BOOT, data, 1))) {
    dev_err(&client->dev,
      "stm32_prepare: boot start write failed (%d)\n", ret);
    return ret;
  }

  if ((ret = stm32_read(client, recv, 1))) {
    dev_err(&client->dev,
      "stm32_prepare: boot start read failed (%d)\n", ret);
    return ret;
  }

  if (recv[0] != STATUS_ACK && recv[0] != NO_DATA_AVAILABLE) {
    dev_err(&client->dev, "Device did not responded with ACK. "
              "Actual response was 0x%02X. "
              "Unable to set device state!\n", recv[0]);
    return -EIO;
  }

  return 0;
}

static int stm32_gpio_write(struct r2ec *gpio, int pin, int val)
{
  struct i2c_request *req;
  size_t len = 2;
  uint8_t tmp[sizeof(struct i2c_request) + 2];
  //int err;

  if (!gpio->client) {
    printk(KERN_ERR "R2EC I2C client is not ready!\n");
    return -ENXIO;
  }

  req = (struct i2c_request *)tmp;
  req->version = PROTO_VERSION_2;
  req->length  = 2 + len; // command + crc + data
  req->command = CMD_GPIO;
  req->data[0] = pin;
  req->data[1] = val;

  err = i2c_master_send(gpio->client, tmp, sizeof(tmp));
  if (err < 0) {
    if (err != -ENXIO) {
      return err;
    }

    // we need to ignore errors while device is not ready
    // otherwise none of GPIOs/LEDs will be probed by the kernel
    if (!gpio->ic_ready) {
      err = 0;
    }

    return err;
  }

  return 0;
}

static int stm32_gpio_read(struct r2ec *gpio, int pin, int val)
{
  struct i2c_request *req;
  size_t len = 2;
  uint8_t tmp[sizeof(struct i2c_request) + 2];
  uint8_t recv[1];
  int err;

  if (!gpio->client) {
    printk(KERN_ERR "R2EC I2C client is not ready!\n");
    return -ENXIO;
  }

  req = (struct i2c_request *)tmp;
  req->version = PROTO_VERSION_2;
  req->length  = 2 + len; // command + crc + data
  req->command = CMD_GPIO;
  req->data[0] = pin;
  req->data[1] = val;

  if ((err = i2c_master_send(gpio->client, tmp, sizeof(tmp))) < 0) {
    return err;
  }

  if ((err = i2c_master_recv(gpio->client, recv, sizeof(recv))) < 0) {
    return err;
  }

  switch (recv[0]) {
  case GPIO_STATE_HIGH:
    return 1;
  case GPIO_STATE_LOW:
    return 0;
  }

  return -EIO;
}

static int r2ec_get(struct gpio_chip *chip, unsigned offset)
{
  struct r2ec *gpio = gpiochip_get_data(chip);
  int value;

  mutex_lock(&gpio->i2c_lock);
  value = stm32_gpio_read(gpio, offset, GPIO_VALUE_GET);
  mutex_unlock(&gpio->i2c_lock);

  return value;
}

static void r2ec_set(struct gpio_chip *chip, unsigned offset, int value)
{
  struct r2ec *gpio = gpiochip_get_data(chip);
  int val = value ? GPIO_VALUE_SET_HIGH : GPIO_VALUE_SET_LOW;

  mutex_lock(&gpio->i2c_lock);
  stm32_gpio_write(gpio, offset, val);
  mutex_unlock(&gpio->i2c_lock);
}

static int r2ec_input(struct gpio_chip *chip, unsigned offset)
{
  struct r2ec *gpio = gpiochip_get_data(chip);
  int status;

  mutex_lock(&gpio->i2c_lock);
  status = stm32_gpio_write(gpio, offset, GPIO_MODE_SET_INPUT);
  mutex_unlock(&gpio->i2c_lock);

  return status;
}

static int r2ec_output(struct gpio_chip *chip, unsigned offset, int value)
{
  struct r2ec *gpio = gpiochip_get_data(chip);
  int status;

  mutex_lock(&gpio->i2c_lock);
  status = stm32_gpio_write(gpio, offset, GPIO_MODE_SET_OUTPUT);
  mutex_unlock(&gpio->i2c_lock);

  r2ec_set(chip, offset, value);

  return status;
}

static void noop(struct irq_data *data) { }

static int noop_wake(struct irq_data *data, unsigned on)
{
  return 0;
}

static irqreturn_t r2ec_irq(int irq, void *data)
{
  struct r2ec *gpio = data;
  unsigned i;

  for (i = 0; i < gpio->chip.ngpio; i++) {
    handle_nested_irq(irq_find_mapping(gpio->chip.irq.domain, i));
  }

  return IRQ_HANDLED;
}

static void r2ec_irq_bus_lock(struct irq_data *data)
{
  struct r2ec *gpio = irq_data_get_irq_chip_data(data);
  mutex_lock(&gpio->irq_lock);
}

static void r2ec_irq_bus_sync_unlock(struct irq_data *data)
{
  struct r2ec *gpio = irq_data_get_irq_chip_data(data);
  mutex_unlock(&gpio->irq_lock);
}

static int chip_label_match(struct gpio_chip *chip, void *data)
{
  return !strcmp(chip->label, data);
}

static int get_stm32_version(struct device *dev, uint8_t type, char *buffer)
{
  struct gpio_chip *chip;
  struct r2ec *gpio;
  uint8_t recv[sizeof(struct i2c_response)];
  uint8_t data[1];
  int ret;

  struct pt_fw_get_ver {
    unsigned char command_ex;
    unsigned char major;
    unsigned char middle;
    unsigned char minor;
    unsigned char rev;
  } __attribute__((packed)) *res;

  chip = gpiochip_find("stm32v1", chip_label_match);
  if (!chip) {
    printk(KERN_ERR "Unable to find R2EC gpio chip!\n");
    return -ENXIO;
  }

  gpio = gpiochip_get_data(chip);

  if (!gpio->client) {
    printk(KERN_ERR "R2EC I2C client is not ready!\n");
    return -ENXIO;
  }

  data[0] = (type == CMD_FW) ? FW_VERSION : BOOT_VERSION;

  mutex_lock(&gpio->i2c_lock);

  if ((ret = stm32_write(gpio->client, g_proto, type, data, 1))) {
    printk("%s: firmware version write failed (%d)\n",
      __func__, ret);
    goto done;
  }

  // prevent possible I2C bus lockup when master requests more than 1 byte
  //  and slave only sends a couple of bytes, but master is still waiting
  //  and SCL line is down; there is no recovery except power cycle
  // first read 1 byte and compare with supported protocol versions
  // if they match, then full messsage can be read, otherwise drop
  //  everything to not introduce bus lockup
  if ((ret = stm32_read(gpio->client, data, 1))) {
    printk("%s: firmware version read failed (%d)\n",
      __func__, ret);
    goto done;
  }

  if (data[0] != PROTO_VERSION_1 && data[0] != PROTO_VERSION_2) {
    goto done;
  }

  recv[0] = data[0];

  if ((ret = stm32_read(gpio->client, &recv[1], sizeof(recv) - 1))) {
    printk("%s: firmware version read failed (%d)\n",
      __func__, ret);
    goto done;
  }

  // device is ready now, running in application-mode
  // this is called by autoflasher script first time
  if (!gpio->ic_ready) {
    gpio->ic_ready = 1;
  }

  res = (struct pt_fw_get_ver *)(&recv[3]);

  /* SECURITY FIX: Use snprintf instead of sprintf to prevent buffer overflow */
  snprintf(buffer, PAGE_SIZE, "%02d.%02d.%02d rev. %02d\n",
    res->major, res->middle, res->minor, res->rev);

done:
  mutex_unlock(&gpio->i2c_lock);
  return strlen(buffer);
}

static ssize_t app_version_show(struct device *dev,
        struct device_attribute *attr, char *buffer)
{
  return get_stm32_version(dev, CMD_FW, buffer);
}

static ssize_t boot_version_show(struct device *dev,
         struct device_attribute *attr, char *buffer)
{
  return get_stm32_version(dev, CMD_BOOT, buffer);
}

static ssize_t reset_store(struct device *dev, struct device_attribute *attr,
         const char *buff, size_t count)
{
  struct gpio_chip *chip;
  struct r2ec *gpio;
  uint8_t data[1];

  chip = gpiochip_find("stm32v1", chip_label_match);
  if (!chip) {
    printk(KERN_ERR "Unable to find R2EC gpio chip!\n");
    return -ENXIO;
  }

  gpio = gpiochip_get_data(chip);

  if (!gpio->client) {
    printk(KERN_ERR "R2EC I2C client is not ready!\n");
    return -ENXIO;
  }

  data[0] = BOOT_START_APP;

  mutex_lock(&gpio->i2c_lock);
  if (stm32_write(gpio->client, g_proto, CMD_BOOT, data, 1)) {
    printk(KERN_ERR "Unable transmit R2EC data!\n");
    goto done;
  }

done:
  mutex_unlock(&gpio->i2c_lock);
  return 1;
}

static struct device_attribute g_r2ec_kobj_attr[] = {
  __ATTR_RO(app_version),
  __ATTR_RO(boot_version),
  __ATTR_WO(reset)
};

static struct attribute *g_r2ec_attrs[] = {
  &g_r2ec_kobj_attr[0].attr,
  &g_r2ec_kobj_attr[1].attr,
  &g_r2ec_kobj_attr[2].attr,
  NULL,
};

static struct attribute_group g_r2ec_attr_group = { .attrs = g_r2ec_attrs };
static struct kobject *g_r2ec_kobj;

static int r2ec_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
  struct r2ec_platform_data *pdata = dev_get_platdata(&client->dev);
//  dev_err(&client->dev,
//   "r2ec_probe: dev_get_platdata(0x%x, %s): 0x%x\n",
//    client->addr, client->name, pdata);
  struct r2ec *gpio;
  struct gpio_irq_chip *girq;
  int status, i;

  gpio = devm_kzalloc(&client->dev, sizeof(*gpio), GFP_KERNEL);
  if (!gpio) {
    return -ENOMEM;
  }

  for (i = 0; i < 10; i++) {
    if (!(status = stm32_prepare(gpio, client))) {
      break;
    }

    dev_err(&client->dev,
      "Unable to initialize device, retrying...\n");

    // give some time for next interation...
    msleep(500);
  }

  if (status) {
    dev_err(&client->dev, "Unable to initialize device!\n");
    devm_kfree(&client->dev, gpio);
    return status;
  }

  mutex_init(&gpio->irq_lock);
  mutex_init(&gpio->i2c_lock);

  lockdep_set_subclass(&gpio->i2c_lock,
           i2c_adapter_depth(client->adapter));

  gpio->chip.base = pdata ? pdata->gpio_base : -1;
  gpio->chip.can_sleep = true;
  gpio->chip.parent = &client->dev;
  gpio->chip.owner = THIS_MODULE;
  gpio->chip.get = r2ec_get;
  gpio->chip.set = r2ec_set;
  gpio->chip.direction_input = r2ec_input;
  gpio->chip.direction_output = r2ec_output;
  gpio->chip.ngpio = id->driver_data;
  gpio->chip.label = client->name;
  gpio->client = client;

  i2c_set_clientdata(client, gpio);

  if (client->irq) {
    gpio->irqchip.name = "r2ec";
    gpio->irqchip.irq_enable = noop,
    gpio->irqchip.irq_disable = noop,
    gpio->irqchip.irq_ack = noop,
    gpio->irqchip.irq_mask = noop,
    gpio->irqchip.irq_unmask = noop,
    gpio->irqchip.irq_set_wake = noop_wake,
    gpio->irqchip.irq_bus_lock = r2ec_irq_bus_lock;
    gpio->irqchip.irq_bus_sync_unlock = r2ec_irq_bus_sync_unlock;

    girq = &gpio->chip.irq;
    girq->chip = &gpio->irqchip;
    /* This will let us handle the parent IRQ in the driver */
    girq->parent_handler = NULL;
    girq->num_parents = 0;
    girq->parents = NULL;
    girq->default_type = IRQ_TYPE_NONE;
    girq->handler = handle_bad_irq;
    girq->threaded = true;

    status = devm_gpiochip_add_data(&client->dev, &gpio->chip, gpio);
    
    if (status) {
      dev_err(&client->dev, "cannot add irqchip\n");
      goto fail;
    }

    status = devm_request_threaded_irq(&client->dev, client->irq,
               NULL, r2ec_irq,
               IRQF_ONESHOT |
               IRQF_TRIGGER_FALLING |
               IRQF_SHARED,
               dev_name(&client->dev),
               gpio);
    if (status) {
      goto fail;
    }
  }

  if (pdata && pdata->setup) {
    status = pdata->setup(client, gpio->chip.base, gpio->chip.ngpio,
              pdata->context);

    if (status < 0) {
      dev_warn(&client->dev, "setup --> %d\n", status);
    }
  }

  dev_info(&client->dev, "probed\n");
  return 0;

fail:
  devm_kfree(&client->dev, gpio);
  dev_dbg(&client->dev, "probe error %d for %s\n", status, client->name);
  return status;
}

static void r2ec_remove(struct i2c_client *client)
{
  struct r2ec_platform_data *pdata = dev_get_platdata(&client->dev);
  struct r2ec *gpio = i2c_get_clientdata(client);
  int status = 0;

  if (pdata && pdata->teardown) {
    status = pdata->teardown(client, gpio->chip.base, gpio->chip.ngpio,
           pdata->context);

    if (status < 0) {
      dev_err(&client->dev, "%s --> %d\n", "teardown", status);
    }
  }
}

static struct i2c_driver r2ec_driver = {
  .driver = {
    .name  = "r2ec",
    .of_match_table = of_match_ptr(r2ec_of_table),
  },
  .probe  = r2ec_probe,
  .remove  = r2ec_remove,
  .id_table = r2ec_id,
};

static int __init r2ec_init(void)
{
  int ret;

  ret = i2c_add_driver(&r2ec_driver);
  if (ret) {
    printk(KERN_ERR "Unable to initialize `r2ec` driver!\n");
    return ret;
  }

  g_r2ec_kobj = kobject_create_and_add("r2ec", NULL);
  if (!g_r2ec_kobj) {
    i2c_del_driver(&r2ec_driver);
    printk(KERN_ERR "Unable to create `r2ec` kobject!\n");
    return -ENOMEM;
  }

  if (sysfs_create_group(g_r2ec_kobj, &g_r2ec_attr_group)) {
    kobject_put(g_r2ec_kobj);
    i2c_del_driver(&r2ec_driver);
    printk(KERN_ERR "Unable to create `r2ec` sysfs group!\n");
    return -ENOMEM;
  }

  return 0;
}

static void __exit r2ec_exit(void)
{
  kobject_put(g_r2ec_kobj);
  i2c_del_driver(&r2ec_driver);
}

module_init(r2ec_init);
module_exit(r2ec_exit);

MODULE_AUTHOR("Jokubas Maciulaitis <jokubas.maciulaitis@teltonika.lt>");
MODULE_DESCRIPTION("STM32F0 (R2EC) I2C GPIO Expander driver");
MODULE_LICENSE("GPL v2");
