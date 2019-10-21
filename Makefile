
# Pull in helper rules

include Rules.pre.mk

# Default rule

all : test

# Link executables and run tests

test : $(BINDIR)/test_example.exe

$(BINDIR)/test_example.exe : $(OBJDIR)/test_example.o | $(BINDIR) $(TESTDIR)

#	Link executable
	vlink -bamigahunk -o $@ $^

##	Run tests
#	testrunner-68k --junit $(TESTDIR)/test_DecodeBitStream_3Bits.xml $(BINDIR)/test_DecodeBitStream_3Bits.exe

# Pull in helper rules

include Rules.post.mk
