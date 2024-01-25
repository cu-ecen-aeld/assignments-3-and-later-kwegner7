#!/bin/sh

set -e # exit immediately if non-zero exit status
set -u # error if unset variable used

if [ $# -lt 2 ]
then
	echo "Second argument missing: search string"
	if [ $# -lt 1 ]
	then
	  echo "First argument missing: directory for search"
	fi
  exit 1
fi

FILES_DIR=$1
SEARCH_STR=$2
if [ ! -d ${FILES_DIR} ]
then
  echo "${FILES_DIR} is not a directory"
  exit 1
fi

NUM_FILES=$(find ${FILES_DIR} -type f | wc -l)
NUM_MATCHING=$(grep -rI ${SEARCH_STR} ${FILES_DIR} | wc -l)

MATCHSTR="The number of files are ${NUM_FILES} and the number of matching lines are ${NUM_MATCHING}"
echo ${MATCHSTR}
exit 0

