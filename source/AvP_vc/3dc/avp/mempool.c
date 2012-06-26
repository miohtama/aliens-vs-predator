#include "ourasert.h"
#include "mem3dc.h"
#include "mempool.h"


#if USE_LEVEL_MEMORY_POOL

#define MAX_NUM_MEMORY_BLOCK 40
#define MEMORY_BLOCK_SIZE (1024*1024)

static char* MemoryBlocks[MAX_NUM_MEMORY_BLOCK];
static int CurrentMemoryBlock =-1;

static char* MemoryPoolPtr=0;
static unsigned int MemoryLeft=0;


void* PoolAllocateMem(unsigned int amount)
{
	char* retval;

	GLOBALASSERT(amount<=MEMORY_BLOCK_SIZE)
	
	if(amount>MemoryLeft)
	{
		CurrentMemoryBlock++;
		GLOBALASSERT(CurrentMemoryBlock<MAX_NUM_MEMORY_BLOCK);
		MemoryBlocks[CurrentMemoryBlock]=AllocateMem(MEMORY_BLOCK_SIZE);
		GLOBALASSERT(MemoryBlocks[CurrentMemoryBlock]);

		
		MemoryLeft=MEMORY_BLOCK_SIZE;
		MemoryPoolPtr=MemoryBlocks[CurrentMemoryBlock];
	}
		
	retval=MemoryPoolPtr;
	MemoryLeft-=amount;
	MemoryPoolPtr+=amount;
	return (void*)retval;
	
}


void ClearMemoryPool()
{
	int i;

	for(i=0;i<=CurrentMemoryBlock;i++)
	{
		DeallocateMem(MemoryBlocks[i]);
		MemoryBlocks[i]=0;
	}
	CurrentMemoryBlock=-1;
	MemoryPoolPtr=0;
	MemoryLeft=0;
}



#endif
