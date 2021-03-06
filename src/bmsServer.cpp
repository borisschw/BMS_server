#include <iostream>
#include <assert.h>
#include <string.h>
#include "bmsServer.h"


using namespace std;


uint32_t BmsServer:: get_balancer_temp()
{
    return bms_status.balancer_temp;
}

void BmsServer:: get_bat_cells_volts(uint32_t *cells)
{
    memcpy((void *)cells, (void *) balancer_cell_voltages.cells, sizeof(uint32_t) * NUMBER_OF_BAT_CELLS);
}

uint32_t BmsServer:: get_bat_total_voltage_mv()
{
    return bms_status.bat_voltage;
}

uint32_t BmsServer:: get_cells_delta_mv()
{
    return bms_status.voltage_delta;
}

uint32_t BmsServer:: get_battery_percent()
{
    return bms_status.bat_percent;
}



void BmsServer:: send_ack(eAckValue val)
{
    uint32_t buf_to_send[4];
    uint32_t ack_value;
    int wlen = 0;

    if (val == e_ACK_VALUE)
        ack_value = e_ACK_VALUE;
    else
        ack_value = e_NACK_VALUE;

    buf_to_send[0] = header;
    buf_to_send[1] = e_ACK_FRAME;
    buf_to_send[2] = sizeof(uint32_t);
    buf_to_send[3] = ack_value;

    #ifdef VERBOSE
        cout <<"\n"<< "Send ACK To BMS" << "\n";
    #endif

    #ifdef VERBOSE
    for (unsigned int i = 0 ; i < 4 ; i++)
            printf("%x ", buf_to_send[i]);
    #endif

    tcflush(fd, TCIOFLUSH);
    wlen = write(fd, buf_to_send, sizeof(buf_to_send));
    if (wlen != sizeof(buf_to_send)) {
        printf("Error from write: %d, %d\n", wlen, errno);
    }
    tcdrain(fd);    /* delay for output */
    //tcflush(fd, TCIOFLUSH);

    /*
    // memcpy((void *) buf_to_send, &bms_frame, \
    // sizeof(bms_frame_struct) - sizeof(uint32_t));

    // memcpy((void *) buf_to_send +, &bms_frame, \
    // sizeof(bms_frame_struct) - sizeof(uint32_t));
*/

}


int BmsServer:: get_data_frame()
{
    #ifdef VERBOSE
    printf("Fuel Gauge\n");
    printf("dev_name        = 0x%x\n",bms_status.dev_name);
	printf("vcell_voltage   = %d\n",bms_status.vcell_voltage);
	printf("rep_cap         = %d\n",bms_status.rep_cap);
	printf("rep_soc         = %d\n",bms_status.rep_soc);
	printf("fule_gauge_temp = %d\n",bms_status.fule_gauge_temp);
	printf("average_current = %d\n",bms_status.average_current);
    printf("\n");
    printf("Balancer\n");
    printf("bms_state       =  %d \n",bms_status.bms_state);
    printf("bat_voltage     =  %d \n",bms_status.bat_voltage);
    printf("bat_percent     =  %d \n",bms_status.bat_percent);
    printf("balancer_temp   =  %d \n",bms_status.balancer_temp);
    printf("max_cell        =  %d \n",bms_status.max_cell);
    printf("min_cell        =  %d \n",bms_status.min_cell);
    printf("voltage_delta   =  %d \n",bms_status.voltage_delta);
    printf("\n");
    #endif
    /*Check Frame fields: TBD */
    /*If there is something wrong return -1*/

    return 0;
}

void BmsServer:: print_byte(uint8_t byte)
{
    printf("%s%s", bit_rep[byte >> 4], bit_rep[byte & 0x0F]);
    printf("\n");
}


