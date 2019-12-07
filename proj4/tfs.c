/*
 *  Copyright (C) 2019 CS416 Spring 2019
 *	
 *	Tiny File System
 *
 *	File:	tfs.c
 *  Author: Yujie REN
 *	Date:	April 2019
 *
 */

#define FUSE_USE_VERSION 26

#include <fuse.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <sys/time.h>
#include <libgen.h>
#include <limits.h>

#include "block.h"
#include "tfs.h"

char diskfile_path[PATH_MAX];

// Declare your in-memory data structures here
struct suberblock sb;
//struct inode in;
//struct dirent dir;

/* 
 * Get available inode number from bitmap
 */
int get_avail_ino() {

	// Step 1: Read inode bitmap from disk
	unsigned char* buf=malloc(sizeof(unsigned char)*BLOCK_SIZE);
	bio_read(1,buf);
	
	// Step 2: Traverse inode bitmap to find an available slot
	int i=0;
	while(get_bitmap((bitmap_t)buf, i)){
		i++;
	}
	
	// Step 3: Update inode bitmap and write to disk 
	set_bitmap((bitmap_t)buf,i);
	bio_write(1,buf);
	return i;
	
	//return 0;
}

/* 
 * Get available data block number from bitmap
 */
int get_avail_blkno() {

	// Step 1: Read data block bitmap from disk
	unsigned char* buf=malloc(sizeof(unsigned char)*BLOCK_SIZE);
	bio_read(2,buf);
	
	// Step 2: Traverse data block bitmap to find an available slot
	int i=0;
	while(get_bitmap((bitmap_t)buf, i)){
		i++;
	}

	// Step 3: Update data block bitmap and write to disk 
	set_bitmap((bitmap_t)buf,i);
	bio_write(2,buf);
	return i+67;

	//return 0;
}

/* 
 * inode operations
 */
int readi(uint16_t ino, struct inode *inode) {

  // Step 1: Get the inode's on-disk block number
  int blockNum = ino/16+3;

  // Step 2: Get offset of the inode in the inode on-disk block
  int offset = ino%16;

  // Step 3: Read the block from disk and then copy into inode structure
  //char* buf=malloc(sizeof(char)*BLOCK_SIZE);
  //bio_read(blockNum, buf);
  struct inode *temp = malloc(sizeof(struct inode));
  bio_read(blockNum, temp);
  mempy(inode, &temp[offset], sizeof(struct inode));
	
	return 0;
}

int writei(uint16_t ino, struct inode *inode) {

	// Step 1: Get the block number where this inode resides on disk
	int blockNum = ino/16+3;
	
	// Step 2: Get the offset in the block where this inode resides on disk
	int offset = ino%16;

	// Step 3: Write inode to disk 
	struct inode *temp = malloc(sizeof(struct inode));
	bio_read(blockNum, temp);
	mempy(inode, &temp[offset], sizeof(struct inode));
	bio_write(blockNum, temp);

	return 0;
}


/* 
 * directory operations
 */
int dir_find(uint16_t ino, const char *fname, size_t name_len, struct dirent *dirent) {

  // Step 1: Call readi() to get the inode using ino (inode number of current directory)
  struct inode *inode = malloc(sizeof(struct inode));
  readi(ino, inode);

  // Step 2: Get data block of current directory from inode
  int block=0, dirc=0;
  struct dirent* dirn = malloc(sizeof(struct dirent));
  for(block=0; block<16; block++){
  	if(inode->direct_ptr[block]!=0){
  // Step 3: Read directory's data block and check each directory entry.
  		bio_read(inode->direct_ptr[block], dirn);
  //If the name matches, then copy directory entry to dirent structure
  		for(dirc=0; dirc<16; dirc++){
  			if(dern[dirc].valid==1){
  				if(strcmp(fname, dirn[dirc].name)==0){
  					memcpy(dirent, &dirn[dirc], sizeof(struct dirent));
  					return inode->direct_ptr[block]
  				}
  			}
  		}
  	}
  }
  
	return 0;
	//return -1; ?
}

