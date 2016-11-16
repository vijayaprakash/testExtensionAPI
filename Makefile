#-
# ==========================================================================
# Copyright (c) 2011 Autodesk, Inc.
# All rights reserved.
# 
# These coded instructions, statements, and computer programs contain
# unpublished proprietary information written by Autodesk, Inc., and are
# protected by Federal copyright law. They may not be disclosed to third
# parties or copied or duplicated in any form, in whole or in part, without
# the prior written consent of Autodesk, Inc.
# ==========================================================================
#+

ifndef INCL_BUILDRULES

#
# Include platform specific build settings
#
TOP := ..
include $(TOP)/buildrules


#
# Always build the local plug-in when make is invoked from the
# directory.
#
all : plugins

endif

#
# Variable definitions
#

SRCDIR := $(TOP)/testExtension
DSTDIR := $(TOP)/testExtension

testExtension_SOURCES  := $(TOP)/testExtension/testExtensionAPI.cpp
testExtension_OBJECTS  := $(TOP)/testExtension/testExtensionAPI.o
testExtension_PLUGIN   := $(DSTDIR)/testExtension.$(EXT)
testExtension_MAKEFILE := $(DSTDIR)/Makefile

#
# Include the optional per-plugin Makefile.inc
#
#    The file can contain macro definitions such as:
#       {pluginName}_EXTRA_CFLAGS
#       {pluginName}_EXTRA_C++FLAGS
#       {pluginName}_EXTRA_INCLUDES
#       {pluginName}_EXTRA_LIBS
-include $(SRCDIR)/Makefile.inc


#
# Set target specific flags.
#

$(testExtension_OBJECTS): CFLAGS   := $(CFLAGS)   $(testExtension_EXTRA_CFLAGS)
$(testExtension_OBJECTS): C++FLAGS := $(C++FLAGS) $(testExtension_EXTRA_C++FLAGS)
$(testExtension_OBJECTS): INCLUDES := $(INCLUDES) $(testExtension_EXTRA_INCLUDES)

depend_testExtension:     INCLUDES := $(INCLUDES) $(testExtension_EXTRA_INCLUDES)

$(testExtension_PLUGIN):  LFLAGS   := $(LFLAGS) $(testExtension_EXTRA_LFLAGS) 
$(testExtension_PLUGIN):  LIBS     := $(LIBS)   -lOpenMaya -lFoundation $(testExtension_EXTRA_LIBS) 

#
# Rules definitions
#

.PHONY: depend_testExtension clean_testExtension Clean_testExtension


$(testExtension_PLUGIN): $(testExtension_OBJECTS) 
	-rm -f $@
	$(LD) -o $@ $(LFLAGS) $^ $(LIBS)

depend_testExtension :
	makedepend $(INCLUDES) $(MDFLAGS) -f$(DSTDIR)/Makefile $(testExtension_SOURCES)

clean_testExtension:
	-rm -f $(testExtension_OBJECTS)

Clean_testExtension:
	-rm -f $(testExtension_MAKEFILE).bak $(testExtension_OBJECTS) $(testExtension_PLUGIN)


plugins: $(testExtension_PLUGIN)
depend:	 depend_testExtension
clean:	 clean_testExtension
Clean:	 Clean_testExtension

