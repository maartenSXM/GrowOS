# paths are relative to GOS_HOME, or absolute paths
GOS_HOME := ..

$(MAKECMDGOALS):
	$(MAKE) -C $(GOS_HOME) -f Makefile \
	    GOS_PROJECT=growBoard0 \
	    GOS_BSP=bsps/growBoardCpu0 \
	    GOS_CONFIG=configs/growBoardCpu0 \
	$(MAKECMDGOALS)
