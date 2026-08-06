#include "sensor.h"
#include "freertos/FreeRTOS.h"
#include "esp_log.h"
#include "driver/i2c.h"
#include "esp_adc/adc_oneshot.h"

static const char *TAG = "SENSORS";

// --- Configuración ADC (MQ-136 en GPIO 1) ---
// En el ESP32-S3, el GPIO 1 corresponde al canal 0 del ADC 1
adc_oneshot_unit_handle_t adc1_handle;
#define ADC_CHANNEL ADC_CHANNEL_0 

// --- Configuración I2C (BME280) ---
#define I2C_MASTER_SDA_IO           5
#define I2C_MASTER_SCL_IO           6
#define I2C_MASTER_NUM              I2C_NUM_0
#define I2C_MASTER_FREQ_HZ          100000 // 100 kHz estándar
#define I2C_MASTER_TX_BUF_DISABLE   0
#define I2C_MASTER_RX_BUF_DISABLE   0

void sensors_init() {
    // 1. Inicializar el ADC (MQ-136)
    adc_oneshot_unit_init_cfg_t init_config1 = {
        .unit_id = ADC_UNIT_1,
        .clk_src = ADC_DIGI_CLK_SRC_DEFAULT,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config1, &adc1_handle));

    adc_oneshot_chan_cfg_t config = {
        .atten = ADC_ATTEN_DB_12, // Permite leer voltajes hasta ~3.3V
        .bitwidth = ADC_BITWIDTH_DEFAULT,
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, ADC_CHANNEL, &config));
    ESP_LOGI(TAG, "ADC configurado exitosamente para el MQ-136");

    // 2. Inicializar el bus I2C (BME280)
    i2c_config_t conf = {};
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = I2C_MASTER_SDA_IO;
    conf.scl_io_num = I2C_MASTER_SCL_IO;
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf.master.clk_speed = I2C_MASTER_FREQ_HZ;
    // Si tu versión de ESP-IDF pide clk_flags, descomenta la siguiente línea:
    // conf.clk_flags = 0; 

    ESP_ERROR_CHECK(i2c_param_config(I2C_MASTER_NUM, &conf));
    ESP_ERROR_CHECK(i2c_driver_install(I2C_MASTER_NUM, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0));
    ESP_LOGI(TAG, "Bus I2C configurado exitosamente");
}

int read_mq136_raw() {
    int adc_raw = 0;
    ESP_ERROR_CHECK(adc_oneshot_read(adc1_handle, ADC_CHANNEL, &adc_raw));
    return adc_raw;
}

BME280_Data read_bme280() {
    BME280_Data data;
    
    // TODO: Implementar lectura real de registros I2C
    // Por ahora retornamos datos fijos para probar la cadena de transmisión
    data.temperature = 22.5f; 
    data.humidity = 45.2f;
    data.pressure = 1013.25f;

    return data;
}