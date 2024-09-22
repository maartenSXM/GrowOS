# This Makefile uses github repo cpphash to build GOS esphome projects
# from ./projects/*.mk.  Use "make PRJ=projects/<prj>/<prj>.mk" to
# select a specific GOS project file otherwise $(GOS_PROJECT_DIR_DEFAULT)
# is selected.
#
# There is also a makeall.sh script that will build all the GOS projects
# for a specific BSP.

ifeq (,$(findstring GNU,$(shell sed --version 2>/dev/null)))
  ifeq (,$(shell which gsed))
    $(error "GNU sed not found. Did you run install.sh?")
  else
    SED=gsed
  endif
else
    SED=sed
endif

# Directory of this Makefile
_DIR := $(patsubst %/,%,$(dir $(abspath $(lastword $(MAKEFILE_LIST)))))

GOS_PROJECT_DIR_DEFAULT := projects/lilygot4s3/debug.mk

# A GrowOS project file must defines the following make variables:
#  - GOS_APP_PATH    to define the project's application yaml file path
#  - GOS_CONFIG_FILE to define the project's config file header file path
#  - GOS_BSP_DIR     to define the project's board support package path

# GOS project files are typically found in ./projects/ but can reside
# anywhere inside or outside of the GOS file tree as long as the path
# specified is relative to this directory.

# Makefile variables set below with ?= can be overridden by a project file.
# Makefile variables set below with += can be prepended  by a project file.

# Makefile variables starting with CH_ are for cpphash/cpphash.mk.
# Makefile variables starting with ESP_ are for cpphash/esphome.mk.

# Makefile variables starting with GOS_ are used by this Makefile and
# by the selected GOS project file.  Some are also available to yaml
# since GOS preprocesses yaml. Those are also passed further down to
# platformio builds for use in C and C++ code. Refer to the definition
# of CH_EXTRA_DEFS below for more comments on exactly which GOS_
# variables are passed, and how they are defined.

# default GOS_HOME to this directory
ifeq (,$(GOS_HOME))
  GOS_HOME:=$(_DIR)
endif

ifeq (,$(wildcard $(GOS_HOME)/cpphash))
  $(error $GOS_HOME/cpphash not found. Did you run install.sh?)
endif

ifeq (,$(VIRTUAL_ENV))
  $(error $$VIRTUAL_ENV not set. Did you source Bashrc?)
endif

# By default build GrowOS starting from esphome.yaml

ESP_INIT ?= esphome.yaml

ifeq (,$(MAKECMDGOALS))
MAKECMDGOALS := all
endif
MAKE         := $(MAKE) --no-print-directory GOS_HOME=$(GOS_HOME)
MAKEFILE     := $(lastword $(MAKEFILE_LIST))

# Record each distinct PRJ in .cpphash_prj_all
define _saveprj
  $(shell grep -sqxF $1 .cpphash_prj_all || echo $1 >> .cpphash_prj_all)
endef

# Check if PRJ= was specified on the command line to select a GOS project.
ifneq (,$(PRJ))
  ifeq (,$(wildcard $(PRJ)))
    $(error $(MAKEFILE): $(PRJ) not found)
  endif
  $(shell echo $(PRJ) >$(GOS_HOME)/.cpphash_prj)
  $(call _saveprj,$(PRJ))
else
  ifneq (,$(wildcard $(GOS_HOME)/.cpphash_prj))
    PRJ := $(shell cat $(GOS_HOME)/.cpphash_prj)
  else
    ifeq (,$(wildcard $(GOS_PROJECT_DIR_DEFAULT)))
      $(error $(MAKEFILE): GOS_PROJECT_DIR_DEFAULT not found)
    endif
    PRJ := $(GOS_PROJECT_DIR_DEFAULT)
    $(shell echo $(PRJ) >$(GOS_HOME)/.cpphash_prj)
    $(call _saveprj,$(PRJ))
  endif
endif

GOS_PROJECT_FILE = $(PRJ)
GOS_PROJECT_DIR  = $(patsubst %/,%,$(dir $(PRJ)))

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
ifeq (,$(wildcard $(GOS_CONFIG_FILE)))
  $(error Makefile: GOS_CONFIG_FILE not found)
