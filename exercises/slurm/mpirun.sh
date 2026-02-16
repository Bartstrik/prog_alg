#!/bin/bash

if [ "$#" -lt 1 ]; then
	echo "Usage: $0 <NTasks> <Executable> [Args...]"
	exit 1
fi

NTASKS="$1"
WORKSPACE=$(realpath $(dirname "$2"))
EXECUTABLE=$(basename "$2")
EXECUTABLE_ARGS="${@:3}"

srun -p performance --nodes=1 --pty --ntasks=$NTASKS --mpi=pmix -t 00:05:00 \
	singularity run -c --pwd $WORKSPACE --bind $WORKSPACE:$WORKSPACE /mnt/nas05/data02/progAlg/progAlg.sif \
	./$EXECUTABLE $EXECUTABLE_ARGS
