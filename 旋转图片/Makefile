
# 
# Includes build rules for a2test and ppmtrans.
#

############## Variables ###############

CC = gcc # The compiler being used

# Updating include path to use Comp 40 .h files and CII interfaces
IFLAGS = -I/comp/40/build/include -I/usr/sup/cii40/include/cii

# Compile flags
# We need to use the GNU 99 standard to get the right items in time.h for the
# the timing support to compile.
# 
CFLAGS = -g -std=gnu99 -Wall -Wextra -Werror -Wfatal-errors -pedantic $(IFLAGS)

# Linking flags
# Set debugging information and update linking path
# to include course binaries and CII implementations
LDFLAGS = -g -L/comp/40/build/lib -L/usr/sup/cii40/lib64

# Libraries needed for linking
# All programs cii40 (Hanson binaries) and *may* need -lm (math)
# 40locality is a catch-all for this assignment, netpbm is needed for pnm
# rt is for the "real time" timing library, which contains the clock support
LDLIBS = -l40locality -lnetpbm -lcii40 -lm -lrt

# Collect all .h files in your directory.
# This way, you can never forget to add
# a local .h file in your dependencies.
#
INCLUDES = $(shell echo *.h)

############### Rules ###############

all: ppmtrans a2test timing_test


## Compile step (.c files -> .o files)

# To get *any* .o file, compile its .c file with the following rule.
%.o: %.c $(INCLUDES)
	$(CC) $(CFLAGS) -c $< -o $@


## Linking step (.o -> executable program)

a2test: a2test.o uarray2b.o uarray2.o a2plain.o
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

timing_test: timing_test.o cputiming.o
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS) 

ppmtrans: ppmtrans.o cputiming.o uarray2b.o uarray2.o a2plain.o a2blocked.o
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

clean:
	rm -f ppmtrans a2test timing_test a2test2 *.o

