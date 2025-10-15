NPROCS              = $(shell nproc)
MAKEFLAGS          += -j$(NPROCS)

PROJECT_NAME        = gnyame
VERSION             = 0.0.0
GIT_COMMIT          = $(shell git rev-parse HEAD)

CC                  = clang
FLAGS               = -std=c23 -ggdb -fenable-matrix -pedantic -Wall -Wextra -Wpedantic -Wno-gnu -Wno-gcc-compat -DVERSION=\"$(VERSION)\" -DGIT_COMMIT=\"$(GIT_COMMIT)\"
FLAGS_DEBUG         = -O0 -fsanitize=address,undefined -DIS_DEBUG=true
FLAGS_RELEASE       = -O3
FLAGS_LINUX         =
FLAGS_WIN           = -static -pthread --target=x86_64-w64-mingw32
INCLUDE_PATHS       = -I./src/ -I./vendor/sdl/include/
LINKER_PATHS_LINUX  = -L./vendor/sdl/build-linux/
LINKER_PATHS_WIN    = -L./vendor/sdl/build-windows/
LINKER_FLAGS        = -flto -lm -lSDL3
LINKER_FLAGS_LINUX  =
LINKER_FLAGS_WIN    = -lcomdlg32 -ldxguid -lgdi32 -limm32 -lkernel32 -lole32 -loleaut32 -lsetupapi -luser32 -luuid -lversion -lwinmm

DEBUG_EXEC          = $(PROJECT_NAME).debug.$(VERSION)
RELEASE_EXEC_LINUX  = $(PROJECT_NAME).linux_x86_64.$(VERSION)
RELEASE_EXEC_WIN    = $(PROJECT_NAME).windows_x86_64.$(VERSION).exe

SRCS                = $(shell find src -name '*.c')
HEADERS             = $(shell find src -name '*.h')
TESTS               = $(patsubst %.c, %, $(shell find test -name '*.c'))
DEBUG_OBJS          = $(addprefix build/debug/, $(patsubst src/%.c, %.o, $(SRCS)))
DEBUG_OBJS_NO_MAIN  = $(filter-out %/main.o, $(DEBUG_OBJS))
RELEASE_OBJS_LINUX  = $(addprefix build/release/linux/, $(patsubst src/%.c, %.o, $(SRCS)))
RELEASE_OBJS_WIN    = $(addprefix build/release/windows/, $(patsubst src/%.c, %.o, $(SRCS)))

SHADERS             = $(shell find assets -name '*.hlsl')
SHADERS_DXIL        = $(patsubst assets/shaders/source/%.hlsl, assets/shaders/compiled/DXIL/%.dxil, $(SHADERS))
SHADERS_MSL         = $(patsubst assets/shaders/source/%.hlsl, assets/shaders/compiled/MSL/%.mls, $(SHADERS))
SHADERS_SPIRV       = $(patsubst assets/shaders/source/%.hlsl, assets/shaders/compiled/SPIRV/%.spv, $(SHADERS))

TIMER_START        := $(shell date +%s%3N)
TIMER_END           = $(shell date +%s%3N)
TIME_MS             = $(shell expr $(TIMER_END) - $(TIMER_START))
TIMER_FORMAT        = $(shell echo "Took $(TIME_MS)ms")

VENDOR_DEPENDENCIES = ./vendor/sdl-shadercross/build/shadercross \
	./vendor/sdl/build-linux/libSDL3.a \
	./vendor/sdl/build-windows/libSDL3.a

# ---------------------------------------------------------------------------------------------------------------------#
# MAJOR TARGETS--------------------------------------------------------------------------------------------------------#
# ---------------------------------------------------------------------------------------------------------------------#

.DEFAULT_GOAL := default
.PHONY: default
default:
	@echo "Available targets:"
	@echo "  - make debug"
	@echo "  - make release"
	@echo "  - make vendor"
	@echo "  - make run"
	@echo "  - make test"
	@echo "  - make docs"
	@echo "  - make stats"

