/*
 * writebitstream.hpp
 *
 *  Created on: Jan 18, 2013
 *      Author: gpalou
 */

#ifndef WRITEBITSTREAM_HPP_
#define WRITEBITSTREAM_HPP_


#include <fstream>
#include <string>


namespace imageplus
{
    namespace segmentation
    {
    	//!
    	//! WriteBitStream open modes
    	//!
    	enum OpenMode
    	{
    		NEW, APPEND
    	};

        //!
        //! \brief Class for writing bitstreams to disk
        //!
        //! Wrapper class that allows you to write to file an specified number of bits. It keeps a buffer
        //!  of 8 bits and when full, it is written to file.
        //!
        //! Usage:
        //! \code
        //!    WriteBitStream writeBS;
        //!    writeBS.open("test.b");
        //!
        //!    uint64 to_write = 10; // 1010
        //!    uint8  n_bits_to_write = 4;
        //!    writeBS.write(to_write, n_bits_to_write);
        //!     //...
        //!    writeBS.force_write();
        //!    writeBS.close();
        //! \endcode
        //!
        //! \author Jordi Pont Tuset <jpont@gps.tsc.upc.edu>
        //!
        //! \date 29-12-2008
        //!
        //! \todo Use boost::dynamic_bitset to handle the sets of bits?
        //!
    	template <OpenMode mode = NEW>
        class WriteBitStream
        {
            public:

                //!
                //! \brief Default Constructor
                //!
                WriteBitStream() {
                	_space  = 8;
                	_buffer = 0;
                }

                //!
                //! \brief Constructor from file name
                //!
                //! \param[in] filename : File name
                //! \param[in] mode     : APPEND or NEW (default: NEW)
                //!
                explicit WriteBitStream( const std::string& filename) {
                	_space  = 8;
                	_buffer = 0;
                	open(filename);
                }

                //!
                //! \brief Destructor
                //!
                ~WriteBitStream() {

                }

                //!
                //! \brief Open a binary file
                //!
                //! \param[in] filename : File name
                //! \param[in] mode     : APPEND or NEW (default: NEW)
                //!
                void open( const std::string& filename) {
                	_filename = filename;

                	if ( _fp.is_open() )
                		throw ImagePlusError("BitStream already open, close it first");

                	_space  = 8;
                	_buffer = 0;

                	// Open file to write
                	if (mode==NEW)
                	{
                		_fp.open(_filename.c_str(), std::ios::out | std::ios::binary );
                	}
                	else if  (mode==APPEND)
                	{
                		_fp.open(_filename.c_str(), std::ios::out | std::ios::binary | std::ios::app);
                	}
                	else
                	{
                		throw ImagePlusError("Unknowm open mode in WriteBitStream");
                	}

                	if (!_fp.is_open())
                	{
                		throw ImagePlusFileNotFound(_filename);
                	}
                }

                //!
                //! \brief Close the bitstream
                //!
                void close( ) {
                	force_write();
                	_fp.close();
                }

                //!
                //! \brief Writes the specified number of bits to the file
                //!
                //! \param[in] to_write: Number to be written to disk (the upper bits from bits_to_write are ignored)
                //! \param[in] n_bits_to_write: Number of bits from \b to_write to be written to the file
                //!
                void write( uint64 to_write, uint8 n_bits_to_write ) {
                	if (!_fp.is_open())
                	{
                		throw;
                	}
                	//#endif

                	ASSERT(n_bits_to_write<=64, "You asked to write more than 64 bits.");
                	ASSERT(n_bits_to_write>0, "You asked to write 0 bits.");

                	if(n_bits_to_write > 64)
                	{
                		n_bits_to_write = 64;
                	}

                	const uint64 mask = 255;

                	if(n_bits_to_write)
                	{
                		if(n_bits_to_write < _space)
                		{
                			_buffer = _buffer | ((uint8)(to_write << (_space-n_bits_to_write) ));
                			_space = _space - n_bits_to_write;
                		}
                		else if (n_bits_to_write == _space)
                		{
                			_buffer = _buffer | ((uint8) to_write);
                			_fp.write((char*)&_buffer, 1);
                			_buffer = 0;
                			_space = 8;
                		}
                		else
                		{
                			uint8 shift = n_bits_to_write-_space;

                			_buffer = _buffer | ((uint8)(to_write >> shift));
                			_fp.write((char*)&_buffer, 1);

                			while(shift>8)
                			{
                				shift = shift-8;
                				_buffer = (uint8)( (to_write & (mask << shift)) >> shift);
                				_fp.write((char*)&_buffer, 1);
                			}

                			if (shift == 8)
                			{
                				_buffer = (uint8)(to_write);
                				_fp.write((char*)&_buffer, 1);
                				_buffer = 0;
                				_space = 8;
                			}
                			else
                			{
                				_buffer = (uint8)(to_write << (8-shift) );
                				_space = 8-shift;
                			}
                		}
                	}
                }

                //!
                //! \brief If the buffer is not empty, it writes the bits in it adding zeros up to a byte
                //!
                void force_write( ) {
                	if (_space < 8)
                	{
                		_fp.write((char*)&_buffer, 1);
                		_buffer = 0;
                		_space = 8;
                	}
                }

            private:

                //! Stores the file name in disk
                std::string _filename;

                //! Output filestream to write to disk
                std::ofstream _fp;

                //! Stores the current buffer byte
                uint8 _buffer;

                //! Stores the current free space in the buffer
                uint8 _space;
        };
    } // namespace segmentation
} //namespace imageplus


#endif /* WRITEBITSTREAM_HPP_ */
