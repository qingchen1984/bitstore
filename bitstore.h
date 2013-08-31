/* 
 * File:   bitstore.h
 * Author: zarin
 *
 * Created on January 11, 2012, 5:42 PM
 */
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#ifndef BITSTORE_H
	#define	BITSTORE_H

	struct bitstore {
		size_t size;
		char *addr;
	};

#endif	/* BITSTORE_H */

