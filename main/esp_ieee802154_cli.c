// Source code based on https://github.com/espressif/esp-idf/blob/master/examples/ieee802154/ieee802154_cli/main/esp_ieee802154_cli.c

#include <stdio.h>
#include <string.h>
#include "ieee802154_cmd.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_console.h"
#include "esp_vfs_dev.h"
#include "esp_vfs_fat.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "esp_ieee802154.h"
#include "esp_phy_init.h"
#include "cmd_system.h"
#include "ieee802154_cmd.h"

#include "sdkconfig.h"

#define PROMPT_STR "ieee802154"

#define DEFAULT_CHANNEL  18
#define DEFAULT_PANID    0xabcd
#define DEFAULT_PROMISC_ENABLE false

static void initialize_nvs(void)
{
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);
}

void app_main(void)
{
    initialize_nvs();

    esp_ieee802154_enable();
    esp_console_repl_t *repl = NULL;
    esp_console_repl_config_t repl_config = ESP_CONSOLE_REPL_CONFIG_DEFAULT();
    /* Prompt to be printed before each line.
     * This can be customized, made dynamic, etc.
     */
    repl_config.prompt = PROMPT_STR ">";
    repl_config.max_cmdline_length = 256;

    /* Register commands */
    esp_console_register_help_command();
    register_ieee802154_cmd();
    register_system_common();
#if CONFIG_IEEE802154_DEBUG
    register_ieee802154_debug_cmd();
#endif


#if CONFIG_ESP_CONSOLE_UART_DEFAULT
esp_console_dev_uart_config_t hw_config = ESP_CONSOLE_DEV_UART_CONFIG_DEFAULT();
ESP_ERROR_CHECK(esp_console_new_repl_uart(&hw_config, &repl_config, &repl));
#endif

#if CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG
    esp_console_dev_usb_serial_jtag_config_t hw_config = ESP_CONSOLE_DEV_USB_SERIAL_JTAG_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_console_new_repl_usb_serial_jtag(&hw_config, &repl_config, &repl));
#endif

    ESP_ERROR_CHECK(esp_console_start_repl(repl));

    esp_ieee802154_set_channel(DEFAULT_CHANNEL);
    esp_ieee802154_set_panid(DEFAULT_PANID);
    esp_ieee802154_set_promiscuous(DEFAULT_PROMISC_ENABLE);

    while(1) {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}