#!/bin/bash
# This script should be called from Jenkins when a new commit has been pushed 
# to the repo. 
# It analyzes what parts of the codebase have been modified, compiles everything
# that is needed, and reports on the results. 
readonly TMP_DIR=tmp
readonly DIFF_FILE=$TMP_DIR/changes.txt
readonly TEST_OUTPUT_DIR=output-tests
readonly TEST_DIR=scripts/testing
readonly CHANGES_SCR=$TEST_DIR/changes.py

# Fill in new tests here
readonly TEST_NAMES=( sampletest ) 


# Create directory for tests and other temp files.
mkdir -p $TMP_DIR
mkdir -p $TEST_OUTPUT_DIR


# Save changed files between last tested commit and current one.
git diff --stat $GIT_PREVIOUS_COMMIT $GIT_COMMIT > $DIFF_FILE

################################################################################
###############                COMPILATION BEGINS                ###############
################################################################################

function build_config() {
	make ARCH=x86 defconfig
}

function build_cross_compiler() {
	cd tools/compilers/gcc-glibc

	echo "# Number of make jobs to spawn.  
	MAKE_JOBS := 3
	RISCV_INSTDIR         := $WORKSPACE/install/riscv-ros-gcc/
	I686_INSTDIR          := $WORKSPACE/install/i686-ros-gcc/
	X86_64_INSTDIR        := $WORKSPACE/install/x86_64-ros-gcc/
	X86_64_NATIVE_INSTDIR := $WORKSPACE/install/x86_64-ros-gcc-native/
	" > Makelocal

	cat Makelocal > ~/deleteme.txt

	cd ../../..
}

if [ "$COMPILE_ALL" == true ]; then
	echo "Building all AKAROS"
	# 1. 
	build_config
	build_cross_compiler
else
	CHANGES=`$CHANGES_SCR $DIFF_FILE`
	echo "Building "$CHANGES

	# TODO: Compile only the rules needed
fi


# TODO: Detect changes and fill TEST_NAMES and COMPILE_PARTS accordingly to the 
# ones needed





# Run tests
for TEST_NAME in "${TEST_NAMES[@]}"
do
	`nosetests $TEST_NAME.py -w $TEST_DIR --with-xunit \
	--xunit-file=$TEST_OUTPUT_DIR/$TEST_NAME.xml`
done
