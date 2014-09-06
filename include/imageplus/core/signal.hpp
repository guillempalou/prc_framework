/*
 * discrete_space_signal.hpp
 *
 *  Created on: Jul 27, 2012
 *      Author: guillem
 */

#ifndef DISCRETE_SPACE_SIGNAL_HPP_
#define DISCRETE_SPACE_SIGNAL_HPP_

#include <Eigen/Dense>

#include <boost/utility.hpp>

#include <imageplus/core/exceptions.hpp>
#include <imageplus/core/imageplus_types.hpp>

#include <imageplus/core/iterators/global_iterator.hpp>
#include <imageplus/core/iterators/adjacency_iterator.hpp>
#include <imageplus/core/iterators/region_iterator.hpp>
#include <imageplus/core/iterators/roi_iterator.hpp>

#include <imageplus/core/signal_container.hpp>

namespace imageplus {

	//! Class for a discrete space signal
	template<typename domain_coords_type, typename codomain_coords_type, uint64 domain_dimensions, uint64 codomain_dimensions>
	class Signal {
	public:

		static const uint64				coord_dimensions = domain_dimensions;
		static const uint64				value_dimensions = codomain_dimensions;
		static const ConnectivityType 	default_connectivity = neighborhood_traits<coord_dimensions>::default_connectivity;
		static const ConnectivityType 	default_forward_connectivity = neighborhood_traits<coord_dimensions>::default_forward_connectivity;

		//!typedef for the inside pixel types
		typedef Eigen::Matrix<codomain_coords_type, codomain_dimensions,1> 	value_type;

		//! typedef for the coord types
		typedef Eigen::Matrix<domain_coords_type, domain_dimensions,1>     	coord_type;

		//! typedef to operate on float vectors for position
		typedef Eigen::Matrix<float64, domain_dimensions,1>     			coord_float_type;

		//! typedef to operate on float vectors for values
		typedef Eigen::Matrix<float64, codomain_dimensions,1> 				value_float_type;

		//! coord datatype
		typedef domain_coords_type										  	coord_data_type;

		//! value data type
		typedef codomain_coords_type										value_data_type;




		//! this class
		typedef Signal<domain_coords_type, codomain_coords_type, domain_dimensions, codomain_dimensions> 			ThisClassType;

		//! Container type for this kind of signal
		typedef SignalContainer<coord_type,value_type>																ContainerType;

		//! Value of the return type (normally an eigen map)
		typedef typename ContainerType::value_ret_type																value_ret_type;

		//! global signal iterator
		typedef global_iterator_type<ThisClassType>																	iterator;

		//! global signal iterator
		typedef roi_iterator_type<ThisClassType>																	roi_iterator;

		//! adjacency iterator
		typedef general_adjacency_iterator_type<ThisClassType>														adjacency_iterator;

		//! general work around for partial template specialization
		template <ConnectivityType c>
		struct general_adjacency_iterator {
			typedef general_adjacency_iterator_type<ThisClassType, c> type;
		};

		//! region iterator
		template<class RegionModel>
		struct region_iterator {
			typedef region_iterator_type<ThisClassType,RegionModel>		type;
		};

		//! Default constructor
		Signal()  {
		}

		//! Constructor with a specific size
		//! \param[in] size : size of the hypercube
		Signal(coord_type size) : _sizes(size), _data(size) {
			_lower_point.fill(0);
			_upper_point = size - coord_type::Ones();
		}

		//! Constructor defining two opposite vertex of an hypercube. WARNING: the two points are included!!!!
		//! \param[in] lower : lower_point
		//! \param[in] upper : upper_point
		Signal(coord_type lower, coord_type upper) : _lower_point(lower), _upper_point(upper), _data(this,lower,upper) {
			_sizes = _upper_point - _lower_point + coord_type::Ones();
		}

