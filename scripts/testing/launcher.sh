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

# Save changed files between last tested commit and current one.
git diff --stat $GIT_PREVIOUS_COMMIT $GIT_COMMIT > $DIFF_FILE

################################################################################
###############                COMPILATION BEGINS                ###############
################################################################################

CHANGES=`$CHANGES_SCR`
echo $CHANGES
# TODO: Compile only the rules needed
# 1. 




# TODO: Detect changes and fill TEST_NAMES and COMPILE_PARTS accordingly to the 
# ones needed




# Create output directory for tests
mkdir -p $TEST_OUTPUT_DIR

# Run tests
for TEST_NAME in "${TEST_NAMES[@]}"
do
	`nosetests $TEST_NAME.py -w $TEST_DIR --with-xunit \
	--xunit-file=$TEST_OUTPUT_DIR/$TEST_NAME.xml`
done
