#ifndef MESSAGE_SLOT_H
#define MESSAGE_SLOT_H

#include <linux/ioctl.h>

#define MSG_SLOT_CHANNEL _IOW(MAJOR_NUM, 0, unsigned long)
#define MAJOR_NUM 240
#define DEVICE_RANGE_NAME "char_dev"
#define DEVICE_FILE_NAME "message_slot_channel"
#define BUF_LEN 128
#define SUCCESS 0
#define ERROR -1

#endif
