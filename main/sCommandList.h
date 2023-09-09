#ifndef __SCOMMANDLIST
#define __SCOMMANDLIST

#include <stdint.h>
#include <stdbool.h>

#include "esp_err.h"

//Command Strings
#define GPIO_W_CMD_STR  "GPIO-WRITE"
#define GPIO_R_CMD_STR  "GPIO-READ"
#define PWM_CMD_STR     "PWM-SET"
#define ADC_CMD_STR     "ADC-READ"
#define CNT_CMD_STR     "COUNTER-SETUP"
#define INT_CMD_STR     "INT-MODE"
#define NULL_CMD_STR     "NULL"

//Control Strings
#define RAW_CTL_STR     "RAW-CONTROL"
#define OP_CLT_STR      "OPERATIONAL-CONTROL"
#define PID_CTL_SRT     "PID-CONTROL"

//Command MODES
#define NULL_MODE       0
#define GPIO_R_MODE     1
#define GPIO_W_MODE     2
#define PWM_MODE        3
#define ADC_MODE        4
#define CNT_MODE        5
#define INT_MODE        0x20  //new inte
#define SPECIAL_MODE    0xFF

//Control MODES
#define RAW_CONTROL_MODE    0
#define OP_CONTROL_MODE     1
#define PID_CONTROL_MODE    2

// Lenght
#define UUID_LENGHT     37
#define COMPACTED_UUID_LEN 17

//Control Structs
typedef struct{
    uint8_t actuator_pin_num;
    uint8_t mode;
    void* control_struct;
} sControl_t;

typedef struct{
    /* data */
} sControl_raw_t;

typedef struct{
    uint8_t operational_mode;   // 0: equal | 1: Less | 2: bigger | 3: less equal | 4: bigger equal
    uint32_t ref_value;
} sControl_op_t;

typedef struct{
    uint8_t  pid_mode;  // bit 1:P | bit 2: I | bit 3: D
    uint32_t P_milli;
    uint32_t I_milli;
    uint32_t D_milli;
    uint32_t ref_value;
    uint32_t last_I;
    uint32_t last_data;
} sControl_pid_t;

//Command Structs
typedef struct{
    char uuid[UUID_LENGHT]; // identificador do comando unico
    uint8_t mode; // Um dos defines
    uint8_t pin_num;
    uint64_t period_us;
    void* cmd_struct;
    sControl_t* control;
} sCommand_t;

typedef struct{
    char uuid[UUID_LENGHT];
    uint8_t pin_num;
    uint8_t mode;
    esp_err_t error_type;
} sError_t;

typedef struct{
    uint8_t pin_num;
    bool state;
} sCommand_gpio_w_t;

typedef struct{
    uint8_t pin_num;
} sCommand_gpio_r_t;

typedef struct {
    uint8_t channel_num; //If channel_num > 7 -> adc2, else -> adc1
    uint8_t width; // resolution between 9 and 12 bits
    uint8_t atten_db;
    uint8_t num_samples;
} sCommand_adc_cfg_t;

typedef struct {
    uint8_t pin_num;
    uint8_t channel_num;
    uint8_t unit;
    uint8_t ctrl_gpio_num;
    uint16_t filter_val;
    bool high_control;
    bool cnt_high_borders;
} sCommand_cnt_cfg_t;

typedef struct {
    uint8_t pin_num;
    uint8_t channel_num;
    uint8_t timer_num;
    uint32_t duty;
    uint32_t frequency; //TO AVOID FLOAT?
    uint8_t speed_mode;
    uint32_t fade_time_ms;
} sCommand_pwm_cfg_t;

typedef struct {
    uint8_t pin_num;
    uint8_t inter_type;//interruption type
    //function to exec.
} sCommand_int_cfg_t;

//Map Struct
typedef struct {
    char uuid[UUID_LENGHT];
    uint8_t pin_mode;
    uint64_t last_time_stamp;
    uint64_t period_us;
    void* cmd_struct;
    sControl_t* control_struct;
} sProgramMap_t;

//Return to Platform Struct
typedef struct{
    char uuid[UUID_LENGHT];
    uint8_t pin_num;
    uint8_t pin_mode;
    int32_t data;
    uint64_t timestamp;
} sData_t;


//Handshake Data
#define MAC_ADDRESS_LEN 18
#define EMAIL_LEN 320
#define PASSWORD_LEN 32
#define UC_IDENTIFIER_LEN 32
typedef struct{
    char email[EMAIL_LEN];
    char password[PASSWORD_LEN];
    char mac_address[MAC_ADDRESS_LEN];
    char uc_identifier[UC_IDENTIFIER_LEN];
}sHandshakeSend_t;

#define MESSAGE_LEN 8
#define AUTORIZATION_LEN 17
typedef struct{
    bool status;
    char message[MESSAGE_LEN];
    char autorization[AUTORIZATION_LEN];
}sHandshakeReceive_t;

#endif