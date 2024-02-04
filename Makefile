RAYLIB_SRC = ./src/raylib/src
C_SRC = ./c_src/rayex

.PHONY: all
all: ensure_raylib ensure_lib ensure_include raylib_make raylib_move

.PHONY: ensure_raylib
.SILENT: ensure_raylib
ensure_raylib:
	if [ ! -d $(RAYLIB_SRC) ]; then\
		echo "Git submodule Raylib has not been pulled. Raylib is expected to exist under: (ProjectRoot)/src/raylib" ;\
	fi

.PHONY: ensure_lib
.SILENT: ensure_lib
ensure_lib:
	if [ ! -d $(C_SRC)/lib ]; then\
		mkdir $(C_SRC)/lib ;\
	fi

.PHONY: ensure_include
.SILENT: ensure_include
ensure_include:
	if [ ! -d $(C_SRC)/include ]; then\
		mkdir $(C_SRC)/include ;\
	fi

.PHONEY: raylib_make
raylib_make:
	make PLATFORM=PLATFORM_DESKTOP --directory $(RAYLIB_SRC)

.PHONEY: raylib_move
raylib_move:
	cp $(RAYLIB_SRC)/libraylib.a $(C_SRC)/lib/libraylib.a
	cp $(RAYLIB_SRC)/raylib.h $(C_SRC)/include/raylib.h