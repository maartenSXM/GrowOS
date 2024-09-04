# This Makefile uses a git submodule cpptext to build GOS esphome projects
# from ./projects/*.mk.  Use "make PRJ=projects/<prj>/<prj>.mk" to
# select a specific GOS project file otherwise $(GOS_PROJECT_DIR_DEFAULT)
# is selected.
#
# There is also a makeall.sh script that will build all the GOS projects
# for a specific BSP.

GOS_PROJECT_DIR_DEFAULT := projects/growBoard0/debug.mk

# A GrowOS project file must defines the following make variables:
#  - GOS_APP_PATH    to define the project's application yaml file path
#  - GOS_CONFIG_FILE to define the project's config file header file path
#  - GOS_BSP_DIR    to define the project's board support package directory path

# GOS project files are typically found in ./projects/ but can reside
# anywhere inside or outside of the GOS file tree as long as the path
# specified is relative to this directory.

# Makefile variables set below with ?= can be overridden by a project file.
# Makefile variables set below with += can be prepended  by a project file.

# Makefile variables starting with CPT_ are for cpptext/Makefile.cpptext.
# Makefile variables starting with ESP_ are for cpptext/Makefile.esphome.

# Makefile variables starting with GOS_ are used by this Makefile and
# by the selected GOS project file.  Some are also available to yaml
# since GOS preprocesses yaml. Those are also passed further down to
# platformio builds for use in C and C++ code. Refer to the definition
# of CPT_EXTRA_DEFS below for more comments on exactly which GOS_
# variables are passed, and how they are defined.

ifeq (,$(MAKECMDGOALS))
MAKECMDGOALS := all
endif
MAKE         := $(MAKE) --no-print-directory
MAKEFILE     := $(lastword $(MAKEFILE_LIST))

# If cpptext submodule is empty, this must be a fresh clone - so get submodules.

ifeq (,$(wildcard cpptext/.git))
  ifneq (,$(BAIL))
    $(error $(MAKEFILE): make loop detected. Bailing out.)
  endif
$(MAKECMDGOALS): 
	@printf "$(MAKEFILE): Retreiving submodule cpptext\n"
	git submodule init
	git submodule update
	cd cpptext && git checkout main
	@printf "$(MAKEFILE): Restarting \"make $(MAKECMDGOALS)\"\n"
	$(MAKE) BAIL=1 $(MAKECMDGOALS)
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
    ifeq (,$(wildcard $(GOS_PROJECT_DIR_DEFAULT)))
      $(error $(MAKEFILE): GOS_PROJECT_DIR_DEFAULT not found)
    endif
    PRJ := $(GOS_PROJECT_DIR_DEFAULT)
  endif
endif

GOS_PROJECT_FILE = $(PRJ)			    # project make file
GOS_PROJECT_DIR  = $(patsubst %/,%,$(dir $(PRJ)))   # project directory

# Include the selected GOS project (Makefile fragment).

include $(GOS_PROJECT_FILE)

ifeq (,$(GOS_CONFIG_FILE))
  $(error Makefile: $(GOS_PROJECT_FILE) did not define GOS_CONFIG_FILE)
endif
ifeq (,$(GOS_BSP_DIR))
  $(error Makefile: $(GOS_PROJECT_FILE) did not define GOS_BSP_DIR)
endif
ifeq (,$(GOS_APP_PATH))
  $(error Makefile: $(GOS_PROJECT_FILE) did not define GOS_APP_PATH)
endif

# This strips the directory and suffix, if any, for further use below

PROJECT_NAME := $(basename $(notdir $(GOS_PROJECT_DIR)))
MAKE_NAME    := $(basename $(notdir $(GOS_PROJECT_FILE)))
BSP_NAME     := $(basename $(notdir $(GOS_BSP_DIR)))
APP_NAME     := $(basename $(notdir $(GOS_APP_PATH)))

# CPT_BUILD_DIR is were GOS projects are built.  It can be changed here. The
# GOS_HOME C preprocessor definition, set below, may also have to change if
# the depth of CPT_BUILD_DIR changes. Refer to the GOS_HOME comments below
# for more details.

CPT_BUILD_DIR := build/$(PROJECT_NAME)_$(MAKE_NAME)
BUILD_LOG := $(CPT_BUILD_DIR)/makeall.log

# restart 'make all' with logging to $(CPT_BUILD_DIR)/build.log and console
ifeq (all,$(GOS_AUTOLOG)$(MAKECMDGOALS))
  SHELL:=bash
all:
	@$(MAKE) -k GOS_AUTOLOG=1 $(MAKECMDGOALS) |& tee $(BUILD_LOG)
	@printf "Makefile: \"make all\" log is $(BUILD_LOG)\n"