int dir_add(struct inode dir_inode, uint16_t f_ino, const char *fname, size_t name_len) {

	// Step 1: Read dir_inode's data block and check each directory entry of dir_inode
	int block, direc;
	struct dirent* diren;
	diren = malloc(BLOCK_SIZE);
	struct dirent* dirn;
	dirn = malloc(sizeof(struct dirent));
	
	// Step 2: Check if fname (directory name) is already used in other entries
	if (dir_find(dir_inode.ino, fname, name_len, dirn) != 0) {
		// add failed
		return 0;
	} else {
		dirn->ino = f_ino;
		dirn->valid = 1;
		strncpy(dirn->name, fname, sizeof(dirn->name));
		dirn->name[name_len] = '\0';
		for (block = 0; block < 16; block++) {
		
	// Step 3: Add directory entry in dir_inode's data block and write to disk
			if (dir_inode.direct_ptr[block] == 0) {
				dir_inode.direct_ptr[block] = get_avail_blkno();
				writei(dir_inode.ino, &dir_inode);
				bio_write(dir_inode.direct_ptr[block], dirn);
				//add success
				return 1;
				
	// Allocate a new data block for this directory if it does not exist
			} else {
				bio_read(dir_inode.direct_ptr[block], diren);
				for (direc = 0; direc < 16; direc++) {
					if (diren[direc].valid == 0) {
						memcpy(&diren[direc], dirn, sizeof(struct dirent));
						bio_write(dir_inode.direct_ptr[block], diren);
						dir_inode.link++;
						writei(dir_inode.ino, &dir_inode);
						//add success
						return 1;
					}
				}
			}
		}
	}
	// Update directory inode

	// Write directory entry
	
	//add failed
	return 0;
}

int dir_remove(struct inode dir_inode, const char *fname, size_t name_len) {

	// Step 1: Read dir_inode's data block and checks each directory entry of dir_inode
	int block, direc;
	struct dirent* diren;
	struct dirent* dirn;
	dirn = malloc(sizeof(struct dirent));
	
	// Step 2: Check if fname exist
	if ((block = dir_find(dir_inode.ino, fname, name_len, dirn)) != 0) {
		diren = malloc(BLOCK_SIZE);
		bio_read(block, diren);
		for (direc = 0; direc < 16; direc++) {
			if (diren[direc].valid == 1) {
			
	// Step 3: If exist, then remove it from dir_inode's data block and write to disk
				if (strcmp(diren[direc].name, fname) == 0) {
					diren[direc].valid = 0;
					bio_write(block, diren);
					//remove success
					free(dirn);
					free(diren);
					return 1;
				}
			}
		}
	}
	//remove failed
	free(dirn);
	return 0;
}

/* 
 * namei operation
 */
int get_node_by_path(const char *path, uint16_t ino, struct inode *inode) {
	
	// Step 1: Resolve the path name, walk through path, and finally, find its inode.
	// Note: You could either implement it in a iterative way or recursive way
	char* root = strdup(path);
	char* nxt;
	struct dirent *temp = malloc(sizeof(struct dirent));
	root = strtok_r(root, "/", &nxt);
	if (root == NULL) {
		readi(ino, inode);
		return 0;
	}
	if (dir_find(ino, root, strlen(root), temp) == 0) {
		return get_node_by_path(nxt, temp->ino, inode);
	}
	else {
		return -1;
	}
	return 0; // should never reach this
}

/* 
 * Make file system
 */
