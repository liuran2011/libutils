#ifndef BLKID_PARTITIONS_H
#define BLKID_PARTITIONS_H

#include "blkidP.h"
#include "blkid_parttypes.h"

int blkid_partitions_get_flags(blkid_probe pr);

blkid_parttable blkid_partlist_new_parttable(blkid_partlist ls,
				const char *type, blkid_loff_t offset);

blkid_partition blkid_partlist_add_partition(blkid_partlist ls,
				blkid_parttable tab,
				blkid_loff_t start, blkid_loff_t size);

int blkid_partlist_set_partno(blkid_partlist ls, int partno);
int blkid_partlist_increment_partno(blkid_partlist ls);

blkid_partition blkid_partlist_get_parent(blkid_partlist ls);

int blkid_partitions_do_subprobe(blkid_probe pr,
			blkid_partition parent, const struct blkid_idinfo *id);

int blkid_partitions_need_typeonly(blkid_probe pr);
int blkid_is_nested_dimension(blkid_partition par,
                        blkid_loff_t start, blkid_loff_t size);

int blkid_partition_set_name(blkid_partition par,
		const unsigned char *name, size_t len);

int blkid_partition_set_utf8name(blkid_partition par,
		const unsigned char *name, size_t len, int enc);

int blkid_partition_set_uuid(blkid_partition par,
		const unsigned char *uuid);

int blkid_partition_set_type(blkid_partition par, int type);

int blkid_partition_set_type_string(blkid_partition par,
                const unsigned char *type, size_t len);

int blkid_partition_set_type_uuid(blkid_partition par,
		const unsigned char *uuid);

int blkid_partition_set_flags(blkid_partition par, unsigned long long flags);

/*
 * partition probers
 */
extern const struct blkid_idinfo aix_pt_idinfo;
extern const struct blkid_idinfo bsd_pt_idinfo;
extern const struct blkid_idinfo unixware_pt_idinfo;
extern const struct blkid_idinfo solaris_x86_pt_idinfo;
extern const struct blkid_idinfo sun_pt_idinfo;
extern const struct blkid_idinfo sgi_pt_idinfo;
extern const struct blkid_idinfo mac_pt_idinfo;
extern const struct blkid_idinfo dos_pt_idinfo;
extern const struct blkid_idinfo gpt_pt_idinfo;
extern const struct blkid_idinfo ultrix_pt_idinfo;

#endif /* BLKID_PARTITIONS_H */
