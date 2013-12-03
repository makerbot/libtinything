#Clean flag
CLN_FLAG = -c

ifeq ($(OS),Windows_NT)
    #Windows stuff
	SCONS_CMD = cmd //c scons.bat
else
    #Linux stuff
    SCONS_CMD = scons -j4 --no-variant
endif
all:
	$(SCONS_CMD)
clean:
	$(SCONS_CMD) $(CLN_FLAG)


