#include <exception>

namespace boost {

void throw_exception( std::exception const & e ) {
  // LOG(LERROR)  << "boost exception, " << e.what();
}

}
