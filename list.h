#ifndef _LIST_H_
#define _LIST_H_

typedef struct _LIST_ENTRY {
	    struct _LIST_ENTRY  *Flink;    
		struct _LIST_ENTRY  *Blink; 
} LIST_ENTRY, *PLIST_ENTRY;

void InitializeListHead(PLIST_ENTRY ListHead);

int IsListEmpty(LIST_ENTRY * ListHead);

PLIST_ENTRY RemoveHeadList(PLIST_ENTRY ListHead);

PLIST_ENTRY RemoveTailList(PLIST_ENTRY ListHead);

void InsertTailList(PLIST_ENTRY ListHead,PLIST_ENTRY Entry);

void InsertHeadList(PLIST_ENTRY ListHead,PLIST_ENTRY Entry);

void AppendTailList(PLIST_ENTRY ListHead,PLIST_ENTRY ListToAppend);

#define OFFSET_OF(type, field) \
	(unsigned int)(&(((type*)0)->field))

#define CONTAINER_OF( node, type, field ) \
	(type*)( (char *)node - OFFSET_OF(type, field)   )


#endif
