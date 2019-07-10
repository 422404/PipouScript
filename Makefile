###############################################################################
# PipouScript                                                                 #
###############################################################################

TARGET   := pipou
VERSION  := 1.0.0
DIRS     := PipouScript Parser Common
BUILD    := $(CURDIR)/Build

###############################################################################

INCLUDES := $(foreach dir,$(DIRS),-I$(CURDIR)/$(dir)/include) \
			-I$(CURDIR)
SOURCES  := $(foreach dir,$(DIRS),$(wildcard $(CURDIR)/$(dir)/*.c))
OBJS      = $(call srcs2objs,$(SOURCES))

###############################################################################

src2obj   = $(subst $(CURDIR),$(BUILD),$(1:.c=.o))
srcs2objs = $(foreach source,$(1),$(call src2obj,$(source)))
obj2src   = $(subst $(BUILD),$(CURDIR),$(1:.o=.c))

###############################################################################

CC       := gcc
CFLAGS   := -g -Wall -Wextra -DBUILD_VERSION=\"$(VERSION)\"
LDFLAGS  :=

###############################################################################

.PHONY: all clean paths tests doc

all: $(TARGET) doc

clean:
	@rm -Rf $(BUILD)

paths:
	@echo "Includes:\n$(INCLUDES)\n"
	@echo "Sources :\n$(SOURCES)\n"
	@echo "Objects :\n$(OBJS)\n"

tests:
	@make -f Tests.mk

doc:
	@doxygen

###############################################################################

$(TARGET): $(OBJS)
	@echo "\nLinking main executable..."
	$(CC) $(CFLAGS) $(INCLUDES) -o $(BUILD)/$@ $^ $(LDFLAGS)
	@echo "Done."

$(OBJS): $(SOURCES)
	@echo "\nBuilding... $@"
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDES) -c $(call obj2src,$@) -o $@
	@echo "Done."
