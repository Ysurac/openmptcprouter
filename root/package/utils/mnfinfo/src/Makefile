MNF_SOURCES = src/mnf_info.c
MNF_OBJS = $(MNF_SOURCES:.c=.o)
MNF_TARGET = mnf_info

MNF_RPC_SOURCES = src/mnf_rpc.c
MNF_RPC_OBJS = $(MNF_RPC_SOURCES:.c=.o)
MNF_RPC_TARGET = mnfinfo.so

LIB_SOURCES = src/libmnfinfo.c
LIB_OBJS = $(LIB_SOURCES:.c=.o)
LIB_TARGET = libmnfinfo.so

CFLAGS += -Wall -Wextra -Wpedantic -Werror -Wno-missing-braces -Wno-unused-function -Wno-unused-parameter -fPIC -I src/include
LDFLAGS += -L.

MNF_LDFLAGS =
MNF_RPC_LDFLAGS = -lmnfinfo

RM = rm -f

all: $(LIB_TARGET) $(MNF_RPC_TARGET) $(MNF_TARGET)

$(LIB_TARGET): $(LIB_OBJS)
	$(CC) $(LDFLAGS) -shared -o $@ $^

$(MNF_RPC_TARGET): $(MNF_RPC_OBJS) $(LIB_TARGET)
	$(CC) $(LDFLAGS) $(MNF_RPC_LDFLAGS) -shared -o $@ $^

$(MNF_TARGET): $(MNF_OBJS) $(LIB_OBJS)
	$(CC) $(LDFLAGS) $(MNF_LDFLAGS) -o $@ $^

clean:
	$(RM) $(LIB_OBJS) $(LIB_TARGET)
	$(RM) $(MNF_RPC_OBJS) $(MNF_RPC_TARGET)
	$(RM) $(MNF_OBJS) $(MNF_TARGET)
