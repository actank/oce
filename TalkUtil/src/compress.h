#ifndef COMPRESS_H_
#define COMPRESS_H_



#include <string>
namespace com {
namespace xiaonei {
namespace talk {
namespace util {

std::string gzip_compress( const std::string& source);

}
}
}
}
#endif /*COMPRESS_H_*/



/*
 * don't remove
 

#include <iostream>
#include <sstream>

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/format.hpp>
#include <boost/numeric/conversion/cast.hpp>
#include <boost/test/minimal.hpp>

#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/device/back_inserter.hpp>
#include <boost/iostreams/filter/zlib.hpp>
#include <boost/iostreams/filtering_stream.hpp>

std::string compress(std::string data)
{
std::stringstream compressed;
std::stringstream decompressed;
decompressed << data;
boost::iostreams::filtering_streambuf<boost::iostreams::input> out;
out.push(boost::iostreams::zlib_compressor());
out.push(decompressed);
boost::iostreams::copy(out, compressed);
return compressed.str();
}

std::string compress1(const std::string& data)
{
std::string compressed;
boost::iostreams::filtering_streambuf<boost::iostreams::output> out;
out.push(boost::iostreams::zlib_compressor());
out.push(boost::iostreams::back_inserter(compressed));
boost::iostreams::copy(boost::make_iterator_range(data), out);
return compressed;
}

void compress2(const std::string& data, std::string& buffer)
{
buffer.clear();
boost::iostreams::filtering_streambuf<boost::iostreams::output> out;
out.push(boost::iostreams::zlib_compressor());
out.push(boost::iostreams::back_inserter(buffer));
boost::iostreams::copy(boost::make_iterator_range(data), out);
}

std::string decompress(std::string data)
{
std::stringstream compressed;
std::stringstream decompressed;
compressed << data;
boost::iostreams::filtering_streambuf<boost::iostreams::input> in;
in.push(boost::iostreams::zlib_decompressor());
in.push(compressed);
boost::iostreams::copy(in, decompressed);
return decompressed.str();
}

std::string decompress1(const std::string& data)
{
boost::iostreams::filtering_streambuf<boost::iostreams::input> in;
in.push(boost::iostreams::zlib_decompressor());
in.push(boost::make_iterator_range(data));
std::string decompressed;
boost::iostreams::copy(in, boost::iostreams::back_inserter(decompressed));
return decompressed;
}

void decompress2(const std::string& data, std::string& buffer)
{
boost::iostreams::filtering_streambuf<boost::iostreams::input> in;
in.push(boost::iostreams::zlib_decompressor());
in.push(boost::make_iterator_range(data));
buffer.clear();
boost::iostreams::copy(in, boost::iostreams::back_inserter(buffer));
}

boost::posix_time::ptime current_time()
{
return boost::posix_time::microsec_clock::local_time();
}

double elapsed_seconds(const boost::posix_time::ptime& start_time)
{
static const double MSEC_PER_SEC = 1000.0;
boost::posix_time::ptime end_time = current_time();
boost::posix_time::time_duration elapsed = end_time - start_time;
return boost::numeric_cast<double>(elapsed.total_milliseconds()) / MSEC_PER_SEC;
}

int test_main(int , char*)
{
const int NUM_ITERATIONS = 10000;
const std::string send_data = "boo! how are you?";

const boost::format time_statement("%1%: %2% cycles took %3% seconds!");
const boost::format improvement_statement("%1%: Improvement %2% %%");

double t0;

// Try original algorithm
{
boost::posix_time::ptime start_time = current_time();

std::string received_data;
for (int i = 0; i < NUM_ITERATIONS; ++i)
{
const std::string buffer = compress(send_data);
received_data = decompress(buffer);
}
BOOST_CHECK(send_data == received_data);

t0 = elapsed_seconds(start_time);
std::cout << (boost::format(time_statement) % "ALGO 0 " % NUM_ITERATIONS % t0)
<< std::endl;
}

// Try new algorithm 1
{
boost::posix_time::ptime start_time = current_time();

std::string received_data;
for (int i = 0; i < NUM_ITERATIONS; ++i)
{
const std::string buffer = compress1(send_data);
received_data = decompress1(buffer);
}
BOOST_CHECK(send_data == received_data);

double e = elapsed_seconds(start_time);
std::cout << (boost::format(time_statement) % "ALGO 1 " % NUM_ITERATIONS % e)
<< std::endl;

double improvement = ((t0 - e) / t0) * 100.0;
std::cout << (boost::format(improvement_statement) % "ALGO 1 " % improvement)
<< std::endl;
}

// Try new algorithm 2
{
boost::posix_time::ptime start_time = current_time();

std::string received_data;
for (int i = 0; i < NUM_ITERATIONS; ++i)
{
std::string buffer;
compress2(send_data, buffer);
decompress2(buffer, received_data);
}
BOOST_CHECK(send_data == received_data);

double e = elapsed_seconds(start_time);
std::cout << (boost::format(time_statement) % "ALGO 2 " % NUM_ITERATIONS % e)
<< std::endl;

double improvement = ((t0 - e) / t0) * 100.0;
std::cout << (boost::format(improvement_statement) % "ALGO 2 " % improvement)
<< std::endl;
}

return 0;
} 
*/
