#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>

#include "xv6fs.h"

static char *fs_img = NULL;  // Pointer to mmap'd filesystem image
static struct superblock sb; // Superblock data

// Read block from image into buffer
void read_block(uint32_t blocknum, void *buf) {
    memcpy(buf, fs_img + blocknum * BSIZE, BSIZE);
}

// Read superblock from block 1
void read_superblock(void) {
    memcpy(&sb, fs_img + BSIZE, sizeof(struct superblock));
}

// Read inode by number
void read_inode(uint32_t inum, struct dinode *dip) {
    uint32_t block = IBLOCK(inum, sb);   // Block containing inode
    uint32_t offset = IOFFSET(inum);     // Offset within block
    memcpy(dip, fs_img + block * BSIZE + offset, sizeof(struct dinode));
}

// Get physical block address for a logical block in file
// Handles direct blocks (0-11) and indirect blocks (12+)
uint32_t get_block_addr(struct dinode *dip, uint32_t logical_block) {
    if (logical_block < NDIRECT) {
        // Direct block
        return dip->addrs[logical_block];
    } else {
        // Indirect block: addrs[12] points to a block containing more addresses
        uint32_t indirect_block = dip->addrs[NDIRECT];
        if (indirect_block == 0) {
            return 0;
        }
        // Read the indirect block to get the actual data block address
        uint32_t indirect_addrs[NINDIRECT];
        read_block(indirect_block, indirect_addrs);
        return indirect_addrs[logical_block - NDIRECT];
    }
}

// Search for file in root directory, returns 1 if found, 0 if not
int find_file(const char *filename, struct dinode *found_inode) {
    struct dinode root_inode;
    struct dirent dir_entry;
    
    read_inode(ROOTINO, &root_inode);
    uint32_t num_blocks = (root_inode.size + BSIZE - 1) / BSIZE;
    
    // Go through each directory block
    for (uint32_t block_idx = 0; block_idx < num_blocks; block_idx++) {
        uint32_t block_addr = get_block_addr(&root_inode, block_idx);
        if (block_addr == 0) continue;
        
        char block_data[BSIZE];
        read_block(block_addr, block_data);
        
        // Check each directory entry in block
        for (uint32_t offset = 0; offset < BSIZE; offset += sizeof(struct dirent)) {
            uint32_t total_offset = block_idx * BSIZE + offset;
            if (total_offset >= root_inode.size) break;
            
            memcpy(&dir_entry, block_data + offset, sizeof(struct dirent));
            if (dir_entry.inum == 0) continue;  // inum 0 means unused entry
            
            if (strncmp(dir_entry.name, filename, DIRSIZ) == 0) {
                read_inode(dir_entry.inum, found_inode);
                return 1;
            }
        }
    }
    return 0;
}

// List root directory contents: name type inode size
void cmd_ls(void) {
    struct dinode root_inode;
    struct dirent dir_entry;
    
    read_inode(ROOTINO, &root_inode);
    uint32_t num_blocks = (root_inode.size + BSIZE - 1) / BSIZE;
    
    for (uint32_t block_idx = 0; block_idx < num_blocks; block_idx++) {
        uint32_t block_addr = get_block_addr(&root_inode, block_idx);
        if (block_addr == 0) continue;
        
        char block_data[BSIZE];
        read_block(block_addr, block_data);
        
        for (uint32_t offset = 0; offset < BSIZE; offset += sizeof(struct dirent)) {
            uint32_t total_offset = block_idx * BSIZE + offset;
            if (total_offset >= root_inode.size) break;
            
            memcpy(&dir_entry, block_data + offset, sizeof(struct dirent));
            if (dir_entry.inum == 0) continue;
            
            struct dinode entry_inode;
            read_inode(dir_entry.inum, &entry_inode);
            
            printf("%s %d %d %d\n", dir_entry.name, entry_inode.type,
                   dir_entry.inum, entry_inode.size);
        }
    }
}

// Copy file from xv6 image to local filesystem
int cmd_cp(const char *xv6file, const char *linuxfile) {
    struct dinode file_inode;
    
    if (!find_file(xv6file, &file_inode)) {
        printf("File %s does not exist in the root directory\n", xv6file);
        return 1;
    }
    
    int out_fd = open(linuxfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (out_fd < 0) {
        perror("Failed to open output file");
        return 1;
    }
    
    uint32_t num_blocks = (file_inode.size + BSIZE - 1) / BSIZE;
    uint32_t remaining = file_inode.size;
    
    // Copy block by block, last block may be partial
    for (uint32_t block_idx = 0; block_idx < num_blocks; block_idx++) {
        uint32_t block_addr = get_block_addr(&file_inode, block_idx);
        uint32_t bytes_to_write = (remaining > BSIZE) ? BSIZE : remaining;
        
        if (block_addr == 0) {
            // Sparse file - write zeros
            char zeros[BSIZE] = {0};
            if (write(out_fd, zeros, bytes_to_write) < 0) {
                perror("Failed to write to output file");
                close(out_fd);
                return 1;
            }
        } else {
            char block_data[BSIZE];
            read_block(block_addr, block_data);
            if (write(out_fd, block_data, bytes_to_write) < 0) {
                perror("Failed to write to output file");
                close(out_fd);
                return 1;
            }
        }
        remaining -= bytes_to_write;
    }
    
    close(out_fd);
    return 0;
}

void print_usage(const char *prog_name) {
    fprintf(stderr, "Usage:\n");
    fprintf(stderr, "  %s <fs.img> ls\n", prog_name);
    fprintf(stderr, "  %s <fs.img> cp <xv6file> <linuxfile>\n", prog_name);
}

int main(int argc, char *argv[]) {
    // Check minimum number of arguments
    if (argc < 3) {
        print_usage(argv[0]);
        return 1;
    }
    
    const char *img_path = argv[1];
    const char *command = argv[2];
    
    // Open and mmap the filesystem image
    int img_fd = open(img_path, O_RDONLY);
    if (img_fd < 0) {
        perror("Failed to open filesystem image");
        return 1;
    }
    
    // Get the file size for mmap
    struct stat img_stat;
    if (fstat(img_fd, &img_stat) < 0) {
        perror("Failed to stat filesystem image");
        close(img_fd);
        return 1;
    }
    
    // mmap lets us access the image as an in-memory array
    fs_img = mmap(NULL, img_stat.st_size, PROT_READ, MAP_PRIVATE, img_fd, 0);
    if (fs_img == MAP_FAILED) {
        perror("Failed to mmap filesystem image");
        close(img_fd);
        return 1;
    }
    close(img_fd);  // fd can be closed after mmap
    
    read_superblock();
    
    int result = 0;
    
    // Handle command
    if (strcmp(command, "ls") == 0) {
        cmd_ls();
    } else if (strcmp(command, "cp") == 0) {
        if (argc < 5) {
            fprintf(stderr, "Error: cp requires source and destination files\n");
            print_usage(argv[0]);
            result = 1;
        } else {
            result = cmd_cp(argv[3], argv[4]);
        }
    } else {
        fprintf(stderr, "Unknown command: %s\n", command);
        print_usage(argv[0]);
        result = 1;
    }
    
    munmap(fs_img, img_stat.st_size);
    return result;
}
