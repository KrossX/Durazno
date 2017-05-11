#NMAKE

CC_FLAGS = /c /O2 /GL /D _USING_V110_SDK71_ /D _WINDLL /D _MBCS /MD /GS- /nologo
LINKER_FLAGS = /NODEFAULTLIB /MANIFEST:NO /SUBSYSTEM:WINDOWS /OPT:REF /OPT:ICF /LTCG /ENTRY:"DllMain" /DYNAMICBASE:NO /NXCOMPAT /SAFESEH:NO /DLL /NOLOGO

SOURCE = durazno\durazno.c

TARGETS = xinput1_1 xinput1_2 xinput1_3 xinput1_4 xinput9_1_0 xinputuap

all: $(TARGETS)
	@editbin /NOLOGO /SUBSYSTEM:WINDOWS,4.00 /OSVERSION:4.00 build\*.dll
	@del durazno.obj
	@del build\*.exp
	@del build\*.lib

durazno.obj:
	@cl $(CC_FLAGS) $(SOURCE) -Fo:durazno.obj
	
$(TARGETS): durazno.obj
	@link durazno.obj kernel32.lib user32.lib $(LINKER_FLAGS) /DEF:durazno\$(@).def /OUT:build\$(@).dll
	@echo.

