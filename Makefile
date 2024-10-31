# This Makefile uses github repo cpphash to build GOS esphome projects
# from ./projects/*.mk.  Use "make PRJ=projects/<prj>/<prj>.mk" to
# select a specific GOS project file otherwise $(PRJ_DEFAULT)
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

MY_DIR := $(patsubst %/,%,$(dir $(abspath $(lastword $(MAKEFILE_LIST)))))

# the GrowOS project file can be specified on the make command line
# with, for example, make PRJ=projects/lilygot4s3/debug.mk

PRJ_DEFAULT := projects/lilygot4s3/debug.mk

# A GrowOS project file must defines the following make variables:
#  GOS_APP_PATH    - the project's application yaml file
#  GOS_CONFIG_FILE - the project's configuration header file 
#  GOS_BSP_DIR     - the project's board support package directory

# GOS project files are typically found in ./projects/ but can reside
# anywhere inside or outside of the GOS file tree as long as the path
# specified is relative to this directory.

# Makefile variables set below with ?= can be overridden by a project file.
# Makefile variables set below with += can be prepended  by a project file.

# Makefile variables starting with CH_  are for cpphash/cpphash.mk.
# Makefile variables starting with ESP_ are for cpphash/esphome.mk.
# Makefile variables starting with GOS_ are for GrowOS yaml and C/C++
# Makefile variables starting with MY_  are local to this file

# default GOS_HOME to this directory

ifeq (,$(GOS_HOME))
  GOS_HOME:=$(MY_DIR)
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
MAKE         := $(MAKE) --no-print-directory
MAKEFILE     := $(lastword $(MAKEFILE_LIST))

# prjsave.mk handles defaulting PRJ to PRJ_DEFAULT, and records PRJ

include cpphash/make/prjsave.mk

MY_PROJECT_FILE = $(PRJ)

# Include the selected GOS project (Makefile fragment).

include $(MY_PROJECT_FILE)

ifeq (,$(GOS_CONFIG_FILE))
  $(error Makefile: $(MY_PROJECT_FILE) did not define GOS_CONFIG_FILE)
endif
ifeq (,$(GOS_BSP_DIR))
  $(error Makefile: $(MY_PROJECT_FILE) did not define GOS_BSP_DIR)
endif
ifeq (,$(GOS_APP_PATH))
  $(error Makefile: $(MY_PROJECT_FILE) did not define GOS_APP_PATH)
endif
ifeq (,$(wildcard $(GOS_CONFIG_FILE)))
  $(error $(GOS_CONFIG_FILE) defined in $(MY_PROJECT_FILE) not found)
endif
ifeq (,$(wildcard $(GOS_BSP_DIR)))
  $(error $(GOS_BSP_DIR) defined in $(MY_PROJECT_FILE) not found)
endif
ifeq (,$(wildcard $(GOS_APP_PATH)))
  $(error $(GOS_APP_PATH) defined in $(MY_PROJECT_FILE) not found)
endif

MY_PROJECT_DIR := $(patsubst %/,%,$(dir $(PRJ)))
MY_APP_DIR     := $(patsubst %/,%,$(dir $(GOS_APP_PATH)))

# This strips the directory and suffix, if any, for further use below

MY_PROJECT_NAME := $(basename $(notdir $(MY_PROJECT_DIR)))
MY_MAKE_NAME    := $(basename $(notdir $(MY_PROJECT_FILE)))
MY_BSP_NAME     := $(basename $(notdir $(GOS_BSP_DIR)))
MY_APP_NAME     := $(basename $(notdir $(MY_APP_DIR)))

# CH_BUILD_DIR is were GOS projects are built. If changed, review
# the definition of GOS_TOP below.  It declares the relative path to
# GOS_HOME from CH_BUILD_DIR. This is done manually because realpath
# does not support the --relative-to option on many non-Linux OSs.

CH_BUILD_DIR := ./build/$(MY_PROJECT_NAME)_$(MY_MAKE_NAME)

include cpphash/make/log.mk

