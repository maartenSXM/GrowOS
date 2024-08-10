# This Makefile" is based on github.com/maartenSXM/cpptext/blob/main/Makefile
#
# This Makefile installs cpptext from git into ./builds/cpptext, if necessary,
# and then builds GOS esphome projects from projects/*.mk using cpptext's
# Makefile fragments as configured by the selected GOS project.

# Use "make PRJ=projects/project.mk" to select a specific GOS project
# otherwise $(GOS_DEFAULT_PROJECT) is selected.

# make variables starting with CPT_ are used by $(CPT_HOME)/Makefile.cpptext.
# Variables starting with CPT_ESP_ are used by $(CPT_HOME)/Makefile.esphome.

# Variables that start with GOS_ are used by this Makefile and by
# whatever GOS project files (makefile fragment) included below.

GOS_DEFAULT_PROJECT := projects/growBoard0.mk

MAKECMDGOALS ?= all
MAKE         := $(MAKE) --no-print-directory
MAKEFILE     := $(lastword $(MAKEFILE_LIST))

# Check that $(CPT_HOME) exists and if it doesn't, install it.

CPT_HOME       := build/cpptext

ifeq (,$(wildcard $(CPT_HOME)))

$(MAKECMDGOALS): 
	@printf "$(MAKEFILE): installing cpptext from $(CPT_URL).\n"
	mkdir -p $(dir $(CPT_HOME))
	git -C $(dir CPT_HOME)) clone git@github.com:maartenSXM/cpptext
	@printf "$(MAKEFILE): cpptext installed. Restarting make.\n"
	$(MAKE) $(MAKECMDGOALS)
else

# The rest of this Makefile is inside the 'ifeq' cpptext check from above.

# Check if PRJ= was specified on the command line to select a GOS project.

ifneq (,$(PRJ))
  ifeq (,$(wildcard $(PRJ)))	    # check specified GOS project exists
    $(error $(MAKEFILE): $(PRJ) not found)
  endif
  $(shell echo $(PRJ) >.gosprj)	    # remember the project in .gosprj
else
  ifneq (,$(wildcard .gosprj))	    # check if a GOS project is remembered
    PRJ := $(shell cat .gosprj)
  else
    PRJ : = $(GOS_DEFAULT_PROJECT)
  endif
endif

# Include the selected GOS project Makefile fragment. A GOS project
# at minimum defines an application (GOS_APP), a config file (GOS_CONFIG)
# and a path to a board support package directory (GOS_BSP).

# GOS projects are typically found in ./projects/ but can reside anywhere
# inside or outside of the GOS file tree as long as the paths specified
# are relative to GOS_HOME.

# The variables set below with ?= can be overridden by a GOS project file.
# The variables set below with += can be prepended by a GOS project file.

include $(PRJ)

# PRJNAME is the project file path minus the directory and suffix, if any.

PRJNAME       := $(basename $(notdir $(PRJ)))

# The cpptext build dir for GOS projects

CPT_BUILD_DIR := build/$(PRJNAME)

# CPT_GEN is the set of files that cpptext runs the C preprocessor on.
# They can include files from CPT_SRCS (defined below) since the cpptext
# tool arranges that de-commented copies are included, not the originals.

CPT_GEN  ?= partitions.csv gosInit.yaml

GOS_DIRS ?= gos apps ui bsps/common $(GOS_BSP)

# CPT_SRCS is the set of files that cpptext will remove hash-style
# comments from while leaving any C preprocessor directives so that
# the file can subsequently be used as a #include by one of the
# CPT_GEN files.

# Builds the list of CPT_SRCS by looking for .yaml files in $(GOS_DIRS).

CPT_SRCS += $(foreach d,$(GOS_DIRS),$(wildcard $(d)/*.yaml)) $(CPT_GEN)

# The next two defines are used by $(CPT_HOME)/Makefile.esphome.

# CPT_ESP_INIT is the file to run yamlmerge.sh on so that esphome
# map keys (AKA esphome components) can be repeated.  Otherwise,
# the yaml spec requires that only one yaml map key, such as
# esphome's "sensor:", can exist.

CPT_ESP_INIT ?= gosInit.yaml

# CPT_ESP_YAML is the output of $(CPT_HOME)/yamlmerge.sh which has
# all the esphome component definitions merged together to form
# legitimate yaml that esphome tools can parse.

CPT_ESP_YAML ?= esphome.yaml

# In addition to updates to $(CPT_SRCS) triggering a rebuild of esphome.yaml,
# updates to source files in $(CPT_ESP_DEPS) are also triggers.

GOS_DEPS ?= utils libraries/libtelnet libraries/console

CPT_ESP_DEPS += $(foreach d,$(GOS_DEPS),$(wildcard $(d)/*.c) \
		$(wildcard $(d)/*.cpp) $(wildcard $(d)/*.h))

# Add GOS_BSP to include paths so that #include "bsp.h" is BSP-specific
CPT_EXTRA_INCS += -I $(GOS_BSP)

# As with PRJNAME, CFGNAME & BSPNAME strip the directory and suffix, if any.

CFGNAME := $(basename $(notdir $(GOS_CONFIG)))
BSPNAME := $(basename $(notdir $(GOS_BSP)))
APPNAME := $(basename $(notdir $(GOS_APP)))

# Note #ifdef GOS_PROJECT_<ppp> can be used for project-specific code
#  and #ifdef GOS_APP_<aaa>     can be used for app-specific code
#  and #ifdef GOS_CONFIG_<ccc>  can be used for config-specific code
#  and #ifdef GOS_BSP_<bbb>     can be used for bsp-specific code
# where ppp/aaa/ccc/bbb are the basenames from the project, config
# and app file names and the bsp directory path name, respectively.

CPT_EXTRA_DEFS += -D GOS_HOME=../..			\
		  -D GOS_PROJDIR=$(CPT_BUILD_DIR)	\
		  -D GOS_PROJECT_PATH=$(PRJ)		\
		  -D GOS_PROJECT_NAME=$(PRJNAME)	\
		  -D GOS_PROJECT_$(PRJNAME)=1		\
		  -D GOS_APP_PATH=$(GOS_APP)		\
		  -D GOS_APP_NAME=$(APPNAME)		\
		  -D GOS_APP_$(APPNAME)=1		\
		  -D GOS_CONFIG_PATH=$(GOS_CONFIG)	\
		  -D GOS_CONFIG_NAME=$(CFGNAME)		\
		  -D GOS_CONFIG_$(CFGNAME)=1		\
		  -D GOS_BSP_PATH=$(GOS_BSP)		\
		  -D GOS_BSP_NAME=$(BSPNAME)		\
		  -D GOS_BSP_$(BSPNAME)=1		\
		  -D GOS_USER=$(USER)			\
		  -D GOS_USER_$(USER)=1

# GOS_HOME is set above to two levels up i.e. ../.. because the
# generated esphome.yaml ends up in build/$(PRJNAME) which is
# two levels down from here. It can be used in yaml or C/C++
# files to refer to files and directories under this directory.

include $(CPT_HOME)/Makefile.cpptext

endif	# From the ifeq check for cpptext at the top of this Makefile.

