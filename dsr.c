#include "dsr.h"
#include "klib.h"

static LIST_ENTRY dsr_head;

void dsr_init()
{
	InitializeListHead(&dsr_head);
}

void dsr_add(dsr_callback fn, void* param)
{
	// FIXME we needs a spin lock here
	dsr_node* node = kmalloc(sizeof(*node));
	memset(node, 0, sizeof(*node));
	node->fn = fn;
	node->param = param;
	InsertTailList(&dsr_head, &(node->dsr_list));	
	
}

void dsr_process()
{
	LIST_ENTRY* node;
	dsr_node *dsr;
	while( !IsListEmpty(&dsr_head) ){
		node = RemoveHeadList(&dsr_head);
		dsr = CONTAINER_OF(node, dsr_node, dsr_list);
		if (dsr->fn){
			dsr->fn(dsr->param);
		}
		kfree(dsr);
	}
}


