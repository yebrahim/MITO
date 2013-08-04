CC = g++
program_CXX_SRCS := libmito.cpp buffer.cpp
program_CXX_OBJS := ${program_CXX_SRCS:.cpp=.o}
program_OBJS := $(program_C_OBJS) $(program_CXX_OBJS)
program_LIBRARIES := pthread

ifdef vtune
	vtune_path_base := $(shell which amplxe-gui)
	vtune_path_child := $(shell dirname $(vtune_path_base) )
	vtune_path := $(shell dirname $(vtune_path_child) )
	program_INCLUDE_DIRS := $(vtune_path)/include/
	program_LIBRARY_DIRS := $(vtune_path)/lib64/
	WITH_VTUNE := -DVTUNE_LABELS
	LDFLAGS += -littnotify
endif

CPPFLAGS := -O0 -D_FILE_OFFSET_BITS=64
CPPFLAGS += $(foreach includedir,$(program_INCLUDE_DIRS),-I$(includedir))
LDFLAGS += $(foreach librarydir,$(program_LIBRARY_DIRS),-L$(librarydir))
LDFLAGS += $(foreach library,$(program_LIBRARIES),-$(library))

all: static
   
static:
	$(CC) $(CPPFLAGS) $(WITH_VTUNE) -c $(LDFLAGS) $(program_CXX_SRCS)
	ar -srv libmito.a $(program_OBJS)
	$(CC) $(CPPFLAGS) main.cpp -L./ -lmito -o main.o $(LDFLAGS)

serial:
	$(CC) $(CPPFLAGS) serial_simple.cpp routine.cpp -o serial_simple.o $(LDFLAGS)

clean:
	@- $(RM) $(program_NAME)
	@- $(RM) $(program_OBJS)
   
distclean: clean
