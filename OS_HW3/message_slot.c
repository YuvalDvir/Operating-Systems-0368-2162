#undef __KERNEL__
#define __KERNEL__
#undef MODULE
#define MODULE

#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/uaccess.h>
#include <linux/string.h>
#include <linux/list.h>
#include <linux/slab.h>
#include "message_slot.h"

MODULE_LICENSE("GPL");


/****************************************************
*                                                   *
*           Operating Systems (0368-2162)           *
*       Message Slot Kernel Module Assignment       *
*                                                   *
*****************************************************/

//================== DEVICE FUNCTIONS ===========================

static struct list_head devices_list = LIST_HEAD_INIT(devices_list);

static struct message_channel

// Message slot channel representation
struct message_channel{
    int id;
    char buffer[BUF_LEN];
    int size;
    struct list_head list;
};

// Message slot device representation
struct message_slot_device{
    unsigned int minor;
    int channel_id;
    int set;
};

// Message slot device getter
// Checks if a device with a specific minor number has already initialized
// Searching it in the static devices linked list
struct message_slot_device* get_device(unsigned int _minor){
    struct list_head *p;
    struct message_slot_device *device;

    printk("-------------------Inside get_device Func-------------------\n");
    printk("before stating loop over all the lists\n");

    list_for_each(p, &devices_list){
        printk("inside a loop, before getting entry\n");
        device = list_entry(p, struct message_slot_device, list);
        printk("inside a loop, after getting entry\n");
        printk("inside a loop, the device minor is: %u\n",device->minor);



        //If current device has the same minor as the one were looking for return it
        if(_minor == device->minor){
            printk("inside a loop, checking entrys minor\n");
            return device;
        }
    }
    printk("Finished looping over the devices_list\n");
    printk("Didnt find the device with this minor: %u\n", _minor);
    return NULL;
}

// Message channel getter
// Searching it in the associated device's channels linked list
struct message_channel* get_channel(struct file* file){
    struct list_head *p;
    struct message_channel *channel;
    struct message_slot_device *device = file->private_data;
    printk("-------------------Inside get_channel Func-------------------\n");


    // Looking for the channel with the appropriate id
    printk("Before looping all over devices\n");
    list_for_each(p, &(device->channels_list)){
        printk("Inside loop before entry\n");
        channel = list_entry(p, struct message_channel, list);
        printk("Inside loop after entry\n");
        printk("current channel id is: %d\n",channel->id);
        if (!channel){
            printk("The channel is null!!!!!\n");
        }
        if (device->channel_id == channel->id){
            printk("matching id!!! returning the channel\n");
            return channel;
        }
    }
    printk("-------------------Going out of get_channel Func-------------------\n");

    return NULL;
}

//Initializing new message slot device
struct message_slot_device* initialize_device(unsigned int minor){
    struct message_slot_device* device = kmalloc(sizeof(struct message_slot_device),GFP_KERNEL);
    printk("Inside initialization\n");
    if (!device){
        return NULL;
    }

    device->minor = minor;
    device->set = 0;
    //device->channels_list = LIST_HEAD_INIT(device->channels_list);
    //INIT_LIST_HEAD(&device->channels_list);
    //LIST_HEAD(&device->channels_list);

    printk("Finish initializing device\n");
    return device;
}

//Initializing new message channel
struct message_channel* initialize_channel(struct file* file){
    struct message_slot_device* device = file->private_data;
    struct message_channel* channel = kmalloc(sizeof(struct message_channel),GFP_KERNEL);

    if (!channel){
        return NULL;
    }
    channel->size = 0;
    channel->id = device->channel_id;

    return channel;
}
//---------------------------------------------------------------
static int device_open( struct inode* inode,
                        struct file*  file )
{
    unsigned int minor;
    struct message_slot_device *device;
    printk("-------------------Inside Open Func-------------------\n");
    // If file or inode is invalid
    if (!file || !inode){
        return -EINVAL;
    }

    minor = iminor(inode);
    printk("The minor number is %u",minor);
    device = get_device(minor);
    printk("Outside of get device\n");


    // If could not find the matching device initialize a new one
    if (!device){
        printk("No device, entering into initialization\n");
        device = initialize_device(minor);
        printk("Outside of initialization\n");
        // If allocation failed
        if (!device){
            printk("Allocation Failed\n");
            return -ENOMEM;
        }
        printk("Before adding device to list\n");
        list_add(&device->list,&devices_list);
        printk("After adding device to list\n");
    }

    // Needs to associate the passed channel id with the file descriptor it was invoked on
    file->private_data = device;
    printk("-------------------Going out of Open Func-------------------\n");
    return SUCCESS;
}
//--------------------------------------------------------------
static int device_release( struct inode* inode,
                           struct file*  file)
{
    struct list_head *p, *n;
    struct message_slot_device *device;
    struct message_channel *my;

    // If file or inode is invalid
    if (!file || !inode){
        return -EINVAL;
    }
    device = (struct message_slot_device*)file->private_data;

    // Free all device's channels from linked list
    list_for_each_safe(p, n, &device->channels_list){
        my = list_entry(p, struct message_channel, list);
        kfree(my);
    }

