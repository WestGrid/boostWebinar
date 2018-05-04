// Vector sum using boost MPI scatter to send chunks to n_processors

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <cmath>
#include <unistd.h>
#include <limits.h>    // some machines define "CLK_TCK" here.
#include <sys/times.h>

#include <boost/mpi.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/mpi/collectives.hpp>

const int N = 100;

namespace mpi = boost::mpi;

using namespace std;

//=============================================================================
double abs_sum( vector<double> & vec )
{
	double sum = 0.0;
	for( auto &v: vec ) sum += fabs(v);
}
//=============================================================================
double vec_sum( vector<double> & vec )
{
	double sum = 0.0;
	for( auto &value:vec ) sum += value;
	return sum;
}
//=============================================================================
double vec_norm( vector<double>& vec )
{
	int tag = 0;         // currently unused
	//MPI::Status status;
	//mpi::environment env;
	mpi::communicator world;
	int rank = world.rank();
	int n_processors = world.size();

	//int n = vec.size();  //ONLY THE MASTER KNOWS THIS!!!
	const int n = N;
	const int n_segments = n_processors;  // the master also does work
	const int stride = 1 + (n-1) / n_segments;

	// MASTER: Send vector chunks out to slaves

    // Each process needs a vector, but only one process actually scatters.
    // It's all in one function so the receive is also in that one function.
    /* Seems weird because every process calls the same function.
     * Would be much better if the scattering process did it.
     * Then I wouldn't need the same data structures everywhere.
     * The kludge here is to only fill the chunk_vec in process 0
    */

  // MASTER: fill the vector of chunks

  vector<vector<double>> chunk_vec;
	if( rank == 0 ){

		cout << "vec_norm " << rank << ": master scattering chunks with stride=" << stride
			<< " n_processors=" << n_processors << endl;

		for( int process=0; process<n_processors; ++process ){
			int istart = process*stride;
			int iend = istart + stride;

			// Need to copy  the chunk into a vector for the send
			// * not great as I would prefer to do address+stride which has no copy
			// * Wonder if I can do this with boost vectors?? Might use "scatter".

			vector<double> chunk;
			for( int i=istart; i<iend; ++i ){
				chunk.push_back(vec[i]);
			}
			chunk_vec.push_back(chunk);
		}
	}

	// Master scatters chunks, all processes receive chunks

	int scatter_process = 0;
	vector<double> received_vec;
	mpi::scatter( world, chunk_vec, received_vec, scatter_process );

	// All processes do their sum

	double sum = abs_sum( received_vec );
	cout << "vec_norm " << rank << ": received_vec.size=" << received_vec.size()
	     << " calculated sum=" << sum << endl;

	// MASTER: Receive the sums and sum them
  //   Note that the master "gather(..)" BOTH sends and receives
  //      * a send for the master is not needed (caused me a nasty debug!)
	// ALL: send the sums

  if( rank == 0 ){
		vector<double> sums;
		//cout << "vec_norm " << rank << ": gathering" << endl;
		mpi::gather( world, sum, sums, scatter_process );
		cout << "vec_norm " << rank << ": gathered sums.size=" << sums.size() << endl;
		return vec_sum( sums );
	} else {
		mpi::gather( world, sum, scatter_process );
	}
	return -1;
}
//=============================================================================
int main()
{
	mpi::environment env;
	mpi::communicator world;
	int rank = world.rank();
	int n_processors = world.size();

	if( rank == 0 ){
		cout << "main " << rank << ": n_processors=" << n_processors
			<< " N=" << N << endl;
		if( N%n_processors != 0 ){
			cout << "main " << rank << ": ERROR: n_processors has to divide evenly in N\n";
			cout << "        Change the number of processors to fix this\n";
			world.abort( 1 );
			exit(1);
		}
	}

	/* The master creates the array
	 * But all the MPI is in the norm function so only the master
	 * actually allocated the vector. The slaves don't see the vector
	 * they just get explicitly sent their chunk.
	*/

	vector<double> vec;
	if( rank == 0 ){
		for( int i=0; i<N; ++i ){
			vec.push_back( double(100+i) );
		}
		cout << "main " << rank << ": vec.size()=" << vec.size() << endl;
	} // End setup on master

	double norm = vec_norm( vec );
	double serial_norm = abs_sum( vec );

	if( rank == 0 ){
		cout << "main " << rank << ": Finished: norm=" << norm
		     << " serial_norm=" << serial_norm << '\n'
			<< " ***********************************************" << endl;
	}
}
