/*
 * image_partition_signal.hpp
 *
 *  Created on: Jul 30, 2012
 *      Author: guillem
 */

#ifndef IMAGE_PARTITION_SIGNAL_HPP_
#define IMAGE_PARTITION_SIGNAL_HPP_

#include <imageplus/core/signal.hpp>
#include <deque>
#include <fstream>

namespace imageplus {
	namespace segmentation {

		//! Class defining a partition of a discrete space. Each pixel is defined a id_type
		template<typename id_type, uint64 dimensions>
		class Partition : public Signal<int64, id_type, dimensions, 1> {
		public:

			typedef Signal<int64, id_type, dimensions, 1> 				BaseClassType;
			typedef typename BaseClassType::coord_type					coord_type;

			static const ConnectivityType default_connectivity = neighborhood_traits<dimensions>::default_connectivity;
			static const ConnectivityType default_forward_connectivity = neighborhood_traits<dimensions>::default_forward_connectivity;

		public:

			//! Default Constructor
			Partition() : BaseClassType(), _max_label(0) {

			}

			//! copy constructor
			//! \param[in] copy : other partition
			Partition(const Partition& copy) : BaseClassType(const_cast<Partition&>(copy)), _max_label(copy._max_label) {
			}

			/*!
			 * Constructor for 2D
			 * \param[in] sx : sixe in x
			 * \param[in] sy : size in y
			 */
			Partition(uint64 sx, uint64 sy) : BaseClassType(coord_type(sx,sy)), _max_label(0) {
			}

			/*!
			 * Constructor for 2D
			 * \param[in] sx : sixe in x
			 * \param[in] sy : size in y
			 * \param[in] sz : size in z
			 */
			Partition(uint64 sx, uint64 sy, uint64 sz) : BaseClassType(coord_type(sx,sy,sz)), _max_label(0) {

			}

			/*!
			 * Constructor for general dimensions
			 * \param[in] size : size for each dimension
			 */
			Partition(const coord_type& size) : BaseClassType(size), _max_label(0) {
			}

			/*!
			 * Copy constructor
			 * \param[in] copy: pointer to another partition
			 */
			Partition(Partition& copy) : BaseClassType(const_cast<Partition&>(copy)), _max_label(copy._max_label) {
			}

			/*!
			 * Returns the size in the first dimension
			 * \return size of x
			 */
			uint64 size_x() {
				return BaseClassType::_sizes(0);
			}

			/*!
			 * Returns the size in the second dimension
			 * \return size of y
			 */
			uint64 size_y() {
				return BaseClassType::_sizes(1);
			}

			/*!
			 * Returns the size in the third dimension
			 * \return size of z
			 */
			uint64 size_z() {
				return BaseClassType::_sizes(2);
			}

			/*!
			 * Sets a unique label for each unit
			 */
			void set_unique_labels() {
				typename BaseClassType::iterator it = (*this).begin();
				typename BaseClassType::iterator end = (*this).end();
				_max_label = 1;
				for (; it != end; ++it) {
					(*it)(0) = _max_label++;
				}
				_max_label--;
			}

			/*!
			 * Finds equal zones and labels them (aka. label_flatzone)
			 */
			template<ConnectivityType adjacency_type, class channel_type, uint64 channels>
			void set_flatzone_labels(Signal<int64, channel_type, dimensions, channels>& img) {

				typedef	Signal<int64, channel_type, dimensions, channels> 				SignalType;
				typedef typename SignalType::coord_type									coord_type;
				typedef typename SignalType::value_type									value_type;

				typename BaseClassType::iterator it = (*this).begin();
				typename BaseClassType::iterator end = (*this).end();

				typedef typename BaseClassType::template general_adjacency_iterator<adjacency_type>::type  neighbor_iterator;

				uint64 curr_value = 1;
				for (;it != end; ++it) {
					//std::cout << " * " << it.pos().transpose() << " " << std::endl;
					if((*it).isZero())
					{
						std::deque<coord_type> to_scan;
						to_scan.push_back(it.pos());

						while(!to_scan.empty())
						{
							coord_type& t = to_scan.front();
							(*this)(t)(0) = curr_value;

							neighbor_iterator neigh_it 			= BaseClassType::template general_adjacency_begin<adjacency_type>(t);
							neighbor_iterator neigh_it_end 		= BaseClassType::template general_adjacency_end<adjacency_type>(t);
							for( ; neigh_it!=neigh_it_end; ++neigh_it)
							{
								//std::cout << t.transpose() << " " << neigh_it().transpose() << std::endl;
								if((*neigh_it).isZero())
								{
									const value_type& v1 =  img(t);
									const value_type& v2 =  img(neigh_it.pos());
									if((v1 - v2).isZero())
									{
										(*this)(neigh_it.pos())(0)=curr_value;
										to_scan.push_back(neigh_it.pos());
									}
								}
							}

							to_scan.pop_front();
						}
						curr_value++;
					}
					//exit(0);
				}
				_max_label = curr_value-1;
			}

			void write_partition(std::string partition_path) {
				std::ofstream fout(partition_path.c_str(), std::fstream::binary);

				// Save the leaves partition
				typename BaseClassType::value_data_type* data = this->data();

				uint64 l = BaseClassType::_sizes.prod();

				uint64 D = dimensions;
				fout.write((const char*) &D, sizeof(uint64));
				for (uint64 i = 0; i < dimensions; i++) fout.write((const char*) &BaseClassType::_sizes(i), sizeof(uint64));
				fout.write((const char*) data, sizeof(typename BaseClassType::value_data_type) * l);
			}

			void read_partition(std::string partition_path) {
				// Read the leaves partition
				std::ifstream fin(partition_path.c_str(), std::fstream::binary);

				uint64 D;
				coord_type sizes;

				fin.read((char*)&D, sizeof(uint64));
				for (uint64 i = 0; i < D; i++) fin.read((char*) &sizes(i), sizeof(uint64));
				if (D == 2 && dimensions == 3) sizes(2) = 1;
				uint64 l = sizes.prod();

				this->init_data(sizes);

				typename BaseClassType::value_data_type* data = this->data();

				fin.read((char*) data, sizeof(typename BaseClassType::value_data_type) * l);
			}

			uint64 max_label() {
				return _max_label;
			}

			void set_max_label(uint64 max_label) {
				 _max_label = max_label;
			}

		private:

			uint64 _max_label;

		};

	}
}



#endif /* IMAGE_PARTITION_SIGNAL_HPP_ */
