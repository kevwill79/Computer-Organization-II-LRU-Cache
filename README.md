# LRU-Cache
I. Purpose
The purpose of this project is to exercise your understanding of caches of various sizes and
configurations, as well as the write policies of write-through and write-back.

II. Basic Description
You will be implementing a cache simulator in C, which will be contained in the single file
proj3.c. You program should take as command line arguments the following information:
- b: size of block in bytes.
- s: number of sets in the cache.
- n: associativity of cache. 

Your submitted proj3.c must compile and run on linprog as in the following example:
 $ gcc –lm proj3.c
 $ ./a.out –b 8 –s 256 –n 2 < test1.trace

You should not rely on any special compilation flags (although –lm will be allowed for use of the
math module) or other input methods. If your program does not compile and execute as
expected with the above commands, you will receive zero points on project 3.

The contents of the .trace files used as input are described below. There is no provided code for
this project so you should start by writing C code to parse the command line arguments and
trace files.

After reading in the command line arguments, you should print back to the user the number of
bytes in a block, the number of sets in the cache, and the associativity of the cache.
Additionally, you should print the number of bits required for each partition of a memory
reference: tag, index, and offset.

Afterward, you should prepare to simulate the behavior of two caches on the sequence of
references in the trace file. The first cache has a write-through, no write allocate policy. The
second has a write-back, write allocate policy. For every reference, you should determine for each cache whether the reference is a hit or a miss, and whether main memory needs to be accessed. After all references have been processed, print the statistics for each cache. 

III. Trace File Description
The .trace files that will be used as input to your cache simulator contain any number of lines,
where each line has the following format:

 <access_type> <byte_address>

The access type can be either ‘R’ for reading or ‘W’ for writing. The byte address is simply the
decimal representation of the 32-bit address of the reference. An example .trace file contents
follows:
 R 4096
 R 5000
 W 5000

Note: MIPS is word-aligned, which means that we can only legitimately access every 4th byte. All
provided test files will contain word-aligned references, but you do not need to check to make
sure the references are word-aligned. 

IV. Suggested Development Approach

In this project, you will not be given any starting file so it is suggested that you start as soon as
possible to allow yourself time to plan the structure of your program.

The first thing you should work on is the parsing of the command line arguments. Note that the
command line arguments may be specified in any order. You may assume that the block size
and number of sets provided as arguments to the program will always be powers-of-two. After
you have extracted the arguments, echo the values back to the user as shown in the sample
output. You should also calculate and print the number of offset bits, index bits, and tag bits, as
discussed in lecture 11. You may assume that the addresses in the test file correspond to 32-bit
binary addresses.

Block size: 16
Number of sets: 64
Associativity: 2
Number of offset bits: 4
Number of index bits: 6
Number of tag bits: 22 

Next, you should implement a write-through, no write allocate cache and supporting
functionality for reading from and writing to the cache. There are several ways to implement
the cache in your C code, but the most straightforward is probably through the use of twodimensional
arrays (or a flattened one-dimensional array). In this case, you may want to be
aware that C does not support variable array declaration. To create a dynamically-sized array
based on the input parameters, you’ll need to use the malloc() function. If you are unfamiliar
with C, please allow a day or so to familiarize yourself with dynamic array manipulation in C if
needed.

For a write-through, no write allocate cache, we have the following properties:
- When a block is present in the cache (hit), a read simply grabs the data for the processor.
- When a block is present in the cache (hit), a write will update both the cache and main
memory (i.e. we are writing through to main memory).
- When a block is not present in the cache (miss), a read will cause the block to be pulled from
main memory into the cache, replacing the least recently used block if necessary.
- When a block is not present in the cache (miss), a write will update the block in main memory
but we do not bring the block into the cache (this is why it is called “no write allocate”).

After you have implemented the write-through cache, you should implement the write-back
cache. One way to approach this is to have two separate caches, one for write-through and the
other for write-back, which are both updated independently every time a reference is read
from the trace file.

For a write-back, write allocate cache, we have the following properties:
- When a block is present in the cache (hit), a read simply grabs the data for the processor.
- When a block is present in the cache (hit), a write will update only the cache block and set the
dirty bit for the block. The dirty bit indicates that the cache entry is not in sync with main
memory and will need to be written back to main memory when the block is evicted from the
cache.
- When a block is not present in the cache (miss), a read will cause the block to be pulled from
main memory into the cache, replacing the least recently used block if necessary. If the block
being evicted is dirty, the block’s contents must be written back to main memory.
- When a block is not present in the cache (miss), a write will cause the block to be pulled from
main memory into the cache, replacing the least recently used block if necessary. If the block
being evicted is dirty, the block’s contents must be written back to main memory. 

Using these properties, you must calculate the number of hits and misses as well as the
memory references made by the sequence of addresses in the trace file. A memory reference is
defined as an access to main memory in order to either update or read a block. You do not
need to model the data or main memory, we are just simulating the effect of the references,
not dealing with actual data. 
