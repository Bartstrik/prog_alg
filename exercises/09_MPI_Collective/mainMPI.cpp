#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <numeric>
#include <iomanip>
#include "mpi.h" 

////////////////////////////////////////////////////////////////////////////////////////
template<typename Iter>
static void print(Iter beg, Iter end) {
	std::cout << '[';
	if (beg != end) std::cout << std::setw(2) << std::setfill('0') << *beg++;
	while (beg != end) {
		std::cout << ',' << std::setw(2) << std::setfill('0') << *beg++;
	}
	std::cout << ']' << std::endl;
}

////////////////////////////////////////////////////////////////////////////////////////
static void show(int p, int id, const std::vector<int>& v, const char msg[]) {
	if (id == 0) {
		std::cout << msg << std::endl;
		std::cout << "p0: ";
		print(v.begin(), v.end());
		std::vector<int> v1(p);

		for (int i = 1; i < p; i++) {
			MPI_Recv(v1.data(), p, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			std::cout << "p" << i << ": ";
			print(v1.begin(), v1.end());
		}
	} else {
		// send data to 0
		MPI_Send(v.data(), p, MPI_INT, 0, 0, MPI_COMM_WORLD);
	}
}

////////////////////////////////////////////////////////////////////////////////////////
static void init(std::vector<int>& v) {
	for (auto& val : v) val = 0;
}

////////////////////////////////////////////////////////////////////////////////////////
static void process() {
	int p, id;

	MPI_Comm_size(MPI_COMM_WORLD, &p);
	MPI_Comm_rank(MPI_COMM_WORLD, &id);

	std::vector<int> v1(p);
	std::vector<int> v2(p);

	if (id == 0) {
		v1[0] = 1;
	}

	show(p, id, v1, "start");

	// TODO use MPI collective operations

	v2.assign(v1.begin(), v1.end());
	MPI_Bcast(v2.data(), p, MPI_INT, 0, MPI_COMM_WORLD);
	show(p, id, v2, "MPI_Bcast");

	MPI_Barrier(MPI_COMM_WORLD);

	MPI_Scan(v2.data(), v1.data(), p, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
	show(p, id, v1, "MPI_Scan");

	MPI_Barrier(MPI_COMM_WORLD);

	v2.assign(v1.begin(), v1.end());
	MPI_Allgather(v2.data(), 1, MPI_INT, v1.data(), 1, MPI_INT, MPI_COMM_WORLD);
	show(p, id, v1, "MPI_Allgather");

	MPI_Barrier(MPI_COMM_WORLD);

	v2.assign(v1.begin(), v1.end());
	MPI_Allreduce(v2.data(), v1.data(), p, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
	show(p, id, v1, "MPI_Allreduce");

	MPI_Barrier(MPI_COMM_WORLD);

	v2.assign(v1.begin(), v1.end());
	v1 = {0,0,0,0,0};
	MPI_Exscan(v2.data(), v1.data(), p, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
	show(p, id, v1, "MPI_Exscan");

	MPI_Barrier(MPI_COMM_WORLD);

	v2.assign(v1.begin(), v1.end());
	v1 = {0,0,0,0,0};
	MPI_Alltoall(v2.data(), 1, MPI_INT, v1.data(), 1, MPI_INT, MPI_COMM_WORLD);
	show(p, id, v1, "MPI_Alltoall");

	MPI_Barrier(MPI_COMM_WORLD);

	v2.assign(v1.begin(), v1.end());
	v1 = {0,0,0,0,0};
	int a[] = {1,1,1,1,1};
	MPI_Reduce_scatter(v2.data(), v1.data(), a, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
	show(p, id, v1, "MPI_Reduce_scatter");

	MPI_Barrier(MPI_COMM_WORLD);

	v2.assign(v1.begin(), v1.end());
	v1 = {0,0,0,0,0};
	MPI_Reduce(v2.data(), v1.data(), p, MPI_INT, MPI_MAX, 0, MPI_COMM_WORLD);
	show(p, id, v1, "MPI_Reduce");
}

////////////////////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[]) {
	MPI_Init(&argc, &argv);

	process();

	MPI_Finalize();
}