# CH_GEN is the set of files that cpphash runs the C preprocessor on.
# They can include files from CH_SRCS (defined below) since the cpphash
# tool arranges that de-commented copies are included, not the originals.

CH_GEN  ?= partitions.csv esphome.yaml

MY_SRC_DIRS ?= gos					    \
	    $(patsubst %/,%,$(dir $(GOS_APP_PATH)))	    \
	    bsps/common $(GOS_BSP_DIR) $(MY_PROJECT_DIR)

# CH_SRCS is the set of files that cpphash will remove hash-style
# comments from while leaving any C preprocessor directives so that
# the file can subsequently be used as a #include by one of the
# CH_GEN files.

# Builds the list of CH_SRCS by looking for .yaml files in $(MY_DIRS).
# Note that any yaml files in the BSP and project directories are
# added also.

CH_SRCS += local.yaml \
	   $(foreach d,$(MY_SRC_DIRS),$(wildcard $(d)/*.yaml)) $(CH_GEN)

# In addition to updates to $(CH_SRCS) triggering a rebuild of espmake.yaml,
# updates to source files in $(ESP_DEPS) are also triggers.

MY_DEP_DIRS ?= utils libraries/console

ESP_DEPS += $(foreach d,$(MY_DEP_DIRS),$(wildcard $(d)/*.c) \
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

# CH_EXTRA_INCS += 

# Note that #ifdef GOS_PROJECT_<ppp>   is for project adapation
#       and #ifdef GOS_MAKE_<mmm>      is for project variant adaptation
#       and #ifdef GOS_APP_<aaa>       is for app adaptation
#       and #ifdef GOS_BSP_<bbb>       is for bsp adaptation
#       and #ifdef GOS_USER_<username> is for user adaptation
# where ppp/mmm/aaa/bbb are the basenames of the project directory,
# project .mk, app directory and bsp directory, respectively.

# Also, GOS_TOP is used to get from the build directory back to GOS_HOME
# and is needed by gos/env.h. It locates GOS_HOME from CH_BUILD_DIR.

CH_EXTRA_DEFS += -D GOS_HOME=$(GOS_HOME)		\
		 -D GOS_TOP=../..			\
		 -D GOS_BUILD_PATH=$(CH_BUILD_DIR)	\
		 -D GOS_PROJECT_DIR=$(MY_PROJECT_DIR)	\
		 -D GOS_PROJECT_NAME=$(MY_PROJECT_NAME)	\
		 -D GOS_PROJECT_$(MY_PROJECT_NAME)	\
		 -D GOS_CONFIG_FILE=$(GOS_CONFIG_FILE)	\
		 -D GOS_BSP_DIR=$(GOS_BSP_DIR)		\
		 -D GOS_BSP_NAME=$(MY_BSP_NAME)		\
		 -D GOS_BSP_$(MY_BSP_NAME)		\
		 -D GOS_APP_PATH=$(GOS_APP_PATH)	\
		 -D GOS_APP_DIR=$(MY_APP_DIR)		\
		 -D GOS_APP_NAME=$(MY_APP_NAME)		\
		 -D GOS_APP_$(MY_APP_NAME)		\
		 -D GOS_MAKE_NAME=$(MY_MAKE_NAME)	\
		 -D GOS_MAKE_$(MY_MAKE_NAME)		\
		 -D GOS_USER_NAME=$(USER)		\
		 -D GOS_USER_$(USER)

# Now include the cpphash Makefile fragment that will dehash GOS yamls files.
# In turn, it will include cpphash/make/esphome.mk which handles the esphome
# file generation and platformio build steps.

include cpphash/make/cpphash.mk

print-config:: $(CH_TMP_DIR)/$(ESP_INIT)
	@printf "Makefile variables:\n"
	@printf "  PROJECT_NAME:  $(MY_PROJECT_NAME)\n"
	@printf "  CH_BUILD_DIR: $(CH_BUILD_DIR)\n"
	@printf "  GOS_HOME: $(GOS_HOME)\n"
	@printf "  APP_NAME: $(MY_APP_NAME)\n"
	@printf "  BSP_NAME: $(MY_BSP_NAME)\n"
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

.PHONY: print-config

