/*******************************************************************************
** File: generic_star_tracker_device.c
**
** Purpose:
**   This file contains the source code for the GENERIC_STAR_TRACKER device.
**
*******************************************************************************/

/*
** Include Files
*/
#include "generic_star_tracker_device.h"

/*
** Generic read data from device
*/
int32_t GENERIC_STAR_TRACKER_ReadData(uart_info_t *device, uint8_t *read_data, uint8_t data_length)
{
    int32_t status             = OS_SUCCESS;
    int32_t bytes              = 0;
    int32_t bytes_available    = 0;
    uint8_t ms_timeout_counter = 0;

    /* Wait until all data received or timeout occurs */
    bytes_available = uart_bytes_available(device);
    while ((bytes_available < data_length) && (ms_timeout_counter < GENERIC_STAR_TRACKER_CFG_MS_TIMEOUT))
    {
        ms_timeout_counter++;
        OS_TaskDelay(1);
        bytes_available = uart_bytes_available(device);
    }

    if (ms_timeout_counter < GENERIC_STAR_TRACKER_CFG_MS_TIMEOUT)
    {
        /* Limit bytes available */
        if (bytes_available > data_length)
        {
            bytes_available = data_length;
        }

        /* Read data */
        bytes = uart_read_port(device, read_data, bytes_available);
        if (bytes != bytes_available)
        {
#ifdef GENERIC_STAR_TRACKER_CFG_DEBUG
            OS_printf("  GENERIC_STAR_TRACKER_ReadData: Bytes read != to requested! \n");
#endif
            status = OS_ERROR;
        } /* uart_read */
    }
    else
    {
        status = OS_ERROR;
    } /* ms_timeout_counter */

    return status;
}

/*
** Generic command to device
** Note that confirming the echoed response is specific to this implementation
*/
int32_t GENERIC_STAR_TRACKER_CommandDevice(uart_info_t *device, uint8_t cmd_code, uint32_t payload)
{
    int32_t status = OS_SUCCESS;
    int32_t bytes  = 0;
    uint8_t write_data[GENERIC_STAR_TRACKER_DEVICE_CMD_SIZE];
    uint8_t read_data[GENERIC_STAR_TRACKER_DEVICE_DATA_SIZE];

    /* Prepare command */
    write_data[0] = GENERIC_STAR_TRACKER_DEVICE_HDR_0;
    write_data[1] = GENERIC_STAR_TRACKER_DEVICE_HDR_1;
    write_data[2] = cmd_code;
    write_data[3] = payload >> 24;
    write_data[4] = payload >> 16;
    write_data[5] = payload >> 8;
    write_data[6] = payload;
    write_data[7] = GENERIC_STAR_TRACKER_DEVICE_TRAILER_0;
    write_data[8] = GENERIC_STAR_TRACKER_DEVICE_TRAILER_1;

    /* Flush any prior data */
    status = uart_flush(device);
    if (status == UART_SUCCESS)
    {
        /* Write data */
        bytes = uart_write_port(device, write_data, GENERIC_STAR_TRACKER_DEVICE_CMD_SIZE);
#ifdef GENERIC_STAR_TRACKER_CFG_DEBUG
        OS_printf("  GENERIC_STAR_TRACKER_CommandDevice[%d] = ", bytes);
        for (uint32_t i = 0; i < GENERIC_STAR_TRACKER_DEVICE_CMD_SIZE; i++)
        {
            OS_printf("%02x", write_data[i]);
        }
        OS_printf("\n");
#endif
        if (bytes == GENERIC_STAR_TRACKER_DEVICE_CMD_SIZE)
        {
            status = GENERIC_STAR_TRACKER_ReadData(device, read_data, GENERIC_STAR_TRACKER_DEVICE_CMD_SIZE);
            if (status == OS_SUCCESS)
            {
                /* Confirm echoed response */
                bytes = 0;
                while ((bytes < (int32_t)GENERIC_STAR_TRACKER_DEVICE_CMD_SIZE) && (status == OS_SUCCESS))
                {
                    if (read_data[bytes] != write_data[bytes])
                    {
                        status = OS_ERROR;
                    }
                    bytes++;
                }
            } /* GENERIC_STAR_TRACKER_ReadData */
            else
            {
#ifdef GENERIC_STAR_TRACKER_CFG_DEBUG
                OS_printf("GENERIC_STAR_TRACKER_CommandDevice - GENERIC_STAR_TRACKER_ReadData returned %d \n", status);
#endif
            }
        }
        else
        {
#ifdef GENERIC_STAR_TRACKER_CFG_DEBUG
            OS_printf("GENERIC_STAR_TRACKER_CommandDevice - uart_write_port returned %d, expected %d \n", bytes,
                      GENERIC_STAR_TRACKER_DEVICE_CMD_SIZE);
#endif
        } /* uart_write */
    }     /* uart_flush*/
    return status;
}

