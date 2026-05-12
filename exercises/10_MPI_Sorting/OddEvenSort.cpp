#include <iostream>
#include <sstream>
#include <cassert>
#include <cstring>
#include <algorithm>
#include <random>
#include <vector>
#include "mpi.h"

// Speedup = Tserial / Tparallel
// The following is the speedup for 1 to 12 processes
//speedup S1 = 0.985187
//speedup S2 = 1.78144
//speedup S3 = 3.07641
//speedup S4 = 5.72818
//speedup S5 = 6.6951
//speedup S6 = 8.18159
// speedup S7 = 8.81854
// speedup S8 = 10.6656
// speedup S9 = 5.32969
//speedup S10 = 10.4327
//speedup S9 = 8.6242
//speedup S12 = 11.2612

// Karp-Flatt Metric  e = (1/S - 1/p) / (1 - 1/p)
//karp-flatt metric e1 = inf
//karp-flatt metric e2 = 0.561343
//karp-flatt metric e3 = 0.325054
//karp-flatt metric e4 = 0.174575
//karp-flatt metric e5 = 0.149363
//karp-flatt metric e6 = 0.122226
//karp-flatt metric e7 = 0.113397
//karp-flatt metric e8 = 0.0937595
//karp-flatt metric e10 = 0.0958525
//karp-flatt metric e10 = 0.0958525
//karp-flatt metric e11 = 0.102802
//karp-flatt metric e12 = 0.0888008

