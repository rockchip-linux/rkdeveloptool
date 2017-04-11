#ifndef _GPT_H
#define _GPT_H

#define MSDOS_MBR_SIGNATURE 0xAA55
#define EFI_PMBR_OSTYPE_EFI 0xEF
#define EFI_PMBR_OSTYPE_EFI_GPT 0xEE

#define GPT_HEADER_SIGNATURE 0x5452415020494645ULL
#define GPT_HEADER_REVISION_V1 0x00010000
#define GPT_PRIMARY_PARTITION_TABLE_LBA 1ULL
#define GPT_ENTRY_NAME "gpt"
#define GPT_ENTRY_NUMBERS		128
#define GPT_ENTRY_SIZE			128
#define PART_PROPERTY_BOOTABLE (1 << 2)

#define EFI_GUID(a,b,c,d0,d1,d2,d3,d4,d5,d6,d7) \
	((efi_guid_t) \
	{{ (a) & 0xff, ((a) >> 8) & 0xff, ((a) >> 16) & 0xff, ((a) >> 24) & 0xff, \
		(b) & 0xff, ((b) >> 8) & 0xff, \
		(c) & 0xff, ((c) >> 8) & 0xff, \
		(d0), (d1), (d2), (d3), (d4), (d5), (d6), (d7) }})

#define PARTITION_IDBLOCK_GUID \
	EFI_GUID(0xDA2BB095, 0x390E, 0x48ca, \
		0x90, 0x47, 0x05, 0xE8, 0x18, 0xB2, 0x97, 0xCE)

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int  u32;
typedef unsigned long long u64;

#define uswap_16(x) \
	((((x) & 0xff00) >> 8) | \
	 (((x) & 0x00ff) << 8))
#define uswap_32(x) \
	((((x) & 0xff000000) >> 24) | \
	 (((x) & 0x00ff0000) >>  8) | \
	 (((x) & 0x0000ff00) <<  8) | \
	 (((x) & 0x000000ff) << 24))
#define _uswap_64(x, sfx) \
	((((x) & 0xff00000000000000##sfx) >> 56) | \
	 (((x) & 0x00ff000000000000##sfx) >> 40) | \
	 (((x) & 0x0000ff0000000000##sfx) >> 24) | \
	 (((x) & 0x000000ff00000000##sfx) >>  8) | \
	 (((x) & 0x00000000ff000000##sfx) <<  8) | \
	 (((x) & 0x0000000000ff0000##sfx) << 24) | \
	 (((x) & 0x000000000000ff00##sfx) << 40) | \
	 (((x) & 0x00000000000000ff##sfx) << 56))
#if defined(__GNUC__)
# define uswap_64(x) _uswap_64(x, ull)
#else
# define uswap_64(x) _uswap_64(x, )
#endif
#define __LITTLE_ENDIAN__ 1
#ifdef __LITTLE_ENDIAN__
# define cpu_to_le16(x)		(x)
# define cpu_to_le32(x)		(x)
# define cpu_to_le64(x)		(x)
# define le16_to_cpu(x)		(x)
# define le32_to_cpu(x)		(x)
# define le64_to_cpu(x)		(x)
# define cpu_to_be16(x)		uswap_16(x)
# define cpu_to_be32(x)		uswap_32(x)
# define cpu_to_be64(x)		uswap_64(x)
# define be16_to_cpu(x)		uswap_16(x)
# define be32_to_cpu(x)		uswap_32(x)
# define be64_to_cpu(x)		uswap_64(x)
#else
# define cpu_to_le16(x)		uswap_16(x)
# define cpu_to_le32(x)		uswap_32(x)
# define cpu_to_le64(x)		uswap_64(x)
# define le16_to_cpu(x)		uswap_16(x)
# define le32_to_cpu(x)		uswap_32(x)
# define le64_to_cpu(x)		uswap_64(x)
# define cpu_to_be16(x)		(x)
# define cpu_to_be32(x)		(x)
# define cpu_to_be64(x)		(x)
# define be16_to_cpu(x)		(x)
# define be32_to_cpu(x)		(x)
# define be64_to_cpu(x)		(x)
#endif


typedef union {
	struct {
	unsigned int time_low;
	unsigned short time_mid;
	unsigned short time_hi_and_version;
	unsigned char clock_seq_hi_and_reserved;
	unsigned char clock_seq_low;
	unsigned char node[6];
	} uuid;
	u8 raw[16];
} efi_guid_t;
#pragma pack(1)
typedef struct {
	u16 usTag;/*0xEEEE*/
	u16 usBackupGpt;/*0:no backup,1:has backup*/
	u16 usEntryCount;
	u64 entryDataSize[32];
} gpt_compact_info;
/* based on linux/include/genhd.h */
typedef struct {
	u8 boot_ind;		/* 0x80 - active */
	u8 head;		/* starting head */
	u8 sector;		/* starting sector */
	u8 cyl;			/* starting cylinder */
	u8 sys_ind;		/* What partition type */
	u8 end_head;		/* end head */
	u8 end_sector;		/* end sector */
	u8 end_cyl;		/* end cylinder */
	u32 start_sect;	/* starting sector counting from 0 */
	u32 nr_sects;	/* nr of sectors in partition */
} mbr_partition ;

/* based on linux/fs/partitions/efi.h */
typedef struct _gpt_header {
	u64 signature;
	u32 revision;
	u32 header_size;
	u32 header_crc32;
	u32 reserved1;
	u64 my_lba;
	u64 alternate_lba;
	u64 first_usable_lba;
	u64 last_usable_lba;
	efi_guid_t disk_guid;
	u64 partition_entry_lba;
	u32 num_partition_entries;
	u32 sizeof_partition_entry;
	u32 partition_entry_array_crc32;
} gpt_header;

typedef union _gpt_entry_attributes {
	struct {
		u64 required_to_function:1;
		u64 no_block_io_protocol:1;
		u64 legacy_bios_bootable:1;
		u64 reserved:45;
		u64 type_guid_specific:16;
	} fields;
	unsigned long long raw;
} gpt_entry_attributes;

#define PARTNAME_SZ	72
typedef struct _gpt_entry {
	efi_guid_t partition_type_guid;
	efi_guid_t unique_partition_guid;
	u64 starting_lba;
	u64 ending_lba;
	gpt_entry_attributes attributes;
	u16 partition_name[PARTNAME_SZ / sizeof(u16)];
} gpt_entry;

typedef struct _legacy_mbr {
	u8 boot_code[440];
	u32 unique_mbr_signature;
	u16 unknown;
	mbr_partition partition_record[4];
	u16 signature;
} legacy_mbr;
#pragma pack()
#endif	/* _GPT_H */
