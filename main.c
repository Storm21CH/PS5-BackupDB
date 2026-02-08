/* Copyright (C) 2026 Storm21

This program is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 3, or (at your option) any
later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; see the file COPYING. If not, see
<http://www.gnu.org/licenses/>.  */



#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdbool.h>
#include "notify.h"
int notify_user;
#include <fcntl.h>
#include <unistd.h>


int file_stat(char* fname)
{
    FILE* file = fopen(fname, "rb");
    if (file)
    {
        fclose(file);
        return 1;
    }
    return 0;
}


void touch_file(char* destfile)
{
    int fd = open(destfile, O_WRONLY | O_CREAT | O_TRUNC, 0777);

    if (fd != -1)
        close(fd);
}


char* getusbpath()
{
    char tmppath[64];
    char tmpusb[64];
    tmpusb[0] = '\0';
    char* retval = malloc(sizeof(char) * 10);

    for (int x = 0; x <= 7; x++)
    {
        sprintf(tmppath, "/mnt/usb%i/.probe", x);
        touch_file(tmppath);
        if (file_stat(tmppath))
        {
            remove(tmppath);
            sprintf(tmpusb, "/mnt/usb%i", x);
            strcpy(retval, tmpusb);
            return retval;
        }
        tmpusb[0] = '\0';
    }
    return NULL;
}


void copy_file(char* sourcefile, char* destfile) {

    int src = open(sourcefile, O_RDONLY, 0);

    if (src != -1) {

        int out = open(destfile, O_WRONLY | O_CREAT | O_TRUNC, 0777);

        if (out != -1) {

            char* buffer = malloc(4194304);

            if (buffer != NULL) {       
                size_t bytes;
                while ((bytes = read(src, buffer, 4194304)) > 0) {
                    write(out, buffer, bytes);
                }
                free(buffer);
            }
            close(out);
        }
        close(src);
    }
}


void touch(const char* filename) {

    int file_descriptor = open(filename, O_CREAT, S_IRUSR | S_IWUSR);

    if (file_descriptor == -1) {

        if (notify_user) {
            notify("Error creating file on USB");
        }

        exit(EXIT_FAILURE);

    }

    close(file_descriptor);
}


bool check_file_exists(const char* filename)
{
    struct stat buffer;
    return stat(filename, &buffer) == 0 ? true : false;
}


int start_backup()
{

    touch("/mnt/usb0/.probe");

    if (!check_file_exists("/mnt/usb0/.probe")) {

        touch("/mnt/usb1/.probe");

        if (!check_file_exists("/mnt/usb1/.probe")) {

            if (notify_user) {
                notify("Insert USB for database backup");
            }

        }
        else {

            if (notify_user) {
                notify("Dumping to USB1");
            }

            mkdir("/mnt/usb1/PS5", 0777);
            mkdir("/mnt/usb1/PS5/db-backup", 0777);

            mkdir("/mnt/usb1/PS5/db-backup/system_data", 0777);
            mkdir("/mnt/usb1/PS5/db-backup/system_data/priv", 0777);
            mkdir("/mnt/usb1/PS5/db-backup/system_data/priv/mms", 0777);

            copy_file("/system_data/priv/mms/app.db", "/mnt/usb1/PS5/db-backup/system_data/priv/mms/app.db"); 
            copy_file("/system_data/priv/mms/appinfo.db", "/mnt/usb1/PS5/db-backup/system_data/priv/mms/appinfo.db"); 
            copy_file("/system_data/priv/mms/addcont.db", "/mnt/usb1/PS5/db-backup/system_data/priv/mms/addcont.db"); 
            copy_file("/system_data/priv/mms/av_content_bg.db", "/mnt/usb1/PS5/db-backup/system_data/priv/mms/av_content_bg.db"); 
            copy_file("/system_data/priv/mms/av_content.db", "/mnt/usb1/PS5/db-backup/system_data/priv/mms/av_content.db");
            copy_file("/system_data/priv/mms/notification.db", "/mnt/usb1/PS5/db-backup/system_data/priv/mms/notification.db");
            copy_file("/system_data/priv/mms/notification2.db", "/mnt/usb1/PS5/db-backup/system_data/priv/mms/notification2.db");

            remove("/mnt/usb1/.probe");   

            if (notify_user) {
                notify("Dump to USB1 done!");
            }

        }

    }

    else {

        if (notify_user) {
            notify("Dumping to USB0");
        }

        mkdir("/mnt/usb0/PS5", 0777);
        mkdir("/mnt/usb0/PS5/db-backup", 0777);

        mkdir("/mnt/usb0/PS5/db-backup/system_data", 0777);
        mkdir("/mnt/usb0/PS5/db-backup/system_data/priv", 0777);
        mkdir("/mnt/usb0/PS5/db-backup/system_data/priv/mms", 0777);

        copy_file("/system_data/priv/mms/app.db", "/mnt/usb0/PS5/db-backup/system_data/priv/mms/app.db"); 
        copy_file("/system_data/priv/mms/appinfo.db", "/mnt/usb0/PS5/db-backup/system_data/priv/mms/appinfo.db"); 
        copy_file("/system_data/priv/mms/addcont.db", "/mnt/usb0/PS5/db-backup/system_data/priv/mms/addcont.db"); 
        copy_file("/system_data/priv/mms/av_content_bg.db", "/mnt/usb0/PS5/db-backup/system_data/priv/mms/av_content_bg.db"); 
        copy_file("/system_data/priv/mms/av_content.db", "/mnt/usb0/PS5/db-backup/system_data/priv/mms/av_content.db");
        copy_file("/system_data/priv/mms/notification.db", "/mnt/usb0/PS5/db-backup/system_data/priv/mms/notification.db");
        copy_file("/system_data/priv/mms/notification2.db", "/mnt/usb0/PS5/db-backup/system_data/priv/mms/notification2.db");

        remove("/mnt/usb0/.probe");   

        if (notify_user) {
            notify("Dump to USB0 done!");
        }

    }

    return 0;
}


int main() {

    notify_user = 1;

    if (notify_user) {
        notify("PS5 Database Backup\nVersion 1.1 (ElfLoader) by Storm");
    }

    sleep(7); 

    char* usb_mnt_path = getusbpath();
    if (usb_mnt_path == NULL)
    {
        do
        {
            if (notify_user) {
                notify("Please insert USB media in exfat/fat32 format");
            }

            sleep(7); 

            usb_mnt_path = getusbpath();

        } while (usb_mnt_path == NULL);
    }
    free(usb_mnt_path);

    start_backup(); 

    notify_user = 0;

}