int BmsServer:: get_balancer_frame()
{
    #ifdef VERBOSE
        printf("Balancer cells:\n");
        printf("state = %d\n", balancer_cell_voltages.state);

        print_byte(balancer_cell_voltages.cb_done );

        printf("cb done = %d\n", balancer_cell_voltages.cb_done);
        for (int i =0; i<6 ;i++)
        {
            printf("cell[%d] = %d\n",i, balancer_cell_voltages.cells[i]);
        }
        printf("max_cell        =  %d \n",balancer_cell_voltages.max_cell);
        printf("min_cell        =  %d \n",balancer_cell_voltages.min_cell);
        printf("voltage_delta   =  %d \n",balancer_cell_voltages.voltage_delta);
        printf("Battery Voltage =  %d \n",balancer_cell_voltages.total_voltage);

        printf("\n");
    #endif
    return 0;
}

void BmsServer:: send_bms_cmd_frame(eBalancerCommands command)
{
    uint32_t buf_to_send[4];
    int wlen = 0;
    buf_to_send[0] = header;
    buf_to_send[1] = e_CONTROL_FRAME;
    buf_to_send[2] = sizeof(uint32_t);
    buf_to_send[3] = command;

    cout <<"\n"<< "Send Command To BMS" << "\n";

    for (unsigned int i = 0 ; i < 4 ; i++)
            printf("%x ", buf_to_send[i]);

    tcflush(fd, TCIOFLUSH);
    wlen = write(fd, buf_to_send, sizeof(buf_to_send));
    if (wlen != sizeof(buf_to_send)) {
        printf("Error from write: %d, %d\n", wlen, errno);
    }
    tcdrain(fd);    /* delay for output */
}


void* BmsServer:: read_bms_data()
{

    uint8_t buf[150];
    bms_general_frame_struct bms_gen_frame;
    bms_data_frame_struct bms_data_frame;
    int rdlen;
    uint32_t bms_data_byte;
    //uint32_t data_frame_length;
    uint32_t frame_metadata_size = sizeof(uint32_t) * 3;

    //tcflush(fd, TCIOFLUSH);

    while(1)
    {
        rdlen = read(fd, buf, sizeof(bms_data_frame_struct));
        // cout<<"read frame length = "<< rdlen << "\n";

        // cout<<"\n"<< "read income frame" << "\n";
        // for (int i = 0 ; i<rdlen ; i++) // debug
        // {
        //     printf("%x ", buf[i]);
        //     if ((i+1)%4 == 0)
        //         printf("\n");
        // }
        // printf("\n\n");

        if (rdlen > 0)
        {
            memcpy(&bms_data_frame,(void *)buf, frame_metadata_size);
            memcpy(&bms_gen_frame,(void *)buf, frame_metadata_size);

            /*Check the header and the length of the frame*/
            if((bms_data_frame.header == header) && (bms_data_frame.length <=(sizeof(uint32_t) * max_frame_len)))
            {
                //bms_frame.data = new uint32_t(bms_frame.length/sizeof(uint32_t));
                //memcpy((void *)&bms_status, (buf + frame_metadata_size), bms_frame.length);
                //bms_frame.data = (uint32_t*)&bms_status;

                /*Read frame and send response*/
                switch (bms_data_frame.type)
                {
                    case e_ACK_FRAME:
                    #ifdef VERBOSE
                        cout<<"------>>>>>>>> Got ACK frame"<<"\n";
                    #endif
                        memcpy((void *)&bms_gen_frame, (buf + frame_metadata_size), sizeof(uint32_t));

                    break;
                    case e_BALANCER_FRAME:
                        #ifdef VERBOSE
                            cout<<"------>>>>>>>> Got balancer frame"<<"\n";
                        #endif
                        memcpy((void *)&balancer_cell_voltages, (buf + frame_metadata_size), sizeof(balancer_cell_voltages));

                        if( get_balancer_frame() == 0 )
                            send_ack(e_ACK_VALUE);
                        else
                            send_ack(e_NACK_VALUE);

                    break;

                    case e_DATA_FRAME:
                        #ifdef VERBOSE
                            cout<<"------>>>>>>>> Got data frame"<<"\n";
                        #endif
                        memcpy((void *)&bms_status, (buf + frame_metadata_size), sizeof(bms_status_struct));

                        if( get_data_frame() == 0 )
                            send_ack(e_ACK_VALUE);
                    else
                        send_ack(e_NACK_VALUE);

                        //send_bms_cmd_frame(BmsServer:: e_STOP_BALANCING);
                    break;
                    case e_CONTROL_FRAME:
                        #ifdef VERBOSE
                            cout<<"------>>>>>>>> Got CONTROL frame"<<"\n";
                        #endif
                        memcpy((void *)&bms_data_byte, (buf + frame_metadata_size), sizeof(uint32_t));
                        /*Perform a command, if succssesful, send ACK*/
                        send_ack(e_ACK_VALUE);
                    break;
                    case e_FAULT_FRAME:
                        #ifdef VERBOSE
                            cout<<"------>>>>>>>> Got Fault frame"<<"\n";
                        #endif
                        memcpy((void *)&bms_data_byte, (buf + frame_metadata_size), sizeof(uint32_t));
                        cout << "Error number " << bms_data_byte << "\n";
                        send_ack(e_ACK_VALUE);

                    break;
                    default:
                    break;

                }
                // if(bms_frame.data)
                //     //free(bms_frame.data);
                //     delete(bms_frame.data);
            }

        }
        else if (rdlen < 0)
        {
            sleep(1);
            cout<< "Error from read:" << rdlen << ":" << strerror(errno)<< "\n";
        }
        else
        {  /* rdlen == 0 */
            cout<< "Timeout from read" << "\n";
        }
    }
}

