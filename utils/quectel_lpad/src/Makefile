CROSS_COMPILE ?=
TARGET ?= quectel_lpad

CC := $(CROSS_COMPILE)gcc
LD := $(CROSS_COMPILE)ld
OBJCOPY := $(CROSS_COMPILE)objcopy
OBJDUMP := $(CROSS_COMPILE)objdump

INCDIRS := 	common/ \
		qmi_manager/ \
		qmi_manager/legacy_qmi/ \
		http_manager/ \

INCLUDE := $(patsubst %, -I %, $(INCDIRS))

SRCDIRS := 	app/ \
		common/ \
		qmi_manager/ \
		qmi_manager/legacy_qmi/ \
		http_manager/ \

CFILES := $(foreach dir, $(SRCDIRS), $(wildcard $(dir)/*.c))

CFILENDIR := $(notdir $(CFILES))
COBJS := $(patsubst %, obj/%, $(CFILENDIR:.c=.o))
OBJS := $(COBJS)
VPATH := $(SRCDIRS)

CFLAGS += -Wall -Werror
LDFLAGS += -lpthread -ldl -lrt -lcurl

release: clean
	$(CC) $(CFLAGS) $(INCLUDE) $(CFILES) -o $(TARGET) $(LDFLAGS) -g

clean :
	rm -rf *.o $(TARGET)
