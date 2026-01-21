// xv6fs.h - xv6 filesystem structures and constants

#ifndef XV6FS_H
#define XV6FS_H

#include <stdint.h>


#define BSIZE 512 // Block size in bytes
#define NDIRECT 12 // Number of direct block pointers in inode
#define NINDIRECT (BSIZE / sizeof(uint32_t)) // Number of addresses in indirect block
#define MAXFILE (NDIRECT + NINDIRECT) // Maximum number of blocks per file
#define ROOTINO 1 // Root inode number
#define DIRSIZ 14 // Maximum filename length

// Inode types
#define T_DIR  1   // Directory
#define T_FILE 2   // File
#define T_DEV  3   // Device

// Superblock - at block 1 (block 0 is boot block)
struct superblock {
    uint32_t size;         // Total size in blocks
    uint32_t nblocks;      // Number of data blocks
    uint32_t ninodes;      // Number of inodes
    uint32_t nlog;         // Number of log blocks
    uint32_t logstart;     // First log block
    uint32_t inodestart;   // First inode block
    uint32_t bmapstart;    // First bitmap block
};

// Disk inode (64 bytes, 8 per block)
struct dinode {
    int16_t type;           // File type (0 = free)
    int16_t major;          // Major device number
    int16_t minor;          // Minor device number
    int16_t nlink;          // Number of links
    uint32_t size;          // File size in bytes
    uint32_t addrs[NDIRECT + 1]; // 12 direct + 1 indirect
};

// Directory entry (16 bytes)
struct dirent {
    uint16_t inum;          // Inode number (0 = unused)
    char name[DIRSIZ];      // Filename
};

#define IPB (BSIZE / sizeof(struct dinode)) // Inodes per block
#define IBLOCK(i, sb) ((sb).inodestart + (i) / IPB) // Block number of inode i
#define IOFFSET(i) (((i) % IPB) * sizeof(struct dinode)) // Offset of inode i within its block

#endif