endif
ifeq (,$(wildcard $(GOS_BSP_DIR)))
  $(error Makefile: GOS_BSP_DIR not found)
endif
ifeq (,$(wildcard $(GOS_APP_PATH)))
  $(error Makefile: GOS_APP_PATH not found)
endif
GOS_APP_DIR  := $(patsubst %/,%,$(dir $(GOS_APP_PATH)))

# This strips the directory and suffix, if any, for further use below

PROJECT_NAME := $(basename $(notdir $(GOS_PROJECT_DIR)))
MAKE_NAME    := $(basename $(notdir $(GOS_PROJECT_FILE)))
BSP_NAME     := $(basename $(notdir $(GOS_BSP_DIR)))
APP_NAME     := $(basename $(notdir $(GOS_APP_DIR)))

# CH_BUILD_DIR is were GOS projects are built. If changed, review
# the definition of GOS_TOP below.  It declares the relative path to
# GOS_HOME from CH_BUILD_DIR. This is done manually because realpath
# does not support the --relative-to option on many non-Linux OSs.

CH_BUILD_DIR := ./build/$(PROJECT_NAME)_$(MAKE_NAME)

GOS_LOG := $(CH_BUILD_DIR)/makeall.log

# Restart 'make all' with logging to $(CH_BUILD_DIR)/build.log and console.

# Optionally, use GOS_AUTOLOG_STDOUT_ONLY=1 on the make command line if you
# want stderr to not go to the log.  That is useful for paging through any
# errors on the command line using:
#    make GOS_AUTOLOG_STDOUT_ONLY=1 2>&1 >/dev/null | more

ifeq (all,$(GOS_AUTOLOG)$(MAKECMDGOALS))
  ifeq (,$(wildcard $(CH_BUILD_DIR)))
    $(shell mkdir -p $(CH_BUILD_DIR))
  endif
all:
  ifneq (,$(GOS_AUTOLOG_STDOUT_ONLY))
	@$(MAKE) -k GOS_AUTOLOG=1 $(MAKECMDGOALS) | tee $(GOS_LOG)
  else
	@$(MAKE) -k GOS_AUTOLOG=1 $(MAKECMDGOALS) 2>&1 | tee $(GOS_LOG)
  endif
	@printf "Makefile: \"make all\" log is $(GOS_LOG)\n"
else

# CH_GEN is the set of files that cpphash runs the C preprocessor on.
# They can include files from CH_SRCS (defined below) since the cpphash
# tool arranges that de-commented copies are included, not the originals.

CH_GEN  ?= partitions.csv esphome.yaml

GOS_DIRS ?= gos						    \
	    $(patsubst %/,%,$(dir $(GOS_APP_PATH)))	    \
	    bsps/common $(GOS_BSP_DIR) $(GOS_PROJECT_DIR)

# CH_SRCS is the set of files that cpphash will remove hash-style
# comments from while leaving any C preprocessor directives so that
# the file can subsequently be used as a #include by one of the
# CH_GEN files.

# Builds the list of CH_SRCS by looking for .yaml files in $(GOS_DIRS).
# Note that any yaml files in the BSP and project directories are
# added also.

