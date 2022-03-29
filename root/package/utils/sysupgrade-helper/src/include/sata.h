#ifndef __SATA_H__
#define __SATA_H__

int init_sata(int dev);
int scan_sata(int dev);
ulong sata_read(int dev, ulong blknr, ulong blkcnt, void *buffer);
ulong sata_write(int dev, ulong blknr, ulong blkcnt, const void *buffer);

int sata_initialize(void);
int __sata_initialize(void);

#endif
