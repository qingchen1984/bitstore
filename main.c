/* 
 * File:   main.c
 * Author: zarin
 *
 * Created on January 11, 2012, 8:06 PM
 */
#include "bitstore.h"

/*
 * 
 */
int main(int argc, char** argv) {
	int sbit, ebit;
	struct bitstore *s;
	char *max;
	
	s = bstore_mmap("/tmp/storage");
	max = bstore_max(s);
	printf("Store size: %lld\n", s->size);
	printf("Max: %s\n", max);
	
	sbit = bstore_get(s, "0x31000000");
	bstore_switch(s, "0x31000000");
	ebit = bstore_get(s, "0x31000000");
	printf("Start bit state: %i\n", sbit);
	printf("End bit state:   %i\n", ebit);
	
	bstore_unmap(s);

	s = bstore_alloc(1024*1024*512);
	
	sbit = bstore_get(s, "0x31000000");
	bstore_switch(s, "0x31000000");
	ebit = bstore_get(s, "0x31000000");
	printf("Start bit state: %i\n", sbit);
	printf("End bit state:   %i\n", ebit);
	
	bstore_free(s);

	return 0;
}

