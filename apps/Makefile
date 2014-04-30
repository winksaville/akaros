# Default target is to just make
PHONY += all fill-kfs unfill-kfs clean
all:

# TODO: when we clean this up, if we ditch OBJDIR, change the root makefile
APPS_DIR = apps

CFLAGS_APPS += $(CFLAGS_USER) -g
APPS_CXXFLAGS += $(CXXFLAGS_USER) -g

APPS_LDLIBS := -lpthread -lbenchutil -lm -liplib -lndb

APPS_SRCS_C := $(wildcard $(APPS_DIR)/*.c)
APPS_SRCS_CPP := $(wildcard $(APPS_DIR)/*.cc)

APPS_LDDEPENDS_C := $(APPS_DIR)/%.c 
APPS_LDDEPENDS_CPP := $(APPS_DIR)/%.cc

APPS_EXECS_C  = $(patsubst $(APPS_DIR)/%.c, \
                            $(OBJDIR)/$(APPS_DIR)/%, \
                            $(APPS_SRCS_C))

APPS_EXECS_CPP  = $(patsubst $(APPS_DIR)/%.cc, \
                              $(OBJDIR)/$(APPS_DIR)/%, \
                              $(APPS_SRCS_CPP))

APPS_EXECS = $(APPS_EXECS_C) $(APPS_EXECS_CPP)

include $(APPS_DIR)/openmp/Makefrag

STATIC := $(findstring static,$(CFLAGS_APPS))
$(OBJDIR)/$(APPS_DIR)/%: $(APPS_LDDEPENDS_C)
	@echo + cc [APPS] $<
	@mkdir -p $(@D)
	$(Q)$(CC) $(CFLAGS_APPS) -o $@ $< $(APPS_LDLIBS)
	@if [ "$(STATIC)" != "static" ]; then \
		$(OBJDUMP) -S $@ > $@.asm; \
		$(NM) -n $@ > $@.sym; \
	fi

# Note that we don't disassemble CPPs by default, even if they aren't static.
# The files are pretty large regardless (9MB for a simple stream test asm).
$(OBJDIR)/$(APPS_DIR)/%: $(APPS_LDDEPENDS_CPP)
	@echo + cc [APPS] $<
	@mkdir -p $(@D)
	$(Q)$(CPP) $(APPS_CXXFLAGS) -o $@ $< $(APPS_LDLIBS)

all: $(APPS_EXECS)
	@:

fill-kfs: $(APPS_EXECS)
	@mkdir -p $(FIRST_KFS_PATH)/bin
	$(Q)cp -uP $^ $(FIRST_KFS_PATH)/bin
	@echo "User space apps installed to KFS"

unfill-kfs:
	$(Q)rm -rf $(addprefix $(FIRST_KFS_PATH)/bin/, $(notdir $(APPS_EXECS)))
	@echo "User space apps removed from KFS"

clean:
	@echo + clean [APPS]
	@rm -rf $(OBJDIR)/apps/

.PHONY: $(PHONY)