    kfree(device);
    return SUCCESS;
}
//---------------------------------------------------------------
static ssize_t device_read( struct file* file,
                            char __user* buffer,
size_t       length,
        loff_t*      offset )
{
struct message_slot_device *device = file->private_data;
struct message_channel *channel = device->channel;
int i;
printk("--------------Inside Reading function--------------\n");

// Check if no channel has been set on the file descriptor or file is invalid
printk("Before check 1\n");
if (!device->set || !file){
return -EINVAL;
}
printk("Before check 2\n");
// If no message exists on the channel
if (!channel->size){
return -EWOULDBLOCK;
}
printk("Before check 3\n");
// If the provided buffer length is too small to hold the last message written on the channel
if ( length < channel->size){
return -ENOSPC;
}

/*
    // Check the validity of user space buffer
    if (!access_ok(VERIFY_READ, buffer)){
        return -EFAULT;
    }
*/
printk("Before Reading\n");

// Reads the last message written on the channel into the user’s buffer
for (i = 0; i < BUF_LEN && i < length; ++i){
printk("Reading\n");
put_user(channel->buffer[i], &buffer[i]);
printk("buffer[%d] = %d\n",i,buffer[i]);

}

// Failed to read message into the user’s buffer
printk("--------------Going out of Reading function--------------\n");
return -ENODATA;
}
//--------------------------------------------------------------
static ssize_t device_write( struct file*       file,
                             const char __user* buffer,
size_t             length,
        loff_t*            offset)
{
ssize_t i;
struct message_slot_device *device = file->private_data;
struct message_channel *channel = device->channel;

printk("--------------Inside Writing function--------------\n");
// Check if no channel has been set on the file descriptor or file is invalid
if (!device->set || !file){
printk("Device set flag is 0 or file is null\n");
return -EINVAL;
}

// Check if the passed message length is 0 or more than 128
if (length < 1 || length > 128){
printk("passed message length is 0 or more than 128\n");
return -EMSGSIZE;
}
/*
    // Check the validity of user space buffer
    if (!access_ok(VERIFY_WRITE, buffer)){
        return -EFAULT;
    }
*/
// Writes a non-empty message of up to 128 bytes from the user’s buffer to the channel
for (i = 0; i < BUF_LEN && i < length; ++i){
printk("Writing\n");
get_user(channel->buffer[i], &buffer[i]);
printk("buffer[%ld] = %d\n",i,buffer[i]);

}

printk("--------------Going out of Writing function--------------\n");
// Successful write() always returns the number of bytes in the supplied message
return i;
}
//----------------------------------------------------------------
static long device_ioctl( struct   file* file,
                          unsigned int   ioctl_command_id,
                          unsigned long  ioctl_param )
{
    struct message_slot_device *device;


    printk("-------------------Inside ioctl Func-------------------\n");

    if(MSG_SLOT_CHANNEL == ioctl_command_id){
        printk("MSG_SLOT_CHANNEL == ioctl_command_id\n");
        // Get the channel associated with the ioctl_param (channel_id)
        if (ioctl_param) {
            printk("ioctal_param: %lu is valid\n",ioctl_param);
            device = (struct message_slot_device *) file->private_data;
            device->channel_id = ioctl_param;
            device->set = 1;
            printk("before get channel\n");
            device->channel = get_channel(file);
            printk("after getting channel\n");

            // If could not find the matching channel initialize a new one
            if(!device->channel){
                printk("coudlnt find the right channel\n");
                printk("before initialzing the channel\n");
                device->channel = initialize_channel(file);
                // If allocation failed
                if (!device->channel){
                    return -ENOMEM;
                }
                list_add(&device->channel->list, &device->channels_list);
            }
        }
            // The passed channel id is 0
        else{
            return -EINVAL;
        }
    }
    // The command is not MSG_SLOT_CHANNEL, i.e not valid
    else {
        return -EINVAL;
    }
    printk("-------------------going out of ioctl Func-------------------\n");

    return SUCCESS;
}
//==================== DEVICE SETUP =============================
struct file_operations Fops =
        {
                .owner	  = THIS_MODULE,
                .read           = device_read,
                .write          = device_write,
                .open           = device_open,
                .unlocked_ioctl = device_ioctl,
                .release        = device_release,
        };

//---------------------------------------------------------------
// Initialize the module - Register the message slot device
static int __init simple_init(void)
{
    int rc = -1;

    // Register driver capabilities. Obtain major num
    rc = register_chrdev(MAJOR_NUM, DEVICE_RANGE_NAME, &Fops);

    // Negative values signify an error
    if( rc < 0 ){
        printk( KERN_ALERT "%s Registration failed for  %d\n",
                DEVICE_FILE_NAME, MAJOR_NUM );
        return rc;
    }

    printk( "Registration is successful. ");
    return 0;
}
//---------------------------------------------------------------
static void __exit simple_cleanup(void)
{
    // Unregister the device
    // Should always succeed
    unregister_chrdev(MAJOR_NUM, DEVICE_RANGE_NAME);
}
//---------------------------------------------------------------
module_init(simple_init);
module_exit(simple_cleanup);
//========================= END OF FILE =========================
