/**
 * Inspired by: http://elinux.org/RPi_Low-level_peripherals#GPIO_hardware_hacking
 */

#include <iostream>

#include <fcntl.h>

#include <stdio.h>

#define LOW 0
#define HIGH 1

#define IN 0
#define OUT 1

#define PIN_MODE_BUFFER_MAX 3
#define PIN_DIRECTION_BUFFER_MAX 35
#define PIN_READ_WRITE_BUFFER_MAX 30

bool setPinDirection(int _pinId, int _direction)
{
    bool success = false;

    int fd;
    char path[PIN_DIRECTION_BUFFER_MAX];

    snprintf(path, PIN_DIRECTION_BUFFER_MAX, "/sys/class/gpio/gpio%d/direction", _pinId);
    fd = open(path, O_WRONLY);
    if (fd < 0)
    {
		std::cerr << "Error: Failed to open gpio " << _pinId << " direction for writing" << std::endl;
    }
    else
    {
        int size = _direction == IN ? 2 : 3;
        //char buffer[size];
        //buffer = _direction == IN ? "in" : "out";
        char buffer[size];
        snprintf(buffer, size, _direction == IN ? "in" : "out");

        write(fd, buffer, size);

        close(fd);

        success = true;
    }

    return success;
}

bool enablePin(int _pinId, int _direction)
{
	bool success = false;

	// open the export file descriptor
	int fd;
	fd = open("/sys/class/gpio/export", O_WRONLY);
	if (fd < 0)
	{
		std::cerr << "Error: Failed to open export for writing" << std::endl;
	}
	else
	{
		char buffer[PIN_MODE_BUFFER_MAX];
		ssize_t nbBytesWritten;

		// enable the pinId by happending it to the export file
		nbBytesWritten = snprintf(buffer, PIN_MODE_BUFFER_MAX, "%d", _pinId);
		write(fd, buffer, nbBytesWritten);

		// close the file descriptor
		close(fd);

        success = setPinDirection(_pinId, _direction);
	}

	return success;
}

bool writeOnPin(int _pinId, int _value)
{
    bool success = false;

    int fd;
    char path[PIN_READ_WRITE_BUFFER_MAX];

    snprintf(path, PIN_READ_WRITE_BUFFER_MAX, "/sys/class/gpio/gpio%d/value", _pinId);
    fd = open(path, O_WRONLY);

    if (fd < 0)
    {
		std::cerr << "Error: Failed to open gpio " << _pinId << " value for writing" << std::endl;
    }
    else
    {
        char buffer[1];
        buffer[0] = _value == HIGH ? '1' : '0';

        write(fd, buffer, 1);

        success = true;
    }

    return success;
}

bool setup()
{
    bool success = false;

    if (enablePin(7, OUT))
    {
        success = true;
    }

    return success;
}

bool loop()
{
    writeOnPin(7, HIGH);
    usleep(1000 * 1000);
    writeOnPin(7, LOW);
    usleep(1000 * 1000);

    return true;
}

int main(int argc, char* argv[])
{
	std::cout << "Blink" << std::endl;

    if (setup())
    {
        /*while(loop())
        {
            usleep(100);
        }*/
        writeOnPin(7, LOW);
    }

	return 0;
}