		//! Copy constructor
		//! \param[in] copy : signal to be copied. Notice that for the container, the space pointer should change from copy._space
		Signal(Signal& copy) : _sizes(copy._sizes), _lower_point(copy._lower_point), _upper_point(copy._upper_point), _data(copy._sizes,copy.data()) {
		}

		//! Constructor with a pointer to the data
		//! \param[in] size : size of the buffer
		//! \param[in] data :  pointer to the buffer
		Signal(coord_type size, value_data_type* data) : _sizes(size), _data(size,data) {
			_lower_point.fill(0);
			_upper_point = _sizes - coord_type::Ones();
		}

		//! Overloading operator =
		//! Must handle the pointer to the data container.
		//! \param[in] copy : signal to be copied. Notice that for the container, the space pointer should change from copy._space
		void operator=(const Signal& copy) {
			_sizes = copy._sizes;
			_lower_point = copy._lower_point;
			_upper_point = copy._upper_point;

			// call the copy constructor for the container
			_data = copy._data;
		}

		//! Implemented functions to retrieve a unit coordinate
		//! \param[in] coord : coordinate of the unit
		//! \return unit at pos coord
		inline value_ret_type operator()(const coord_type& coord) {
			return value_at_coord(coord);
		}

		//! Implemented functions to retrieve a unit coordinate for 2D coords
		//! \param[in] x : x coordinate of the unit
		//! \param[in] y : y coordinate of the unit
		//! \return unit at pos (x,y)
		inline value_ret_type operator()(domain_coords_type x, domain_coords_type y) {
			return value_at_coord(x,y);
		}

		//! Implemented functions to retrieve a unit coordinate for 3D coords
		//! \param[in] x : x coordinate of the unit
		//! \param[in] y : y coordinate of the unit
		//! \param[in] z : z coordinate of the unit
		//! \return unit at pos (x,y)
		inline value_ret_type operator()(domain_coords_type x, domain_coords_type y, domain_coords_type z) {
			return value_at_coord(x,y,z);
		}

		//! Implemented functions to retrieve a unit coordinate
		//! \param[in] coord : coordinate of the unit
		//! \return unit at pos coord
		inline value_ret_type value_at_coord(const coord_type& coord) {
			return _data.value_at_coord(coord);
		}

		//! Implemented functions to retrieve a unit coordinate for 2D coords
		//! \param[in] x : x coordinate of the unit
		//! \param[in] y : y coordinate of the unit
		//! \return unit at pos (x,y)
		inline value_ret_type value_at_coord(domain_coords_type x, domain_coords_type y) {
			coord_type coord(x,y);
			return _data.value_at_coord(coord);
		}

		//! Implemented functions to retrieve a unit coordinate for 3D coords
		//! \param[in] x : x coordinate of the unit
		//! \param[in] y : y coordinate of the unit
		//! \param[in] z : z coordinate of the unit
		//! \return unit at pos (x,y)
		inline value_ret_type value_at_coord(domain_coords_type x, domain_coords_type y, domain_coords_type z) {
			coord_type coord(x,y,z);
			return _data.value_at_coord(coord);
		}

	//Sizes method
	public:

		//! Returns the sizes for every dimensions
		//! \return vector
		const coord_type& sizes() {
			return _sizes;
		}

		//! Returns the size for the first dimension
		//! \return x size
		coord_data_type size_x() {
			return _sizes(0);
		}

		//! Returns the size for the second dimension
		//! \return y size
		coord_data_type size_y() {
			return _sizes(1);
		}

		//! Returns the size for the third dimension
		//! \return z size
		coord_data_type size_z() {
			return _sizes(2);
		}

		//! Returns the lower point
		//! \return the lower point
		const coord_type& lower_point() {
			return _lower_point;
		}

		//! Returns the upper point
		//! \return the upper point
		const coord_type& upper_point() {
			return _upper_point;
		}

		bool inside(const coord_type& x) {
			coord_type a = x - lower_point();
			coord_type b = upper_point() - x;
			if (a.minCoeff() < 0 || b.minCoeff() < 0) {
				return false;
			}
			return true;
		}

