# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.6

# Default target executed when no arguments are given to make.
default_target: all

.PHONY : default_target

# Allow only one "make -f Makefile2" at a time, but pass parallelism.
.NOTPARALLEL:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/utking/projects/c/cdir_snapshot

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/utking/projects/c/cdir_snapshot

#=============================================================================
# Targets provided globally by CMake.

# Special rule for the target edit_cache
edit_cache:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "Running CMake cache editor..."
	/usr/bin/ccmake -H$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR)
.PHONY : edit_cache

# Special rule for the target edit_cache
edit_cache/fast: edit_cache

.PHONY : edit_cache/fast

# Special rule for the target rebuild_cache
rebuild_cache:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "Running CMake to regenerate build system..."
	/usr/bin/cmake -H$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR)
.PHONY : rebuild_cache

# Special rule for the target rebuild_cache
rebuild_cache/fast: rebuild_cache

.PHONY : rebuild_cache/fast

# The main all target
all: cmake_check_build_system
	$(CMAKE_COMMAND) -E cmake_progress_start /home/utking/projects/c/cdir_snapshot/CMakeFiles /home/utking/projects/c/cdir_snapshot/CMakeFiles/progress.marks
	$(MAKE) -f CMakeFiles/Makefile2 all
	$(CMAKE_COMMAND) -E cmake_progress_start /home/utking/projects/c/cdir_snapshot/CMakeFiles 0
.PHONY : all

# The main clean target
clean:
	$(MAKE) -f CMakeFiles/Makefile2 clean
.PHONY : clean

# The main clean target
clean/fast: clean

.PHONY : clean/fast

# Prepare targets for installation.
preinstall: all
	$(MAKE) -f CMakeFiles/Makefile2 preinstall
.PHONY : preinstall

# Prepare targets for installation.
preinstall/fast:
	$(MAKE) -f CMakeFiles/Makefile2 preinstall
.PHONY : preinstall/fast

# clear depends
depend:
	$(CMAKE_COMMAND) -H$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR) --check-build-system CMakeFiles/Makefile.cmake 1
.PHONY : depend

#=============================================================================
# Target rules for targets named cdir_snapshot

# Build rule for target.
cdir_snapshot: cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 cdir_snapshot
.PHONY : cdir_snapshot

# fast build rule for target.
cdir_snapshot/fast:
	$(MAKE) -f CMakeFiles/cdir_snapshot.dir/build.make CMakeFiles/cdir_snapshot.dir/build
.PHONY : cdir_snapshot/fast

main.o: main.c.o

.PHONY : main.o

# target to build an object file
main.c.o:
	$(MAKE) -f CMakeFiles/cdir_snapshot.dir/build.make CMakeFiles/cdir_snapshot.dir/main.c.o
.PHONY : main.c.o

main.i: main.c.i

.PHONY : main.i

# target to preprocess a source file
main.c.i:
	$(MAKE) -f CMakeFiles/cdir_snapshot.dir/build.make CMakeFiles/cdir_snapshot.dir/main.c.i
.PHONY : main.c.i

main.s: main.c.s

.PHONY : main.s

# target to generate assembly for a file
main.c.s:
	$(MAKE) -f CMakeFiles/cdir_snapshot.dir/build.make CMakeFiles/cdir_snapshot.dir/main.c.s
.PHONY : main.c.s

snapshot.o: snapshot.c.o

.PHONY : snapshot.o

# target to build an object file
snapshot.c.o:
	$(MAKE) -f CMakeFiles/cdir_snapshot.dir/build.make CMakeFiles/cdir_snapshot.dir/snapshot.c.o
.PHONY : snapshot.c.o

snapshot.i: snapshot.c.i

.PHONY : snapshot.i

# target to preprocess a source file
snapshot.c.i:
	$(MAKE) -f CMakeFiles/cdir_snapshot.dir/build.make CMakeFiles/cdir_snapshot.dir/snapshot.c.i
.PHONY : snapshot.c.i

snapshot.s: snapshot.c.s

.PHONY : snapshot.s

# target to generate assembly for a file
snapshot.c.s:
	$(MAKE) -f CMakeFiles/cdir_snapshot.dir/build.make CMakeFiles/cdir_snapshot.dir/snapshot.c.s
.PHONY : snapshot.c.s

# Help Target
help:
	@echo "The following are some of the valid targets for this Makefile:"
	@echo "... all (the default if no target is provided)"
	@echo "... clean"
	@echo "... depend"
	@echo "... edit_cache"
	@echo "... rebuild_cache"
	@echo "... cdir_snapshot"
	@echo "... main.o"
	@echo "... main.i"
	@echo "... main.s"
	@echo "... snapshot.o"
	@echo "... snapshot.i"
	@echo "... snapshot.s"
.PHONY : help



#=============================================================================
# Special targets to cleanup operation of make.

# Special rule to run CMake to check the build system integrity.
# No rule that depends on this can have commands that come from listfiles
# because they might be regenerated.
cmake_check_build_system:
	$(CMAKE_COMMAND) -H$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR) --check-build-system CMakeFiles/Makefile.cmake 0
.PHONY : cmake_check_build_system

