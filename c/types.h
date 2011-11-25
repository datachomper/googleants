#ifndef _TYPES_H_
#define _TYPES_H_

#define LIST_POISON1 ((void *)0x10101010)
#define LIST_POISON2 ((void *)0x20202020)
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))
#define EXPORT_SYMBOL(__x__)

struct list_head {
        struct list_head *next, *prev;
};

struct hlist_head {
        struct hlist_node *first;
};

struct hlist_node {
        struct hlist_node *next, **pprev;
};

#undef offsetof
#ifdef __compiler_offsetof
#define offsetof(TYPE,MEMBER) __compiler_offsetof(TYPE,MEMBER)
#else
#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)
#endif

/**
 * container_of - cast a member of a structure out to the containing structure
 * @ptr:        the pointer to the member.
 * @type:       the type of the container struct this is embedded in.
 * @member:     the name of the member within the struct.
 *
 */
#define container_of(ptr, type, member) ({                      \
        const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
        (type *)( (char *)__mptr - offsetof(type,member) );})


#ifndef NULL
#define NULL 0
#endif

#endif
