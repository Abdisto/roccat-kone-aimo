#include <stdio.h>
#include <wchar.h>
#include <string.h>
#include <stdlib.h>

#include <hidapi.h>

#ifdef _WIN32
    #include <windows.h>
#else
    #include <unistd.h>
    #include <fcntl.h>
    #include <linux/uinput.h>
#endif

#define MAX_STR 255

#ifdef __linux__
int setup_virtual_device() {
    int fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
    if (fd < 0) {
        perror("open /dev/uinput");
        return -1;
    }

    ioctl(fd, UI_SET_EVBIT, EV_KEY);
    ioctl(fd, UI_SET_EVBIT, EV_SYN);

    ioctl(fd, UI_SET_KEYBIT, KEY_F24);
    ioctl(fd, UI_SET_KEYBIT, KEY_F23);
    ioctl(fd, UI_SET_KEYBIT, KEY_F22);
    ioctl(fd, UI_SET_KEYBIT, KEY_F21);
    ioctl(fd, UI_SET_KEYBIT, KEY_F20);
    ioctl(fd, UI_SET_KEYBIT, KEY_F19);

    struct uinput_setup usetup = {0};
    usetup.id.bustype = BUS_USB;
    usetup.id.vendor  = 0x1e7d;
    usetup.id.product = 0x2e27;
    strcpy(usetup.name, "ROCCAT ROCCAT Kone Aimo Virtual Buttons");

    ioctl(fd, UI_DEV_SETUP, &usetup);
    ioctl(fd, UI_DEV_CREATE);

    return fd;
}

void emit(int fd, int type, int code, int val) {
    struct input_event ie = {0};
    ie.type = type;
    ie.code = code;
    ie.value = val;
    if (write(fd, &ie, sizeof(ie)) == -1)
        perror("write");
}
#endif

int main(void)
{
    int res;
    unsigned char buf[256];
    hid_device *handle;
    // Track button states globally to handle independent releases
    int left_is_down = 0;
    int right_is_down = 0;

    printf("hidapi test with uinput mapping\n");

    if (hid_init())
        return -1;

    // Set up the command buffer.
    memset(buf, 0, sizeof(buf));
    buf[0] = 0x01;
    buf[1] = 0x81;

    // Open the device using the VID, PID,
	// and optionally the Serial number.
	////handle = hid_open(0x4d8, 0x3f, L"12345");
    handle = hid_open(0x1e7d, 0x2e27, NULL);
    if (!handle) {
        printf("unable to open device\n");
        hid_exit();
        return 1;
    }

    // Set the hid_read() function to be non-blocking.
    hid_set_nonblocking(handle, 1);

#ifdef __linux__
    int uinput_fd = setup_virtual_device();
#endif
    buf[0] = 0x1;
	buf[1] = 0x81;
	hid_write(handle, buf, 17);
	if (res < 0) {
		printf("Unable to write()/2: %ls\n", hid_error(handle));
	}

	res = 0;
    for (;;) {
        res = hid_read(handle, buf, sizeof(buf));
        if (res <= 0) continue;

        // debug
        for (int i = 0; i < res; i++)
            printf("%02x ", buf[i]);
        printf("\n");

#ifdef __linux__
        if (uinput_fd < 0) continue;

        if (res >= 5 && buf[0] == 0x03 && buf[1] == 0x00) {

            int code = buf[2];
            int state = buf[3];
            int extra = buf[4];

            // --- EasyShift+ button -> Super ---
            if (code == 0xee) {
                if (state == 0x02) { // press
                    emit(uinput_fd, EV_KEY, KEY_F24, 1);
                } else if (state == 0x00) { // release
                    emit(uinput_fd, EV_KEY, KEY_F24, 0);
                }
                emit(uinput_fd, EV_SYN, SYN_REPORT, 0);
            }

            // --- EasyShift+ mouse buttons (EF 02 XX) ---
            else if (code == 0xef) {
                // We expect buf[3] to be 0x02 for these specific button events
                if (state == 0x02) {
                    int current_bits = extra; // This is 00, 01, 02, or 03

                    // --- Handle Left Button (Bit 0x01) ---
                    if ((current_bits & 0x01) && !left_is_down) {
                        emit(uinput_fd, EV_KEY, KEY_F23, 1);
                        left_is_down = 1;
                    }
                    else if (!(current_bits & 0x01) && left_is_down) {
                        emit(uinput_fd, EV_KEY, KEY_F23, 0);
                        left_is_down = 0;
                    }

                    // --- Handle Right Button (Bit 0x02) ---
                    if ((current_bits & 0x02) && !right_is_down) {
                        emit(uinput_fd, EV_KEY, KEY_F22, 1);
                        right_is_down = 1;
                    }
                    else if (!(current_bits & 0x02) && right_is_down) {
                        emit(uinput_fd, EV_KEY, KEY_F22, 0);
                        right_is_down = 0;
                    }

                    // Sync the report once after checking both bits
                    emit(uinput_fd, EV_SYN, SYN_REPORT, 0);
                }
            }

            // --- scroll buttons down, left, right ---
            else if (code == 0xf0) {
                if (state == 0x23) { // press down
                    if (extra == 0x01) {
                        emit(uinput_fd, EV_KEY, KEY_F21, 1);
                    } else if (extra == 0x00) { // release
                        emit(uinput_fd, EV_KEY, KEY_F21, 0);
                    }
                }
                else if (state == 0x21) { // press left
                    if (extra == 0x01) {
                        emit(uinput_fd, EV_KEY, KEY_F20, 1);
                    } else if (extra == 0x00) { // release
                        emit(uinput_fd, EV_KEY, KEY_F20, 0);
                    }
                }
                else if (state == 0x22) { // press right
                    if (extra == 0x01) {
                        emit(uinput_fd, EV_KEY, KEY_F19, 1);
                    } else if (extra == 0x00) { // release
                        emit(uinput_fd, EV_KEY, KEY_F19, 0);
                    }
                }
                emit(uinput_fd, EV_SYN, SYN_REPORT, 0);
            }
        }
#endif
    }

    hid_close(handle);
    hid_exit();
    return 0;
}
