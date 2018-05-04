// Multiple communicators with broadcase example
// Based on https://theboostcpplibraries.com/boost.mpi-communicators
#include <boost/mpi.hpp>
#include <boost/serialization/string.hpp>
#include <string>
#include <iostream>
namespace mpi = boost::mpi;
using namespace std;

int main(int argc, char *argv[])
{
    mpi::environment env{argc, argv};
    mpi::communicator world;
    mpi::communicator local = world.split(world.rank() < 2 ? 11 : 22);
    string s;
    if( world.rank() == 0 ) s = "Hello, world!";
    mpi::broadcast(local, s, 0);
    cout << world.rank() << ": " << s << '\n';
}
