#include "bitstore.h"

int bstore_fd;
size_t bstore_file_size;

void * bmalloc(size_t size) {
	void *ptr;
	ptr = malloc(size);
	
	if(!ptr) {
		perror("bmalloc");
		exit(EXIT_FAILURE);
	} else {
		memset(ptr, 0, size);
		return ptr;
	}
}

void * bmmap(void *addr, size_t len, int prot, int flags, int fd, off_t offset) {
	void *ptr;
	ptr = mmap(addr, len, prot, flags, fd, offset);
	
	if(ptr == MAP_FAILED) {
		switch(errno) {
			case EACCES:
				perror("[EACCES] Permission denied! bmmap");
				break;
			case EAGAIN:
				perror("[EAGAIN] File locked! bmmap");
				break;
			case EBADF:
				perror("[EBADF] Bad file descriptor! bmmap");
				break;
			case EINVAL:
				perror("[EINVAL] Invalid argument! bmmap");
				break;
			case ENFILE:
				perror("[ENFILE] File table overflow! bmmap");
				break;
			case ENODEV:
				perror("[ENODEV] No such device! bmmap");
				break;
			case ENOMEM:
				perror("[ENOMEM] Out of memory! bmmap");
				break;
			case EOVERFLOW:
				perror("[EOVERFLOW] Value too large for defined data type! bmmap");
				break;
			case EPERM:
				perror("[EPERM] Operation not permitted! bmmap");
				break;
		}
		exit(EXIT_FAILURE);
	} else {
		return ptr;
	}
}

void bfree(void *ptr) {
	if(!ptr) {
		perror("Storage is null, nothing to free! bfree");
	} else {
		free(ptr);
	}
}

struct bitstore* bstore_alloc(size_t mem_size) {
    struct bitstore *storage;

	storage = bmalloc(sizeof(struct bitstore));
	
	storage->size = mem_size;
    storage->addr = bmalloc(storage->size);

	return storage;
}

struct bitstore* bstore_mmap(const char* file) {
	struct bitstore *storage;
	struct stat *fdstat;
	
	fdstat = bmalloc(sizeof(struct stat));
	bstore_fd = open(file, O_RDWR);
	
	if(bstore_fd == -1) {
		perror("Could not open file! bstore_mmap");
		return 0;
	}

	fstat(bstore_fd, fdstat);
	
	bstore_file_size = fdstat->st_size;
	bfree(fdstat);

	storage = bmalloc(sizeof(struct bitstore));
	storage->size = bstore_file_size;
	storage->addr = bmmap(NULL, bstore_file_size, PROT_WRITE, MAP_SHARED, bstore_fd, 0);
	
	return storage;
}

int bstore_unmap(struct bitstore *storage) {
	int sync;

	sync = msync(storage->addr, bstore_file_size, MS_SYNC);
	if(sync == -1) {
		switch(errno) {
			case EINVAL:
				perror("[EINVAL] Invalid argument! msync in bstore_unmap");
				break;
			case ENOMEM:
				perror("[ENOMEM] Out of memory! msync in bstore_unmap");
				break;
		}
		return 0;
	}

	if(munmap(storage->addr, bstore_file_size) == -1) {
		perror("[EINVAL] Invalid argument! munmap in bstore_unmap");
		return 0;
	}
	
	if(!storage) {
		perror("Storage is null, nothing to free! bstore_unmap");
		return 0;
	}

	if(close(bstore_fd) == -1) {
		perror("Error with closing file! bstore_unmap");
		return 0;
	}
	
	bfree(storage);
	
	return 1;
}

int bstore_free(struct bitstore *storage) {
	bfree(storage->addr);
	bfree(storage);
	return 1;
}

int bstore_get(struct bitstore *storage, char* hash) {
	int bit_number, bit_value;
	long long offset;
	long byte_number;

	offset = strtoull(hash, NULL, 0);

	if(offset / 8 >= storage->size) {
		perror("Offset out of range");
		return -1;
	}
	
	byte_number = offset >> 3;
	bit_number = offset % 8;
	bit_value = (storage->addr[byte_number] >> bit_number) & 1;

	return bit_value;
}

int bstore_switch(struct bitstore *storage, char* hash) {
	int bit_number;
	unsigned long long offset;
	long byte_number;

	offset = strtoull(hash, NULL, 0);
	
	if(offset / 8 >= storage->size) {
		perror("Offset out of range");
		return -1;
	}
	
	byte_number = offset >> 3;
	bit_number = offset % 8;
	storage->addr[byte_number] ^= (1 << bit_number);

	return 1;
}

void bstore_max(struct bitstore *storage, char *max_bit) {
	snprintf(max_bit, 18, "%llx", (long long)storage->size * 8 - 1);
}