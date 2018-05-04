// Just send a string - nice simple example

#include <boost/mpi.hpp>
#include <string>
#include <iostream>

int main(int argc, char *argv[])
{
    boost::mpi::environment env{argc, argv};
    boost::mpi::communicator world;
    int tag = 10;
    if (world.rank() == 0)
    {
        std::string s;
        world.recv(boost::mpi::any_source, tag, s);
        std::cout << "Master received string \"" << s << "\"\n";
    }
    else
    {
        std::string s = "Hello, world!";
        world.send(0, tag, s);
    }
}