		bool inside(const coord_float_type& x) {
			coord_float_type a = x - _lower_point.template cast<float64>();
			coord_float_type b = _upper_point.template cast<float64>() - x;
			if (a.minCoeff() < 0 || b.minCoeff() < 0) {
				return false;
			}
			return true;
		}

		//! reinits the data if an space is read
		//! \param[in] sizes: size of the space
		void init_data(const coord_type& sizes) {
			_sizes = sizes;
			_lower_point = coord_type::Zero();
			_upper_point = sizes - coord_type::Ones();
			_data.init_data(sizes);
		}

		//! Returns the pointer to the data
		value_data_type* data() {
			return _data.data();
		}

		//! Returns the pointer to the data
		//! \param[in] offset: offset to the initial position
		value_data_type* data(const coord_type& offset) {
			return _data.data(offset);
		}

	//iterator functions
	public:

		//! iterator for thew whole signal
		//! \returns begin to the signal
		iterator begin() {
			return iterator(this,false);
		}

		//! iterator for the whole signal
		//! \returns the end of the signal
		iterator end() {
			return iterator(this,true);
		}

		//! iterator for the whole signal in a specific dimension order (default is 0,1,2...)
		//! \return begin to the signal
		iterator begin(const coord_type& order) {
			return iterator(this,false,order);
		}

	//adjacency iterator
	public:

		//! Definition of iterator functions
		//! \return iterator
		adjacency_iterator adjacency_begin(const coord_type& pos) {
			return adjacency_iterator(this,pos,false);
		}

		//! Definition of iterator functions
		adjacency_iterator adjacency_end(const coord_type& pos) {
			return adjacency_iterator(this,pos,true);
		}

		//! Definition of iterator functions
		template<ConnectivityType c>
		typename general_adjacency_iterator<c>::type general_adjacency_begin(const coord_type& pos) {
			return typename general_adjacency_iterator<c>::type(this, pos, false);
		}

		//! Definition of iterator functions
		template<ConnectivityType c>
		typename general_adjacency_iterator<c>::type general_adjacency_end(const coord_type& pos) {
			return typename general_adjacency_iterator<c>::type(this, pos, true);
		}

	//region iterators
	public:
		//! iterator for a given portion/region of the signal
		//! \returns begin to the signal

		template<class RegionModel>
		typename region_iterator<RegionModel>::type begin(RegionModel& r) {
			return typename region_iterator<RegionModel>::type(this, r, false);
		}

		//! iterator for a given portion/region of the signal
		//! \returns the end of the signal
		template<class RegionModel>
		typename region_iterator<RegionModel>::type end(RegionModel& r) {
			return typename region_iterator<RegionModel>::type(this, r, true);
		}


	// ROI iterators
	public:

		//! Function that creates an iterator to the beginning of the signal
		//! \return an iterator
		roi_iterator roi_begin(const coord_type& initial_point, const coord_type& end_point) {
			return roi_iterator(*this, initial_point, end_point, false);
		}

		//! Function that creates an iterator to the end of the signal
		//! \return an iterator
		roi_iterator roi_end(const coord_type& initial_point, const coord_type& end_point) {
			return roi_iterator(*this, initial_point, end_point, true);
		}

		//! Function that creates an iterator to the beginning of the signal with an specific search order
		//! \return an iterator
		roi_iterator roi_begin(const coord_type& initial_point, const coord_type& end_point, const coord_type& order) {
			return roi_iterator(*this, initial_point, end_point, order, false);
		}

	protected:

		//! Sizes
		coord_type										_sizes;

		//! lower point of the space
		coord_type 										_lower_point;

		//! upper point of the space
		coord_type										_upper_point;

		// TODO The container can be changed
		ContainerType 									 _data;
	};

}




#endif /* DISCRETE_SPACE_SIGNAL_HPP_ */
