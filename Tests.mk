###############################################################################
# PipouScript - tests                                                         #
###############################################################################

TARGET   := pipou-tests
VERSION  := 1.0.0
DIRS     := PipouScript Parser Common Tests
BUILD    := $(CURDIR)/Build
MAIN     := $(CURDIR)/PipouScript/main.c

###############################################################################

INCLUDES := $(foreach dir,$(DIRS),-I$(CURDIR)/$(dir)/include) \
			-I$(CURDIR)
SOURCES  := $(filter-out $(MAIN),$(foreach dir,$(DIRS),$(wildcard $(CURDIR)/$(dir)/*.c)))
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

.PHONY: all clean paths $(TARGET)

all: $(TARGET)

clean:
	@rm -Rf $(BUILD)

paths:
	@echo "Includes:\n$(INCLUDES)\n"
	@echo "Sources :\n$(SOURCES)\n"
	@echo "Objects :\n$(OBJS)\n"

###############################################################################

$(TARGET): $(BUILD)/$(TARGET)

$(BUILD)/$(TARGET): $(OBJS)
	@echo "\nLinking main test executable..."
	$(CC) $(CFLAGS) $(INCLUDES) -o $(BUILD)/$(TARGET) $^ $(LDFLAGS)
	@echo "Done."

$(OBJS): $(SOURCES)
	@echo "\nBuilding test... $@"
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDES) -c $(call obj2src,$@) -o $@
	@echo "Done."
