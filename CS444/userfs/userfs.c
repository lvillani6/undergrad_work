#include "crash.h"
#include "parse.h"
#include "userfs.h"

/* GLOBAL  VARIABLES */
int virtual_disk;
/*Use these variable as a staging space before they get writen out to disk*/
superblock sb;
BIT bit_map[BIT_MAP_SIZE];
dir_struct dir;
inode curr_inode;
char buffer[BLOCK_SIZE_BYTES]; /* assert( sizeof(char) ==1)); */

/*
  man 2 read
  man stat
  man memcopy
*/

void usage (char * command) {
	fprintf(stderr, "Usage: %s -reformat disk_size_bytes file_name\n", command);
	fprintf(stderr, "        %s file_name\n", command);
}

char * buildPrompt() {
	return  "%";
}


int main(int argc, char** argv) {
	char * cmd_line;
	/* info stores all the information returned by parser */
	parseInfo *info;
	/* stores cmd name and arg list for one command */
	struct commandType *cmd;

	init_crasher();

	if ((argc == 4) && (argv[1][1] == 'r')) {
		/* ./userfs -reformat diskSize fileName */
		if (!u_format(atoi(argv[2]), argv[3])) {
			fprintf(stderr, "Unable to reformat\n");
			exit(-1);
		}
	} else if (argc == 2) {
		/* ./userfs fileName will attempt to recover a file. */
		if ((!recover_file_system(argv[1]))) {
			fprintf(stderr, "Unable to recover virtual file system from file: %s\n", argv[1]);
			exit(-1);
		}
	} else {
		usage(argv[0]);
		exit(-1);
	}

	/* before begin processing set clean_shutdown to FALSE */
	sb.clean_shutdown = 0;
	lseek(virtual_disk, BLOCK_SIZE_BYTES*SUPERBLOCK_BLOCK, SEEK_SET);
	crash_write(virtual_disk, &sb, sizeof(superblock));
	sync();
	fprintf(stderr, "userfs available\n");

	while(1) {
		cmd_line = readline(buildPrompt());
		if (cmd_line == NULL) {
			fprintf(stderr, "Unable to read command\n");
			continue;
		}

		/* calls the parser */
		info = parse(cmd_line);
		if (info == NULL) {
			free(cmd_line);
			continue;
		}

		/* com contains the info. of command before the first "|" */
		cmd = &info->CommArray[0];
		if ((cmd == NULL) || (cmd->command == NULL)) {
			free_info(info);
			free(cmd_line);
			continue;
		}

		/************************   u_import ****************************/
		if (strncmp(cmd->command, "u_import", strlen("u_import")) == 0){
			if (cmd->VarNum != 3){
				fprintf(stderr, "u_import externalFileName userfsFileName\n");
			} else {
				if (!u_import(cmd->VarList[1], cmd->VarList[2])) {
					fprintf(stderr, "Unable to import external file %s into userfs file %s\n", cmd->VarList[1], cmd->VarList[2]);
				}
			}


		/************************   u_export ****************************/
		} else if (strncmp(cmd->command, "u_export", strlen("u_export")) == 0){
			if (cmd->VarNum != 3){
				fprintf(stderr, "u_export userfsFileName externalFileName \n");
			} else {
				if (!u_export(cmd->VarList[1], cmd->VarList[2])) {
					fprintf(stderr, "Unable to export userfs file %s to external file %s\n", cmd->VarList[1], cmd->VarList[2]);
				}
			}


		/************************   u_del ****************************/
		} else if (strncmp(cmd->command, "u_del", strlen("u_export")) == 0) {
			if (cmd->VarNum != 2){
				fprintf(stderr, "u_del userfsFileName \n");
			} else {
				if (!u_del(cmd->VarList[1]) ){
					fprintf(stderr, "Unable to delete userfs file %s\n", cmd->VarList[1]);
				}
			}


		/******************** u_ls **********************/
		} else if (strncmp(cmd->command, "u_ls", strlen("u_ls")) == 0) {
			u_ls();


		/********************* u_quota *****************/
		} else if (strncmp(cmd->command, "u_quota", strlen("u_quota")) == 0) {
			int free_blocks = u_quota();
			fprintf(stderr, "Free space: %d bytes %d blocks\n", free_blocks * BLOCK_SIZE_BYTES, free_blocks);


		/***************** exit ************************/
		} else if (strncmp(cmd->command, "exit", strlen("exit")) == 0) {
			/*
			 * take care of clean shut down so that u_fs
			 * recovers when started next.
			 */
			if (!u_clean_shutdown()){
				fprintf(stderr, "Shutdown failure, possible corruption of userfs\n");
			}
			exit(1);


		/****************** other ***********************/
		} else {
			fprintf(stderr, "Unknown command: %s\n", cmd->command);
			fprintf(stderr, "\tTry: u_import, u_export, u_ls, u_del, u_quota, exit\n");
		}


		free_info(info);
		free(cmd_line);
	}
}

