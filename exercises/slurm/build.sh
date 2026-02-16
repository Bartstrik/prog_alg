#!/bin/bash

if [ "$#" -lt 2 ]; then
	echo "Usage: $0 <CMakeRoot> <Target>"
	exit 1
fi

WORKSPACE=$(realpath "$1")
TARGET=$(basename "$2")

srun -p performance --nodes=1 --ntasks=1 --gpus=1 -t 00:05:00 \
	singularity run -c --nv --pwd $WORKSPACE --bind $WORKSPACE:$WORKSPACE /mnt/nas05/data02/progAlg/progAlg.sif \
	bash -c "cmake -S . -B build && cmake --build build --target $TARGET"
