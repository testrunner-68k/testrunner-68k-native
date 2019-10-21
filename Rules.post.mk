
# Clean all intermediates

.PHONY: clean
clean:
ifeq ($(OS),Windows_NT)

	if exist $(OBJDIR) rd /s /q $(OBJDIR)
	if exist $(DEPDIR) rd /s /q $(DEPDIR)
	if exist $(BINDIR) rd /s /q $(BINDIR)

else

	$(RM) -r $(OBJDIR) $(DEPDIR) $(BINDIR)

endif

# Rules for creating intermediate folders, when necessary

ifeq ($(OS),Windows_NT)

$(DEPDIR): ; @md $@
$(OBJDIR): ; @md $@
$(BINDIR): ; @md $@
$(TESTDIR): ; @md $@

else

$(DEPDIR): ; @mkdir -p $@
$(OBJDIR): ; @mkdir -p $@
$(BINDIR): ; @mkdir -p $@
$(TESTDIR): ; @mkdir -p $@

endif

# Dependency handling

DEPS := $(SRCS:%.s=$(DEPDIR)/%.d)
$(DEPS):

include $(wildcard $(DEPS))