/*
 * Initializes the bit map.
 */
void init_bit_map() {
	for (int i = 0; i < BIT_MAP_SIZE; i++) {
		bit_map[i] = 0;
	}
}

void allocate_block(int blockNum) {
	assert(blockNum < BIT_MAP_SIZE);
	bit_map[blockNum] = 1;
	sb.num_free_blocks--;
}

void free_block(int blockNum) {
	assert(blockNum < BIT_MAP_SIZE);
	bit_map[blockNum] = 0;
	sb.num_free_blocks++;
}

int superblockMatchesCode() {
	if (sb.size_of_super_block != sizeof(superblock)){
		return 0;
	}
	if (sb.size_of_directory != sizeof (dir_struct)){
		return 0;
	}
	if (sb.size_of_inode != sizeof(inode)){
		return 0;
	}
	if (sb.block_size_bytes != BLOCK_SIZE_BYTES){
		return 0;
	}
	if (sb.max_file_name_size != MAX_FILE_NAME_SIZE){
		return 0;
	}
	if (sb.max_blocks_per_file != MAX_BLOCKS_PER_FILE){
		return 0;
	}
	return 1;
}

void init_superblock(int diskSizeBytes) {
	sb.disk_size_blocks  = diskSizeBytes/BLOCK_SIZE_BYTES;
	sb.num_free_blocks = u_quota();
	sb.clean_shutdown = 1;

	sb.size_of_super_block = sizeof(superblock);
	sb.size_of_directory = sizeof (dir_struct);
	sb.size_of_inode = sizeof(inode);

	sb.block_size_bytes = BLOCK_SIZE_BYTES;
	sb.max_file_name_size = MAX_FILE_NAME_SIZE;
	sb.max_blocks_per_file = MAX_BLOCKS_PER_FILE;
}

int compute_inode_loc(int inode_number) {
	int whichInodeBlock;
	int whichInodeInBlock;
	int inodeLocation;

	whichInodeBlock = inode_number/INODES_PER_BLOCK;
	whichInodeInBlock = inode_number%INODES_PER_BLOCK;

	inodeLocation = (INODE_BLOCK + whichInodeBlock) *BLOCK_SIZE_BYTES +
		whichInodeInBlock*sizeof(inode);

	return inodeLocation;
}

int write_inode(int inode_number, inode * in) {
	int inodeLocation;
	assert(inode_number < MAX_INODES);

	inodeLocation = compute_inode_loc(inode_number);

	lseek(virtual_disk, inodeLocation, SEEK_SET);
	crash_write(virtual_disk, in, sizeof(inode));

	sync();

	return 1;
}


int read_inode(int inode_number, inode * in) {
	int inodeLocation;
	assert(inode_number < MAX_INODES);

	inodeLocation = compute_inode_loc(inode_number);

	lseek(virtual_disk, inodeLocation, SEEK_SET);
	read(virtual_disk, in, sizeof(inode));

	return 1;
}


/*
 * Initializes the directory.
 */
void init_dir() {
	for (int i = 0; i< MAX_FILES_PER_DIRECTORY; i++) {
		dir.u_file[i].free = 1;
	}

}


/*
 * Returns the number of free blocks in the file system.
 */
int u_quota() {
	int freeCount=0;

	/* It might be advantageous to return sb.num_free_blocks if you keep it up to
	   date, which is up to you to do. Do keep in mind when that number might be
	   invalid and thus requires the following. */

	// Calculate the number of free blocks
	for (int i = 0; i < sb.disk_size_blocks; i++) {
		/* Right now we are using a full unsigned int
		   to represent each bit - we really should use
		   all the bits in there for more efficient storage */
		if (bit_map[i] == 0) {
			freeCount++;
		}
	}
	return freeCount;
}

