/*
 * Copyright (c) 2022, iProgramInCpp <iprogramincpp@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

namespace Kernel::Ext2 {
// The second extended filesystem constants/structures

constexpr u32 ext4_feature_incompat_64bit = (1 << 7);
constexpr u32 max_name_len = 255;
constexpr u16 super_magic = 0xEF53;

// Constants relative to the data blocks
constexpr u32 num_direct_blocks = 12;

// FIXME: Give these variables a better name.
constexpr u32 indirect_block = num_direct_blocks,
              dindirect_block = indirect_block + 1,
              tindirect_block = dindirect_block + 1,
              num_blocks = tindirect_block + 1;

// Group descriptor sizes
constexpr size_t min_group_descriptor_size = 32,
                 min_group_descriptor_size_64bit = 64,
                 max_group_descriptor_size = min_group_descriptor_size;

// Block sizes
constexpr size_t min_block_log_size = 10,
                 max_block_log_size = 16,
                 min_block_size = (1 << min_block_log_size),
                 max_block_size = (1 << max_block_log_size);

// Fragment sizes
constexpr size_t min_fragment_size = min_block_size,
                 max_fragment_size = max_block_size,
                 min_fragment_log_size = min_block_log_size;

// The size of an inode
constexpr size_t good_old_inode_size = 128;

// This is how many hard links are allowed at maximum.
constexpr u32 link_max = 65000;

enum class FeatureFullCompat : u32 {
    DirPrealloc = (1 << 0),
    ImagicInodes = (1 << 1),
    HasJournal = (1 << 2),
    ExtendedAttrs = (1 << 3),
    ResizeInode = (1 << 4),
    DirIndex = (1 << 5),
};

enum class FeatureReadOnlyCompat {
    SparseSuper = (1 << 0),
    LargeFile = (1 << 1),
};

AK_ENUM_BITWISE_OPERATORS(FeatureFullCompat);
AK_ENUM_BITWISE_OPERATORS(FeatureReadOnlyCompat);

// Special inode numbers
enum class InodeNums : u64 {
    Bad = 1,     // Bad blocks inode
    Root,        // Root inode
    ACLIndex,    // ACL inode
    ACLData,     // ACL inode
    BootLoader,  // Boot loader inode
    UndeleteDir, // Undelete directory inode
    Resize,      // Reserved group descriptors inode
    Journal,     // Journal inode

    FirstInode = 11, // First usable inode
};

// File system states
enum class FileSystemState : u32 {
    Valid,  // Cleanly unmounted.
    Error,  // Errors detected
    Orphan, // EXT3 Orphans being recovered
};

// Structure of a block group descriptor
struct GroupDescriptor {
    u32 bg_block_bitmap;      // Blocks bitmap block
    u32 bg_inode_bitmap;      // Inodes bitmap block
    u32 bg_inode_table;       // Inodes table block
    u16 bg_free_blocks_count; // Free blocks count
    u16 bg_free_inodes_count; // Free inodes count
    u16 bg_used_dirs_count;   // Directories count
    u16 bg_flags;
    u32 bg_reserved[2];
    u16 bg_itable_unused; // Unused inodes count
    u16 bg_checksum;      // crc16(volume_uuid+group_num+group_desc)
};

// Structure of an inode on the disk
struct Inode {
    u16 mode;        // File mode
    u16 owner_uid;   // Low 16 bits of Owner Uid
    u32 file_size;   // Size in bytes
    u32 access_time; // Access time
    u32 create_time; // Inode change time
    u32 modify_time; // Modification time
    u32 delete_time; // Deletion Time
    u16 group_id;    // Low 16 bits of Group Id
    u16 num_links;   // Links count
    u32 num_blocks;  // Blocks count
    u32 file_flags;  // File flags

    // This is Linux dependent
    u32 linux_version;

    u32 block_ptrs[Ext2::num_blocks]; // Pointers to blocks
    u32 file_generation;              // File version (for NFS)
    u32 file_acl;                     // File ACL
    u32 directory_acl;                // Directory ACL
    u32 fragment_addr;                // Fragment address

    // These fields below are also Linux dependent.
    u16 num_blocks_high;
    u16 file_acl_high;
    u16 owner_uid_high;
    u16 group_id_high;
    u32 reserved2;
};

// Revision levels
enum class Revision : u32 {
    Original, // The good old (original) format
    Dynamic,  // V2 format w/ dynamic inode sizes
};

// The structure of the super block
struct SuperBlock {
    u32 num_inodes;              // Inodes count
    u32 num_blocks;              // Blocks count
    u32 num_reserved_blocks;     // Reserved blocks count
    u32 num_free_blocks;         // Free blocks count
    u32 num_free_inodes;         // Free inodes count
    u32 first_data_block;        // First Data Block
    u32 block_log_size;          // Block size
    i32 fragment_log_size;       // Fragment size
    u32 num_blocks_per_group;    // # Blocks per group
    u32 num_fragments_per_group; // # Fragments per group
    u32 num_inodes_per_group;    // # Inodes per group
    u32 mount_time;              // Mount time
    u32 write_time;              // Write time
    u16 num_mounts;              // Mount count
    i16 num_max_mounts;          // Maximal mount count
    u16 magic_number;            // Magic signature
    u16 file_system_state;       // File system state
    u16 error_behavior;          // Behavior when detecting errors
    u16 minor_rev_level;         // minor revision level
    u32 last_check_time;         // time of last check
    u32 check_interval;          // max. time between checks
    u32 os_creator;              // OS
    u32 rev_level;               // Revision level
    u16 reserved_block_uid;      // Default uid for reserved blocks
    u16 reserved_block_gid;      // Default gid for reserved blocks

    // These fields are for EXT2_DYNAMIC_REV superblocks only.
    //
    // Note: the difference between the compatible feature set and
    // the incompatible feature set is that if there is a bit set
    // in the incompatible feature set that the kernel doesn't
    // know about, it should refuse to mount the filesystem.
    //
    // e2fsck's requirements are more strict; if it doesn't know
    // about a feature in either the compatible or incompatible
    // feature set, it must abort and not try to meddle with
    // things it doesn't understand...

    u32 first_inode_num;        // First non-reserved inode
    u16 inode_size_num;         // size of inode structure
    u16 num_block_group;        // block group # of this superblock
    u32 compatible_features;    // compatible feature set
    u32 incompatible_features;  // incompatible feature set
    u32 ro_compatible_features; // readonly-compatible feature set
    u8 volume_uuid[16];         // 128-bit uuid for volume
    char volume_name[16];       // volume name
    char last_mounted_dir[64];  // directory where last mounted
    u32 algorithm_usage_bitmap; // For compression

    // Performance hints.  Directory preallocation should only
    // happen if the EXT2_FEATURE_COMPAT_DIR_PREALLOC flag is on.
    u8 prealloc_blocks;      // Number of blocks to try to preallocate
    u8 prealloc_dir_blocks;  // Number to preallocate for dirs
    u16 reserved_gdt_blocks; // Per group table for online growth

    // Journaling support valid if EXT2_FEATURE_COMPAT_HAS_JOURNAL is set.
    u8 journal_sb_uuid[16];        // uuid of journal superblock
    u32 journal_inode_num;         // inode number of journal file
    u32 journal_device;            // device number of journal file
    u32 last_orphan;               // start of list of inodes to delete
    u32 htree_hash_seed[4];        // HTREE hash seed
    u8 default_hash_version;       // Default hash version to use
    u8 journal_backup_type;        // Default type of journal backup
    u16 group_descriptor_size_num; // Group desc. size if INCOMPAT_64BIT is set
    u32 default_mount_options;
    u32 first_meta_block_group;   // First metablock group
    u32 fs_create_time;           // When the filesystem was created
    u32 journal_inode_backup[17]; // Backup of the journal inode
    u32 num_blocks_high;          // Blocks count high 32bits
    u32 num_reserved_blocks_high; // Reserved blocks count high 32 bits
    u32 num_free_blocks_high;     // Free blocks count
    u16 min_extra_inode_size;     // All inodes have at least # bytes
    u16 want_extra_inode_size;    // New inodes should reserve # bytes
    u32 misc_flags;               // Miscellaneous flags
    u16 raid_stride;              // RAID stride
    u16 mmp_interval;             // # seconds to wait in MMP checking
    u64 mmp_block;                // Block for multi-mount protection
    u32 raid_stripe_width;        // blocks on all data disks (N*stride)
    u8 log_groups_per_flex;       // FLEX_BG group size
    u8 reserved_char_pad;
    u16 reserved_pad;  // Padding to next 32bits
    u32 reserved[162]; // Padding to the end of the block

    InodeIndex first_inode() const
    {
        if (rev_level == to_underlying(Revision::Original))
            return to_underlying(Ext2::InodeNums::FirstInode);
        else
            return first_inode_num;
    }
    size_t inode_size() const
    {
        if (rev_level == to_underlying(Revision::Original))
            return good_old_inode_size;
        else
            return inode_size_num;
    }
    size_t block_size() const
    {
        return (1 << min_block_log_size) << block_log_size;
    }
    size_t fragment_size() const
    {
        return (1 << min_fragment_log_size) << fragment_log_size;
    }
    // Returns the exponent of the block's size.
    size_t block_size_bits() const
    {
        return block_log_size + min_block_log_size;
    }
    size_t inodes_per_block() const
    {
        return block_size() / inode_size();
    }
    size_t fragments_per_block() const
    {
        return block_size() / fragment_size();
    }
    size_t addresses_per_block() const
    {
        return block_size() / sizeof(u32);
    }
    size_t group_descriptor_size() const
    {
        if (incompatible_features & ext4_feature_incompat_64bit)
            return group_descriptor_size_num;
        else
            return min_group_descriptor_size;
    }
    size_t group_descriptors_per_block() const
    {
        return block_size() / group_descriptor_size();
    }
};

// The new version of the directory entry.  Since EXT2 structures are
// stored in intel byte order, and the name_len field could never be
// bigger than 255 chars, it's safe to reclaim the extra byte for the
// file_type field.

struct DirEntry {
    u32 inode;   // Inode number
    u16 rec_len; // Directory entry length
    u8 name_len; // Name length
    u8 file_type;
    char name[max_name_len]; // Filename
};

// Ext2 directory file types.  Only the low 3 bits are used.  The
// other bits are reserved for now.
enum class FileType : u8 {
    Unknown,   // Unknown file type
    RegFile,   // Regular old file
    Directory, // Directory
    CharDev,   // Character device
    BlockDev,  // Block device
    FIFO,      // FIFO
    Socket,    // Socket
    SymLink,   // Symbolic link
    Max        // The maximum file type plus one.
};

// This pads a dir entry name's length to be a multiple of 4
constexpr size_t dir_round = (4 - 1);
static inline size_t pad_directory_name_length(size_t length)
{
    return (length + 8 + dir_round) & ~dir_round;
}

}