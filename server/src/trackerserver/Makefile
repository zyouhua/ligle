EXE_NAME  = trackerd
EXE_PATH  = ./

CC        = g++ -g
#CFLAGS    = -g -gstabs
CFLAGS    =
ISE_PATH  = ../ise/
LIBS      = -L/usr/lib/mysql -lmysqlclient -lz -lpthread
INCS      = -I./ -I$(ISE_PATH) -I/usr/include/mysql

ISEOBJS   = $(ISE_PATH)ise_system.o \
            $(ISE_PATH)ise_classes.o \
            $(ISE_PATH)ise_sysutils.o \
            $(ISE_PATH)ise_socket.o \
            $(ISE_PATH)ise_server.o \
            $(ISE_PATH)ise_database.o \
            $(ISE_PATH)ise_db_mysql.o \
            $(ISE_PATH)ise_main.o

MODOBJS   = packet_base.o packet_udp.o packet_dtp.o \
            database.o dispatcher.o utilities.o hash_sha1.o \
            xmldoc.o configmgr.o messages.o \
            resource.o extension.o svrmodmgr.o user.o cfinderpacket.o  file.o \
            finder.o  


OBJS      = $(ISEOBJS) $(MODOBJS)

all: $(EXE_NAME)

.cpp.o: 
	$(CC) -c $(CFLAGS) $(INCS) $< -o $(patsubst %.cpp,%.o,$<)

$(EXE_NAME): $(OBJS)
	$(CC) -o $(EXE_NAME) $(OBJS) $(LIBS)

clean:
	rm -f *.o core $(EXE_NAME) $(ISE_PATH)*.o 

cleancur:
	rm -f *.o core $(EXE_NAME) 
