#include <iostream>
#include <string>
#include <array>
#include "mpi.h"

//////////////////////////////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[]) {
	int p, id;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &p);
	MPI_Comm_rank(MPI_COMM_WORLD, &id);

	if (id == 0) {
		constexpr size_t gr_sz = 50;

		std::cout << "Process " << id << " receives greetings from " << p - 1 << " processes!" << std::endl;
		for (int i = 1; i < p; i++) {
			// TODO use MPI_Recv
			char greeting[gr_sz] = "";
			
			MPI_Recv(&greeting, gr_sz, MPI_CHAR, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			std::cout << greeting << std::endl;
		}
		std::cout << "Greetings, done!" << std::endl;
	} else {
		// TODO use MPI_Send
		std::string greetings("greetings from " + std::to_string(id));

		std::cout << id << " start send now!" << std::endl;
		MPI_Send(greetings.c_str(), greetings.size(), MPI_CHAR, 0, 0, MPI_COMM_WORLD);
	}

	MPI_Finalize();
}
