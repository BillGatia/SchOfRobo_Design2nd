#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <cstring>
#include <errno.h>

class Uart
{
private:
    //char char *data_;
    const char *port_ = "/dev/ttyAMA0";
    int baudrate_ = 115200;

public:
    Uart();
    Uart(const char *port, int baudrate);
    void Send(const char *data);
};