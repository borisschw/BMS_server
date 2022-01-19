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
        e_BALANCER_FRAME,
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

        uint32_t dev_name;
        uint32_t vcell_voltage;
        uint32_t rep_cap;
        uint32_t rep_soc;
        uint32_t fule_gauge_temp;
        uint32_t average_current;
        uint32_t bms_state;
        uint32_t bat_voltage;
        uint32_t bat_percent;
        uint32_t balancer_temp;
        uint32_t max_cell;
        uint32_t min_cell;
        uint32_t voltage_delta;
    };

    struct balancer_cells_struct{
        uint32_t state;
        uint32_t cb_done;
        uint32_t cells[6];
        uint32_t max_cell;
        uint32_t min_cell;
        uint32_t total_voltage;
        uint32_t voltage_delta;
    } ;

    enum eBalancerCommands
    {
        e_STOP_BALANCING = 0x10,
        e_START_BALANCING = 0x11,
        e_STOP_CHARGING = 0x12,
        e_START_CHARGING = 0x13,
        e_TURN_OFF_TEGRA = 0x14,
        e_TURN_ON_TEGRA = 0x15
    };

    struct bms_data_frame_struct{
        uint32_t header;
        eFrameType type;
        uint32_t length;
        bms_status_struct data;
    };


    struct balancer_frame_struct{
	uint32_t header;
	eFrameType type;
	uint32_t length;
	balancer_cells_struct data;
    } ;


    struct bms_general_frame_struct{
	uint32_t header;
	eFrameType type;
	uint32_t length;
	uint32_t data;
    };

    void get_bms_frame(uint32_t *frame);
    void send_bms_cmd_frame(eBalancerCommands command);

    private:

    int fd;
    int set_interface_attribs(int fd, int speed);
    int get_data_frame();
    int get_balancer_frame();
    void send_ack(eAckValue val);
    void send_control_frame(eAckValue val);
    void print_byte(uint8_t byte);

    bms_status_struct bms_status;
    balancer_cells_struct balancer_cell_voltages;
    const char *portname = "/dev/ttyUSB2";
    const uint32_t header = 0xa5a5;
    const uint32_t max_frame_len = 50;

    const char *bit_rep[16] = {
    [ 0] = "0000", [ 1] = "0001", [ 2] = "0010", [ 3] = "0011",
    [ 4] = "0100", [ 5] = "0101", [ 6] = "0110", [ 7] = "0111",
    [ 8] = "1000", [ 9] = "1001", [10] = "1010", [11] = "1011",
    [12] = "1100", [13] = "1101", [14] = "1110", [15] = "1111",
    };

};

#endif //BMS_SERVER_H

