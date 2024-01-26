#!/bin/sh

set -e # exit immediately if non-zero exit status
set -u # error if unset variable used

if [ $# -lt 2 ]
then
    echo "Second argument missing: string to be written"
    if [ $# -lt 1 ]
    then
        echo "First argument missing: full path to a file"
    fi
    exit 1
fi

FILE_PATH=$1
STR_TO_WRITE=$2
if [ -d ${FILE_PATH} ]
then
  echo "${FILE_PATH} is a directory, not a file"
  exit 1
fi

mkdir --parents $(dirname ${FILE_PATH})
echo ${STR_TO_WRITE} > ${FILE_PATH}
if [ $? -eq 0 ]
then
    echo "File ${FILE_PATH} has been created"  
    exit 0
else
    echo "File ${FILE_PATH} could not be created"
    exit 1
fi

