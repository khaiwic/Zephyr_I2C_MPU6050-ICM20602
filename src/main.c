#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/i2c.h>
#include <stdio.h>

#define ICM20602_I2C_ADDR 0x68
#define REG_WHO_AM_I      0x75
#define REG_PWR_MGMT_1    0x6B
#define REG_ACCEL_XOUT_H  0x3B

/* Khởi tạo thiết bị I2C từ Devicetree */
#define I2C_NODE DT_NODELABEL(i2c1)
static const struct device *const i2c_dev = DEVICE_DT_GET(I2C_NODE);

void read_icm20602_i2c(void) {
    uint8_t id;
    
    if (!device_is_ready(i2c_dev)) {
        printk("Loi: I2C bus chua san sang!\n");
        return;
    }

    /* Đọc WHO_AM_I (Kết quả chuẩn là 0x12) */
    i2c_reg_read_byte(i2c_dev, ICM20602_I2C_ADDR, REG_WHO_AM_I, &id);
    printk("I2C WHO_AM_I: 0x%02X\n", id);

    /* Đánh thức cảm biến */
    i2c_reg_write_byte(i2c_dev, ICM20602_I2C_ADDR, REG_PWR_MGMT_1, 0x01);
    k_msleep(10);

    /* Đọc Burst 6 byte gia tốc thô */
    uint8_t buf[6];
    if (i2c_burst_read(i2c_dev, ICM20602_I2C_ADDR, REG_ACCEL_XOUT_H, buf, 6) == 0) {
        int16_t ax = (int16_t)((buf[0] << 8) | buf[1]);
        printk("I2C Accel X (Raw): %d\n", ax);
    }
}

int main(void) {
    read_icm20602_i2c();
    return 0;
}