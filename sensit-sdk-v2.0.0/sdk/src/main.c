/*!******************************************************************
 * \file main.c
 * \brief Sens'it SDK template
 * \author Sens'it Team
 * \copyright Copyright (c) 2018 Sigfox, All Rights Reserved.
 *
 * This file is an empty main template.
 * You can use it as a basis to develop your own firmware.
 *******************************************************************/
/******* INCLUDES **************************************************/
#include "sensit_types.h"
#include "sensit_api.h"
#include "error.h"
#include "button.h"
#include "battery.h"
#include "radio_api.h"
#include "fxos8700.h"
#include "discovery.h"

/******** DEFINES **************************************************/
#define MEASUREMENT_PERIOD                 60 /* Measurement & Message sending period, in second */
#define VIBRATION_THRESHOLD                0x10 /* With 2g range, 3,9 mg threshold */
#define VIBRATION_COUNT                    2

#define BATTERY_LVL_MIN         2700 /* 2.7 V  */
#define BATTERY_LVL_MAX         4250 /* 4.25 V */
#define BATTERY_LVL_OFFSET      2700 /* 2.7 V  */
#define BATTERY_LVL_STEP        50   /* 50 mV  */

/******* GLOBAL VARIABLES ******************************************/
u8 firmware_version[] = "TEMPLATE";

/*!******************************************************************
 * \struct discovery_payload_s
 * \brief Payload structure of Sens'it Discovery
 *
 * To convert battery level from payload in V:
 *     (battery × 0.05) + 2.7
 *
 * To convert temperature from payload in °C:
 *     (temperature - 200) / 8
 *
 * To convert relative humidity from payload in %:
 *     humidity / 2
 *
 * To convert brightness from payload in lux:
 *     brightness / 96
 *******************************************************************/
typedef struct {

    struct {
        u8 reserved:3;       /* Must be 0b110 */
        u8 battery:5;	     /* Battery level */
    };

    struct {
        u8 special_value:2;  /* Mode VIBRATION: 01 -> vibration detected */
        u8 button:1;         /* If TRUE, double presses message */
        u8 mode:5;           /* Payload mode type */
    };

	union {
		u8 event_counterMSB; /* Mode DOOR, VIBRATION & MAGNET */
	};

	union {
		u8 event_counterLSB; /* Mode DOOR, VIBRATION & MAGNET */
	};

} payload_s;

/*!******************************************************************
 * \struct discovery_data_s
 * \brief Input data structure to build payload.
 *******************************************************************/
typedef struct {
    u16 battery;
    bool vibration;
    u16 event_counter;
    bool button;
} data_s;

/*!************************************************************************
 * \fn void DISCOVERY_build_payload(discovery_payload_s* payload, discovery_mode_e mode, discovery_data_s* data)
 * \brief Function to build the Sens'it Discovery payload.
 *
 * \param[out] payload                     Built payload
 * \param[in] mode                         Mode of the payload structure
 * \param[in] data                         Input data
 **************************************************************************/
void build_payload(payload_s* payload, data_s* data);

/*******************************************************************/

int main()
{
    error_t err;
    button_e btn;
    u16 battery_level;
    bool send = FALSE;

    /* Discovery payload variable */
    data_s data = {0};
    payload_s payload;

    /* Start of initialization */

    /* Configure button */
    SENSIT_API_configure_button(INTERRUPT_BOTH_EGDE);

    /* Initialize Sens'it radio */
    err = RADIO_API_init();
    ERROR_parser(err);

    /* Initialize accelerometer */
    err = FXOS8700_init();
    ERROR_parser(err);

    /* Initialize RTC alarm timer */
    SENSIT_API_set_rtc_alarm(MEASUREMENT_PERIOD);

    /* Clear pending interrupt */
    pending_interrupt = 0;

    /* Put accelerometer in transient mode */
    FXOS8700_set_transient_mode(FXOS8700_RANGE_2G, VIBRATION_THRESHOLD, VIBRATION_COUNT);

    /* End of initialization */

    while (TRUE)
    {
        /* Execution loop */

        /* Check of battery level */
        BATTERY_handler(&battery_level);

        /* RTC alarm interrupt handler */
        if ((pending_interrupt & INTERRUPT_MASK_RTC) == INTERRUPT_MASK_RTC)
        {
            send = TRUE;
            /* Clear interrupt */
            pending_interrupt &= ~INTERRUPT_MASK_RTC;
        }

        /* Button interrupt handler */
        if ((pending_interrupt & INTERRUPT_MASK_BUTTON) == INTERRUPT_MASK_BUTTON)
        {
            /* RGB Led ON during count of button presses */
            SENSIT_API_set_rgb_led(RGB_MAGENTA);

            /* Count number of presses */
            btn = BUTTON_handler();

            SENSIT_API_set_rgb_led(RGB_OFF);

            if (btn == BUTTON_FOUR_PRESSES)
            {
                /* Reset the device */
                SENSIT_API_reset();
            }

            /* Clear interrupt */
            pending_interrupt &= ~INTERRUPT_MASK_BUTTON;
        }

        /* Accelerometer interrupt handler */
        if ((pending_interrupt & INTERRUPT_MASK_FXOS8700) == INTERRUPT_MASK_FXOS8700)
        {
            // FXOS8700_read_acceleration (fxos8700_data_s *acc);

            /* Read transient interrupt register */
            FXOS8700_clear_transient_interrupt(&(data.vibration));
            /* Check if a movement has been detected */
            if (data.vibration == TRUE)
            {
                /* Increment event counter */
                data.event_counter++;
            }

            /* Clear interrupt */
            pending_interrupt &= ~INTERRUPT_MASK_FXOS8700;
        }

        /* Check if we need to send a message */
        if (send == TRUE)
        {
            /* Build the payload */
            build_payload(&payload, &data);

            /* Send the message */
            err = RADIO_API_send_message(RGB_BLUE, (u8*)&payload, 4, FALSE, NULL);
            /* Parse the error code */
            ERROR_parser(err);

            if (err == RADIO_ERR_NONE)
            {
                /* Reset event counter */
                data.event_counter = 0;
            }

            /* Clear vibration flag */
            data.vibration = FALSE;

            /* Clear button flag */
            data.button = FALSE;

            /* Clear send flag */
            send = FALSE;
        }

        /* Check if all interrupt have been clear */
        if (pending_interrupt == 0)
        {
            /* Wait for Interrupt */
            SENSIT_API_sleep(FALSE);
        }
    } /* End of while */
}

/*******************************************************************/

void build_payload(payload_s* payload, data_s* data)
{
    payload->reserved = 0b110;
    payload->mode = MODE_VIBRATION;
    payload->button = data->button;

    if ( data->battery <= BATTERY_LVL_MIN )
    {
         payload->battery = (BATTERY_LVL_MIN - BATTERY_LVL_OFFSET)/BATTERY_LVL_STEP;
    }
    else if ( data->battery >= BATTERY_LVL_MAX )
    {
         payload->battery = (BATTERY_LVL_MAX - BATTERY_LVL_OFFSET)/BATTERY_LVL_STEP;
    }
    else
    {
        payload->battery = (data->battery - BATTERY_LVL_OFFSET)/BATTERY_LVL_STEP;
    }

    payload->special_value = data->vibration;
    payload->event_counterMSB = (u8)(data->event_counter >> 8);
    payload->event_counterLSB = (u8)data->event_counter;
    
}

/*******************************************************************/