else

# CPT_GEN is the set of files that cpptext runs the C preprocessor on.
# They can include files from CPT_SRCS (defined below) since the cpptext
# tool arranges that de-commented copies are included, not the originals.

CPT_GEN  ?= partitions.csv espinit.yaml

GOS_DIRS ?= gos apps gui bsps/common $(GOS_BSP_DIR) $(GOS_PROJECT_DIR)

# CPT_SRCS is the set of files that cpptext will remove hash-style
# comments from while leaving any C preprocessor directives so that
# the file can subsequently be used as a #include by one of the
# CPT_GEN files.

# Builds the list of CPT_SRCS by looking for .yaml files in $(GOS_DIRS).
# Note that any yaml files in the BSP and project directories are
# added also.

CPT_SRCS += $(foreach d,$(GOS_DIRS),$(wildcard $(d)/*.yaml)) $(CPT_GEN)

# In addition to updates to $(CPT_SRCS) triggering a rebuild of esphome.yaml,
# updates to source files in $(ESP_DEPS) are also triggers.

GOS_DEPS ?= utils libraries/console

ESP_DEPS += $(foreach d,$(GOS_DEPS),$(wildcard $(d)/*.c) \
		$(wildcard $(d)/*.cpp) $(wildcard $(d)/*.h))

# If there is a secrets.h file in ./ or ../, use it

ifneq (,$(wildcard ./secrets.h))
  CPT_EXTRA_FLAGS += -include ./secrets.h
else
  ifneq (,$(wildcard ../secrets.h))
    CPT_EXTRA_FLAGS += -include ../secrets.h
  endif
endif

# GOS_BSP_DIR is added to include paths so #include "bsp.h" is BSP-specific.

CPT_EXTRA_INCS += -I $(GOS_BSP_DIR)

# Note that #ifdef GOS_PROJECT_<ppp>   is for project adapation anywhere
#       and #ifdef GOS_BSP_<bbb>       is for bsp adaptation anywhere
#       and #ifdef GOS_MAKE_<mmm>      is for $(GOS_CONFIG_FILE) adaptation
#       and #ifdef GOS_APP_<aaa>       is for app adaptation anywhere
#       and #ifdef GOS_USER_<username> is for user adaptation anywhere
# where ppp/aaa/ccc/bbb are the basenames from the project, config
# and app file paths and the bsp directory path, respectively.
# These are also passed into C/C++ code.  If changed, update gos/env.h.

CPT_EXTRA_DEFS += -D GOS_HOME=../..				\
		  -D GOS_BUILD_PATH=$(CPT_BUILD_DIR)		\
		  -D GOS_PROJECT_DIR=$(GOS_PROJECT_DIR)		\
		  -D GOS_CONFIG_FILE=$(GOS_CONFIG_FILE)		\
		  -D GOS_BSP_DIR=$(GOS_BSP_DIR)			\
		  -D GOS_APP_PATH=$(GOS_APP_PATH)		\
		  -D GOS_PROJECT_NAME=$(PROJECT_NAME)		\
		  -D GOS_MAKE_NAME=$(MAKE_NAME)			\
		  -D GOS_BSP_NAME=$(BSP_NAME)			\
		  -D GOS_APP_NAME=$(APP_NAME)			\
		  -D GOS_USER_NAME=$(USER)			\
		  -D GOS_PROJECT_$(PROJECT_NAME)		\
		  -D GOS_MAKE_$(MAKE_NAME)			\
		  -D GOS_BSP_$(BSP_NAME)			\
		  -D GOS_APP_$(APP_NAME)			\
		  -D GOS_USER_$(USER)

# The reason GOS_HOME is set above to two levels up i.e. ../.. is because
# the generated esphome.yaml ends up in $(CPT_BUILD_DIR) which is
# build/$(PROJECT_NAME) which is two levels down from this directory.
# GOS_HOME is used in yaml or C/C++ files to refer to files and directories
# under this directory and is define as a relative so that build trees
# are reproducable regardless of where they are built.

# Now include the cpptext Makefile fragment that will dehash GOS yamls files.
# In turn, it will include cpptext/Makefile.esphome which handles the esphome
# file generation and platformio build steps.

include cpptext/Makefile.cpptext

define print_bsp_has_rule
print-hardware::
	@printf "BSP hardware definitions for $(1) are:\n"
	@$(CPT_CPP) -dM $(CPT_TMP_DIR)/$(1) | grep GOS_BSP_HAS
endef

$(foreach gen,$(patsubst ./%,%,$(CPT_GEN)), \
    $(eval $(call print_bsp_has_rule,$(gen))))

# this endif is from the autolog restart
endif

# This last endif is needed from the git submodule install check way above

endif

