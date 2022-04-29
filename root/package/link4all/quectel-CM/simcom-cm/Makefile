#CROSS_COMPILE=arm-hisiv100nptl-linux-
# ifneq ($(CROSS_COMPILE),)
# CROSS-COMPILE:=$(CROSS_COMPILE)
# endif

# CC:=$(CROSS-COMPILE)gcc
# LD:=$(CROSS-COMPILE)ld

# release: clean
# 	$(CC) -Wall -s QmiWwanCM.c GobiNetCM.c main.c MPQMUX.c QMIThread.c util.c udhcpc.c -o simcom-cm -lpthread

# debug: clean
# 	$(CC) -Wall -g QmiWwanCM.c GobiNetCM.c main.c MPQMUX.c QMIThread.c util.c udhcpc.c -o simcom-cm -lpthread

# clean:
# 	rm -rf simcom-cm *~


quectel-CM:clean
	$(CC) -Wall -s QmiWwanCM.c GobiNetCM.c main.c MPQMUX.c QMIThread.c util.c udhcpc.c -o quectel-CM -lpthread

clean:
	rm -rf quectel-CM *~

