#include "shell.h"
#include "console.h"
#include "string.h"
#include "reboot.h"
#include "calc.h"
#include "keyboard.h" 
#include "input.h"
#include "fs.h"
#include "ata.h"
//#include <string.h>

  // <--- IMPORTANT

#define SHELL_BUFFER_SIZE 128

static char input_buffer[SHELL_BUFFER_SIZE];
static int input_len = 0;

// forward declaration for calculator input collector
extern void keyboard_collect(char c);

static int chatbot_reply(const char* msg) {

    if (strcmp(msg, "hi") == 0 ||
        strcmp(msg, "hello") == 0 ||
        strcmp(msg, "hey") == 0) {

        kprint("Hello! How can I help you?\n");
        return 1;
    }

    if (strcmp(msg, "who are you") == 0) {
        kprint("I am AtariOS assistant, a tiny built-in chatbot.\n");
        return 1;
    }

    if (strcmp(msg, "help me") == 0) {
        kprint("Sure! You can ask basic questions.\n");
        return 1;
    }

    // Add more later!
    return 0;   // not recognized by chatbot
}


static void shell_execute(char *cmd) {
    if (cmd[0] == 0) return;

    // First, try chatbot, if you added it:
    if (chatbot_reply(cmd)) {
        return;
    }

    // Split into command and rest: cmd_name, arg1/rest
    char *p = cmd;
    // skip leading spaces
    while (*p == ' ') p++;

    char *name = p;

    // find first space
    while (*p && *p != ' ') p++;
    char *args = 0;
    if (*p) {
        *p = 0;
        p++;
        while (*p == ' ') p++;
        args = p;   // rest of line after command
    }


    if (!strcmp(name, "help")) {
        kprint("Commands:\n");
        kprint("  help         - show this message\n");
        kprint("  cls          - clear screen\n");
        kprint("  reboot       - reboot system\n");
        kprint("  calc         - open calculator\n");
        kprint("  ls           - list files\n");
        kprint("  cat <name>   - show file content\n");
        kprint("  write <name> <text> - create/overwrite file\n");
        kprint("  rm <name>    - delete file\n");
        return;
    }

    if (!strcmp(name, "cls")) {
        terminal_clear();
        return;
    }

    if (!strcmp(name, "reboot")) {
        kprint("Rebooting...\n");
        reboot();
        return;
    }

    if (!strcmp(name, "calc")) {
        calc_shell();
        return;
    }

    // ----- FS commands -----

    if (!strcmp(name, "ls")) {
        fs_list();
        return;
    }

    if (!strcmp(name, "cat")) {
        if (!args || !args[0]) {
            kprint("Usage: cat <name>\n");
            return;
        }
        fs_file_t* f = fs_get(args);
        if (!f) {
            kprint("File not found.\n");
            return;
        }
        if (f->size == 0) {
            kprint("(empty file)\n");
            return;
        }
        kprint(f->data);
        kprint("\n");
        return;
    }

    if (!strcmp(name, "rm")) {
        if (!args || !args[0]) {
            kprint("Usage: rm <name>\n");
            return;
        }
        if (fs_delete(args)) {
            kprint("Deleted.\n");
        } else {
            kprint("No such file.\n");
        }
        return;
    }

    if (!strcmp(name, "write")) {
        if (!args || !args[0]) {
            kprint("Usage: write <name> <text>\n");
            return;
        }

        // Split args into filename and content
        char *a = args;
        while (*a && *a != ' ') a++;
        char *content = 0;
        if (*a) {
            *a = 0;
            a++;
            while (*a == ' ') a++;
            content = a;
        }

        if (!content || !content[0]) {
            kprint("Usage: write <name> <text>\n");
            return;
        }

        if (fs_write(args, content)) {
            kprint("Written.\n");
        } else {
            kprint("Write failed.\n");
        }
        return;
    }

    if (!strcmp(name, "dtest")) {
        uint8_t buffer[512];

        strcpy((char*)buffer, "HelloDisk!");

        ata_write_sector(100, buffer);

        memset(buffer, 0, 512);

        ata_read_sector(100, buffer);

        kprint("Disk sector 100 says: ");
        kprint((char*)buffer);
        kprint("\n");

        return;
    }



    // fallback
    kprint("Unknown command or question: ");
    kprint(cmd);
    kprint("\n");
}


void shell_init(void) {
    input_len = 0;
    kprint("AtariOS> ");
}

void shell_input(char c) {
    // forward to calculator line-buffer system
    //keyboard_collect(c);

    if (c == '\n') {
        terminal_put_char('\n');

        input_buffer[input_len] = 0;
        shell_execute(input_buffer);
        input_len = 0;

        kprint("\nAtariOS> ");
        return;
    }

    if (c == '\b') {
        if (input_len > 0) {
            input_len--;
            terminal_put_char('\b');
        }
        return;
    }

    if (input_len < SHELL_BUFFER_SIZE - 1) {
        input_buffer[input_len++] = c;
        terminal_put_char(c);
    }
}
