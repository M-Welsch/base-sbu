#include <avr/io.h>
#include <avr/interrupt.h>

/* pin definitions */

#define v_meas_flt_port PORTA
#define v_meas_flt 1 //PA1

#define button_port PORTA
#define button_0 PIN2_bm //PA2
#define button_0_ctrl PIN2CTRL //PA2
#define button_1 PIN3_bm //PA3
#define button_1_ctrl PIN3CTRL //PA3

#define en_5v_port PORTA
#define en_5v PIN4_bm //PA4

#define dis_rs_port PORTA
#define dis_rs PIN6_bm //PA6

#define dis_e_port PORTA
#define dis_e PIN7_bm //PA7

#define dis_pwm_port PORTB
#define dis_pwm PIN0_bm //PB0
#define dis_bpi_sply PIN1_bm //PB1
#define led_hmi PIN4_bm //PB4

/* PB7: BPI Heartbeat */
#define bpi_heartbeat_port PORTB
#define bpi_heartbeat PIN5_bm
#define bpi_heartbeat_ctrl PIN5CTRL

/* PC0 - PC3: Display Data Bus */
#define dis_data_port PORTC
#define dis_db4 PIN0_bm
#define dis_db5 PIN1_bm
#define dis_db6 PIN2_bm
#define dis_db7 PIN3_bm

/* function prototypes */

void init_pins(void);
void disable_digital_input_buffers_for_adc();
int button_0_pressed(void);
int button_1_pressed(void);

/* HMI Buttons */

void set_interrupts_for_buttons(void);

/* HMI Display */

void dim_display(int dimming_value);
void toggle_display_backlight(void);
void display_enable(uint8_t duration_ms);
void set_display_data_pins(uint8_t data_nibble);
void toggle_rs_pin(void);

void display_set_rs_pin(void);
void display_clear_rs_pin(void);

/* USART */
void set_pb2_txd_and_pb3_rxd(void);
void disable_usart_tx(void);

/* Heartbeat */
void setup_interrupt_for_hb_edge_detection(void);

/* Interrupt Service Routines */
ISR(PORTA_PORT_vect);