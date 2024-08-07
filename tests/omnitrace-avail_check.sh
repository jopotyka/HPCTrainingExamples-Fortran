#!/bin/bash

module purge

module load rocm
ROCM_VERSION=`cat ${ROCM_PATH}.info/version | head -1 | cut -f1 -d'-' `
result=`echo ${ROCM_VERSION} | awk '$1<=6.1.2'` && echo $result
module unload rocm

if [[ "${result}" ]]; then
   module load omnitrace
else
   module load rocm
fi   

omnitrace-avail -G $PWD/.omnitrace.cfg

rm .omnitrace.cfg