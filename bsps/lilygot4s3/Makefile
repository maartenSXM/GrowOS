
MAKECMDGOALS ?= all
MAKEBSP := $(MAKE) -C ../.. --no-print-directory 
BSP = $(shell basename $(dir $(abspath $(lastword $(MAKEFILE_LIST)))))

$(MAKECMDGOALS): 
	@$(MAKEBSP) BSPS=$(BSP)