/*
** Request housekeeping command
*/
int32_t GENERIC_STAR_TRACKER_RequestHK(uart_info_t *device, GENERIC_STAR_TRACKER_Device_HK_tlm_t *data)
{
    int32_t status = OS_SUCCESS;
    uint8_t read_data[GENERIC_STAR_TRACKER_DEVICE_HK_SIZE];

    /* Command device to send HK */
    status = GENERIC_STAR_TRACKER_CommandDevice(device, GENERIC_STAR_TRACKER_DEVICE_REQ_HK_CMD, 0);
    if (status == OS_SUCCESS)
    {
        /* Read HK data */
        status = GENERIC_STAR_TRACKER_ReadData(device, read_data, sizeof(read_data));
        if (status == OS_SUCCESS)
        {
#ifdef GENERIC_STAR_TRACKER_CFG_DEBUG
            OS_printf("  GENERIC_STAR_TRACKER_RequestHK = ");
            for (uint32_t i = 0; i < sizeof(read_data); i++)
            {
                OS_printf("%02x", read_data[i]);
            }
            OS_printf("\n");
#endif

            /* Verify data header and trailer */
            if ((read_data[0] == GENERIC_STAR_TRACKER_DEVICE_HDR_0) &&
                (read_data[1] == GENERIC_STAR_TRACKER_DEVICE_HDR_1) &&
                (read_data[6] == GENERIC_STAR_TRACKER_DEVICE_TRAILER_0) &&
                (read_data[7] == GENERIC_STAR_TRACKER_DEVICE_TRAILER_1))
            {
                data->DeviceCounter = read_data[2] << 24;
                data->DeviceCounter |= read_data[3] << 16;
                data->DeviceCounter |= read_data[4] << 8;
                data->DeviceCounter |= read_data[5];

#ifdef GENERIC_STAR_TRACKER_CFG_DEBUG
                OS_printf("  Header  = 0x%02x%02x  \n", read_data[0], read_data[1]);
                OS_printf("  Trailer = 0x%02x%02x  \n", read_data[6], read_data[7]);
#endif
            }
            else
            {
#ifdef GENERIC_STAR_TRACKER_CFG_DEBUG
                OS_printf("  GENERIC_STAR_TRACKER_RequestHK: GENERIC_STAR_TRACKER_ReadData reported error %d \n",
                          status);
#endif
                status = OS_ERROR;
            }
        } /* GENERIC_STAR_TRACKER_ReadData */
    }
    else
    {
#ifdef GENERIC_STAR_TRACKER_CFG_DEBUG
        OS_printf("  GENERIC_STAR_TRACKER_RequestHK: GENERIC_STAR_TRACKER_CommandDevice reported error %d \n", status);
#endif
    }
    return status;
}

/*
** Request data command
*/
int32_t GENERIC_STAR_TRACKER_RequestData(uart_info_t *device, GENERIC_STAR_TRACKER_Device_Data_tlm_t *data)
{
    int32_t status = OS_SUCCESS;
    uint8_t read_data[GENERIC_STAR_TRACKER_DEVICE_DATA_SIZE];

    /* Command device to send HK */
    status = GENERIC_STAR_TRACKER_CommandDevice(device, GENERIC_STAR_TRACKER_DEVICE_REQ_DATA_CMD, 0);
    if (status == OS_SUCCESS)
    {
        /* Read HK data */
        status = GENERIC_STAR_TRACKER_ReadData(device, read_data, sizeof(read_data));
        if (status == OS_SUCCESS)
        {
#ifdef GENERIC_STAR_TRACKER_CFG_DEBUG
            OS_printf("  GENERIC_STAR_TRACKER_RequestData = ");
            for (uint32_t i = 0; i < sizeof(read_data); i++)
            {
                OS_printf("%02x", read_data[i]);
            }
            OS_printf("\n");
#endif

            /* Verify data header and trailer */
            if ((read_data[0] == GENERIC_STAR_TRACKER_DEVICE_HDR_0) &&
                (read_data[1] == GENERIC_STAR_TRACKER_DEVICE_HDR_1) &&
                (read_data[11] == GENERIC_STAR_TRACKER_DEVICE_TRAILER_0) &&
                (read_data[12] == GENERIC_STAR_TRACKER_DEVICE_TRAILER_1))
            {
                uint16_t q0, q1, q2, q3;
                q0 = read_data[2] << 8;
                q0 |= read_data[3];
                data->Q0 = (((double)q0) - 32768.0) / 32767.0;

                q1 = read_data[4] << 8;
                q1 |= read_data[5];
                data->Q1 = (((double)q1) - 32768.0) / 32767.0;

                q2 = read_data[6] << 8;
                q2 |= read_data[7];
                data->Q2 = (((double)q2) - 32768.0) / 32767.0;

                q3 = read_data[8] << 8;
                q3 |= read_data[9];
                data->Q3 = (((double)q3) - 32768.0) / 32767.0;

                data->IsValid = read_data[10];

#ifdef GENERIC_STAR_TRACKER_CFG_DEBUG
                OS_printf("  Header  = 0x%02x%02x \n", read_data[0], read_data[1]);
                OS_printf("  Data Q0 = 0x%d, %f   \n", q0, data->Q0);
                OS_printf("  Data Q1 = 0x%d, %f   \n", q1, data->Q1);
                OS_printf("  Data Q2 = 0x%d, %f   \n", q2, data->Q2);
                OS_printf("  Data Q3 = 0x%d, %f   \n", q3, data->Q3);
                OS_printf("  Is valid= %d         \n", data->IsValid);
                OS_printf("  Trailer = 0x%02x%02x \n", read_data[12], read_data[13]);
#endif
            }
        }
        else
        {
#ifdef GENERIC_STAR_TRACKER_CFG_DEBUG
            OS_printf("  GENERIC_STAR_TRACKER_RequestData: Invalid data read! \n");
#endif
            status = OS_ERROR;
        } /* GENERIC_STAR_TRACKER_ReadData */
    }
    else
    {
#ifdef GENERIC_STAR_TRACKER_CFG_DEBUG
        OS_printf("  GENERIC_STAR_TRACKER_RequestData: GENERIC_STAR_TRACKER_CommandDevice reported error %d \n",
                  status);
#endif
    }
    return status;
}