/*
 * Imports a linux file into the u_fs
 * Need to take care in the order of modifying the data structures
 * so that it can be revored consistently.
 *
 * Returns 1 on success, 0 on failure.
 *
 * TODO: Implement the rest of this function.
 */
int u_import(char* linux_file, char* u_file) {
	int free_space;
	free_space = u_quota();

	/* Ensure correct userfs file name length. */
	if(strlen(u_file) > 15)
	{
		printf("error, file name %s is too long\n", u_file);
		return 0;
	}

	for(int i = 0; i < MAX_FILES_PER_DIRECTORY; i++)
	{
		if(strncmp(dir.u_file[i].file_name, u_file, strlen(u_file)) == 0)
		{
			printf("error, %s already exists\n", u_file);
			return 0;
		}
	}

	/* Open file in linux file system. */
	int handle = open(linux_file, O_RDONLY);
	if (handle == -1) {
		printf("error, reading file %s\n", linux_file);
		return 0;
	}

	/* Get info about linux file: size and time last modified. */
	struct stat file_stat;
	stat(linux_file, &file_stat);

	/* Ensures requisite space on disk. */
	int bytes_needed = file_stat.st_size;
	if(bytes_needed > free_space*BLOCK_SIZE_BYTES)
	{
		printf("error, file too large, not enough space on disk\n");
		return 0;
	}

	/* Finds a free inode */
	int inode_num = find_free_inode();
	if(inode_num == -1)
	{
		printf("error, no free inodes\n" );
		return 0;
	}

	/* Sets clean shutdown to false, saves to userfs disk. */
	sb.clean_shutdown = FALSE;
	lseek(virtual_disk, BLOCK_SIZE_BYTES*SUPERBLOCK_BLOCK, SEEK_SET);
	crash_write(virtual_disk, &sb, sizeof(superblock));
	sync();


	/* Writes file contents to userfs disk */
	int block_index = 0;
	int num_bytes_read;
	int block_num;
	while(TRUE)
	{
		num_bytes_read = crash_read(handle, &buffer, BLOCK_SIZE_BYTES);
		if(num_bytes_read == 0)
			break;
		block_num = find_free_block();
		curr_inode.blocks[block_index++] = block_num*BLOCK_SIZE_BYTES;
		allocate_block(block_num);

		lseek(virtual_disk, BLOCK_SIZE_BYTES*block_num, SEEK_SET);
		crash_write(virtual_disk, &buffer, num_bytes_read);

	}
	/* Updates bitmap in userfs disk. */
	lseek(virtual_disk, BLOCK_SIZE_BYTES*BIT_MAP_BLOCK, SEEK_SET);
	crash_write(virtual_disk, bit_map, sizeof(BIT)*BIT_MAP_SIZE);
	sync();
	close(handle);

	/* Sets inode information: free = FALSE, file_size_bytes, num_blocks, last_modified. */
	curr_inode.free = FALSE;
	curr_inode.file_size_bytes = bytes_needed;
	curr_inode.num_blocks = block_index;
	curr_inode.last_modified = file_stat.st_mtime;

	/* Updates inode in userfs disk. */
	write_inode(inode_num, &curr_inode);

	/* Finds a free fle space in the directory. */
	int file_num = find_free_file();

	/* Sets directory information: inode_number, file_name, free = FALSE, increment number of files. */
	dir.u_file[file_num].inode_number = inode_num;
	strncpy(dir.u_file[file_num].file_name, u_file, strlen(u_file));
	dir.u_file[file_num].free = FALSE;
	dir.num_files++;

	/* Updates directory in userfs disk. */
	lseek(virtual_disk, BLOCK_SIZE_BYTES*DIRECTORY_BLOCK, SEEK_SET);
	crash_write(virtual_disk, &dir, sizeof(dir_struct));
	sync();

	/* Sets clean shutdown to true, saves to userfs disk. */
	sb.clean_shutdown = TRUE;
	lseek(virtual_disk, BLOCK_SIZE_BYTES*SUPERBLOCK_BLOCK, SEEK_SET);
	crash_write(virtual_disk, &sb, sizeof(superblock));
	sync();

	return 1;
}
/* Helper functions for u_import */

