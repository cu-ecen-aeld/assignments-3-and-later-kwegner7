#!/bin/sh
# Tester script for assignment 1 and assignment 2
# Author: Siddhant Jajoo

set -e
set -u

NUMFILES=10
WRITESTR=AELD_IS_FUN
WRITEDIR=/tmp/aeld-data/results
username=$(cat conf/username.txt)

if [ $# -eq 2 ]
then
   echo "Using default value ${WRITEDIR}"
   NUMFILES=$1
   WRITESTR=$2
fi

if [ $# -eq 1 ]
then
   echo "Using default values ${WRITESTR} ${WRITEDIR}"
   NUMFILES=$1
fi

if [ $# -eq 0 ]
then
   echo "Using default values ${NUMFILES} ${WRITESTR} ${WRITEDIR}"
fi

if [ $# -gt 2 ]
then
   NUMFILES=$1
   WRITESTR=$2
   WRITEDIR=/tmp/aeld-data/$3
fi

# removed in assignment 3
if false # false for assignment 3
then
  make clean
  make
fi

MATCHSTR="The number of files are ${NUMFILES} and the number of matching lines are ${NUMFILES}"

echo "Writing ${NUMFILES} files containing string ${WRITESTR} to ${WRITEDIR}"

rm -rf "${WRITEDIR}"

# create $WRITEDIR if not assignment1
assignment=`cat ../conf/assignment.txt`

if [ $assignment != 'assignment1' ]
then
	mkdir -p "$WRITEDIR"

	#The WRITEDIR is in quotes because if the directory path consists of spaces, then variable substitution 
	#will consider it as multiple argument.
	#The quotes signify that the entire string in WRITEDIR is a single string.
	#This issue can also be resolved by using double square brackets i.e [[ ]] instead of using quotes.
	if [ -d "$WRITEDIR" ]
	then
		echo "$WRITEDIR created"
	else
		exit 1
	fi
fi
#echo "Removing the old writer utility and compiling as a native application"
#make clean
#make

for i in $( seq 1 $NUMFILES)
do
  if [ $assignment = 'assignment1' ]
  then
    ./writer.sh "$WRITEDIR/${username}$i.txt" "$WRITESTR"
  fi
  if [ $assignment = 'assignment2' ] || [ $assignment = 'assignment3' ]
  then
    ./writer "$WRITEDIR/${username}$i.txt" "$WRITESTR"
    if [ $? -eq 1 ]; then
	    exit 1
    fi
  fi
done

OUTPUTSTRING=$(./finder.sh "$WRITEDIR" "$WRITESTR")

# remove temporary directories
rm -rf /tmp/aeld-data

set +e
echo ${OUTPUTSTRING} | grep "${MATCHSTR}"
if [ $? -eq 0 ]; then
	echo "success"
	exit 0
else
	echo "failed: expected  ${MATCHSTR} in ${OUTPUTSTRING} but instead found"
	exit 1
fi
