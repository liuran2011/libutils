#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/mman.h>

#include "common.h"
#include "dmidecode.h"

#define SUPPORTED_SMBIOS_VER 0x0208

#ifdef __ia64__
#define ALIGNMENT_WORKAROUND
#endif

typedef unsigned char u8;
typedef unsigned short u16;
typedef signed short i16;
typedef unsigned int u32;

#ifdef ALIGNMENT_WORKAROUND
#	ifdef BIGENDIAN
#	define WORD(x) (u16)((x)[1] + ((x)[0] << 8))
#	define DWORD(x) (u32)((x)[3] + ((x)[2] << 8) + ((x)[1] << 16) + ((x)[0] << 24))
#	else 
#	define WORD(x) (u16)((x)[0] + ((x)[1] << 8))
#	define DWORD(x) (u32)((x)[0] + ((x)[1] << 8) + ((x)[2] << 16) + ((x)[3] << 24))
#	endif 
#else
#define WORD(x) (u16)(*(const u16 *)(x))
#define DWORD(x) (u32)(*(const u32 *)(x))
#endif

struct dmi_header
{
	u8 type;
	u8 length;
	u16 handle;
	u8 *data;
};

static u8 _system_uuid[UUID_LENGTH]={'\0'};
static u8 *_system_serial_number=NULL;
static u8 *_base_board_serial_no=NULL;
static u8 *_chassis_serial_no=NULL;

static int myread(int fd, u8 *buf, size_t count)
{
	ssize_t r = 1;
	size_t r2 = 0;

	while (r2 != count && r != 0)
	{
		r = read(fd, buf + r2, count - r2);
		if (r == -1)
		{
			if (errno != EINTR)
			{
				return -1;
			}
		}
		else
			r2 += r;
	}

	if (r2 != count)
	{
		return -1;
	}

	return 0;
}

static int checksum(const u8 *buf, size_t len)
{
	u8 sum = 0;
	size_t a;

	for (a = 0; a < len; a++)
		sum += buf[a];
	return (sum == 0);
}

static void *mem_chunk(size_t base, 
            size_t len, 
            const char *devmem)
{
	void *p=NULL;
	int fd;
	size_t mmoffset;
	void *mmp;

	if ((fd = open(devmem, O_RDONLY)) == -1)
	{
	    goto err_open;
	}

	if ((p = malloc(len)) == NULL)
	{
	    goto err_malloc;
	}

#ifdef _SC_PAGESIZE
	mmoffset = base % sysconf(_SC_PAGESIZE);
#else
	mmoffset = base % getpagesize();
#endif 

	mmp = mmap(0, mmoffset + len, PROT_READ, MAP_SHARED, fd, base - mmoffset);
	if (mmp == MAP_FAILED)
		goto try_read;

	memcpy(p, (u8 *)mmp + mmoffset, len);

	munmap(mmp, mmoffset + len);

	goto out;

try_read:
	if (lseek(fd, base, SEEK_SET) == -1)
	{
		goto err;
	}

	if (myread(fd, p, len) == -1)
	{
		goto err;
	}

out:
    close(fd);

	return p;

err:
    free(p);
    p=NULL;
err_malloc:
    close(fd);
err_open:
    return p;
}

static int dmi_system_uuid(const u8 *p, u16 ver)
{
	int only0xFF = 1, only0x00 = 1;
	int i;

	for (i = 0; i < 16 && (only0x00 || only0xFF); i++)
	{
		if (p[i] != 0x00) only0x00 = 0;
		if (p[i] != 0xFF) only0xFF = 0;
	}

	if (only0xFF)
	{
		return -1;
	}
	if (only0x00)
	{
		return -1;
	}

	if (ver >= 0x0206)
    {
        _system_uuid[0]=p[3];
        _system_uuid[1]=p[2];
        _system_uuid[2]=p[1];
        _system_uuid[3]=p[0];
        _system_uuid[4]=p[5];
        _system_uuid[5]=p[4];
        _system_uuid[6]=p[7];
        _system_uuid[7]=p[6];
    }
    else
    {
        memcpy(_system_uuid,p,8);
    }

    memcpy(_system_uuid+8,p+8,8);

    return 0;
}

static void to_dmi_header(struct dmi_header *h, u8 *data)
{
	h->type = data[0];
	h->length = data[1];
	h->handle = WORD(data + 2);
	h->data = data;
}

static const char *dmi_string(const struct dmi_header *dm, u8 s)
{
	char *bp = (char *)dm->data;
	size_t i, len;

	if (s == 0)
		return NULL;

	bp += dm->length;
	while (s > 1 && *bp)
	{
		bp += strlen(bp);
		bp++;
		s--;
	}

	if (!*bp)
		return NULL;

	len = strlen(bp);
	for (i = 0; i < len; i++)
	{
		if (bp[i] < 32 || bp[i] == 127)
		{
			bp[i] = '.';
	    }
    }
    
	return bp;
}