.PHONY: debug
debug: $(DEBUG_EXEC)
	@echo $(TIMER_FORMAT)

.PHONY: release
release: linux windows

.PHONY: linux
linux: $(RELEASE_EXEC_LINUX)
	@echo $(TIMER_FORMAT)

.PHONY: windows
windows: $(RELEASE_EXEC_WIN)
	@echo $(TIMER_FORMAT)

.PHONY: vendor
vendor: $(VENDOR_DEPENDENCIES)

.PHONY: run
run: debug
	ASAN_OPTIONS=detect_leaks=1 LSAN_OPTIONS=suppressions=lsan.supp ./$(DEBUG_EXEC)

.PHONY: test
test: vendor_no_rebuild $(DEBUG_OBJS_NO_MAIN)
	@for test in $(TESTS); do \
		echo ./build/$$test | grep -oP '.*/' | xargs mkdir -p; \
		$(CC) -o ./build/$$test $$test.c $(DEBUG_OBJS_NO_MAIN) \
			$(FLAGS) $(FLAGS_DEBUG) $(FLAGS_LINUX) \
			$(INCLUDE_PATHS) $(LINKER_FLAGS) $(LINKER_PATHS_LINUX) $(LINKER_FLAGS_LINUX); \
		./build/$$test; \
		[ $$? -eq 0 ] && echo "Test $$test passed." || echo "Test $$test failed."; \
		[ $$? -eq 0 ] || exit 1; \
	done
	@echo $(TIMER_FORMAT)

.PHONY: docs
docs:
	doxygen ./docs/doxygen.config && xdg-open ./docs/doxygen/html/index.html

.PHONY: stats
stats:
	@tokei . --exclude vendor

# --------------------------------------------------------------------------------------------------------------------- #
# COMPILING & LINKING-------------------------------------------------------------------------------------------------- #
# --------------------------------------------------------------------------------------------------------------------- #

$(DEBUG_EXEC): vendor_no_rebuild shaders $(DEBUG_OBJS)
	@echo LD $(DEBUG_EXEC)
	@$(CC) -o $(DEBUG_EXEC) $(DEBUG_OBJS) \
		$(FLAGS) $(FLAGS_DEBUG) $(FLAGS_LINUX) \
		$(INCLUDE_PATHS) $(LINKER_PATHS_LINUX) $(LINKER_FLAGS) $(LINKER_FLAGS_LINUX)

./build/debug/%.o: src/%.c $(HEADERS)
	@echo CC $<
	@mkdir $(dir $@) -p
	@$(CC) -c $< -o $@ $(FLAGS) $(FLAGS_DEBUG) $(FLAGS_LINUX) $(INCLUDE_PATHS)

$(RELEASE_EXEC_LINUX): vendor_no_rebuild shaders $(RELEASE_OBJS_LINUX)
	@echo LD $(RELEASE_EXEC_LINUX)
	@$(CC) -o $(RELEASE_EXEC_LINUX) $(RELEASE_OBJS_LINUX) \
		$(FLAGS) $(FLAGS_RELEASE) $(FLAGS_LINUX) \
		$(INCLUDE_PATHS) $(LINKER_PATHS_LINUX) $(LINKER_FLAGS) $(LINKER_FLAGS_LINUX)

./build/release/linux/%.o: src/%.c $(HEADERS)
	@echo CC $<
	@mkdir $(dir $@) -p
	@$(CC) -c $< -o $@ $(FLAGS) $(FLAGS_RELEASE) $(FLAGS_LINUX) $(INCLUDE_PATHS)

$(RELEASE_EXEC_WIN): vendor_no_rebuild shaders $(RELEASE_OBJS_WIN)
	@echo LD $(RELEASE_EXEC_WIN)
	@$(CC) -o $(RELEASE_EXEC_WIN) $(RELEASE_OBJS_WIN) \
		$(FLAGS) $(FLAGS_RELEASE) $(FLAGS_WIN) \
		$(INCLUDE_PATHS) $(LINKER_PATHS_WIN) $(LINKER_FLAGS) $(LINKER_FLAGS_WIN)

