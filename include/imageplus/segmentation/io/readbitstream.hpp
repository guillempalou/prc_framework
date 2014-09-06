/*
 * readbitstream.hpp
 *
 *  Created on: Mar 7, 2013
 *      Author: gpalou
 */

#ifndef READBITSTREAM_HPP_
#define READBITSTREAM_HPP_

#include <string>
#include <fstream>
#include <imageplus/core/imageplus_types.hpp>

namespace imageplus
{
    namespace segmentation
    {


    //!
    //! \brief Class for reading bitstreams from disk
    //!
    //! The read function returns a uint64 with the asked bits in the low significance part.
    //!
    //! Usage:
    //! \code
    //!    ReadBitStream readBS;
    //!    readBS.open("test.b");
    //!
    //!    uint8  n_bits_to_read = 4;
    //!    uint64 read_value = readBS.read(n_bits_to_write);
    //!    readBS.close();
    //! \endcode
    //!
    //! \author Jordi Pont Tuset <jpont@gps.tsc.upc.edu>
    //!
    //! \date 30-12-2008
    //!
    //! \todo Use boost::dynamic_bitset to handle the sets of bits?
    //!
    template<uint64 D>
    class ReadBitStream
    {
    public:

    	//!
    	//! \brief Default Constructor
    	//!
    	ReadBitStream() {
    		_bits_left  = 0;
    		_buffer = 0;
    	}

    	//!
    	//! \brief Constructor from file name
    	//!
    	//! \param[in] filename : File name
    	//!
    	ReadBitStream( const std::string& filename ) {
    		_filename = filename;

    		if ( _fp.is_open() )
    			throw;

    		_bits_left = 0;
    		_buffer = 0;

    		// Open file to read
    		_fp.open(_filename.c_str(), std::ios::in | std::ios::binary);
    		if (!_fp.is_open())
    		{
    			throw;
    		}
    	}

    	//!
    	//! \brief Open a file
    	//!
    	//! \param[in] filename : File name
    	//!
    	void open( const std::string& filename ){
    		_filename = filename;

    		if ( _fp.is_open() )
    			throw;

    		_bits_left = 0;
    		_buffer = 0;

    		// Open file to read
    		_fp.open(_filename.c_str(), std::ios::in | std::ios::binary);
    		if (!_fp.is_open())
    		{
    			throw;
    		}
    	}

    	//!
    	//! \brief Close the bitstream
    	//!
    	void close( ) {
    		 _fp.close( );
    	}

    	//!
    	//! \brief Move the pointer (next to be read) to a specified number of bits (absolute position)
    	//!
    	//! \param[in] position : Number of bits to be moved
    	//!
    	void seekg( uint64 position ) {
    		uint8  offset_bits  = (uint8)(position % 8);
    		uint64 offset_bytes = position >> 3;

    		if (!_fp.is_open())
    		{
    			throw ;
    		}

    		_fp.seekg(offset_bytes);
    		_bits_left  = 0;
    		_buffer = 0;

    		if (offset_bits)
    		{
    			read(offset_bits);
    		}
    	}

    	//!
    	//! \brief Returns the absolute position of the pointer (next to be read) in bits
    	//!
    	//! \return Absolute position of the pointer in bits
    	//!
    	uint64 tellg( ) {
    		if (!_fp.is_open()) throw;
    		if (_fp.tellg() < 0 )
    			throw;

    		return (uint64)(_fp.tellg()*8 - _bits_left);
    	}

    	//!
    	//! \brief Returns the number of bits until the end of the stream
    	//!
    	//! \return Number of bits remaining in the bitstream
    	//!
    	uint64 remaining_size( ) {
    		uint64 curr_pos = tellg();
    		_fp.seekg(0,std::ios::end);
    		uint64 end_pos  = tellg();
    		seekg(curr_pos);

    		return (end_pos - curr_pos);
    	}


    	//!
    	//! \brief Reads the specified number of bits from the file and moves the internal pointer
    	//!
    	//! \param[in] n_bits_to_read: Number of bits from to be read from the file
    	//! \return Number read from disk (the upper bits in the uint64 are 0)
    	//!
    	uint64 read( uint8 n_bits_to_read ) {
    		if (!_fp.is_open())
    		{
    			throw;
    		}

    		//ASSERT(n_bits_to_read<=64, "You asked to read more than 64 bits.");
    		//ASSERT(n_bits_to_read>0, "You asked to read 0 bits.");

    		if(n_bits_to_read > 64)
    		{
    			n_bits_to_read = 64;
    		}

    		//std::cout << "reading " << (uint64)n_bits_to_read << std::endl;

    		uint64 to_return = 0;

    		if (n_bits_to_read)
    		{
    			const uint8 mask = 255;

    			uint8 still_to_read = n_bits_to_read;

    			while(still_to_read>_bits_left)
    			{
    				to_return = to_return + ((uint64)_buffer<<(still_to_read-_bits_left));
    				_fp.read((char*)&_buffer, 1);
    				still_to_read = still_to_read - _bits_left;
    				_bits_left = 8;
    			}

    			_bits_left = _bits_left-still_to_read;
    			to_return = to_return + ((uint64)_buffer>>_bits_left);
    			_buffer = _buffer&(mask>>(8-_bits_left));
    		}
    		else
    		{
    			to_return=0;
    		}
    		//std::cout << "read " << to_return << std::endl;


    		return to_return;
    	}

    	//!
    	//! \brief Reads the specified number of bits from the file without moving the internal pointer
    	//!
    	//! \param[in] n_bits_to_read: Number of bits from to be read from the file
    	//! \return Number read from disk (the upper bits in the uint64 are 0)
    	//!
    	uint64 read_ahead( uint8 n_bits_to_read ) {
    		if (!_fp.is_open())
    		{
    			throw ;
    		}

    		uint64 curr_pos  = tellg();
    		uint64 to_return = read(n_bits_to_read);
    		seekg(curr_pos);

    		return to_return;
    	}

    private:

    	//! Stores the file name in disk
    	std::string _filename;

    	//! Input filestream to read from disk
    	std::ifstream _fp;

    	//! Stores the current buffer byte
    	uint8 _buffer;

    	//! Stores the current number of bits left to read in the buffer
    	uint8 _bits_left;
    };
    } // namespace segmentation
} //namespace imageplus


#endif /* READBITSTREAM_HPP_ */
