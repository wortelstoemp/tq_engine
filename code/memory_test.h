#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/* 
	References:
	(Fast Efficient Fixed-Size Memory Pool by Ben Kenwright
 	http://www.doresearch.org/memory_pools/paper.pdf )
	--> Too difficult
	
	Use:
	Memory Pools from Game Coding Complete 4th Edition
*/

/* 
	chunks in blocks
	Grow: new block if chunks exceed limit, but is costly (uses malloc)
*/

const static size_t TQ_MEMORY_POOL_CHUNK_HEADER_SIZE = sizeof(unsigned uint8_t*);
 
typedef struct tqMemoryPool
{
	uint8_t**		ppMemoryBlocks;
	uint8_t*		pHead;
	unsigned int	chunkSize;
	unsigned int	numChunks;
	unsigned int	numMemoryBlocks;
	bool			allowResize;	 
} tqMemoryPool;


static void
tqResetMemoryPool(tqMemoryPool* self)
{
	ppMemoryBlocks = NULL;
	pHead = NULL;
	chunkSize = 0;
	numChunks = 0;
	numMemoryBlocks = 0;
	allowResize = true;
}

static uint8_t*
tqAllocNewMemoryBlock(tqMemoryPool* self)
{
	return NULL;
}

static bool
tqGrowMemoryPool(tqMemoryPool* self)
{
	/* Create new memory pool as grown version of old pool */
	const size_t oldSize = self->numMemoryBlocks;
	const size_t newSize = sizeof(uint8_t*) * (oldSize + 1);
	uint8_t** ppNewMemoryBlocks = (uint8**) malloc(newSize);
	
	if (!ppNewMemoryBlocks) {
		return false;
	}
	
	for (unsigned int i = 0; i < oldSize; i++) {
		ppNewMemoryBlocks[i] = self->ppMemoryBlocks[i];
	}
	
	/* Create new memory block */
	ppNewMemoryBlocks[oldSize] = tqAllocNewMemoryBlock(self);
	
	/* Put new block at end of pool */
	if (self->pHead) {
		uint8_t* pCurrentBlock = self->pHead;
		uint8_t* pNextBlock = ((uint8_t**) self->pHead)[0];
		
		while (pNextBlock) {
			pCurrentBlock = pNextBlock;
			pNextBlock = ((uint8_t**)pNextBlock)[0];
		}
		
		((uint8_t**)pCurrentBlock)[0] = ppNewMemoryBlocks[oldSize];		
		
	} else {
		self->pHead = ppNewMemoryBlocks[oldSize];
	}
	
	/* free old pool and replace with new pool */
	if (self->ppMemoryBlocks) {
		free(self->ppMemoryBlocks);
	}
	
	self->ppMemoryBlocks = ppNewMemoryBlocks;
	self->numMemoryBlocks++;
	
	return true;
}

bool 
tqCreateMemoryPool(tqMemoryPool* self, unsigned int chunkSize, unsigned int numChunks)
{
	self->chunkSize = chunkSize;
	self->numChunks = numChunks;
	
	if (tqGrowMemoryPool(self)) {
		return true;
	} 
	
	return false;
}

void
tqDestroyMemoryPool(tqMemoryPool* self)
{
	for (unsigned int i = 0; i < self->numMemoryBlocks; i++) {
		free(self->ppMemoryBlocks[i]);
	}
	free(self->ppMemoryBlocks);
	tqResetMemoryPool(self);
}

void*
tqAllocFromMemoryPool(tqMemoryPool* self)
{
	return NULL;
}

void
tqFreeFromMemoryPool(tqMemoryPool* self, void* data)
{
	
}