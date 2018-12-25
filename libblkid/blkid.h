/*
 * blkid.h - Interface for libblkid, a library to identify block devices
 *
 * Copyright (C) 2001 Andreas Dilger
 * Copyright (C) 2003 Theodore Ts'o
 * Copyright (C) 2008 Karel Zak <kzak@redhat.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
 */

#ifndef _BLKID_BLKID_H
#define _BLKID_BLKID_H

#include <stdint.h>
#include <sys/types.h>

#define BLKID_VERSION   "2.20.0"
#define BLKID_DATE      "19-Oct-2011"

/**
 * blkid_probe:
 *
 * low-level probing setting
 */
typedef struct blkid_struct_probe *blkid_probe;

/**
 * blkid_partlist
 *
 * list of all detected partitions and partitions tables
 */
typedef struct blkid_struct_partlist *blkid_partlist;

/**
 * blkid_partition:
 *
 * information about a partition
 */
typedef struct blkid_struct_partition *blkid_partition;

/**
 * blkid_parttable:
 *
 * information about a partition table
 */
typedef struct blkid_struct_parttable *blkid_parttable;

/**
 * blkid_loff_t:
 *
 * 64-bit signed number for offsets and sizes
 */
typedef int64_t blkid_loff_t;

/* devno.c */
char *blkid_devno_to_devname(dev_t devno);
int blkid_devno_to_wholedisk(dev_t dev, char *diskname,
                        size_t len, dev_t *diskdevno);

/* getsize.c */
blkid_loff_t blkid_get_dev_size(int fd);

/* tag.c */
int blkid_parse_tag_string(const char *token, char **ret_type,
				  char **ret_val);

/* probe.c */
blkid_probe blkid_new_probe(void);
blkid_probe blkid_new_probe_from_filename(const char *filename);
void blkid_free_probe(blkid_probe pr);
void blkid_reset_probe(blkid_probe pr);

int blkid_probe_set_device(blkid_probe pr, int fd,
	                blkid_loff_t off, blkid_loff_t size);

dev_t blkid_probe_get_devno(blkid_probe pr);
dev_t blkid_probe_get_wholedisk_devno(blkid_probe pr);
int blkid_probe_is_wholedisk(blkid_probe pr);

blkid_loff_t blkid_probe_get_size(blkid_probe pr);
blkid_loff_t blkid_probe_get_offset(blkid_probe pr);
unsigned int blkid_probe_get_sectorsize(blkid_probe pr);
blkid_loff_t blkid_probe_get_sectors(blkid_probe pr);

int blkid_probe_get_fd(blkid_probe pr);

/*
 * superblocks probing
 */
int blkid_known_fstype(const char *fstype);
int blkid_superblocks_get_name(size_t idx, const char **name, int *usage);

int blkid_probe_enable_superblocks(blkid_probe pr, int enable);

#define BLKID_SUBLKS_LABEL	(1 << 1) /* read LABEL from superblock */
#define BLKID_SUBLKS_LABELRAW	(1 << 2) /* read and define LABEL_RAW result value*/
#define BLKID_SUBLKS_UUID	(1 << 3) /* read UUID from superblock */
#define BLKID_SUBLKS_UUIDRAW	(1 << 4) /* read and define UUID_RAW result value */
#define BLKID_SUBLKS_TYPE	(1 << 5) /* define TYPE result value */
#define BLKID_SUBLKS_SECTYPE	(1 << 6) /* define compatible fs type (second type) */
#define BLKID_SUBLKS_USAGE	(1 << 7) /* define USAGE result value */
#define BLKID_SUBLKS_VERSION	(1 << 8) /* read FS type from superblock */
#define BLKID_SUBLKS_MAGIC	(1 << 9) /* define SBMAGIC and SBMAGIC_OFFSET */

#define BLKID_SUBLKS_DEFAULT	(BLKID_SUBLKS_LABEL | BLKID_SUBLKS_UUID | \
				 BLKID_SUBLKS_TYPE | BLKID_SUBLKS_SECTYPE)

int blkid_probe_set_superblocks_flags(blkid_probe pr, int flags);

int blkid_probe_reset_superblocks_filter(blkid_probe pr);
int blkid_probe_invert_superblocks_filter(blkid_probe pr);

/**
 * BLKID_FLTR_NOTIN
 */
#define BLKID_FLTR_NOTIN		1
/**
 * BLKID_FLTR_ONLYIN
 */
