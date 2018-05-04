#include <iostream>
#include <boost/mpi.hpp>
namespace mpi = boost::mpi;
using namespace std;

int main()
{
    mpi::environment env {};
    mpi::communicator world;
    int rank = world.rank();
    int tag = 31;           // always identify the messages
    if( rank == 0 ){
        int i;
        // One receive gets whichever other process sent first.
        world.recv( mpi::any_source, tag, i );
        cout << "received from " << i << '\n';
    } else {
        world.send( 0, tag, rank );
    }
}
