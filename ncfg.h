#ifndef _NCFG_H
#define _NCFG_H

typedef unsigned int u_int;

typedef struct TCFGENTRY
{
    char *section;
    char *id;
    char *value;
    char *comment;
    u_short flags;
} TCFGENTRY, *PCFGENTRY;

#define CFE_MUST_FREE_SECTION	0x8000
#define CFE_MUST_FREE_ID	0x4000
#define CFE_MUST_FREE_VALUE	0x2000
#define CFE_MUST_FREE_COMMENT	0x1000

/* configuration file */
typedef struct TCFGDATA
{
    char *fileName;		/* Current file name */

    int dirty;			/* Did we make modifications? */

    char *image;		/* In-memory copy of the file */
    size_t size;		/* Size of this copy (excl. \0) */
    time_t mtime;		/* Modification time */

    u_int numEntries;
    u_int maxEntries;
    PCFGENTRY entries;

    /* Compatibility */
    u_int cursor;
    char *section;
    char *id;
    char *value;
    char *comment;
    u_short flags;

} TCONFIG, *PCONFIG;

#define CFG_VALID		0x8000
#define CFG_EOF			0x4000

#define CFG_ERROR		0x0000
#define CFG_SECTION		0x0001
#define CFG_DEFINE		0x0002
#define CFG_CONTINUE		0x0003

#define CFG_TYPEMASK		0x000F
#define CFG_TYPE(X)		((X) & CFG_TYPEMASK)
#define cfg_valid(X)		((X) != NULL && ((X)->flags & CFG_VALID))
#define cfg_eof(X)		((X)->flags & CFG_EOF)
#define cfg_section(X)		(CFG_TYPE((X)->flags) == CFG_SECTION)
#define cfg_define(X)		(CFG_TYPE((X)->flags) == CFG_DEFINE)
#define cfg_continue(X)		(CFG_TYPE((X)->flags) == CFG_CONTINUE)

#ifdef __cplusplus
extern "C"  {
#endif

int cfg_init (PCONFIG *ppconf, char *filename);
int cfg_init2 (PCONFIG *ppconf, char *filename, int doCreate);
int cfg_done (PCONFIG pconfig);
int cfg_freeimage (PCONFIG pconfig);
int cfg_refresh (PCONFIG pconfig);
int cfg_storeentry (PCONFIG pconfig, char *section, char *id, char *value, char *comment, int dynamic);
int cfg_rewind (PCONFIG pconfig);
int cfg_nextentry (PCONFIG pconfig);
int cfg_find (PCONFIG pconfig, char *section, char *id);
int cfg_merge (PCONFIG pconfig, PCONFIG src);
int cfg_write (PCONFIG pconfig, char *section, char *id, char *value);
int cfg_commit (PCONFIG pconfig);
int cfg_getstring (PCONFIG pconfig, char *section, char *id, char **valptr);
int cfg_getint (PCONFIG pconfig, char *section, char *id, int32_t  *valptr);
int cfg_getshort (PCONFIG pconfig, char *section, char *id, short *valptr);

#ifdef __cplusplus 
}
#endif

#endif
