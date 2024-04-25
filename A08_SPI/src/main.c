/**
Author: Tarush Sharma 

Description: This code is used to initiate full duplex communication using Serial Peripheral Interface (SPI)
             for interfacing with the ADC ADS7883.
 */
#include <asf.h>

#define ADC_CIPO   PIN_PA20
#define ADC_SCLK   PIN_PB03
#define ADC_CS1    PIN_PA02
#define ADC_CS2    PIN_PA03
#define ADC_CS3    PIN_PA10
#define ADC_CS4    PIN_PA11

// Data buffers for SPI 
uint16_t buffer_length = 4;
uint16_t dummy_char = 0xDD;  // Dummy character to send during SPI read
const uint8_t tx_buffer[2] = {0x25, 0x56};
uint8_t rx_buffer[4];
bool select_slave = 1;
bool deselect_slave = 0;
struct spi_module spi_master_instance;
struct spi_slave_inst slave1;

// Function prototypes
void configure_spi_master(void);
void pinmux(void);

int main(void)
{
    system_init();

    configure_spi_master();

    /* Continuous SPI communication */
    while (1) {
        spi_select_slave(&spi_master_instance, &slave1, select_slave);
        spi_read_buffer_wait(&spi_master_instance, rx_buffer, buffer_length, dummy_char);
        spi_select_slave(&spi_master_instance, &slave1, deselect_slave);
    }
}

void configure_spi_master() {
    struct spi_config config_spi_master;
    struct spi_slave_inst_config slave_dev_config;

    spi_get_config_defaults(&config_spi_master);
    spi_slave_inst_get_config_defaults(&slave_dev_config);
    slave_dev_config.ss_pin = ADC_CS2;

    spi_attach_slave(&slave1, &slave_dev_config);

    config_spi_master.mux_setting = SPI_SIGNAL_MUX_SETTING_B; 
    config_spi_master.pinmux_pad0 = PINMUX_PA20C_SERCOM5_PAD2; // MISO
    config_spi_master.pinmux_pad1 = PINMUX_UNUSED;            // SS controlled manually
    config_spi_master.pinmux_pad2 = PINMUX_PB02D_SERCOM5_PAD0; // MOSI
    config_spi_master.pinmux_pad3 = PINMUX_PB03D_SERCOM5_PAD1; // SCK

    config_spi_master.mode_specific.master.baudrate = 500000;  // 1 MHz clock rate
    config_spi_master.transfer_mode = SPI_TRANSFER_MODE_3;  // Mode 1 for ADS7883
    config_spi_master.select_slave_low_detect_enable = false;

    spi_init(&spi_master_instance, SERCOM5, &config_spi_master);
    spi_enable(&spi_master_instance);
}

void pinmux() {
    struct port_config pin_conf;
    port_get_config_defaults(&pin_conf);

    // Set CS pins as outputs
    pin_conf.direction = PORT_PIN_DIR_OUTPUT;
    port_pin_set_config(ADC_CS1, &pin_conf);
    port_pin_set_config(ADC_CS2, &pin_conf);
    port_pin_set_config(ADC_CS3, &pin_conf);
    port_pin_set_config(ADC_CS4, &pin_conf);
}