./build/release/windows/%.o: src/%.c $(HEADERS)
	@echo CC $<
	@mkdir $(dir $@) -p
	@$(CC) -c $< -o $@ $(FLAGS) $(FLAGS_RELEASE) $(FLAGS_WIN) $(INCLUDE_PATHS)

# --------------------------------------------------------------------------------------------------------------------- #
# SHADER CROSS-COMPILING----------------------------------------------------------------------------------------------- #
# --------------------------------------------------------------------------------------------------------------------- #

.PHONY: shaders
shaders: vendor_no_rebuild $(SHADERS_DXIL) $(SHADERS_MSL) $(SHADERS_SPIRV)

./assets/shaders/compiled/DXIL/%.dxil: assets/shaders/source/%.hlsl
	@echo CC $< to dxil
	@mkdir $(dir $@) -p
	@./vendor/sdl-shadercross/build/shadercross $< -o $@ -s hlsl -d dxil

./assets/shaders/compiled/SPIRV/%.spv: assets/shaders/source/%.hlsl
	@echo CC $< to spirv
	@mkdir $(dir $@) -p
	@./vendor/sdl-shadercross/build/shadercross $< -o $@ -s hlsl -d spirv

./assets/shaders/compiled/MSL/%.mls: assets/shaders/source/%.hlsl
	@echo CC $< to msl
	@mkdir $(dir $@) -p
	@./vendor/sdl-shadercross/build/shadercross $< -o $@ -s hlsl -d msl

# --------------------------------------------------------------------------------------------------------------------- #
# LIBRARIES------------------------------------------------------------------------------------------------------------ #
# --------------------------------------------------------------------------------------------------------------------- #

.SECONDEXPANSION:
.PHONY: vendor_no_rebuild
vendor_no_rebuild: $(filter-out $(wildcard $(VENDOR_DEPENDENCIES)), $(VENDOR_DEPENDENCIES))

# shadercross

./vendor/sdl-shadercross/build/shadercross:
	@cd ./vendor/sdl-shadercross; \
		cmake -S . -B build -GNinja -DSDLSHADERCROSS_VENDORED=ON; \
		cmake --build build --config Release -- -j $(NPROCS)

# sdl3

./vendor/sdl/build-linux/libSDL3.a:
	@cd ./vendor/sdl; \
		cmake -S . -B build-linux -GNinja \
			-DCMAKE_BUILD_TYPE=Release \
			-DCMAKE_POSITION_INDEPENDENT_CODE=ON \
			-DSDL_SHARED=OFF \
			-DSDL_STATIC=ON; \
		cmake --build build-linux --config Release -- -j $(NPROCS)

./vendor/sdl/build-windows/libSDL3.a:
	@cd ./vendor/sdl; \
		cmake -S . -B build-windows -GNinja \
			  -DCMAKE_C_COMPILER="/usr/bin/x86_64-w64-mingw32-gcc" \
			  -DCMAKE_CXX_COMPILER="/usr/bin/x86_64-w64-mingw32-g++" \
			  -DCMAKE_LINKER="/usr/bin/x86_64-w64-mingw32-ld" \
			  -DCMAKE_FIND_ROOT_PATH="/usr/x86_64-w64-mingw32" \
			  -DCMAKE_FIND_ROOT_PATH_MODE_INCLUDE="BOTH" \
			  -DCMAKE_FIND_ROOT_PATH_MODE_LIBRARY="ONLY" \
			  -DCMAKE_FIND_ROOT_PATH_MODE_PROGRAM="BOTH" \
			  -DCMAKE_RC_COMPILER="/usr/bin/x86_64-w64-mingw32-windres" \
			  -DCMAKE_SYSTEM_NAME="Windows" \
			  -DSDL_SHARED=OFF \
			  -DSDL_STATIC=ON; \
		cmake --build build-windows --config Release -- -j $(NPROCS)
