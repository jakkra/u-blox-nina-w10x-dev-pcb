#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_spi_flash.h"
#include "driver/gpio.h"

#define NUM_GPIO_PAIRS (sizeof(inputs) / sizeof(inputs[0]))

// On NINA-W106 pin 32 and 33 are swapped
#ifdef W106
static gpio_num_t outputs[] = { GPIO_NUM_23, GPIO_NUM_22, GPIO_NUM_15, GPIO_NUM_2, GPIO_NUM_19, GPIO_NUM_18, GPIO_NUM_5, GPIO_NUM_13, GPIO_NUM_21, GPIO_NUM_4, GPIO_NUM_0 };
static gpio_num_t inputs[] = { GPIO_NUM_34, GPIO_NUM_35, GPIO_NUM_32, GPIO_NUM_33, GPIO_NUM_25, GPIO_NUM_26, GPIO_NUM_12, GPIO_NUM_27, GPIO_NUM_14, GPIO_NUM_36, GPIO_NUM_39 };
#else
static gpio_num_t outputs[] = { GPIO_NUM_23, GPIO_NUM_22, GPIO_NUM_15, GPIO_NUM_2, GPIO_NUM_19, GPIO_NUM_18, GPIO_NUM_5, GPIO_NUM_13, GPIO_NUM_21, GPIO_NUM_4, GPIO_NUM_0 };
static gpio_num_t inputs[] = { GPIO_NUM_34, GPIO_NUM_35, GPIO_NUM_33, GPIO_NUM_32, GPIO_NUM_25, GPIO_NUM_26, GPIO_NUM_12, GPIO_NUM_27, GPIO_NUM_14, GPIO_NUM_36, GPIO_NUM_39 };
#endif

const char* TAG = "GPIO_TEST_APP";

static void configure_output_gpio(gpio_num_t pin) {
	gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = 1ULL << pin;
    io_conf.pull_down_en = 1;
    io_conf.pull_up_en = 0;
    gpio_config(&io_conf);
}

static void configure_input_gpio(gpio_num_t pin) {
	gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = 1ULL << pin;
    io_conf.pull_down_en = 1;
    io_conf.pull_up_en = 0;
    gpio_config(&io_conf);
}

static esp_err_t verify_all_gpio_low_except_pin(gpio_num_t except_this_pin) {
    esp_err_t err = ESP_OK;

    for (uint32_t i = 0; i < NUM_GPIO_PAIRS; i++) {
        uint32_t in = gpio_get_level(inputs[i]);
        
        if (inputs[i] == except_this_pin && in != 1) {
            ESP_LOGE(TAG, "FAIL. Expected pin %d to be changed to 1\n", inputs[i]);
            err = ESP_FAIL;
        }
        if (in != 0 && inputs[i] != except_this_pin) {
            ESP_LOGE(TAG, "FATAL FAIL. Error on GPIO pair %d-%d was %d, expected 0\n", outputs[i], inputs[i], in);
            err = ESP_FAIL;
        }
    }

    if (err == ESP_OK) {
        ESP_LOGI(TAG,"PASS\n");
    }

    return err;
}

void app_main(void)
{
    ESP_LOGI(TAG, "Starting GPIO test!\n");
    esp_err_t test_status = ESP_OK;

    /* Print chip information */
    esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);
    ESP_LOGI(TAG, "This is ESP32 chip with %d CPU cores, WiFi%s%s, ",
            chip_info.cores,
            (chip_info.features & CHIP_FEATURE_BT) ? "/BT" : "",
            (chip_info.features & CHIP_FEATURE_BLE) ? "/BLE" : "");

    ESP_LOGI(TAG, "silicon revision %d, ", chip_info.revision);

    ESP_LOGI(TAG, "%dMB %s flash\n", spi_flash_get_chip_size() / (1024 * 1024),
            (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");

    for (uint32_t i = 0; i < NUM_GPIO_PAIRS; i++) {
        configure_input_gpio(inputs[i]);
        configure_output_gpio(outputs[i]);
    }

    // Make sure all low default
    for (uint32_t i = 0; i < NUM_GPIO_PAIRS; i++) {
        gpio_set_level(outputs[i], 0);
        uint32_t in = gpio_get_level(inputs[i]);
        if (in != 0) {
            ESP_LOGE(TAG, "ERROR detected on GPIO pair %d-%d\n", outputs[i], inputs[i]);
            test_status = ESP_FAIL;
        }
    }

    for (uint32_t i = 0; i < NUM_GPIO_PAIRS; i++) {
        ESP_LOGI(TAG, "Set %d to 1 expects %d to match", outputs[i], inputs[i]);
        gpio_set_level(outputs[i], 1);
        if (verify_all_gpio_low_except_pin(inputs[i]) == ESP_FAIL) {
            test_status = ESP_FAIL;
        }
        // Restore pin back to 0
        gpio_set_level(outputs[i], 0);
        vTaskDelay(pdMS_TO_TICKS(1));
    }

    if (test_status == ESP_OK) {
        ESP_LOGW(TAG, "GPIO TEST PASSED");
    } else {
        ESP_LOGE(TAG, "GPIO TEST FAILED");
    }
}
