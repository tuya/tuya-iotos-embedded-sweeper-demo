#include <sys/types.h>

// #include "wifi.h"
#include "define.h"
// #include "serialcom.h"

#include <unistd.h>
#include <sys/types.h> 
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>

#if defined(USE_GYRO1)
        const unsigned char gyro_num = 1;
#elif defined(USE_GYRO2)
        const unsigned char gyro_num = 2;
#elif defined(USE_GYRO3)
        const unsigned char gyro_num = 3;
# else
        
#endif


int g_fdNo;


#define MS 1000
///////////////////////////////////////////////

void open_exec_pro(int cmd);
void close_exec_pro(int cmd);

static void kill_cmd(char *pCmd)
{
    FILE *pp;
    pid_t pid;
    char tmp[64];
    snprintf(tmp,64,"ps -ef | grep %s | grep -v \"grep\" | awk '{print $2}'", pCmd);
    pp = popen(tmp, "r");
    if (!pp){
        return;
    }
    while (fgets(tmp, sizeof(tmp), pp) != NULL){
		sscanf(tmp, "%d", &pid);
		if (pid > 0){
                        char cmd_str[20];
                        snprintf(cmd_str,20,"kill -9 %d", pid);
			system(cmd_str);
		}
    }

    pclose(pp);
}

/*****************************************************************
 * @Function: gyro_dev_switch
 * @Description: 设备进程切换 1 -> 2 -> 3 -> 1
 * @Param: Do not edit
 * @Return: void
 *****************************************************************/
void gyro_dev_switch(void)
{
        if(gyro_num == 1){
                open_exec_pro(2);
                sleep(1);
                close_exec_pro(1);
        }else if(gyro_num == 2){
                open_exec_pro(3);
                sleep(1);
                close_exec_pro(2);
        }
        else if(gyro_num == 3){
                open_exec_pro(1);
                sleep(1);
                close_exec_pro(3);
        }
}

void open_exec_pro(int cmd)
{
    switch(cmd)
    {
        case 1: system("./build/gyro1 &"); break;
        case 2: system("./build/gyro2 &"); break;
        case 3: system("./build/gyro3 &"); break;
        case 4: system("gnome-terminal -e ./pro4"); break;
        case 5: system("gnome-terminal -e ./pro5"); break;
        case 6: system("gnome-terminal -e ./pro6"); break;
        default: break;
    }
}

void close_exec_pro(int cmd)
{
    switch(cmd)
    {
        case 1: kill_cmd("gyro1"); break;
        case 2: kill_cmd("gyro2"); break;
        case 3: kill_cmd("gyro3"); break;
        case 4: system("/bin/kill -9 pro4"); break;
        case 5: system("/bin/kill -9 pro5"); break;
        case 6: system("/bin/kill -9 pro6"); break;
        default: break;
    }
}

///////////////////////////////////////////////
// int main(int argc, char *argv[])
// {
//     g_fdNo = UART0_Open(g_fdNo,"/dev/ttyUSB0"); //打开串口，返回文件描述符

//     if(UART0_Init(g_fdNo, 115200, 0, 8, 1, 'N') == -1)
//     {
//         FS_INFO("usb0 failed\n");
        
//         return 0;
//     }
//     else
//     {
//        FS_INFO("usb0 opened\n");
//     }

//     wifi_protocol_init();

//     unsigned char rvChar;

//     while (1)
//     {
//         wifi_uart_service();

//         if (uart_read_char(&rvChar) != -1)
//         {
//             uart_receive_input(rvChar);
//         }

//         usleep(1000);
//     }

//     return 1;
// }