#define BLKID_FLTR_ONLYIN		2
int blkid_probe_filter_superblocks_type(blkid_probe pr, int flag, char *names[]);

#define BLKID_USAGE_FILESYSTEM		(1 << 1)
#define BLKID_USAGE_RAID		(1 << 2)
#define BLKID_USAGE_CRYPTO		(1 << 3)
#define BLKID_USAGE_OTHER		(1 << 4)
int blkid_probe_filter_superblocks_usage(blkid_probe pr, int flag, int usage);

/*
 * partitions probing
 */
int blkid_known_pttype(const char *pttype);
int blkid_probe_enable_partitions(blkid_probe pr, int enable);

int blkid_probe_reset_partitions_filter(blkid_probe pr);
int blkid_probe_invert_partitions_filter(blkid_probe pr);
int blkid_probe_filter_partitions_type(blkid_probe pr, int flag, char *names[]);


/* partitions probing flags */
#define BLKID_PARTS_FORCE_GPT		(1 << 1)
#define BLKID_PARTS_ENTRY_DETAILS	(1 << 2)
int blkid_probe_set_partitions_flags(blkid_probe pr, int flags);

/* binary interface */
blkid_partlist blkid_probe_get_partitions(blkid_probe pr);

int blkid_partlist_numof_partitions(blkid_partlist ls);
blkid_parttable blkid_partlist_get_table(blkid_partlist ls);
blkid_partition blkid_partlist_get_partition(blkid_partlist ls, int n);
blkid_partition blkid_partlist_devno_to_partition(blkid_partlist ls, dev_t devno);

blkid_parttable blkid_partition_get_table(blkid_partition par);
const char *blkid_partition_get_name(blkid_partition par);
const char *blkid_partition_get_uuid(blkid_partition par);
int blkid_partition_get_partno(blkid_partition par);
blkid_loff_t blkid_partition_get_start(blkid_partition par);
blkid_loff_t blkid_partition_get_size(blkid_partition par);
int blkid_partition_get_type(blkid_partition par);
const char *blkid_partition_get_type_string(blkid_partition par);
unsigned long long blkid_partition_get_flags(blkid_partition par);
int blkid_partition_is_logical(blkid_partition par);
int blkid_partition_is_extended(blkid_partition par);
int blkid_partition_is_primary(blkid_partition par);

const char *blkid_parttable_get_type(blkid_parttable tab);
blkid_loff_t blkid_parttable_get_offset(blkid_parttable tab);
blkid_partition blkid_parttable_get_parent(blkid_parttable tab);

/*
 * NAME=value low-level interface
 */
int blkid_do_probe(blkid_probe pr);
int blkid_do_safeprobe(blkid_probe pr);
int blkid_do_fullprobe(blkid_probe pr);

int blkid_probe_numof_values(blkid_probe pr);
int blkid_probe_get_value(blkid_probe pr, int num, const char **name,
                        const char **data, size_t *len);
int blkid_probe_lookup_value(blkid_probe pr, const char *name,
                        const char **data, size_t *len);
int blkid_probe_has_value(blkid_probe pr, const char *name);

/*
 * Deprecated functions/macros
 */
#ifndef BLKID_DISABLE_DEPRECATED

#define BLKID_PROBREQ_LABEL     BLKID_SUBLKS_LABEL
#define BLKID_PROBREQ_LABELRAW  BLKID_SUBLKS_LABELRAW
#define BLKID_PROBREQ_UUID      BLKID_SUBLKS_UUID
#define BLKID_PROBREQ_UUIDRAW   BLKID_SUBLKS_UUIDRAW
#define BLKID_PROBREQ_TYPE      BLKID_SUBLKS_TYPE
#define BLKID_PROBREQ_SECTYPE   BLKID_SUBLKS_SECTYPE
#define BLKID_PROBREQ_USAGE     BLKID_SUBLKS_USAGE
#define BLKID_PROBREQ_VERSION   BLKID_SUBLKS_VERSION

int blkid_probe_set_request(blkid_probe pr, int flags);
int blkid_probe_filter_usage(blkid_probe pr, int flag, int usage);
int blkid_probe_filter_types(blkid_probe pr, int flag, char *names[]);
int blkid_probe_invert_filter(blkid_probe pr);
int blkid_probe_reset_filter(blkid_probe pr);

#endif /* BLKID_DISABLE_DEPRECATED */

#endif /* _BLKID_BLKID_H */
