#include <stdbool.h>	/* includes bool */
#include <stdio.h>	/* includes printf() */
#include <string.h>	/* includes strcpy() */
#include <unistd.h>	/* includes sbrk() */
#include <assert.h> /* includes assert() */

static void *block_get(size_t size); //declare function 

struct block {
	void *addr; //the starting address of the allocated memory.
	size_t size; //the size of the allocated memory in bytes
	bool used; //defines whether the block is in use, or has been freed by free().
	struct block *prev; //is a pointer to the previous block
};

static struct block *head = NULL;

/* allocate a new block using sbrk() */
static void *block_alloc(size_t size)
{
	struct block *bptr;
	void *address;
	
	//add the size of the block stucture and the size parameter together
	size_t new_size = sizeof(struct block) + size;
	
	address = sbrk(new_size);
	
	if (address == (void*)-1){ //Check the return value of sbrk() for failure
		return NULL;
	}
	
	bptr = address;

	if(head == NULL) {
		bptr->prev = NULL;
	} else {
		bptr->prev = head;
	} 

	/* TODO your code here */
	bptr->addr = address + sizeof(struct block); //Make the bptr pointer point to the start of the new writable memory space
	bptr->size = size;
	bptr->used = true; 

	/* append the new block structure to the head of the linked list */
	head = bptr;

	return bptr->addr;
}

/* mark a block as unused or free it from memory */
static void block_free(struct block *bptr)
{	
	bptr->used = false;
	
	if(head == bptr){ //Check if current block is equal to the head
		//delete the block
		head = bptr->prev;
		sbrk(-(bptr->size+sizeof(struct block)));
	}
}

/* get an unused block that has already been allocated */
static void *block_get(size_t size)
{
	struct block *current = head;
	void *address;
	while(current){
		if(current->used == false && current->size >= size){
			current->used = true;
			return current->addr;//return the address of the block that is available
		}
		current = current->prev;
	}

	return NULL;
}

/* return a new memory space of at least "size" bytes */
void *malloc(size_t size)
{
	void *address;
	if(size == 0){
		return NULL;
	}

	//check if block is available
	void *available_address = block_get(size);

	if(available_address){
		return available_address;
	}else{ 
		//if there is no available block
		address = block_alloc(size);
	}

	if(!address){ //check for failure
		return NULL;
	}
	
	return address;
}

/* free a memory space from memory */
void free(void *ptr)
{	
	if(ptr != NULL || (ptr < sbrk(0))){ //if ptr points to an address beyond our allocated memory space
		//Free the block
		block_free((struct block *)(ptr - sizeof(struct block)));
	}
}

int main()
{
  void *str = malloc(10);
	printf("Address = %p\n", str);
	printf("Program = %p\n", sbrk(0));
	printf("Size = %li\n", ((struct block *)(str-sizeof(struct block)))->size);
	printf("================\n");

	void *str2 = malloc(20);
	printf("Address = %p\n", str2);
	printf("Program = %p\n", sbrk(0));
	printf("Size = %li\n", ((struct block *)(str2-sizeof(struct block)))->size);
	printf("================\n");

	printf("Free Str1\n");
	printf("================\n");
	
	free(str);
	
	void *str3 = malloc(10);
	printf("Address = %p\n", str3);
	printf("Program = %p\n", sbrk(0));
	printf("Size = %li\n", ((struct block *)(str3-sizeof(struct block)))->size);
	printf("================\n");

	void *str4 = malloc(20);
	printf("Address = %p\n", str4);
	printf("Program = %p\n", sbrk(0));
	printf("Size = %li\n", ((struct block *)(str4-sizeof(struct block)))->size);
	printf("================\n");

	strcpy(str4, "Dit is een zin"); 
	printf("Succes!\n");
	return 0;
}
