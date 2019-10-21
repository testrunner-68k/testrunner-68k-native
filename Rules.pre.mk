
# Recursive wildcard definition (taken from https://blog.jgc.org/2011/07/gnu-make-recursive-wildcard-function.html)
rwildcard=$(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2) $(filter $(subst *,%,$2),$d))

# Perform dependency tracking on all assembly files in folder structure
SRCS := $(call rwildcard,,*.s)

# intermediate directory for generated object files
OBJDIR := .o
# intermediate directory for generated dependency files
DEPDIR := .d
# intermediate directory for executables
BINDIR := bin
# intermediate directory for test results
TESTDIR := junit

# object files, auto generated from source files
OBJS := $(patsubst %,$(OBJDIR)/%.o,$(basename $(SRCS)))
# dependency files, auto generated from source files
DEPS := $(patsubst %,$(DEPDIR)/%.d,$(basename $(SRCS)))

# Implicit rule for assembling individual assembly source files
$(OBJDIR)/%.o : %.s $(DEPDIR)/%.d | $(DEPDIR) $(OBJDIR)

#	Generate dependency information
ifeq ($(OS),Windows_NT)

	@echo $@ : $(shell vasmm68k_mot -quiet -depend=make $<) > $(DEPDIR)/$*.d
	@echo $(shell vasmm68k_mot -quiet -depend=make $<): >> $(DEPDIR)/$*.d

else

	@echo "$@ : $(shell vasmm68k_mot -quiet -depend=make $<)" > $(DEPDIR)/$*.d
	@echo "$(shell vasmm68k_mot -quiet -depend=make $<):" >> $(DEPDIR)/$*.d

endif

#	Assemble file
	vasmm68k_mot -quiet -Fhunk -o $@ $<
