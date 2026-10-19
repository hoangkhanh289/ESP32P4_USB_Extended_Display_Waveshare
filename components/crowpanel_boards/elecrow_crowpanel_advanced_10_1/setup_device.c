#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/i2c_master.h"
#include "esp_log.h"
#include "esp_check.h"
#include "esp_ldo_regulator.h"

#include "dev_display_lcd.h"

#if __has_include(<esp_lcd_jd9365_10_1.h>)
#define HAS_JD9365 1
#include "esp_lcd_jd9365_10_1.h"
#endif

#if __has_include(<esp_lcd_touch_gt911.h>)
#define HAS_GT911 1
#include "esp_lcd_touch_gt911.h"
#endif

static const char *TAG = "WAVESHARE_SETUP";

static esp_err_t waveshare_backlight_init(void)
{
    // 1. Ensure Backlight GPIO 26 is configured as output and pulled HIGH
    gpio_config_t bk_gpio_conf = {
        .pin_bit_mask = (1ULL << 26),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&bk_gpio_conf);
    gpio_set_level(26, 1);
    ESP_LOGI(TAG, "Backlight GPIO 26 initialized HIGH");

    // 2. Also ensure LCD RST (GPIO 27) is released HIGH
    gpio_config_t rst_gpio_conf = {
        .pin_bit_mask = (1ULL << 27),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&rst_gpio_conf);
    gpio_set_level(27, 0);
    vTaskDelay(pdMS_TO_TICKS(10));
    gpio_set_level(27, 1);
    vTaskDelay(pdMS_TO_TICKS(20));
    ESP_LOGI(TAG, "Panel reset GPIO 27 pulsed and set HIGH");

    return ESP_OK;
}

#if defined(HAS_JD9365)

__attribute__((weak))
esp_err_t lcd_dsi_panel_factory_entry_t(
    esp_lcd_dsi_bus_handle_t dsi_handle,
    dev_display_lcd_config_t *lcd_cfg,
    dev_display_lcd_handles_t *lcd_handles)
{
    ESP_LOGI(TAG, "Initializing Waveshare 10.1 JD9365 panel");

    // Initialize backlight and reset pins
    waveshare_backlight_init();

    jd9365_vendor_config_t vendor_config = {
        .flags = {
            .use_mipi_interface = 1,
        },
        .mipi_config = {
            .dsi_bus = dsi_handle,
            .dpi_config = &lcd_cfg->sub_cfg.dsi.dpi_config,
            .lane_num = 2,
        },
    };

    const esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = lcd_cfg->sub_cfg.dsi.reset_gpio_num,
        .rgb_ele_order = lcd_cfg->rgb_ele_order,
        .data_endian = lcd_cfg->data_endian,
        .bits_per_pixel = lcd_cfg->bits_per_pixel,
        .flags = {
            .reset_active_high = lcd_cfg->sub_cfg.dsi.reset_active_high,
        },
        .vendor_config = &vendor_config,
    };

    ESP_LOGI(TAG, "Calling esp_lcd_new_panel_jd9365");
    return esp_lcd_new_panel_jd9365(
        lcd_handles->io_handle,
        &panel_config,
        &lcd_handles->panel_handle
    );
}

#endif


#if defined(HAS_GT911)

__attribute__((weak))
esp_err_t lcd_touch_factory_entry_t(
    esp_lcd_panel_io_handle_t io,
    const esp_lcd_touch_config_t *touch_dev_config,
    esp_lcd_touch_handle_t *ret_touch)
{
    ESP_LOGI(TAG, "Creating GT911/GT9271 touch driver");
    return esp_lcd_touch_new_i2c_gt911(
        io,
        touch_dev_config,
        ret_touch
    );
}

#endif