/* Finds the first free inode, returns -1 if no free inodes found. */
int find_free_inode()
{
	inode temp;
	for(int i = 0; i < MAX_INODES; i++)
	{
		read_inode(i, &temp);
		if(temp.free)
			return i;
	}
	return -1;
}

/* Finds the first free block, returns -1 if no free blocks found.
 * Starts at 3 + NUM_INODE_BLOCKS because the first 3 + NUM_INODE_BLOCKS are taken by:
 * SUPERBLOCK, BIT_MAP, DIRECTORY, and INODES. */
int find_free_block()
{
	int block_start = 3 + NUM_INODE_BLOCKS;
	for(int i = block_start; i < sb.disk_size_blocks; i++)
		if(bit_map[i]==0)
			return i;
	return -1;
}

/* Finds the first free file, returns -1 if no free files found. */
int find_free_file()
{
	dir_struct temp;
	lseek(virtual_disk, BLOCK_SIZE_BYTES*DIRECTORY_BLOCK, SEEK_SET);
	crash_read(virtual_disk, &temp, sizeof(dir_struct));
	for(int i = 0; i<MAX_FILES_PER_DIRECTORY; i++)
		if(temp.u_file[i].free)
			return i;
	return -1;
}

/*
 * Exports a u_file to linux.
 * Need to take care in the order of modifying the data structures
 * so that it can be recovered consistently.
 *
 * Return 1 on success, 0 on failure.
 *
 * TODO: Implement this function.
 */