int tfs_mkfs() {

	// Call dev_init() to initialize (Create) Diskfile
	dev_init(diskfile_path);

	// write superblock information
	sb->magic_num = MAGIC_NUM;
	sb->max_inum = MAX_INUM;
	sb->max_dnum = MAX_DNUM;
	
	char *buff= malloc(BLOCK_SIZE*sizeof(char));
	memcpy(buff, &sb, sizeof(sb));
	bio_write(0, buff);

	// initialize inode bitmap
	unsigned char inoMap = malloc (sizeof(unsigned char)*MAX_INUM / 8);
	memset(inoMap, 0, MAX_INUM / 8);
	sb->i_bitmap_blk = 1;
	sb->i_start_blk = 3;

	// initialize data block bitmap
	unsigned char dataMap = malloc (sizeof(unsigned char)*MAX_DNUM / 8);
	memset(dataMap, 0, MAX_DNUM / 8);
	bio_write(2, dataMap);
	sb->d_bitmap_blk = 2;
	sb->d_start_blk = 67;

	// update bitmap information for root directory
	set_bitmap((bitmap_t)inoMap, 0);
	bio_write(1, inoMap);

	// update inode for root directory
	struct inode root;
	root.ino = 0;
	root.valid = 1;
	root.size = 0;
	root.type = 1;
	root.link = 2;
	memset(root.direct_ptr, 0, 16*sizeof(int));
	memset(root.indirect_ptr, 0, 8*sizeof(int));

	root.vstat.st_mtime = time(0);
	root.vstat.st_atime = time(0);
	writei(root.ino, &root);

	return 0;
}


/* 
 * FUSE file operations
 */
static void *tfs_init(struct fuse_conn_info *conn) {

	// Step 1a: If disk file is not found, call mkfs
	if (access(diskfile_path, F_OK) == -1){
		tfs_mkfs();
	}

  // Step 1b: If disk file is found, just initialize in-memory data structures
  // and read superblock from disk
  else{
  	dev_open(diskfile_path);
	char* buff = malloc (sizeof(char)*BLOCK_SIZE);
	bio_read(0, buff); //(void*)
	memcpy(&sb, buff, sizeof(sb));
	if (sb.magic_num != MAGIC_NUM){
		printf("magic_num does not match\n");
		exit(EXIT_FAILURE);
	}
  }
	return NULL;
}

static void tfs_destroy(void *userdata) {

	// Step 1: De-allocate in-memory data structures

	// Step 2: Close diskfile
	dev_close();

}

static int tfs_getattr(const char *path, struct stat *stbuf) {

	// Step 1: call get_node_by_path() to get inode from path

	// Step 2: fill attribute of file into stbuf from inode

		stbuf->st_mode   = S_IFDIR | 0755;
		stbuf->st_nlink  = 2;
		time(&stbuf->st_mtime);

	return 0;
}

static int tfs_opendir(const char *path, struct fuse_file_info *fi) {

	// Step 1: Call get_node_by_path() to get inode from path
	struct inode* ino= malloc(sizeof(struct inode));
	if (get_node_by_path(path, 0, ino) == 0) {
		//free(ino);
		return 0;
	}

	// Step 2: If not find, return -1
	else{
		return -1;
	}

    return 0;
}

static int tfs_readdir(const char *path, void *buffer, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi) {

	// Step 1: Call get_node_by_path() to get inode from path

	// Step 2: Read directory entries from its data blocks, and copy them to filler

	return 0;
}


static int tfs_mkdir(const char *path, mode_t mode) {

	// Step 1: Use dirname() and basename() to separate parent directory path and target directory name

	// Step 2: Call get_node_by_path() to get inode of parent directory

	// Step 3: Call get_avail_ino() to get an available inode number

	// Step 4: Call dir_add() to add directory entry of target directory to parent directory

	// Step 5: Update inode for target directory

	// Step 6: Call writei() to write inode to disk
	

	return 0;
}

static int tfs_rmdir(const char *path) {

	// Step 1: Use dirname() and basename() to separate parent directory path and target directory name

	// Step 2: Call get_node_by_path() to get inode of target directory

	// Step 3: Clear data block bitmap of target directory

	// Step 4: Clear inode bitmap and its data block

	// Step 5: Call get_node_by_path() to get inode of parent directory

	// Step 6: Call dir_remove() to remove directory entry of target directory in its parent directory

	return 0;
}