//////////////////////////////////////////////////////////////////////////////////////////////////
// compare-split of nlocal data elements
// pre-condition:  sent contains the data sent to another process
//                 received contains the data received by the other process
// post-condition: result contains the kept data elements
static void CompareSplit(int nlocal, float *sent, float *received, float *result, bool keepSmall) {
	if (keepSmall) {
		for (int i = 0, j = 0, k = 0; k < nlocal; k++) {
			if (j == nlocal || (i < nlocal && sent[i] <= received[j])) {
				result[k] = sent[i++];
			} else {
				result[k] = received[j++];
			}
		}
	} else {
		const int last = nlocal - 1;
		for (int i = last, j = last, k = last; k >= 0; k--) {
			if (j == -1 || (i >= 0 && sent[i] >= received[j])) {
				result[k] = sent[i--];
			} else {
				result[k] = received[j--];
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// TODO: measure the wall-clock times of all processes with MPI_Wtime() and use reduction to
// determine the maximum wall-clock time.
void oddEvenSort1(int n) {
	int p, id, idOdd, idEven;
	MPI_Status status;
	std::default_random_engine e;
	std::uniform_real_distribution<float> dist;

	MPI_Comm_size(MPI_COMM_WORLD, &p);
	MPI_Comm_rank(MPI_COMM_WORLD, &id);

	const int nlocal = n/p;
	n = nlocal*p;

	if (id == 0) {
		std::cout << "Odd-Even sort V1" << std::endl;
	}
	std::vector<float> sorted;
	std::vector<float> elements(nlocal);
	std::vector<float> received(nlocal);
	std::vector<float> temp(nlocal);

	// TODO use barrier to synchronize start time
	MPI_Barrier(MPI_COMM_WORLD);
	double start = MPI_Wtime();

	// fill in elements with random values
	for (int i = 0; i < nlocal; i++) {
		received[i] = dist(e);
	}

	// sort values locally
	std::sort(received.begin(), received.end());

	// determine the id of the processes that it needs to communicate during the odd and even phases
	if (id & 1) {
		idOdd = id + 1;
		idEven = id - 1;
	} else {
		idOdd = id - 1;
		idEven = id + 1;
	}
	if (idEven < 0 || idEven == p) idEven = MPI_PROC_NULL;
	if (idOdd  < 0 || idOdd  == p) idOdd = MPI_PROC_NULL;

	// main loop of odd-even sort: local data to send is in received buffer
	for (int i = 0; i < p; i++) {
		if (i & 1) {
			// odd phase
			MPI_Sendrecv(received.data(), nlocal, MPI_FLOAT, idOdd, 1, elements.data(), nlocal, MPI_FLOAT, idOdd, 1, MPI_COMM_WORLD, &status);
		} else {
			// even phase
			MPI_Sendrecv(received.data(), nlocal, MPI_FLOAT, idEven, 1, elements.data(), nlocal, MPI_FLOAT, idEven, 1, MPI_COMM_WORLD, &status);
		}
		if (status.MPI_SOURCE != MPI_PROC_NULL) {
			// sent data is in received buffer
			// received data is in elements buffer
			CompareSplit(nlocal, received.data(), elements.data(), temp.data(), id < status.MPI_SOURCE);
			// temp contains result of compare-split operation: copy temp back to received buffer
			copy(temp.begin(), temp.end(), received.begin());
		}
	}

	// TODO stop time measuring and reduce maximum time
	double localElapsed, elapsed = 0;

	// TODO use reduction to determine maximum time
	localElapsed = MPI_Wtime() - start;
	MPI_Reduce(&localElapsed, &elapsed, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

	if (id == 0) {
		// check if all elements are sorted in ascending order
		sorted.resize(n);
		MPI_Gather(received.data(), nlocal, MPI_FLOAT, sorted.data(), nlocal, MPI_FLOAT, 0, MPI_COMM_WORLD);

		std::cout << std::endl;
		if (std::is_sorted(sorted.begin(), sorted.end())) {
			std::cout << n << " elements have been sorted in ascending order in " << elapsed << " s" << std::endl;
			std::cout << p << " processes" << std::endl;
		} else {
			std::cout << "elements are not correctly sorted" << std::endl;
		}
		std::cout << std::endl;
	} else {
		// send sorted elements to process 0
		MPI_Gather(received.data(), nlocal, MPI_FLOAT, sorted.data(), nlocal, MPI_FLOAT, 0, MPI_COMM_WORLD);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// Process 0 initializes all array elements and distributes them
// calculate speedup and Karp-Flatt metric
void oddEvenSort2(int n) {
	double seqElapsed = 0;
	int p, id, idOdd, idEven;
	MPI_Status status;
	std::default_random_engine e;
	std::uniform_real_distribution<float> dist;

	MPI_Comm_size(MPI_COMM_WORLD, &p);
	MPI_Comm_rank(MPI_COMM_WORLD, &id);

	const int nlocal = n/p;
	n = nlocal*p;
	assert(n % p == 0);

	if (id == 0) {
		std::cout << "Odd-Even sort V2" << std::endl;
	}
	std::vector<float> sorted;
	std::vector<float> total(n);
	std::vector<float> elements(nlocal);
	std::vector<float> received(nlocal);
	std::vector<float> temp(nlocal);

	// TODO use barrier to synchronize start time
	MPI_Barrier(MPI_COMM_WORLD);
	double start = MPI_Wtime();

	// fill in elements with random values
	if (id == 0) {
		for (int i = 0; i < n; i++) {
			total[i] = dist(e);
		}
	}

	MPI_Scatter(total.data(), nlocal, MPI_FLOAT, received.data(), nlocal, MPI_FLOAT, 0, MPI_COMM_WORLD);

	// sort values locally
	std::sort(received.begin(), received.end());

	// determine the id of the processes that it needs to communicate during the odd and even phases
	if (id & 1) {
		idOdd = id + 1;
		idEven = id - 1;
	} else {
		idOdd = id - 1;
		idEven = id + 1;
	}
	if (idEven < 0 || idEven == p) idEven = MPI_PROC_NULL;
	if (idOdd  < 0 || idOdd  == p) idOdd = MPI_PROC_NULL;

	// main loop of odd-even sort: local data to send is in received buffer
	for (int i = 0; i < p; i++) {
		if (i & 1) {
			// odd phase
			MPI_Sendrecv(received.data(), nlocal, MPI_FLOAT, idOdd, 1, elements.data(), nlocal, MPI_FLOAT, idOdd, 1, MPI_COMM_WORLD, &status);
		} else {
			// even phase
			MPI_Sendrecv(received.data(), nlocal, MPI_FLOAT, idEven, 1, elements.data(), nlocal, MPI_FLOAT, idEven, 1, MPI_COMM_WORLD, &status);
		}
		if (status.MPI_SOURCE != MPI_PROC_NULL) {
			// sent data is in received buffer
			// received data is in elements buffer
			CompareSplit(nlocal, received.data(), elements.data(), temp.data(), id < status.MPI_SOURCE);
			// temp contains result of compare-split operation: copy temp back to received buffer
			copy(temp.begin(), temp.end(), received.begin());
		}
	}

	// TODO stop time measuring and reduce maximum time
	double localElapsed, elapsed = 0;

	// TODO use reduction to determine maximum time
	localElapsed = MPI_Wtime() - start;
	MPI_Reduce(&localElapsed, &elapsed, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

	if (id == 0) {
		// check if all elements are sorted in ascending order
		sorted.resize(n);
		MPI_Gather(received.data(), nlocal, MPI_FLOAT, sorted.data(), nlocal, MPI_FLOAT, 0, MPI_COMM_WORLD);

		// compute serial time
		start = MPI_Wtime();
		std::sort(total.begin(), total.end());
		seqElapsed = MPI_Wtime() - start;

		// compute Speedup S
		double S = seqElapsed / elapsed;

		//compute Karp-Flatt metric e
		double e_ = (1/S - 1/p) / (1 - 1/p);

		std::cout << std::endl;
		if (std::is_sorted(sorted.begin(), sorted.end())) {
			std::cout << n << " elements have been sorted in ascending order in " << elapsed << " s" << std::endl;
			std::cout << p << " processes" << std::endl;
			std::cout << "serial elapsed time = " << seqElapsed << std::endl;
			std::cout << "speedup S" << p << " = " << S << std::endl;
			std::cout << "karp-flatt metric e" << p << " = " << e_ << std::endl;
		} else {
			std::cout << "elements are not correctly sorted" << std::endl;
		}
		std::cout << std::endl;
	} else {
		// send sorted elements to process 0
		MPI_Gather(received.data(), nlocal, MPI_FLOAT, sorted.data(), nlocal, MPI_FLOAT, 0, MPI_COMM_WORLD);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
void oddEvenSortTests() {
	constexpr int n = 16000000;

	oddEvenSort1(n);
	oddEvenSort2(n);
}