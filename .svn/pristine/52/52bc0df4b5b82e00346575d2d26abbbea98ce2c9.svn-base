/*
 * partition2d_write.hpp
 *
 *  Created on: Jan 18, 2013
 *      Author: gpalou
 */

#ifndef PARTITION2D_WRITE_HPP_
#define PARTITION2D_WRITE_HPP_

#include <imageplus/segmentation/io/writebitstream.hpp>

namespace imageplus {
	namespace segmentation {

	//! \brief magick number for mult files
	const uint16 MULT_MAGIC_NUMBER = 255;

	//! \brief default filetype for mult files
	const uint8 MULT_FILETYPE = 1;

	//!
	//! Data types for multiarray files
	//!
	enum DataType
	{
		UINT8,INT8,UINT16,INT16,UINT32,INT32,UINT64,INT64,FLOAT32,FLOAT64,NONETYPE
	};

	template<class PartitionModel>
	class PRLWriter {

	public:

		void write(PartitionModel& partition, std::string filename) {

			// Open file to write
			_filename = filename;

			std::ofstream fp (_filename.c_str(), std::ios::out | std::ios::binary);
			if (!fp.is_open())
				throw;

			// write magic number to start of file
			fp.write((char*)&MULT_MAGIC_NUMBER,sizeof(MULT_MAGIC_NUMBER));
			if (!fp.good())
				throw;

			// write filetype to start of file
			fp.write((char*)&MULT_FILETYPE,sizeof(MULT_FILETYPE));
			if (!fp.good())
				throw;

			uint8 c = 1; //To make it compatible with partition run length (PRL)
			fp.write((char*)&c,sizeof(c));

			//write header
			c = (uint8)UINT32;
			fp.write((char*)&c,sizeof(c));

			// write number of dimensions
			uint64 d = 2;
			fp.write((char*)&d,sizeof(uint64));

			// write size of each dimension
			for (uint32 i = 0; i < 2; i++)
			{
				d = partition.sizes()(i);
				fp.write((char*)&d,sizeof(uint64));
			}

			// write multiarray
			_write(partition,fp);

			// finished
			fp.close();

		}

	private:

		//!
		//! \brief Private and overloaded method to write a MultiArray<uint32,2>
		//!
		//! \param[in]        m : partition to write
		//! \param[in]       fp : File pointer to write data
		//!
		void _write(PartitionModel& partition, std::ofstream& fp) {

			Partition<uint32, 2> m(partition.sizes());
			Partition<uint32, 2>::iterator p = m.begin();
			Partition<uint32, 2>::iterator p_end = m.end();
			for (;p!=p_end;++p){
				(*p)(0) = partition(p.pos())(0);
			}

			// Pointer to the input data
			const uint32 *input = m.data();

			// Width of the multiarray
			const uint64 width = m.size_x();

			// Compute the maximum value
			const uint64 num_elements = m.size_x() * m.size_y();
			uint32 max_value = 0;
			const uint32 *temp = input;
			for(uint64 ii=0; ii<num_elements; ii++)
			{
				if(*temp>max_value) max_value = *temp;
				temp++;
			}

			// Compute the number of bits needed
			const uint8 num_bits = (uint8)ceil(log((float64)(max_value+1))/log(2.));

			// Write the number of bits used
			fp.write((char*)&num_bits, 1);

			uint32 current_label= *input;
			uint32 max_label = current_label;

			uint8  current_length = 0;
			uint8  n_bits_to_write;
			uint64 to_write;

			fp.close();

			WriteBitStream<APPEND> wbs(_filename.c_str());

			// Write first label
			to_write = (uint64)(((uint64)7)<<num_bits)+(uint64)current_label; // 111 + N bits label
			n_bits_to_write = num_bits+3;
			wbs.write(to_write, n_bits_to_write);


			for(uint64 ii=0; ii<num_elements; ii++)
			{
				if(*input==current_label && current_length < 255)
				{
					current_length++;
				}
				else
				{
					// Length casuistry
					_code_length(current_length, to_write, n_bits_to_write);

					// Write length
					wbs.write(to_write, n_bits_to_write);

					// New label
					current_label = *input;
					current_length = 1;

					if (ii>=width) //We are not in the first row
					{
						// Label casuistry:
						// 	"Up" value           -->  0
						// 	"Up-one-right" value -->  10
						// 	"Max label" +1       -->  110
						//  "Label directly"     -->  111 (+ N bits of label)
						uint32 ref_label1 = *(input-width);
						uint8 num_changes = 0;
						bool found = false;
						uint32 offset = 0;

						do
						{
							uint32 ref_label2 = ref_label1;
							ref_label1 = *(input-width+offset);

							if(ref_label1 != ref_label2)
								num_changes++;

							if(ref_label1 == current_label)
								found = true;

							offset++;
						}
						while(num_changes<1 && ((ii+offset)%width>0) && !found);

						if (found)
						{
							switch (num_changes)
							{
							case 0:
							{
								to_write = 0; //0
								n_bits_to_write = 1;
							}
							break;
							case 1:
							{
								to_write = 2; //10
								n_bits_to_write = 2;
							}
							break;
							default: // We shouldn't get here...
							ASSERT(false, "Looking for current_label in the upper row, but something went wrong");
							break;
							}
						}
						else
						{
							if ((current_label-max_label)==1)
							{
								to_write = 6; //110
								n_bits_to_write = 3;
							}
							else
							{
								to_write = (uint64)(((uint64)7)<<num_bits)+(uint64)current_label; // 111 + N bits label
								n_bits_to_write = num_bits+3;
							}
						}
					}
					else // We are in the first row, we have no "up" reference
					{
						if ((current_label-max_label)==1)
						{
							to_write = 6; //110
							n_bits_to_write = 3;
						}
						else
						{
							to_write = (uint64)(((uint64)7)<<num_bits)+(uint64)current_label; // 111 + N bits label
							n_bits_to_write = num_bits+3;
						}
					}

					// Write label
					wbs.write(to_write, n_bits_to_write);

					// Update max_label
					if(max_label<current_label)
					{
						max_label = current_label;
					}
				}
				input++;
			}

			//Write last length
			_code_length(current_length, to_write, n_bits_to_write);

			// Actually write it
			wbs.write(to_write, n_bits_to_write);
			wbs.force_write();

			// close the bitstream
			wbs.close();

		}


		//!
		//! \brief Private method to code a "run-length" in the ELEMENT_RUNLENGTH compression method
		//!
		//! \param[in]   length         : Length to code
		//! \param[in]   to_write       : Coded binary stream
		//! \param[in]   n_bits_to_write: Number of bits to be written
		//!
		void _code_length(const uint8& length, uint64& to_write, uint8& n_bits_to_write) {
			switch (length)
			{
			case 1:
				to_write = 0; //000
				n_bits_to_write = 3;
				break;
			case 2:
				to_write = 1; //001
				n_bits_to_write = 3;
				break;
			case 3:
				to_write = 2; //010
				n_bits_to_write = 3;
				break;
			case 4:
				to_write = 3; //011
				n_bits_to_write = 3;
				break;
			default:  // >4
				to_write = (uint64)256+(uint64)length; //1 + 8 bits length
				n_bits_to_write = 9;
			}
		}

	public:
		std::string _filename;
	};

	}
}

#endif /* PARTITION2D_WRITE_HPP_ */
