/*
 * discrete_space_signal_container.hpp
 *
 *  Created on: Jul 27, 2012
 *      Author: guillem
 */

#ifndef DISCRETE_SPACE_SIGNAL_CONTAINER_HPP_
#define DISCRETE_SPACE_SIGNAL_CONTAINER_HPP_

#include <imageplus/math/math_types.hpp>
#include <iostream>

namespace imageplus {

	//! Base class for the container of a Discrete Space Signal
	// The SignalPtr should be a pointer
	template<typename domain_coord_type, typename codomain_coord_type>
	class SignalContainer {

	public:

		//! dimensions of the coordinates
		static const uint64 coord_dimensions = domain_coord_type::RowsAtCompileTime;

		//! dimensions of the values
		static const uint64 value_dimensions = codomain_coord_type::RowsAtCompileTime;

		//! Coordinates type
		typedef domain_coord_type      					coord_type;

		//! Coord data type (int, float...)
		typedef typename coord_type::Scalar 			coord_data_type;

		//! Value type
		typedef codomain_coord_type						value_type;

		//! Value returned type
		typedef Eigen::Map<value_type>					value_ret_type;

		//! Value data type (int,float...)
		typedef typename value_type::Scalar				value_data_type;

		//! Default constructor
		SignalContainer() :  _data(NULL) {

		}

		//! Constructor with a delimiting hypercube
		//! \param[in] lower_point : lower_point of the hypercube (normally 0)
		//! \param[in] upper_point : upper_point of the hypercube (normally its size)
		SignalContainer(const coord_type& lower_point, const coord_type& upper_point) {
			_sizes = upper_point - lower_point;
			_lower_point = lower_point;
			_upper_point = upper_point;
			_owner = true;
			_init();
		}

		//! Constructor specifying sizes for the discrete space
		//! \param[in] size : vector containing each dimensions size
		SignalContainer(const coord_type& size) {
			_sizes = size;
			_lower_point = coord_type();
			_lower_point.fill(0);
			_upper_point = size;
			_owner = true;
			_init();
		}

		//! Constructor specifying sizes for the discrete space
		//! \param[in] size : vector containing each dimensions size
		//! \param[in] data : buffer to the data
		SignalContainer(const coord_type& size, value_data_type* data) {
			_sizes = size;
			_lower_point = coord_type();
			_lower_point.fill(0);
			_upper_point = size;
			_owner = false;
			_data = data;
			_init();
		}

		//! Copy constructor.
		//! Be careful! The space pointer should change if this copy constructor is called from a DiscreteSpaceSignal copy constructor
		//! \param[in] copy : container to copy
		SignalContainer(const SignalContainer& copy) : _sizes(copy._sizes), _w(copy._w), _lower_point(copy._lower_point), _upper_point(copy._upper_point) {
			_owner = false;
			_data = copy._data;
			//_init();
			//_copy_data(copy);
		}

		//! Default destructor
		~SignalContainer() {
			if (_owner) delete _data;
			//delete _units;
		}

		void operator=(const SignalContainer& copy) {
			if (_owner) delete _data;
			_sizes = copy._sizes;
			_w = copy._w;
			_lower_point = copy._lower_point;
			_upper_point = copy._upper_point;
			_owner = true;
			_init();
			_copy_data(copy);
		}

		//SignalContainer clone() {
			//_init();
		//}

		//! Function returning the address of a given coordinate in the discrete space
		//! \param[in] coord : coordinate of the space
		//! \return memory address of the data
		inline value_data_type* value_at_coord(const coord_type& coord) const {
			uint64 disp = (_w.transpose()*(coord-_lower_point)).sum();
			return _data+disp;
		}

		//! return a pointer to the data
		//! \return pointer
		value_data_type* data(const coord_type& offset) {
			uint64 disp = (_w.transpose()*(offset-_lower_point)).sum();
			return _data+disp;
		}

		//! return a pointer to the data
		//! \return pointer
		value_data_type* data() {
			return _data;
		}

		//! inits the data if something is read
		void init_data(const coord_type& size) {
			if (_data != NULL) delete _data;
			_owner = true;
			_sizes = size;
			_lower_point = coord_type();
			_lower_point.fill(0);
			_upper_point = size;
			_init();
		}

	private:

		//! Function called to init the container. Sets the sizes, and the memory storage order
		void _init() {

			_w = coord_type();
			_w(0) = value_dimensions;
			for (uint64 i = 1; i < coord_dimensions; i++) {
				_w(i) = _w(i-1)*_sizes(i-1);
			}

			uint64 N = _sizes.prod();

			if (_owner)
				_data = new value_data_type[N*value_dimensions]();
		}

		//! Used to copy data
		void _copy_data(const SignalContainer& copy) {
			uint64 N = _sizes.prod()*value_dimensions;
			for (uint64 i = 0; i < N; i++) {
				_data[i] = copy._data[i];
			}
		}

	protected:

		//! size vector
		coord_type _sizes;

		//! weight vector for calculating memory displacements
		coord_type _w;

		//! lower hypercube point
		coord_type _lower_point;

		//! upper hypercube point
		coord_type _upper_point;

		//! pointer to the data
		value_data_type *_data;

		//! owner of the data?
		bool _owner;
	};

}
#endif /* DISCRETE_SPACE_SIGNAL_CONTAINER_HPP_ */