int u_export(char* u_file, char* linux_file) {

	/* Check if the file already exists in the linux file system. */
	int handle = open(linux_file, O_RDONLY);
	if (handle != -1) {
		printf("file: %s already exists\n", linux_file);
		return 0;
	}
	close(handle);

	/* Search the user file system for the file and seek to it if found. */
	int found_file = FALSE;
	for(int i = 0; i < MAX_FILES_PER_DIRECTORY; i++)
	{
		if(strcmp(dir.u_file[i].file_name, u_file) == 0)
		{
			found_file = TRUE;
			read_inode(dir.u_file[i].inode_number, &curr_inode);
			lseek(virtual_disk, curr_inode.blocks[0]*BLOCK_SIZE_BYTES, SEEK_SET);
			break;
		}
	}
	if(!found_file)
	{
		printf("error, file not found\n");
		return 0;
	}

	/* Create the file in the linux file system and write to it.
	 * Give user read/write access to file. */
	int file_out = open(linux_file, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
	int full_blocks = curr_inode.file_size_bytes/BLOCK_SIZE_BYTES;
	int rem_bytes = curr_inode.file_size_bytes - (full_blocks * BLOCK_SIZE_BYTES);
	for(int i = 0; i < full_blocks; i++)
	{
		crash_read(virtual_disk, &buffer, BLOCK_SIZE_BYTES);
		crash_write(file_out, &buffer, BLOCK_SIZE_BYTES);
	}
	crash_read(virtual_disk, &buffer, rem_bytes);
	crash_write(file_out, &buffer, rem_bytes);
	close(file_out);


	return 1;
}


/*
 * Deletes the file from u_fs.
 * Keep the order of updates to data structures in mind to ensure consistency.
 *
 * Return 1 on success, 0 on failure.
 *
 * TODO: Implement this function.
 */
int u_del(char* u_file) {

	/* Sets clean shutdown to false, saves to userfs disk. */
	sb.clean_shutdown = FALSE;
	lseek(virtual_disk, BLOCK_SIZE_BYTES*SUPERBLOCK_BLOCK, SEEK_SET);
	crash_write(virtual_disk, &sb, sizeof(superblock));
	sync();

	/* Search the user file system for the file.
	 * Free the blocks used as well as the inode and directory.
	 * Finally, decrement the number of files in the directory. */
	int found_file = FALSE;
	for(int i = 0; i < MAX_FILES_PER_DIRECTORY; i++)
	{
		if(strncmp(dir.u_file[i].file_name, u_file, strlen(u_file)) == 0)
		{
			found_file = TRUE;
			read_inode(dir.u_file[i].inode_number, &curr_inode);

			for(int j = 0; j < curr_inode.num_blocks; j++)
			{
				free_block(curr_inode.blocks[j]/BLOCK_SIZE_BYTES);

			}
			/* Updates bitmap in userfs disk. */
			lseek(virtual_disk, BLOCK_SIZE_BYTES*BIT_MAP_BLOCK, SEEK_SET);
			crash_write(virtual_disk, bit_map, sizeof(BIT)*BIT_MAP_SIZE);
			sync();

			curr_inode.free = TRUE;
			strncpy(dir.u_file[i].file_name, "", 15);
			dir.u_file[i].free = TRUE;
			dir.num_files--;

			/* Updates directory in userfs disk. */
			lseek(virtual_disk, BLOCK_SIZE_BYTES*DIRECTORY_BLOCK, SEEK_SET);
			crash_write(virtual_disk, &dir, sizeof(dir_struct));
			sync();

			break;
		}

	}
	if(!found_file)
	{
		printf("error, file not found\n");
		return 0;
	}

	/* Sets clean shutdown to true, saves to userfs disk. */
	sb.clean_shutdown = TRUE;
	lseek(virtual_disk, BLOCK_SIZE_BYTES*SUPERBLOCK_BLOCK, SEEK_SET);
	crash_write(virtual_disk, &sb, sizeof(superblock));
	sync();

	return 1;
}

/*
 * Checks the file system for consistency.
 * Detects if there are orphaned inodes or blocks, and tries its best
 * to recover these objects.
 *
 * Return 1 on success, 0 on failure.
 *
 * TODO: Implement this function.
 */
int u_fsck() {

	/* Counts the number of free files in the directory and compares to number recorded in superblock. */
	int file_count = 0;
	for(int i = 0; i < MAX_FILES_PER_DIRECTORY; i++)
	{
		if(!dir.u_file[i].free)
			file_count++;
	}
	if(file_count != dir.num_files)
	{
		fprintf(stderr, "error, mismatch between expected and actual number of files\n");
		return 0;
	}

	/* Look at inodes in use and make sure that theres at least one file associated. */
	printf("\n");
	// for(int i = 0; i < 32; i++)
	// {
	// 	for(int j = 0; j < 32; j++)
	// 		printf("%d", bit_map[32*i+j]);
	// 	printf("\n");
	// }
	for(int i = 0; i < MAX_INODES; i++)
	{
		read_inode(i, &curr_inode);
		if(!curr_inode.free)
		{
			int in_use = FALSE;
			for(int j = 0; j < MAX_FILES_PER_DIRECTORY; j++)
			{
				if(dir.u_file[i].inode_number == i)
					in_use = TRUE;
			}
			if(!in_use)
			{
				fprintf(stderr, "error, found unused inode not free, freeing\n");
				curr_inode.free = TRUE;
				return 0;
			}
		}
	}
	/* Count the number of blocks in use. */
	int block_count = 0;
	for(int i = 0; i < MAX_INODES; i++)
	{
		inode temp;
		read_inode(i, &temp);
		for(int j = 0; j < MAX_BLOCKS_PER_FILE; j++)
		{
			if(temp.blocks[j] != 0)
				block_count++;
		}
	}
	int blocks_used = 3 + NUM_INODE_BLOCKS + block_count;
	/* Compare the actual and expected number of blocks in use. */
	if(sb.disk_size_blocks-blocks_used != sb.num_free_blocks)
	{
		fprintf(stderr, "error, mismatch between expected and actual number of free blocks\n");
		return 0;
	}

	/* Ensure that any allocated blocks are pointed to by a file. */
	int abandoned_block = TRUE;
	int block_start = 3 + NUM_INODE_BLOCKS;
	for(int i = block_start; i < BIT_MAP_SIZE; i++)
	{
		if(bit_map[i] == 1)
		{
			for(int j = 0; j < MAX_FILES_PER_DIRECTORY; j++)
			{
				if(!dir.u_file[j].free)
				{
					read_inode(dir.u_file[j].inode_number, &curr_inode);
					for(int k = 0; k < curr_inode.num_blocks; k++)
					{
						if(curr_inode.blocks[k] == i)
							abandoned_block = FALSE;
					}
				}
			}
		}
	}
	if(!abandoned_block)
	{
		fprintf(stderr, "error, abandoned block found\n");
		return 0;
	}

	fprintf(stderr, "File System Check: SUCCESS\n");
	return 1;
}
/* Ensure that any allocated blocks are pointed to by a file. */

/*
 * Iterates through the directory and prints the
 * file names, size and last modified date and time.
 */
void u_ls() {
	struct tm *loc_tm;
	int numFilesFound = 0;

	for (int i = 0; i< MAX_FILES_PER_DIRECTORY; i++) {
		if (!(dir.u_file[i].free)) {
			if(!numFilesFound)
				fprintf(stderr, "Name\tBytes\tLast Modified\n");
			numFilesFound++;
			/* file_name size last_modified */

			read_inode(dir.u_file[i].inode_number, &curr_inode);
			loc_tm = localtime(&curr_inode.last_modified);
			fprintf(stderr,"%s\t%d\t%s",dir.u_file[i].file_name,
				curr_inode.file_size_bytes, asctime(loc_tm));

		}
	}
	if (numFilesFound == 0){
		fprintf(stdout, "Directory empty\n");
	}
}

/*
 * Formats the virtual disk. Saves the superblock
 * bit map and the single level directory.
 */
int u_format(int diskSizeBytes, char* file_name) {
	int minimumBlocks;

	/* create the virtual disk */
	if ((virtual_disk = open(file_name, O_CREAT|O_RDWR, S_IRUSR|S_IWUSR)) < 0) {
		fprintf(stderr, "Unable to create virtual disk file: %s\n", file_name);
		return 0;
	}


	fprintf(stderr, "Formatting userfs of size %d bytes with %d block size in file %s\n", diskSizeBytes, BLOCK_SIZE_BYTES, file_name);

	minimumBlocks = 3+ NUM_INODE_BLOCKS+1;
	if (diskSizeBytes/BLOCK_SIZE_BYTES < minimumBlocks) {
		/* If it's impossible to have the superblock, bitmap,
		   directory, inodes and at least one datablock then
			 there no point attempting to continue. */
		fprintf(stderr, "Minimum size virtual disk is %d bytes %d blocks\n", BLOCK_SIZE_BYTES*minimumBlocks, minimumBlocks);
		fprintf(stderr, "Requested virtual disk size %d bytes results in %d bytes %d blocks of usable space\n", diskSizeBytes, BLOCK_SIZE_BYTES*minimumBlocks, minimumBlocks);
		return 0;
	}


	/*************************  BIT MAP **************************/
	assert(sizeof(BIT)* BIT_MAP_SIZE <= BLOCK_SIZE_BYTES);
	fprintf(stderr, "%d blocks %d bytes reserved for bitmap (%ld bytes required)\n", 1, BLOCK_SIZE_BYTES, sizeof(BIT)*BIT_MAP_SIZE);
	fprintf(stderr, "\tImplies Max size of disk is %ld blocks or %ld bytes\n", BIT_MAP_SIZE, BIT_MAP_SIZE*BLOCK_SIZE_BYTES);

	if (diskSizeBytes >= BIT_MAP_SIZE* BLOCK_SIZE_BYTES) {
		fprintf(stderr, "Unable to format a userfs of size %d bytes\n", diskSizeBytes);
		return 0;
	}

	init_bit_map();

	/* first three blocks will be taken with the
	   superblock, bitmap and directory */
	allocate_block(SUPERBLOCK_BLOCK);
	allocate_block(BIT_MAP_BLOCK);
	allocate_block(DIRECTORY_BLOCK);
	/* next NUM_INODE_BLOCKS will contain inodes */
	for (int i = 3; i < 3+NUM_INODE_BLOCKS; i++) {
		allocate_block(i);
	}

	lseek(virtual_disk, BLOCK_SIZE_BYTES*BIT_MAP_BLOCK, SEEK_SET);
	crash_write(virtual_disk, bit_map, sizeof(BIT)*BIT_MAP_SIZE);


	/***********************  DIRECTORY  ***********************/
	assert(sizeof(dir_struct) <= BLOCK_SIZE_BYTES);

	fprintf(stderr, "%d blocks %d bytes reserved for the userfs directory (%ld bytes required)\n", 1, BLOCK_SIZE_BYTES, sizeof(dir_struct));
	fprintf(stderr, "\tMax files per directory: %d\n", MAX_FILES_PER_DIRECTORY);
	fprintf(stderr,"Directory entries limit filesize to %d characters\n", MAX_FILE_NAME_SIZE);

	init_dir();
	lseek(virtual_disk, BLOCK_SIZE_BYTES*DIRECTORY_BLOCK, SEEK_SET);
	crash_write(virtual_disk, &dir, sizeof(dir_struct));


	/***********************  INODES ***********************/
	fprintf(stderr, "userfs will contain %ld inodes (directory limited to %d)\n", MAX_INODES, MAX_FILES_PER_DIRECTORY);
	fprintf(stderr,"Inodes limit filesize to %d blocks or %d bytes\n", MAX_BLOCKS_PER_FILE, MAX_BLOCKS_PER_FILE* BLOCK_SIZE_BYTES);

	curr_inode.free = 1;
	for (int i = 0; i < MAX_INODES; i++) {
		write_inode(i, &curr_inode);
	}


	/***********************  SUPERBLOCK ***********************/
	assert(sizeof(superblock) <= BLOCK_SIZE_BYTES);
	fprintf(stderr, "%d blocks %d bytes reserved for superblock (%ld bytes required)\n", 1, BLOCK_SIZE_BYTES, sizeof(superblock));
	init_superblock(diskSizeBytes);
	fprintf(stderr, "userfs will contain %d total blocks: %d free for data\n", sb.disk_size_blocks, sb.num_free_blocks);
	fprintf(stderr, "userfs contains %ld free inodes\n", MAX_INODES);

	lseek(virtual_disk, BLOCK_SIZE_BYTES*SUPERBLOCK_BLOCK, SEEK_SET);
	crash_write(virtual_disk, &sb, sizeof(superblock));
	sync();

	/* when format complete there better be at
	   least one free data block */
	assert(u_quota() >= 1);
	fprintf(stderr,"Format complete!\n");

	return 1;
}

/*
 * Attempts to load a file system given the virtual disk name
 * It will perform an automated fsck, which will try to recover any
 * potentially lost data and fix inconsistencies.
 */
int recover_file_system(char *file_name) {
	if ((virtual_disk = open(file_name, O_RDWR)) < 0) {
		printf("virtual disk open error\n");
		return 0;
	}

	/* read the superblock */
	lseek(virtual_disk, BLOCK_SIZE_BYTES*SUPERBLOCK_BLOCK, SEEK_SET);
	read(virtual_disk, &sb, sizeof(superblock));

	/* read the bit_map */
	lseek(virtual_disk, BLOCK_SIZE_BYTES*BIT_MAP_BLOCK, SEEK_SET);
	read(virtual_disk, bit_map, sizeof(BIT)*BIT_MAP_SIZE);

	/* read the single level directory */
	lseek(virtual_disk, BLOCK_SIZE_BYTES* DIRECTORY_BLOCK, SEEK_SET);
	read(virtual_disk, &dir, sizeof(dir_struct));

	if (!superblockMatchesCode()) {
		fprintf(stderr, "Unable to recover: userfs appears to have been formatted with another code version\n");
		return 0;
	}
	if (!sb.clean_shutdown) {
		/* Try to recover your file system */
		fprintf(stderr, "u_fsck in progress......");
		if (u_fsck()) {
			fprintf(stderr, "Recovery complete\n");
			return 1;
		} else {
			fprintf(stderr, "Recovery failed\n");
			return 0;
		}
	} else {
		fprintf(stderr, "Clean shutdown detected\n");
		return 1;
	}
}

/* Cleanly performs a shutdown and ensures everything is consistent.
 * Pay attention to what order you update values in case a crash occurs during
 * the shutdown.
 *
 * Returns 1 on success, 0 on failure.
 *
 * TODO: Finish implementing this function.
 */
int u_clean_shutdown() {
	sb.num_free_blocks = u_quota();
	sb.clean_shutdown = TRUE;

	lseek(virtual_disk, BLOCK_SIZE_BYTES* SUPERBLOCK_BLOCK, SEEK_SET);
	crash_write(virtual_disk, &sb, sizeof(superblock));
	sync();

	close(virtual_disk);
	/* is this all that needs to be done on clean shutdown? */
	return sb.clean_shutdown;
}
