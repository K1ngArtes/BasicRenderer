# HIL: No spaces or comments after otherwise it captures them!
# Determine the platform
UNAME_S := $(shell uname -s)

# CC
ifeq ($(UNAME_S),Darwin)
    CC := clang++ -arch x86_64
else
    CC := g++
endif

# Folders
SRCDIR := src
BUILDDIR := build
TARGETDIR := bin

# Targets
EXECUTABLE := BasicRenderer
TARGET := $(TARGETDIR)/$(EXECUTABLE)

# Final paths
INSTALLBINDIR := /usr/local/bin

# Code Lists
SRCEXT := cpp
SOURCES := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
# $(patsubst pattern,replacement,text)
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))

# Folder Lists
INCDIRS := $(shell find include/** -name '*.h' -exec dirname {} \; | sort | uniq)
INCLIST := $(patsubst include/%,-I include/%,$(INCDIRS))
# BUILDLIST := $(patsubst include/%,$(BUILDDIR)/%,$(INCDIRS))
BUILDLIST := build/include

#  Flags
# use c++11, don't show warnings (most associated with OpenGL deprecation)
# and enable debugging
CFLAGS := -std=c++11 -Wno-deprecated-declarations -g
LIB := -L /usr/local/lib -lglfw -framework OpenGL
INC := -I /usr/local/include -I include

$(TARGET): $(OBJECTS)
	@echo "$(INCDIRS)"
	@mkdir -p $(TARGETDIR)
	@echo "Linking..."
	@echo "$(CC) $^ -o $(TARGET) $(LIB)"
	@echo "Linking $(TARGET)"; $(CC) $^ -o $(TARGET) $(LIB)

$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(BUILDLIST)
	@echo "LOL: $(CC) $(CFLAGS) $(INC) $(INCLIST) -c -o $@ $<"
	@echo "Compiling $<..."; $(CC) $(CFLAGS) $(INC) $(INCLIST) -c -o $@ $<

clean:
	@echo "Cleaning $(TARGET)..."; $(RM) -r $(BUILDDIR) $(TARGET)

install:
	@echo "Installing $(EXECUTABLE)..."; cp $(TARGET) $(INSTALLBINDIR)
  
distclean:
	@echo "Removing $(EXECUTABLE)"; rm $(INSTALLBINDIR)/$(EXECUTABLE)

.PHONY: clean