#!/bin/bash
######################
# This will create lots of big files
#######################
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
echo ${DIR}
mkdir -p bigtmp
for (( index = 0; index <= 1000; index++ ))
do
  echo "creating file ${index}"
  ../gen -N 10000000 -M 1000000000 > bigtmp/bigtmp${index}
done
