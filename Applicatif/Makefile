
CC = gcc

CFLAGS = -DK7_TRD -I $(PWD)
LFLAGS = -lpthread
ifeq ($(APP_MODE),RAWETHERNET)
CFLAGS += -DRAW_ETH
endif

GUI_OBJS = App.o 
CLEAN = clean
EXEC = APP

all : $(CLEAN) $(EXEC)

APP:	$(GUI_OBJS)
	$(CC) $(CFLAGS) $(GUI_OBJS) $(LFLAGS) -lpng -o App

App.o:	threads.c 
	$(CC) -c $(CFLAGS) -I threads.c -o App.o

clean:
	rm -f $(GUI_OBJS) App  xt_app_comm_socket
