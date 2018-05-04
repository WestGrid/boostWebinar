//#############################################################
// $Source: /home/mann/Dropbox/PJM/research/1/examples/c++11_mpi/boostWebinar2018/RCS/sendclass1.cpp,v $
// $Revision: 1.1 $
// $Date: 2018/05/04 16:04:21 $
/*
  MPI Example: Package up a class and send it off. USES BOOST MPI.

  Output: just to keep each process separate
  * the master (sender 0) uses cout
  * the receiver outputs to a separate file

  The recommended approach would be to send output messages
  back to the master (or at least an output process) and let
  that single process handle all error messages, output, etc.
*/
//##############################################################
#include <cstdlib>

#include <sstream>
#include <iostream>
#include <fstream>

#include <boost/mpi.hpp>
#include <boost/serialization/string.hpp>

namespace mpi = boost::mpi;
using namespace std;
const string PROGRAM = "sendclass1";

//==============================================================
// Simple class containing a few data items
// Includes serialize (from Boost template)

class TestClass
{
private:
  int i;
  double a;
  string desc;

public:
  TestClass();
  TestClass( int i_in, double a_in, string desc_in ) :
     i(i_in), a(a_in), desc(desc_in) {}

  ~TestClass(){}

  friend class boost::serialization::access;

  template<class Archive>
    void serialize( Archive &ar, const unsigned int version )
    {
      ar & i;
      ar & a;
      ar & desc;
    }

  friend ostream& operator << ( ostream&, TestClass& );
};

ostream& operator << ( ostream& s, TestClass& tclass )
{
  s << "i:    " << tclass.i << '\n'
    << "a:    " << tclass.a << '\n'
    << "desc: \"" << tclass.desc << "\"\n";
  return s;
}
//==============================================================
TestClass::TestClass()
{
  i = 9876;
  a = -1.234;
  desc = "Test string from TestClass constructor";
}
//==============================================================
int main(int argc, char *argv[])
{
  mpi::environment env(argc,argv);
  mpi::communicator world;
  int rank = world.rank();
  int n_processes = world.size();

  // We need at least 2 processes

  if( n_processes < 2 ){
    cerr << PROGRAM << ": ERROR: n_processes = "
         << n_processes << '\n';
    exit(1);
  }

  if( rank == 0 ){
      cout << PROGRAM << rank << ": INFO: "
           << "n_processes = " << n_processes << endl;
  }

  // send/receive pair

  const int sender = 0;
  const int receiver = 1;

  stringstream receiver_filename;
  receiver_filename << "sendclass1_from" << receiver << ".out";

  int tag = 0;

  if( rank == sender ){
	  cout << "********************************************\n"
	       << "sendclass1: Output from the receiver is in file \"" << receiver_filename.str() << "\"\n"
	       << "********************************************\n";
    cout << "sendclass1 " << rank << " (sender): n_processes=" << n_processes << '\n';

    std::vector<TestClass> tvector;
    tvector.push_back( TestClass(1,2.0,"hi") );
    tvector.push_back( TestClass(10,10.0, "hi again") );

    world.send( receiver, tag, tvector );

    cout << "sendclass1 " << rank << ": sent message." << endl;

  } else if( rank == receiver ){
    ofstream s( receiver_filename.str().c_str() );
    s << "sendclass1 " << rank << ": receiver started.  Waiting for message." << endl;

    std::vector<TestClass> treceive;
    world.recv( sender, tag, treceive );
    for( auto &tvalue: treceive ){
      s << tvalue << '\n';
    }
  }
}
