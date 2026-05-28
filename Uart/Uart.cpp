#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <cstring>
#include <errno.h>
#include "Uart.hpp"

Uart::Uart()
{
}

Uart::Uart(const char *port, int baudrate)
{
    port_ = port;
    baudrate_ = baudrate;
}

void Uart::Send(const char *data)
{
    // data_ = const_cast<char *>(data);
    const char *port = port_;

    int fd = open(port, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (fd < 0)
    {
        std::cerr << "open err: " << strerror(errno) << std::endl;
    }

    termios tty;
    memset(&tty, 0, sizeof(tty));
    tcgetattr(fd, &tty);
    cfsetispeed(&tty, B115200);
    cfsetospeed(&tty, B115200);
    tty.c_cflag = CS8 | CREAD;                   // 8N1 标准配置
    tty.c_lflag = tty.c_iflag = tty.c_oflag = 0; // 原始模式
    tty.c_cc[VMIN] = 0;
    tty.c_cc[VTIME] = 10;
    tcsetattr(fd, TCSANOW, &tty);

    write(fd, data, strlen(data));
    //std::cout << "发送: " << data;
    close(fd);
}
