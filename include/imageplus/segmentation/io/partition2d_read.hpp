/*
 * partition2d_read.hpp
 *
 *  Created on: Mar 7, 2013
 *      Author: gpalou
 */

#ifndef PARTITION2D_READ_HPP_
#define PARTITION2D_READ_HPP_

#include <boost/filesystem.hpp>
#include <imageplus/segmentation/partition/partition.hpp>
#include <imageplus/segmentation/io/readbitstream.hpp>

namespace imageplus {
	namespace segmentation {

	template<class PartitionModel>
	class PRLReader {

	public:

		PartitionModel read(std::string filename, bool read_mergings = true) {

			// Open file to write
			_filename = filename;

			boost::filesystem::path file_path( _filename );
			_filename_branch = file_path.branch_path();

			std::ifstream in_file;
			in_file.open( _filename.c_str(), std::ios::in);
			if(!in_file.is_open()) {
				throw;
			}

			std::string partition_file;
			in_file >> partition_file;
			boost::filesystem::path part_path(partition_file);

			// Read leaves partition
			std::string partition_path = "";
			if(part_path.is_complete()) //Partition path is complete
			{
				partition_path = partition_file;
			}
			else // Partition path is relative to the position of the XML file
			{
				partition_path = (_filename_branch / partition_file).string();
			}

			std::cout << "partition " << partition_path;
			PartitionModel partition = _read(partition_path);

			if (read_mergings) {
				// Read image (ignore it)
				std::string image_file;
				in_file >> image_file;

				// Reading the fusions (The file must contain groups of three integers, otherwise,
				//  the behaviour is undefined)
				uint64 num_father, num_son1, num_son2;
				in_file >> num_son1;

				_mergings.clear();

				while(!in_file.eof())
				{
					in_file >> num_son2;
					in_file >> num_father;

					std::vector<uint64> merging(3);
					merging[0] = num_son1; merging[1] = num_son2; merging[2] = num_father;
					_mergings.push_back(merging);

					in_file >> num_son1;
				}
			}

			return partition;
		}

		PartitionModel read_prl(std::string path) {
			return _read(path);
		}

		std::vector<std::vector<uint64> > mergings() {
			return _mergings;
		}

	private:

		//!
		//! \brief Private and overloaded method to write a MultiArray<uint32,2>
		//!
		//! \param[in]        m : partition to write
		//! \param[in]       fp : File pointer to write data
		//!
		PartitionModel _read(std::string& path) {
			// Open file

			std::ifstream fp (path.c_str(), std::ios::in | std::ios::binary);
			if (!fp.is_open())
			{
				throw ImagePlusFileNotFound(path);
			}

			// read magic number to start of file
			uint16 mn;
			fp.read((char*)&mn,sizeof(mn));
			if (mn != 255)
			{
				throw;
			}

			// read filetype
			uint8 ft;
			fp.read((char*)&ft,sizeof(ft));
			if (ft != 1)
			{
				throw;
			}

			// read compress
			uint8 c;
			fp.read((char*)&c,sizeof(c));
			//uint64 compress = static_cast<uint64>(c);

			// read datatype
			uint8 dt;
			fp.read((char*)&dt,sizeof(dt));
			//uint64 datatype = static_cast<uint64>(dt);

			// read dimensions
			uint64 d;
			fp.read((char*)&d,sizeof(d));

			std::vector<uint64> dims(d);
			for (uint64 i = 0; i < d; i++)
			{
				uint64 s;
				fp.read((char*)&s,sizeof(s));
				dims[i] = s;
			}

			//std::cout << mn << " " << ft << " " << d << " " << dims[0] << " " << dims[1] << std::endl;

			PartitionModel p(dims[0],dims[1]);

			_read_file(p,fp,path);
			//std::cout << "Partition read" << std::endl;

			return p;
		}

		void _read_file(PartitionModel& partition, std::ifstream& fp, std::string& filename) {

			Partition<uint32, 2> m(partition.sizes());

			uint32 *pointer = m.data();

			// Read  num_bits
			uint8 num_bits;
			fp.read((char*)&num_bits,  1);

			//std::cout << (uint64)num_bits << std::endl;

			// Close the file to read it in binary
			fp.close();

			ReadBitStream<2> rbs(filename.c_str());

			// Skip the header
			rbs.seekg(112+64*2);

			std::size_t total_elem = m.size_x() * m.size_y();
			std::size_t num_read_elem = 0;
			uint64 read_bits = 0;
			uint32 max_label = 0;
			uint32 current_label;
			uint16 current_length;  // It should be uint8 but we had problems with GCC 4.3
			std::size_t  width = m.size_x();

			while(num_read_elem < total_elem)
			{
				//std::cout << num_read_elem << std::endl;
				// Label casuistics
				read_bits = rbs.read(1);

				if(read_bits == 0) // "Up" value
				{
					current_label = *(pointer - width);
				}
				else
				{
					read_bits = rbs.read(1);

					if(read_bits == 0) // "Up-one-right" value
					{
						const uint32 *up_value = pointer - width;
						uint32 *up_right_value = pointer - width + 1;
						while(*up_value == *up_right_value)
						{
							up_right_value++;
						}
						current_label = *up_right_value;
					}
					else
					{
						read_bits = rbs.read(1);

						if(read_bits == 0) // "Max+1" value
						{
							max_label = max_label + 1;
							current_label = max_label;
						}
						else
						{
							read_bits = rbs.read(num_bits);
							current_label = (uint32)read_bits;
							if(current_label>max_label)
							{
								max_label = current_label;
							}
						}
					}
				}

				//Length casuistics
				read_bits = rbs.read(1);

				if(read_bits == 1) // 8 bits length
				{
					read_bits = rbs.read(8);
					current_length = static_cast<uint8>(read_bits);
				}
				else
				{
					read_bits = rbs.read(2);
					current_length = static_cast<uint8>(read_bits) + 1;
				}

				for(uint8 ii=0; ii<current_length; ii++)
				{
					*pointer++ = current_label;
				}

				num_read_elem += current_length;
			}

			typename PartitionModel::iterator p = partition.begin();
			typename PartitionModel::iterator p_end = partition.end();
			for (;p!=p_end;++p){
				(*p)(0) = m(p.pos())(0);
			}
		}


	public:

		std::vector<std::vector<uint64>>	_mergings;

		std::string _filename;

		//! Stores the branch of the filename (directory to be written)
		boost::filesystem::path _filename_branch;
	};

	}
}


#endif /* PARTITION2D_READ_HPP_ */
