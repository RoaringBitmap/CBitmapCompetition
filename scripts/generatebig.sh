#!/bin/bash
######################
# This will create lots of big files
#######################
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
mkdir -p ${DIR}/../bigtmp
find ${DIR}/../bigtmp -size 0c -delete
for (( index = 0; index < 100; index++ ))
do
  filename=${DIR}/../bigtmp/bigtmp${index}.txt
  if [ -f ${filename} ]
  then 
    echo "creating file ${index}"
    ${DIR}/../gen -N 10000000 -M 1000000000 > ${filename}
  fi
done
