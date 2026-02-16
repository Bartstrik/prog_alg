#!/bin/bash

if [ "$#" -lt 1 ]; then
	echo "Usage: $0 <NCores> <Executable> [Args...]"
	exit 1
fi

NCORES="$1"
WORKSPACE=$(realpath $(dirname "$2"))
EXECUTABLE=$(basename "$2")
EXECUTABLE_ARGS="${@:3}"

srun -p performance --nodes=1 --pty --cpus-per-task=$NCORES -t 00:05:00 \
	singularity run -c --pwd $WORKSPACE --bind $WORKSPACE:$WORKSPACE /mnt/nas05/data02/progAlg/progAlg.sif \
	./$EXECUTABLE $EXECUTABLE_ARGS
