/* Multiple Communicators example.
Based on https://theboostcpplibraries.com/boost.mpi-communicators
*/
#include <boost/mpi.hpp>
#include <string>
#include <iostream>
namespace mpi = boost::mpi;
using namespace std;
//=============================================================================
int main(int argc, char *argv[])
{
  mpi::environment env{argc, argv};
  mpi::communicator world;
  /*
  All processes that pass the same integer to split(..) are linked
  to the same communicator. In this case any process with rank<2
  (processes 0 and 1) is one communicator, and any other process is
  another communicator.
  */
  const int comm1 = 100;
  const int comm2 = 200;
  const int comm_id = world.rank() < 2 ? comm1 : comm2;
  mpi::communicator local = world.split( comm_id );
  string s = "Hello from world rank " + to_string( world.rank() )
            + ", local rank " + to_string( local.rank() ) + ", comm_id=" + to_string(comm_id);
  /*
  Broadcast from local rank 0 to all processes in the local communicator.
  */
  mpi::broadcast(local, s, 0); // process with rank 0 in the LOCAL communicator.
  cout << world.rank() << ": " << s << '\n';
}
