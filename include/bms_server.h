#ifndef BMS_SERVER_H
#define BMS_SERVER_H


#define TERMINAL    "/dev/ttyUSB2"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>			//Used for UART
#include <fcntl.h>			//Used for UART
#include <termios.h>			//Used for UART


class BmsServer {

public:
    BmsServer();

    ~BmsServer();

    uint32_t get_bat_voltage();
    uint32_t get_balancer_temp();



    enum eBalancerFaults
    {
        e_OK = 0,
        e_DOCKED_FAULT,
        e_CHARGER_EN_FAULT,
        e_SYSFAULT1_TWARN_FAULT,
        e_DIE_TEMP_HIGH_FAULT,
        e_BATTERY_CURRENT_FAULT
    } ;

    enum eFrameType
    {
        e_ACK_FRAME = 0,
        e_DATA_FRAME,
        e_CONTROL_FRAME,
        e_FAULT_FRAME
    };

    enum eAckValue
    {
        e_ACK_VALUE  = 0x10,
        e_NACK_VALUE = 0x01
    };


    struct bms_status_struct {
        uint32_t bms_state;
        uint32_t bat_voltage;
        uint32_t bat_percent;
        uint32_t balancer_temp;
        uint32_t state_of_health;
        uint32_t state_of_charge;
        uint32_t max_cell;
        uint32_t min_cell;
        uint32_t voltage_delta;
        eBalancerFaults fault;

    };


    struct bms_frame_struct{
        uint32_t header;
        eFrameType type;
        uint32_t length;
        uint32_t* data;
    };

    void read_bms_frame(uint32_t *frame);

    private:

    int fd;
    int set_interface_attribs(int fd, int speed);
    void get_data_frame(uint8_t *buf, bms_frame_struct *bms_frame);
    void send_ack(eAckValue val);
    bms_status_struct bms_status;
    const char *portname = "/dev/ttyUSB2";
    const uint32_t header = 0xa5a5;

};

#endif //BMS_SERVER_H

