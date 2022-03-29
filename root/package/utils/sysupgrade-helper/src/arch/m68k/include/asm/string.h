#ifndef _M68K_STRING_H_
#define _M68K_STRING_H_

#if 0
#define __HAVE_ARCH_STRCPY
#define __HAVE_ARCH_STRNCPY
#define __HAVE_ARCH_STRLEN
#define __HAVE_ARCH_STRCMP
#define __HAVE_ARCH_STRCAT
#define __HAVE_ARCH_MEMSET
#define __HAVE_ARCH_BCOPY
#define __HAVE_ARCH_MEMCPY
#define __HAVE_ARCH_MEMMOVE
#define __HAVE_ARCH_MEMCMP
#define __HAVE_ARCH_MEMCHR
#endif

extern int strcasecmp(const char *, const char *);
extern int strncasecmp(const char *, const char *, int);
extern char * strcpy(char *,const char *);
extern char * strncpy(char *,const char *, __kernel_size_t);
extern __kernel_size_t strlen(const char *);
extern int strcmp(const char *,const char *);
extern char * strcat(char *, const char *);
extern void * memset(void *,int,__kernel_size_t);
extern void * memcpy(void *,const void *,__kernel_size_t);
extern void * memmove(void *,const void *,__kernel_size_t);
extern int memcmp(const void *,const void *,__kernel_size_t);
extern void * memchr(const void *,int,__kernel_size_t);

#endif