static int tfs_releasedir(const char *path, struct fuse_file_info *fi) {
	// For this project, you don't need to fill this function
	// But DO NOT DELETE IT!
    return 0;
}

static int tfs_create(const char *path, mode_t mode, struct fuse_file_info *fi) {

	// Step 1: Use dirname() and basename() to separate parent directory path and target file name

	// Step 2: Call get_node_by_path() to get inode of parent directory

	// Step 3: Call get_avail_ino() to get an available inode number

	// Step 4: Call dir_add() to add directory entry of target file to parent directory

	// Step 5: Update inode for target file

	// Step 6: Call writei() to write inode to disk

	return 0;
}

static int tfs_open(const char *path, struct fuse_file_info *fi) {

	// Step 1: Call get_node_by_path() to get inode from path
	struct inode *ino = malloc(sizeof(struct inode));
	if(get_node_by_path(path, 0, ino) == 0 ) {
		//free(ino);
		return 0;
	}
	// Step 2: If not find, return -1
	else{
		return -1;
	}

	return 0;
}

static int tfs_read(const char *path, char *buffer, size_t size, off_t offset, struct fuse_file_info *fi) {

	// Step 1: You could call get_node_by_path() to get inode from path

	// Step 2: Based on size and offset, read its data blocks from disk

	// Step 3: copy the correct amount of data from offset to buffer

	// Note: this function should return the amount of bytes you copied to buffer
	return 0;
}

static int tfs_write(const char *path, const char *buffer, size_t size, off_t offset, struct fuse_file_info *fi) {
	// Step 1: You could call get_node_by_path() to get inode from path

	// Step 2: Based on size and offset, read its data blocks from disk

	// Step 3: Write the correct amount of data from offset to disk

	// Step 4: Update the inode info and write it to disk

	// Note: this function should return the amount of bytes you write to disk
	return size;
}

static int tfs_unlink(const char *path) {

	// Step 1: Use dirname() and basename() to separate parent directory path and target file name

	// Step 2: Call get_node_by_path() to get inode of target file

	// Step 3: Clear data block bitmap of target file

	// Step 4: Clear inode bitmap and its data block

	// Step 5: Call get_node_by_path() to get inode of parent directory

	// Step 6: Call dir_remove() to remove directory entry of target file in its parent directory

	return 0;
}

static int tfs_truncate(const char *path, off_t size) {
	// For this project, you don't need to fill this function
	// But DO NOT DELETE IT!
    return 0;
}

static int tfs_release(const char *path, struct fuse_file_info *fi) {
	// For this project, you don't need to fill this function
	// But DO NOT DELETE IT!
	return 0;
}

static int tfs_flush(const char * path, struct fuse_file_info * fi) {
	// For this project, you don't need to fill this function
	// But DO NOT DELETE IT!
    return 0;
}

static int tfs_utimens(const char *path, const struct timespec tv[2]) {
	// For this project, you don't need to fill this function
	// But DO NOT DELETE IT!
    return 0;
}


static struct fuse_operations tfs_ope = {
	.init		= tfs_init,
	.destroy	= tfs_destroy,

	.getattr	= tfs_getattr,
	.readdir	= tfs_readdir,
	.opendir	= tfs_opendir,
	.releasedir	= tfs_releasedir,
	.mkdir		= tfs_mkdir,
	.rmdir		= tfs_rmdir,

	.create		= tfs_create,
	.open		= tfs_open,
	.read 		= tfs_read,
	.write		= tfs_write,
	.unlink		= tfs_unlink,

	.truncate   = tfs_truncate,
	.flush      = tfs_flush,
	.utimens    = tfs_utimens,
	.release	= tfs_release
};


int main(int argc, char *argv[]) {
	int fuse_stat;

	getcwd(diskfile_path, PATH_MAX);
	strcat(diskfile_path, "/DISKFILE");

	fuse_stat = fuse_main(argc, argv, &tfs_ope, NULL);

	return fuse_stat;
}