static void dmi_decode(const struct dmi_header *h, u16 ver)
{
    const u8 *data = h->data;
    const u8 *tmp=NULL;
    
    switch (h->type)
    {
        case 1:
        {
            if (h->length < 0x08) 
            {
                break;
            }

            tmp=dmi_string(h,data[0x07]);
            if(tmp!=NULL)
            {
                _system_serial_number=strdup(tmp);
            }
            
            if (h->length < 0x19) 
            {
                break;
            }
           
            dmi_system_uuid(data + 0x08, ver);
           
            break;
        }
        
        case 2: 
        {
            if (h->length < 0x08) 
            {
                break;
            }

            tmp=dmi_string(h,data[0x07]);
            if(tmp!=NULL)
            {
                _base_board_serial_no=strdup(tmp);
            }
           
            break;
        }
        
        case 3: 
        {
            if (h->length < 0x09) 
            {   
                break;
            }
            
            tmp=dmi_string(h,data[0x07]);
            if(tmp!=NULL)
            {
                _chassis_serial_no=strdup(tmp);
            }
            
            break;
        }

        default: break;
    }
}

static int dmi_table(u32 base, u16 len, 
            u16 num, u16 ver, 
            const char *devmem)
{
	u8 *buf;
	u8 *data;
	int i = 0;
    int ret=-1;

	if (ver > SUPPORTED_SMBIOS_VER)
	{   
        /*return ret; */
	}

	if ((buf = mem_chunk(base, len, devmem)) == NULL)
	{
		return ret;
	}

	data = buf;
	while (i < num && data+4 <= buf + len) 
	{
		u8 *next;
		struct dmi_header h;

		to_dmi_header(&h, data);

		if (h.length < 4)
		{
			break;
		}

		if (h.type == 127)
			break;

		next = data + h.length;
		while (next - buf + 1 < len && (next[0] != 0 || next[1] != 0))
			next++;
		next += 2;

	    if (next - buf <= len)
		{
			dmi_decode(&h, ver);
		}
      
		data = next;
		i++;
	}

	free(buf);

    return 0;
}

static int smbios_decode(u8 *buf, const char *devmem)
{
	u16 ver;

	if (!checksum(buf, buf[0x05])
	 || memcmp(buf + 0x10, "_DMI_", 5) != 0
	 || !checksum(buf + 0x10, 0x0F))
		return -1;

	ver = (buf[0x06] << 8) + buf[0x07];
	switch (ver)
	{
		case 0x021F:
		case 0x0221:
			ver = 0x0203;
			break;
		case 0x0233:
			ver = 0x0206;
			break;
	}

	return dmi_table(DWORD(buf + 0x18), WORD(buf + 0x16), WORD(buf + 0x1C),
		ver, devmem);
}

static int legacy_decode(u8 *buf, const char *devmem)
{
	if (!checksum(buf, 0x0F))
		return -1;

	return dmi_table(DWORD(buf + 0x08), WORD(buf + 0x06), WORD(buf + 0x0C),
		((buf[0x0E] & 0xF0) << 4) + (buf[0x0E] & 0x0F), devmem);
}

static int address_from_efi(size_t *address)
{
	FILE *efi_systab;
	char linebuf[64];
	char filename[PATH_LENGTH];
	int ret;
 
	*address = 0; 

    sprintf(filename,"%s/%s/%s/%s",SYSFS_PATH,
            FIRMWARE,EFI,SYSTAB);
    efi_systab=fopen(filename,"r");
    if(NULL==efi_systab)
    {   
        sprintf(filename,"%s/%s/%s",PROCFS_PATH,
                EFI,SYSTAB);
        efi_systab=fopen(filename,"r");
        if(NULL==efi_systab)
        {
            return EFI_NOT_FOUND;
        }
	}
	
	ret = EFI_NO_SMBIOS;
	while ((fgets(linebuf, sizeof(linebuf) - 1, efi_systab)) != NULL)
	{
		char *addrp = strchr(linebuf, '=');
		*(addrp++) = '\0';
		if (strcmp(linebuf, "SMBIOS") == 0)
		{
			*address = strtoul(addrp, NULL, 0);
			ret = 0;
			break;
		}
	}

    fclose(efi_systab);

	return ret;
}

int dmi_decode_init()
{
	int ret = 0; 
	size_t fp;
	int efi;
	u8 *buf;
    char mem_dev[PATH_LENGTH];

    sprintf(mem_dev,"%s/%s",DEVFS_PATH,DEVFS_MEM_FILE);
    
	efi = address_from_efi(&fp);
	switch (efi)
	{
		case EFI_NOT_FOUND:
			goto memory_scan;
		case EFI_NO_SMBIOS:
			ret = -1;
			goto exit;
	}

	if ((buf = mem_chunk(fp, 0x20, mem_dev)) == NULL)
	{
		ret = -1;
		goto exit;
	}

    ret=smbios_decode(buf,mem_dev);
    goto done;

memory_scan:
	if ((buf = mem_chunk(0xF0000, 0x10000, mem_dev)) == NULL)
	{
		ret = -1;
		goto exit;
	}

	for (fp = 0; fp <= 0xFFF0; fp += 16)
	{
		if (memcmp(buf + fp, "_SM_", 4) == 0 && fp <= 0xFFE0)
		{
            ret=smbios_decode(buf+fp,mem_dev);
            if(ret==0)
            {
                fp+=16;
            }
		}
		else if (memcmp(buf + fp, "_DMI_", 5) == 0)
		{
			legacy_decode(buf + fp,mem_dev);
		}
	}

done:
	free(buf);
exit:
	return ret;
}

uint8_t *dmi_get_system_uuid()
{
    return _system_uuid;
}

uint8_t *dmi_get_system_serial_no()
{
    return _system_serial_number;
}

uint8_t *dmi_get_base_board_serial_no()
{
    return _base_board_serial_no;
}

uint8_t *dmi_get_chassis_serial_no()
{
    return _chassis_serial_no;
}
