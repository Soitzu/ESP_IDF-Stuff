#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "sdkconfig.h"
#include "driver/i2c_master.h"


#define BLINK_LED GPIO_NUM_27
#define BLINK_BUTTON GPIO_NUM_25

#define LCD_SDA GPIO_NUM_32
#define LCD_SCL GPIO_NUM_33
#define LCD_ADDRESS 0x27

#define I2C_MASTER_TIMEOUT_MS 1000

TaskHandle_t CheckAddressHandle = NULL;

void initLED() {
    //LED 
    gpio_reset_pin(BLINK_LED);
    gpio_set_direction(BLINK_LED, GPIO_MODE_OUTPUT);
}

void checkButtonLED() {
    if (!gpio_get_level(BLINK_BUTTON)) {
        gpio_set_level(BLINK_LED, 1);            
    } else {
        gpio_set_level(BLINK_LED, 0);
    }
}

void initButton() {
    //BUTTON
    gpio_reset_pin(BLINK_BUTTON);
    gpio_set_direction(BLINK_BUTTON, GPIO_MODE_INPUT);
    gpio_set_pull_mode(BLINK_BUTTON, GPIO_PULLUP_ONLY);
}

static void i2c_master_init_bus(i2c_master_bus_handle_t *bus_handle) {
    i2c_master_bus_config_t i2c_mst_config = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = I2C_NUM_0,
        .scl_io_num = LCD_SCL,
        .sda_io_num = LCD_SDA,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };
    ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_mst_config, bus_handle));   
}

static void i2c_master_init_handle(i2c_master_bus_handle_t *bus_handle, i2c_master_dev_handle_t *dev_handle, uint8_t address) {
    i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_7,
        .device_address = address,
        .scl_speed_hz = 100000,
    };
    ESP_ERROR_CHECK(i2c_master_bus_add_device(*bus_handle, &dev_cfg, dev_handle));
}

void check_address_task(void *arg) {
    i2c_master_bus_handle_t bus_handle = (i2c_master_bus_handle_t) arg;
    while(1) {
        esp_err_t err;
        for(uint8_t i = 3; i < 0x78; i++) {
            err = i2c_master_probe(bus_handle, i, 1000);
            if (err == ESP_OK) {
                printf("I2C Scanner found I2C device at: 0x%X \n", i);
            }
        }
        printf("Scanner complete!");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}




void app_main(void)
{
    ESP_LOGI("Example", "Programm zum Üben!");

    //init i2c handle
    i2c_master_bus_handle_t bus_handle;
    i2c_master_init_bus(&bus_handle);

    //device handle
    i2c_master_dev_handle_t dev_handle;

    xTaskCreatePinnedToCore(check_address_task, "Scan I2C", 16384, (void *) bus_handle, 10, &CheckAddressHandle, 1);

    

    esp_err_t i2c_del_master_bus(i2c_master_bus_handle_t bus_handle);
    esp_err_t i2c_master_bus_rm_device(i2c_master_dev_handle_t dev_handle);

    






    initLED();
    initButton();


    while(1) {
        checkButtonLED();
        vTaskDelay(10 / portTICK_PERIOD_MS);


    }


}
