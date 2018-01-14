CC=gcc
UNAME := $(shell uname)
# Assume target is Mac OS if build host is Mac OS; any other host targets Linux
ifeq ($(UNAME), Darwin)
	DEPENDENCIES := -lpthread -lobjc -framework IOKit -framework CoreFoundation
else
	DEPENDENCIES := -lpthread -lrt
endif

LBITS := $(shell getconf LONG_BIT)
ifeq ($(LBITS),64)
   # do 64 bit stuff here, like set some CFLAGS
   CFLAGS=ftdi/x64/libftd2xx.a $(DEPENDENCIES) -Wall -Wextra
else
   # do 32 bit stuff here
   CFLAGS=ftdi/x86/libftd2xx.a $(DEPENDENCIES) -Wall -Wextra
endif



APP = relaycontrol

all: $(APP);chmod 755 $(APP)

$(APP): main.c	
	$(CC) main.c -o $(APP) $(CFLAGS)
	
clean:
	-rm -f *.o ; rm $(APP)
