/*
 * adjacency_iterator.hpp
 *
 *  Created on: Nov 26, 2012
 *      Author: guillem
 */

#ifndef ADJACENCY_ITERATOR_HPP_
#define ADJACENCY_ITERATOR_HPP_

#include <imageplus/core/iterators/space_neighbors.hpp>

namespace imageplus {

	//! adjacency iterator for discretized spaces
	template<class Signal, ConnectivityType connectivity = neighborhood_traits<Signal::coord_dimensions>::default_connectivity>
	class general_adjacency_iterator_type : public std::iterator<std::forward_iterator_tag, typename Signal::value_type> {

		//! neighborhood structure
		Neighborhood<typename Signal::coord_data_type, Signal::coord_dimensions, connectivity> _neighborhood;

		typedef typename Signal::coord_type	coord_type;

	public:

		//! defaults constructor
		//! \param[in] signal : signal where the discrete unit is
		//! \param[in] pos : position of the pixel
		//! \param[in] end : true if the end_iterator is created
		general_adjacency_iterator_type(Signal* signal, const coord_type& pos, bool end = false) {

			_signal = signal;
			_current_coord = 0;

			_pos = pos;

			_end_neighbor = _neighborhood.neighbors.size();

			// Build the end iterator
			if (end) {
				_current_coord = _end_neighbor;
			}
			else {
				_find_inside_neighbor();
			}
		}

		//! operator ++ overload
		//! \return itself
		general_adjacency_iterator_type& operator++() {
			if (_current_coord >= _end_neighbor) return *this;

			_current_coord++;
			_find_inside_neighbor();

			return *this;
		}

		//! operator != overload
		//! \return boolean indicating whether or not a != *this
		bool operator!=(const general_adjacency_iterator_type& a) const {
			if (_current_coord >= _end_neighbor && a._current_coord >= a._end_neighbor) return false;
			if (_current_coord >= _end_neighbor || a._current_coord >= a._end_neighbor) return true;

			return ((_neighbor_coords - a._neighbor_coords).isZero() != 0); // check if two vectors are equal
		}

		//! dereference pointer operator * overload
		//! returns the discrete space unit with a given coords
		typename Signal::value_ret_type operator*() {
			return _signal->operator()(_neighbor_coords);
		}

		const coord_type& pos() const {
			return _neighbor_coords;
		}

	private:

		inline void _find_inside_neighbor() {
			bool inside = false;

			while (!inside) { // find a neighbor inside the discrete space signal
				if (_current_coord >= _end_neighbor) return;
				// find the neighbor coords
				_neighbor_coords = _pos + _neighborhood.neighbors[_current_coord];

				const coord_type& lower = _signal->lower_point();
				const coord_type& upper = _signal->upper_point();

				coord_type a = _neighbor_coords - lower;
				coord_type b = upper - _neighbor_coords;

				// Check if the neighbor is inside the signal
				if (a.minCoeff() >= 0 && b.minCoeff() >= 0) {
					inside = true;
					break;
				}
				_current_coord++;
			}
		}

	private:

		//! signal
		Signal* _signal;

		//! central position
		coord_type	_pos;

		//! neighboring coords
		coord_type  _neighbor_coords;

		//! end neighbor (number of neighbors)
		uint64 _end_neighbor;

		//! current neighbor
		uint64 _current_coord;
	};

}


#endif /* ADJACENCY_ITERATOR_HPP_ */
