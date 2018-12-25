#ifndef __COMMON_H
#define __COMMON_H

#define UUID_LENGTH 16
#define PATH_LENGTH 512
#define LINE_LENGTH 512
#define KERNEL_PATH_LENGTH  2048

#define SYSFS_PATH  "/sys"
#define PROCFS_PATH "/proc"
#define DEVFS_PATH  "/dev"
#define USR_PATH "/usr"
#define VAR_PATH "/var"

#define LIB_NAME "lib"
#define NFC_NAME "nfc"
#define TOMCAT7_NAME "tomcat7"
#define LICENSE_FILE_NAME "license"
#define JVM_NAME "jvm"
#define JDK7_NAME "java-7-openjdk-amd64"
#define JRE_NAME "jre"
#define BIN_NAME "bin"
#define JAVA_NAME "java"
#define SBIN_NAME "sbin"
#define APACHE2_NAME "apache2"
#define SELF_NAME "self"
#define EXE_NAME "exe"

#define DEVFS_MEM_FILE  "mem"

#define FIRMWARE    "firmware"
#define EFI "efi"
#define SYSTAB  "systab"

#define PROCFS_MOUNT_FILE    "mounts"

#define ALIGNUP_4_BYTES(len) (((len)+3)&(~3)) 

#define EXPORT_API __attribute__((visibility("default")))

#endif