CH_SRCS += $(foreach d,$(GOS_DIRS),$(wildcard $(d)/*.yaml)) $(CH_GEN)

# In addition to updates to $(CH_SRCS) triggering a rebuild of espmake.yaml,
# updates to source files in $(ESP_DEPS) are also triggers.

GOS_DEPS ?= utils libraries/console

ESP_DEPS += $(foreach d,$(GOS_DEPS),$(wildcard $(d)/*.c) \
		$(wildcard $(d)/*.cpp) $(wildcard $(d)/*.h))

# If there is a secrets.h file in ./ or ../, use it

ifneq (,$(wildcard ./secrets.h))
  CH_EXTRA_FLAGS += -include ./secrets.h
else
  ifneq (,$(wildcard ../secrets.h))
    CH_EXTRA_FLAGS += -include ../secrets.h
  endif
endif

# cpphash sets up include paths to be the dehashes source directories first,
# followed by the source directories. Thus, includes of yaml files will
# include the dehashed variants of the same file and includes of non-yaml
# files such as images will come from the source tree.

CH_EXTRA_INCS += 

# Note that #ifdef GOS_PROJECT_<ppp>   is for project adapation anywhere
#       and #ifdef GOS_BSP_<bbb>       is for bsp adaptation anywhere
#       and #ifdef GOS_MAKE_<mmm>      is for $(GOS_CONFIG_FILE) adaptation
#       and #ifdef GOS_APP_<aaa>       is for app adaptation anywhere
#       and #ifdef GOS_USER_<username> is for user adaptation anywhere
# where ppp/aaa/ccc/bbb are the basenames from the project, config
# and app file paths and the bsp directory path, respectively.
# These are also passed into C/C++ code.  If changed, update gos/env.h.

# Also GOS_TOP is how to get from the build directory back to GOS_HOME
# and is used by gos/env.h.  It is GOS_HOME relative from CH_BUILD_DIR.

CH_EXTRA_DEFS += -D GOS_HOME=$(GOS_HOME)			    \
		  -D GOS_TOP=../..				    \
		  -D GOS_BUILD_PATH=$(CH_BUILD_DIR)		    \
		  -D GOS_PROJECT_DIR=$(GOS_PROJECT_DIR)		    \
		  -D GOS_PROJECT_NAME=$(PROJECT_NAME)		    \
		  -D GOS_PROJECT_$(PROJECT_NAME)		    \
		  -D GOS_CONFIG_FILE=$(GOS_CONFIG_FILE)		    \
		  -D GOS_BSP_DIR=$(GOS_BSP_DIR)			    \
		  -D GOS_BSP_NAME=$(BSP_NAME)			    \
		  -D GOS_BSP_$(BSP_NAME)			    \
		  -D GOS_APP_PATH=$(GOS_APP_PATH)		    \
		  -D GOS_APP_DIR=$(GOS_APP_DIR)			    \
		  -D GOS_APP_NAME=$(APP_NAME)			    \
		  -D GOS_APP_$(APP_NAME)			    \
		  -D GOS_MAKE_NAME=$(MAKE_NAME)			    \
		  -D GOS_MAKE_$(MAKE_NAME)			    \
		  -D GOS_USER_NAME=$(USER)			    \
		  -D GOS_USER_$(USER)

# Now include the cpphash Makefile fragment that will dehash GOS yamls files.
# In turn, it will include cpphash/esphome.mk which handles the esphome
# file generation and platformio build steps.

include $(GOS_HOME)/cpphash/cpphash.mk

print-config:: $(CH_TMP_DIR)/$(ESP_INIT)
	@printf "Makefile variables:\n"
	@printf "  PROJECT_NAME:  $(PROJECT_NAME)\n"
	@printf "  CH_BUILD_DIR: $(CH_BUILD_DIR)\n"
	@printf "  GOS_HOME: $(GOS_HOME)\n"
	@printf "  APP_NAME: $(APP_NAME)\n"
	@printf "  BSP_NAME: $(BSP_NAME)\n"
	@printf "  ESP_INIT: $(ESP_INIT)\n"
	@printf "  CH_GEN:  $(CH_GEN)\n"
	@printf "  CH_SRCS:\n"
	@$(foreach f,$(CH_SRCS),printf "    $(f)\n";)
	@printf "  ESP_DEPS:\n"
	@$(foreach f,$(ESP_DEPS),printf "    $(f)\n";)
	@printf "Makefile #defines available to yaml files:"
	@printf "  $(subst -, ,$(subst -D,#define,$(CH_EXTRA_DEFS)))\n" | $(SED) -e 's/ #/\n  #/g' -e 's/=/ /g'
	@printf "bsp.h #defines available to yaml and C/C++ files:\n"
	@$(CH_CPP) -dM $(CH_TMP_DIR)/$(ESP_INIT) | \
				grep GOS_BSP_HAS | sed 's/^/  /'
	@printf "For #defaults available to yaml files, "
	@printf "use: make print-defaults\n"

.PHONY: print-config

# this endif is from the autolog restart
endif

