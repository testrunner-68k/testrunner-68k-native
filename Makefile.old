
# Pull in helper rules

include Rules.pre.mk

AMIGAOS_CC=m68k-amigaos-gcc
AMIGAOS_CFLAGS=-c
AMIGAOS_LD=vlink
AMIGAOS_LDFLAGS=-bamigahunk

# Default rule

.PHONY: all test examples

all : test examples

# Link executables and run tests

test : $(BINDIR)/test_example.exe

$(BINDIR)/test_example.exe : $(OBJDIR)/test_example.o | $(BINDIR) $(TESTDIR)

#	Link executable
	vlink -bamigahunk -o $@ $^

##	Run tests
#	testrunner-68k --junit $(TESTDIR)/test_DecodeBitStream_3Bits.xml $(BINDIR)/test_DecodeBitStream_3Bits.exe

.PHONY: minimal_c

examples: minimal_c

minimal_c: $(BINDIR)/minimal_c.exe

$(BINDIR)/minimal_c.exe : $(OBJDIR)/minimal_c_hello.o | $(BINDIR)
	$(AMIGAOS_LD) $(AMIGAOS_LDFLAGS) -o $@ $^

$(OBJDIR)/minimal_c_hello.o : examples/minimal_c/hello.c | $(OBJDIR)
	$(AMIGAOS_CC) $(AMIGAOS_CFLAGS) -o $@ $^

# Pull in helper rules

include Rules.post.mk
