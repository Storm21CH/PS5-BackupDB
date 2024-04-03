#include "resolve.h"

#define PAYLOAD_NAME "PS5 Backup DB v1.0"

void printf_notification(const char* fmt, ...) {
    SceNotificationRequest noti_buffer;

    va_list args;
    va_start(args, fmt);
    f_vsprintf(noti_buffer.message, fmt, args);
    va_end(args);

    noti_buffer.type = 0;
    noti_buffer.unk3 = 0;
    noti_buffer.use_icon_image_uri = 1;
    noti_buffer.target_id = -1;
    f_strcpy(noti_buffer.uri, "cxml://psnotification/tex_icon_system");

    f_sceKernelSendNotificationRequest(0, (SceNotificationRequest * ) & noti_buffer, sizeof(noti_buffer), 0);
}

int file_stat(char* fname)
{
    FILE* file = f_fopen(fname, "rb");
    if (file)
    {
        f_fclose(file);
        return 1;
    }
    return 0;
}

void touch_file(char* destfile)
{
    int fd = f_open(destfile, O_WRONLY | O_CREAT | O_TRUNC, 0777);
    if (fd != -1)
        f_close(fd);
}

int sock;

#define printfsocket(format, ...)\
do {\
  char __printfsocket_buffer[512];\
  int __printfsocket_size = f_sprintf(__printfsocket_buffer, format, ##__VA_ARGS__);\
  f_sceNetSend(sock, __printfsocket_buffer, __printfsocket_size, 0);\
} while(0)

char* getusbpath()
{
    char tmppath[64];
    char tmpusb[64];
    tmpusb[0] = '\0';
    char* retval = f_malloc(sizeof(char) * 10);

    for (int x = 0; x <= 7; x++)
    {
        f_sprintf(tmppath, "/mnt/usb%i/.probe", x);
        touch_file(tmppath);
        if (file_stat(tmppath))
        {
            f_unlink(tmppath);
            f_sprintf(tmpusb, "/mnt/usb%i", x);
            printfsocket("[USB MOUNT PATH]: %s\n", tmpusb);

            f_strcpy(retval, tmpusb);
            return retval;
        }
        tmpusb[0] = '\0';
    }
    printfsocket("[NO USB FOUND.Wait...]\n");
    return NULL;
}

void copy_file(char* sourcefile, char* destfile) {

    int src = f_open(sourcefile, O_RDONLY, 0);

    if (src != -1) {

        int out = f_open(destfile, O_WRONLY | O_CREAT | O_TRUNC, 0777);

        if (out != -1) {

            char* buffer = f_malloc(4194304);

            if (buffer != NULL) {
                size_t bytes;
                while ((bytes = f_read(src, buffer, 4194304)) > 0) {
                    f_write(out, buffer, bytes);
                }
                f_free(buffer);
            }
            f_close(out);
        }
        f_close(src);
    }
}

void touch(const char* filename) {

    int file_descriptor = f_open(filename, O_CREAT, S_IRUSR | S_IWUSR);

    if (file_descriptor == -1) {

        printf_notification("Error creating file on USB");

        f_exit(EXIT_FAILURE);

    }

    f_close(file_descriptor);
}

int check_file_exists(const char* filename) {
    struct stat buffer;
    int exist = f_stat(filename, &buffer);
    if (exist == 0)
        return 1;
    else
        return 0;
}

int start_backup()
{

    touch("/mnt/usb0/.probe");

    if (!check_file_exists("/mnt/usb0/.probe")) {

        touch("/mnt/usb1/.probe");

        if (!check_file_exists("/mnt/usb1/.probe")) {

            printf_notification("Insert USB for database backup");

        }
        else {

            printf_notification("Dumping to USB1");

            f_mkdir("/mnt/usb1/PS5", 0777);
            f_mkdir("/mnt/usb1/PS5/db-backup", 0777);

            f_mkdir("/mnt/usb1/PS5/db-backup/system_data", 0777);
            f_mkdir("/mnt/usb1/PS5/db-backup/system_data/priv", 0777);
            f_mkdir("/mnt/usb1/PS5/db-backup/system_data/priv/mms", 0777);

            copy_file("/system_data/priv/mms/app.db", "/mnt/usb1/PS5/db-backup/system_data/priv/mms/app.db");
            copy_file("/system_data/priv/mms/appinfo.db", "/mnt/usb1/PS5/db-backup/system_data/priv/mms/appinfo.db");
            copy_file("/system_data/priv/mms/addcont.db", "/mnt/usb1/PS5/db-backup/system_data/priv/mms/addcont.db");
            copy_file("/system_data/priv/mms/av_content_bg.db", "/mnt/usb1/PS5/db-backup/system_data/priv/mms/av_content_bg.db");
            copy_file("/system_data/priv/mms/av_content.db", "/mnt/usb1/PS5/db-backup/system_data/priv/mms/av_content.db");
            copy_file("/system_data/priv/mms/notification.db", "/mnt/usb1/PS5/db-backup/system_data/priv/mms/notification.db");
            copy_file("/system_data/priv/mms/notification2.db", "/mnt/usb1/PS5/db-backup/system_data/priv/mms/notification2.db");

            f_unlink("/mnt/usb1/.probe");

            printf_notification("Dump to USB1 done!");

        }

    }

    else {

        printf_notification("Dumping to USB0");

        f_mkdir("/mnt/usb0/PS5", 0777);
        f_mkdir("/mnt/usb0/PS5/db-backup", 0777);

        f_mkdir("/mnt/usb0/PS5/db-backup/system_data", 0777);
        f_mkdir("/mnt/usb0/PS5/db-backup/system_data/priv", 0777);
        f_mkdir("/mnt/usb0/PS5/db-backup/system_data/priv/mms", 0777);

        copy_file("/system_data/priv/mms/app.db", "/mnt/usb0/PS5/db-backup/system_data/priv/mms/app.db");
        copy_file("/system_data/priv/mms/appinfo.db", "/mnt/usb0/PS5/db-backup/system_data/priv/mms/appinfo.db");
        copy_file("/system_data/priv/mms/addcont.db", "/mnt/usb0/PS5/db-backup/system_data/priv/mms/addcont.db");
        copy_file("/system_data/priv/mms/av_content_bg.db", "/mnt/usb0/PS5/db-backup/system_data/priv/mms/av_content_bg.db");
        copy_file("/system_data/priv/mms/av_content.db", "/mnt/usb0/PS5/db-backup/system_data/priv/mms/av_content.db");
        copy_file("/system_data/priv/mms/notification.db", "/mnt/usb0/PS5/db-backup/system_data/priv/mms/notification.db");
        copy_file("/system_data/priv/mms/notification2.db", "/mnt/usb0/PS5/db-backup/system_data/priv/mms/notification2.db");

        f_unlink("/mnt/usb0/.probe");

        printf_notification("Dump to USB0 done!");

    }

    return 0;
}

int payload_main(struct payload_args *args) {
    dlsym_t *dlsym = args->dlsym;

    int libKernel = 0x2001;

    dlsym(libKernel, "sceKernelSleep", &f_sceKernelSleep);
    dlsym(libKernel, "sceKernelLoadStartModule", &f_sceKernelLoadStartModule);
    dlsym(libKernel, "sceKernelDebugOutText", &f_sceKernelDebugOutText);
    dlsym(libKernel, "sceKernelSendNotificationRequest", &f_sceKernelSendNotificationRequest);
    dlsym(libKernel, "sceKernelUsleep", &f_sceKernelUsleep);
    dlsym(libKernel, "scePthreadMutexLock", &f_scePthreadMutexLock);
    dlsym(libKernel, "scePthreadMutexUnlock", &f_scePthreadMutexUnlock);
    dlsym(libKernel, "scePthreadExit", &f_scePthreadExit);
    dlsym(libKernel, "scePthreadMutexInit", &f_scePthreadMutexInit);
    dlsym(libKernel, "scePthreadCreate", &f_scePthreadCreate);
    dlsym(libKernel, "scePthreadMutexDestroy", &f_scePthreadMutexDestroy);
    dlsym(libKernel, "scePthreadJoin", &f_scePthreadJoin);
    dlsym(libKernel, "socket", &f_socket);
    dlsym(libKernel, "bind", &f_bind);
    dlsym(libKernel, "listen", &f_listen);
    dlsym(libKernel, "accept", &f_accept);
    dlsym(libKernel, "open", &f_open);
    dlsym(libKernel, "read", &f_read);
    dlsym(libKernel, "write", &f_write);
    dlsym(libKernel, "close", &f_close);
    dlsym(libKernel, "stat", &f_stat);
    dlsym(libKernel, "fstat", &f_fstat);
    dlsym(libKernel, "rename", &f_rename);
    dlsym(libKernel, "rmdir", &f_rmdir);
    dlsym(libKernel, "mkdir", &f_mkdir);
    dlsym(libKernel, "getdents", &f_getdents);
    dlsym(libKernel, "unlink", &f_unlink);
    dlsym(libKernel, "readlink", &f_readlink);
    dlsym(libKernel, "lseek", &f_lseek);
    dlsym(libKernel, "puts", &f_puts);
    dlsym(libKernel, "mmap", &f_mmap);
    dlsym(libKernel, "munmap", &f_munmap);
    dlsym(libKernel, "_read", &f__read);

    int libNet = f_sceKernelLoadStartModule("libSceNet.sprx", 0, 0, 0, 0, 0);
    dlsym(libNet, "sceNetSocket", &f_sceNetSocket);
    dlsym(libNet, "sceNetConnect", &f_sceNetConnect);
    dlsym(libNet, "sceNetHtons", &f_sceNetHtons);
    dlsym(libNet, "sceNetAccept", &f_sceNetAccept);
    dlsym(libNet, "sceNetSend", &f_sceNetSend);
    dlsym(libNet, "sceNetInetNtop", &f_sceNetInetNtop);
    dlsym(libNet, "sceNetSocketAbort", &f_sceNetSocketAbort);
    dlsym(libNet, "sceNetBind", &f_sceNetBind);
    dlsym(libNet, "sceNetListen", &f_sceNetListen);
    dlsym(libNet, "sceNetSocketClose", &f_sceNetSocketClose);
    dlsym(libNet, "sceNetHtonl", &f_sceNetHtonl);
    dlsym(libNet, "sceNetInetPton", &f_sceNetInetPton);
    dlsym(libNet, "sceNetGetsockname", &f_sceNetGetsockname);
    dlsym(libNet, "sceNetRecv", &f_sceNetRecv);
    dlsym(libNet, "sceNetErrnoLoc", &f_sceNetErrnoLoc);
    dlsym(libNet, "sceNetSetsockopt", &f_sceNetSetsockopt);

    int libC = f_sceKernelLoadStartModule("libSceLibcInternal.sprx", 0, 0, 0, 0, 0);
    dlsym(libC, "vsprintf", &f_vsprintf);
    dlsym(libC, "memset", &f_memset);
    dlsym(libC, "sprintf", &f_sprintf);
    dlsym(libC, "snprintf", &f_snprintf);
    dlsym(libC, "snprintf_s", &f_snprintf_s);
    dlsym(libC, "strcat", &f_strcat);
    dlsym(libC, "free", &f_free);
    dlsym(libC, "memcpy", &f_memcpy);
    dlsym(libC, "strcpy", &f_strcpy);
    dlsym(libC, "strncpy", &f_strncpy);
    dlsym(libC, "sscanf", &f_sscanf);
    dlsym(libC, "malloc", &f_malloc);
    dlsym(libC, "calloc", &f_calloc);
    dlsym(libC, "strlen", &f_strlen);
    dlsym(libC, "strcmp", &f_strcmp);
    dlsym(libC, "strchr", &f_strchr);
    dlsym(libC, "strrchr", &f_strrchr);
    dlsym(libC, "gmtime_s", &f_gmtime_s);
    dlsym(libC, "time", &f_time);
    dlsym(libC, "localtime", &f_localtime);

    dlsym(libC, "fclose", &f_fclose);
    dlsym(libC, "fopen", &f_fopen);
    dlsym(libC, "fseek", &f_fseek);
    dlsym(libC, "ftell", &f_ftell);

    int libNetCtl = f_sceKernelLoadStartModule("libSceNetCtl.sprx", 0, 0, 0, 0, 0);
    dlsym(libNetCtl, "sceNetCtlInit", &f_sceNetCtlInit);
    dlsym(libNetCtl, "sceNetCtlTerm", &f_sceNetCtlTerm);
    dlsym(libNetCtl, "sceNetCtlGetInfo", &f_sceNetCtlGetInfo);

    dlsym(libC, "opendir", &f_opendir);
    dlsym(libC, "readdir", &f_readdir);
    dlsym(libC, "closedir", &f_closedir);

    printf_notification("PS5 Database Backup\nVersion 1.0 by Storm");

    f_sceKernelSleep(7);

    char* usb_mnt_path = getusbpath();
    if (usb_mnt_path == NULL)
    {
        do
        {
            printf_notification("Please insert USB media in exfat/fat32 format");
            f_sceKernelSleep(7);
            usb_mnt_path = getusbpath();
        } while (usb_mnt_path == NULL);
    }
    f_free(usb_mnt_path);

    start_backup();

    return 0;
}
