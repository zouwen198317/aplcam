#ifndef __FILE_UTILS_H__
#define __FILE_UTILS_H__

#include <string>

bool file_exists( const std::string &infile );
bool directory_exists( const std::string &infile );
void mkdir_p( const std::string &dir );

const std::string fileHashSHA1( const std::string &filename );


#endif
