/* Kevin Williams
 * kmw15g
 * CDA 3101
 * Project 3 
 
 * Description:
 * Implement a LRU cache simulator in C
 * Program accepts the following command line arguments
 * -b: size of block in bytes
 * -s: number of sets in cache
 * -n: associativity of cache */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

/* Used to get the cache's tag and index */
int tagToBinary(int addr, int offSize);
int indexToBinary(int addr, int tSize, int sSize);

 int main (int argc, char *argv[]) 
 {
	/* 7 arguments are expected:
	 * The program name (a.out), 3 cmd line args (-b, -s, -n), 
	 * and 3 ints followed by each cmd line arg */
	if(argc != 7)
	{
		printf("\nIntruder Alert!\nNot enough arguments!\nIntruder Alert!\n\n");
		return 0;
	}
	
	/* Iterate through the cmd line args 
	 * and print the data that goes with
	 * the corresponding cmd line arg */
	int i, offsetBits, indexBits, tagBits, blockSize, numSets, assoc;

	/* Start with 1 because the first arg is the program's name (a.out) */
	for(i = 1; i < argc; i++)
	{
		if(strcmp(argv[i], "-b") == 0)
		{
			blockSize = atoi(argv[i+1]);
			
			/* atoi converts string arg to an int */
			offsetBits = log2(atoi(argv[i+1]));
		}
	
		else if(strcmp(argv[i], "-s") == 0)
		{
			numSets = atoi(argv[i+1]);

			indexBits = log2(atoi(argv[i+1]));
		}
		
		else if(strcmp(argv[i], "-n") == 0)
			assoc = atoi(argv[i+1]);
	}
	
	/* Calculate tag bits */
	tagBits = 32 - (offsetBits + indexBits);
				
	printf("\nBlock size: %d\n", blockSize);
	printf("Number of sets: %d\n", numSets);
	printf("Associativity: %d\n", assoc);

	printf("Number of offset bits: %d\n", offsetBits);
	printf("Number of index bits: %d\n", indexBits);
	printf("Number of tag bits: %d\n\n\n", tagBits);

	/* Variable to hold the size of cache
	 * needed for 2D cache array */
	int size;
	size = pow(2, indexBits);
	
	struct cache
	{
		unsigned int offset;
		unsigned int index;
		unsigned int tag;
		unsigned int reference;
		unsigned int valid;
		unsigned int address;

	}tempData, writeThrough_noWrite[size][assoc], writeBack_write[size][assoc];

	/* tempData will be used to check if an entry is in the cache
	 * when compared to the write through and write back caches
	 * Start with an empty cache */
	tempData.tag = -1;
	tempData.valid = -1;
	tempData.index = -1;
	tempData.reference = -1;
		
	/* Will hold the access type and the byte address */
	char instrInfo[10];
	
	/* The number of lines in trace file is equal to the  
	 * number of references in trace file.
	 * This variable will be used to calculate the 
	 * number of references */
	int numLines = 0;

	/* numLines will be incremented everytime a newline is encountered */
	char * c = " \n";

	/* For read or write access */
	char * accessType = "";
	
	/* Holds the byte address */
	int byteAddr;

	/* Variables hold the hits and misses for the 2 caches and 
	 * the number of times memory is referenced */
	int writeThroughHit = 0;
	int writeThroughMiss = 0;
	int writeBackHit = 0;
	int writeBackMiss = 0;
	int writeThroughMem = 0;
	int writeBackMem = 0;

	/* Variable will be used to update the 
	 * hits and misses for the caches and memory */
	int writeThroughEntryHit = -1;
	int writeBackEntryHit = -1;
	
	while((fgets(instrInfo, 10, stdin)) != NULL)
	{
		/* Gets the access type and the byte address */
		accessType = strtok(instrInfo, c);
		byteAddr = atoi(strtok(NULL, c));

		if(c)
			numLines++;
	
		/* Store the contents of the trace file into the temp cache */
		tempData.address = byteAddr;
		tempData.offset = offsetBits;
		tempData.reference = numLines;
		tempData.valid = 1;

		/* Convert the decimal address to 
		 * binary to get the index and the tag */
		/*tempData.index = convertDecimalToBinary(byteAddr);*/
		tempData.index = indexToBinary(byteAddr, offsetBits, numSets);
		tempData.offset = tagToBinary(byteAddr, tagBits);

		/* Check if enty is in the cache */
		writeThroughEntryHit = -1;
		writeBackEntryHit = -1;

		for(i = 0; i < assoc; ++i)
		{
			if(writeThrough_noWrite[tempData.index][i].index == tempData.index
		    && writeThrough_noWrite[tempData.index][i].tag == tempData.tag
		    && writeThrough_noWrite[tempData.index][i].valid == 1)
			{
				writeThroughEntryHit = i;
			}

			if(writeBack_write[tempData.index][i].index == tempData.index
		    && writeBack_write[tempData.index][i].tag == tempData.tag
		    && writeBack_write[tempData.index][i].valid == 1)
			{
				writeBackEntryHit = i;
			}
		}
		
		/* Write through no write allocate cache initialization
		 * Check for hits and misses for the write through 
		 * no write allocate cache */
		if(writeThroughEntryHit != -1)
			writeThroughHit++;
		else
		{
			writeThroughMiss++;
			writeThroughMem++;

			if(*accessType == 'R')
			{
				for(i = 0; i < assoc; ++i)
				{
					if(writeThrough_noWrite[tempData.index][i].valid == 0)
						writeThroughEntryHit = i;
				}
			

				if(writeThroughEntryHit != -1)
					writeThrough_noWrite[tempData.index][writeThroughEntryHit] = tempData;
				else
				{
					int lru = 0;

					for(i = 0; i < assoc; ++i)
					{
						if(writeThrough_noWrite[tempData.index][i].reference < lru)
							lru = i;
					}
				
					writeThrough_noWrite[tempData.index][lru] = tempData;
				}
			}
		}
	
		/* Write back write allocate cache initialization
	 	* Check for hits and misses for the write through 
	 	* no write allocate cache */
		if(writeBackEntryHit != -1)
			writeBackHit++;
		else
		{
			writeBackMiss++;
			writeBackMem++;

			if(writeBackEntryHit != -1)
				writeBack_write[tempData.index][writeBackEntryHit] = tempData;
			else
			{
				int lru = writeBack_write[tempData.index][0].reference;

				for(i = 0; i < assoc; ++i)
				{
					if(writeBack_write[tempData.index][i].reference < lru)
						lru = writeBack_write[tempData.index][i].reference;
				}

				writeBack_write[tempData.index][lru] = tempData;
			}
		}
	
		writeThroughEntryHit = 0;
		writeBackEntryHit = 0;
	}

	printf("****************************************\n");
	printf("Write-through with No Write Allocate\n");
	printf("****************************************\n");
	printf("Total number of references: %d\n", numLines);
	printf("Hits: %d\n", writeThroughHit);
	printf("Misses: %d\n", writeThroughMiss);
	printf("Memory References: %d\n\n\n", writeThroughMem);


	printf("****************************************\n");
	printf("Write-back with Write Allocate\n");
	printf("****************************************\n");
	printf("Total number of references: %d\n", numLines);
	printf("Hits: %d\n", writeBackHit);
	printf("Misses: %d\n", writeBackMiss);
	printf("Memory References: %d\n\n\n", writeBackMem);
 
 return 0;
 
 }

int indexToBinary(int addr, int offSize, int sSize)
{
    return (addr >> offSize) % sSize;
}

int tagToBinary(int addr, int tSize)
{
    return (addr >> (32-tSize));
}