int BmsServer:: set_interface_attribs(int fd, int speed)
{
    struct termios tty;

    if (tcgetattr(fd, &tty) < 0) {
        cout<<"Error from tcsetattr:" << strerror(errno) << "\n";
        return -1;
    }

    cfsetospeed(&tty, (speed_t)speed);
    cfsetispeed(&tty, (speed_t)speed);

    tty.c_cflag |= (CLOCAL | CREAD);    /* ignore modem controls */
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;         /* 8-bit characters */
    tty.c_cflag &= ~PARENB;     /* no parity bit */
    tty.c_cflag &= ~CSTOPB;     /* only need 1 stop bit */
    tty.c_cflag &= ~CRTSCTS;    /* no hardware flowcontrol */

    /* setup for non-canonical mode */
    tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
    tty.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
    tty.c_oflag &= ~OPOST;

    /* fetch bytes as they become available */

    //TBD
    tty.c_cc[VMIN] = 1;
    tty.c_cc[VTIME] = 1;

    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        cout<<"Error from tcsetattr:" << strerror(errno) << "\n";
        return -1;
    }
    return 0;
}

BmsServer::BmsServer()
{
    /*Open and define serial interface*/
    fd = open(portname, O_RDWR | O_NOCTTY | O_SYNC);
    if (fd < 0) {
        cout<<"Error opening: serial port" << "\n";
        return;
    }

    set_interface_attribs(fd, B115200);
}

BmsServer::~BmsServer()
{
    cout<<"~BmsServer() \n";
    close(fd);
}


// int main(int argc, char** argv)
// {

//     BmsServer *bms = new BmsServer;


//     uint32_t cells[6] = {0};

//     while(1)
//     {

//         for(int i =0 ; i< 6; i++)
//             cells[i] = 0;

//         bms->read_bms_data();

//         cout << "Balancer Temp " << bms->get_balancer_temp()<< "\n\n";
//         cout << "Battery Cell Voltages"<<"\n\n";
//         bms->get_bat_cells_volts(cells);
//         for(int i=0 ;i<6; i++)
//             cout<<cells[i]<< " ";
//         cout<<"\n";

//         cout<<"Total Battery Voltage "<<bms->get_bat_total_voltage_mv()<<"\n\n";
//         cout<<"Cells delta "<<bms->get_cells_delta_mv()<<"\n\n";
//         cout<<"Battery Percentage "<<bms->get_battery_percent()<<"\n\n\n\n";
//     }

//     delete bms;
//     return 0;
// }

