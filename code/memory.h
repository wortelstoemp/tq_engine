#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/* Byte sizes */
inline uint64_t
tqKilobytes(size_t size)
{
	return size * 1024LL;	
}

inline uint64_t
tqMegabytes(size_t size)
{
	return size * 1048576LL;	
}

inline uint64_t
tqGigabytes(size_t size)
{
	return size * 1073741824LL;	
}

inline uint64_t
tqTerabytes(size_t size)
{
	return size * 1099511627776LL;	
}

/* Memory management */
/* References: 
	http://www.gamasutra.com/blogs/MichaelKissner/20151104/258271/Writing_a_Game_Engine_from_Scratch__Part_2_Memory.php 
 	http://www.ibm.com/developerworks/aix/tutorials/au-memorymanager/
 */
/* Persistent memory is stack-based right now. */

typedef struct tqMemory
{
	void*		persistentMemory;
	void*		transientMemory;
	uint64_t	persistentMemorySize;
	uint64_t	transientMemorySize;			
	int			persistentMemoryCursor;
} tqMemory;

inline bool
tqCreateMemory(tqMemory* m, size_t persistentBytes, size_t transientBytes)
{
	m->persistentMemory = malloc(persistentBytes);
	if (!m->persistentMemory) {
		return false;	
	}
	
	m->transientMemory = malloc(transientBytes);
	if (!m->transientMemory) {
		return false;	
	}
	
	persistentMemoryCursor = 0;
	
	memset(m->persistentMemory, 0, persistentBytes);
	memset(m->transientMemory, 0, transientBytes);	
	return true;
}

inline bool
tqCreatePersistentMemory(tqMemory* m, size_t persistentBytes)
{
	m->persistentMemory = malloc(persistentBytes);
	
	if (!m->persistentMemory) {
		return false;	
	}
	
	memset(m->persistentMemory, 0, persistentBytes);
	return true;
}

inline bool 
tqCreateTransientMemory(tqMemory* m, size_t transientBytes)
{
	m->transientMemory = malloc(transientBytes);
	
	if (!m->transientMemory) {
		return false;
	}
	
	memset(m->transientMemory, 0, transientBytes);
	return true;
}

inline bool 
tqDestroyMemory(tqMemory* m)
{
	if (!m->persistentMemory) {
		return false;
	}
	
	if (!m->transientMemory) {
		return false;
	}
	
	free(m->persistentMemory);
	free(m->transientMemory);
	
	return true;
}

inline bool 
tqDestroyPersistentMemory(tqMemory* m)
{
	if (!m->persistentMemory) {
		return false;
	}
	
	free(m->persistentMemory);
	return true;
}

inline bool 
tqDestroyTransientMemory(tqMemory* m)
{
	if (!m->transientMemory) {
		return false;
	}
	
	free(m->transientMemory);
	return true;
}

inline void* 
tqAllocatePersistentMemory(tqMemory* m, size_t persistentBytes)
{
	if (m->persistentMemoryCursor >= m->persistentMemorySize) {
		return NULL;
	}
	
	m->persistentMemoryCursor += persistentBytes;
	return &m->persistentMemory[m->persistentMemoryCursor - persistentBytes];
}

inline void 
tqFreePersistentMemory(tqMemory* m, size_t persistentBytes)
{
	if (m->persistentMemoryCursor > 0) {
		m->persistentMemoryCursor -= persistentBytes;	
	}	
}


