#NMAKE

CC_FLAGS = /c /O1 /Oi /fp:fast /GS- /GR- /MT /nologo /Wall
LINKER_FLAGS = /DLL /SUBSYSTEM:WINDOWS /MANIFEST:NO /NOLOGO

SOURCE = durazno\durazno.c

TARGETS = xinput1_1 xinput1_2 xinput1_3 xinput1_4 xinput9_1_0 xinputuap

all: $(TARGETS)
	@editbin /NOLOGO /SUBSYSTEM:WINDOWS,4.00 /OSVERSION:4.00 build\*.dll
	@del durazno.obj
	@del build\*.exp
	@del build\*.lib

durazno.obj:
	@cl $(CC_FLAGS) $(SOURCE) /Fodurazno.obj
	
$(TARGETS): durazno.obj
	@link durazno.obj kernel32.lib user32.lib $(LINKER_FLAGS) /DEF:durazno\$(@).def /OUT:build\$(@).dll
	@echo.

