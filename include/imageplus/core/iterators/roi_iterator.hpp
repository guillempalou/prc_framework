/*
 * roi_iterator.hpp
 *
 *  Created on: Nov 26, 2012
 *      Author: guillem
 */

#ifndef ROI_ITERATOR_HPP_
#define ROI_ITERATOR_HPP_

namespace imageplus {

	//! Class to iterator along a rectangular ROI (all points inside the volume initial_point - end_point)
	template<class Signal>
	class roi_iterator_type : public std::iterator<std::forward_iterator_tag, typename Signal::value_type> {

		typedef typename Signal::coord_type coord_type;

	public:

		roi_iterator_type(Signal& signal, const coord_type& initial_point, const coord_type& end_point, bool end) : _signal(&signal) {
			_initial_point = initial_point;
			_end_point = end_point + coord_type::Ones();
			_end = end;
			_sizes = _signal->sizes();
			_reached = end;

			for (uint64 i = 0; i < Signal::coord_dimensions; i++) _order(i) = i;

			_current_coord = _initial_point;
		}

		roi_iterator_type(Signal& signal, const coord_type& initial_point, const coord_type& end_point, const coord_type& order, bool end) : _signal(&signal) {
			_initial_point = initial_point;
			_end_point = end_point;
			_order = order;
			_end = end;
			_sizes = _signal->sizes();

			_reached = end;

			_current_coord = _initial_point;
		}

		roi_iterator_type& operator++() {
			if (_end) return *this;

			if (_reached) {_end = true; return *this;}

			_current_coord(_order(0))++;

			uint64 i = 0;

			if ((_end_point - _current_coord).isZero()) {
				_reached = true;
				return *this;
			}

			while (_current_coord(_order(i)) >= _end_point(_order(i))) {
				_current_coord(_order(i)) = _initial_point(_order(i));
				i++;
				_current_coord(_order(i))++;

				if ((_end_point - _current_coord).isZero()) {
					_reached = true;
					return *this;
				}

				if (i == Signal::coord_dimensions-1 && _current_coord(_order(i)) == _end_point(_order(i))) {_end = true; return *this;}
			}

			return *this;
		}

		bool operator!=(const roi_iterator_type& a) {
			if (a._end == _end) return false;
			if ((a._end || _end) == true && a._end != _end) return true;

			return ((a._current_coord - _current_coord).isZero() != 0);
		}

		typename Signal::value_ret_type operator*() {
			return _signal->operator()(_current_coord);
		}

		const coord_type& pos() {
			return _current_coord;
		}
	protected:

		Signal* _signal;
		bool _end;
		bool _reached;

		coord_type _initial_point;
		coord_type _end_point;

		//! size for every dimension
		coord_type _sizes;

		//! current coordinate
		coord_type _current_coord;

		//! order to traverse the dimensions
		coord_type _order;

	};

}


#endif /* ROI_ITERATOR_HPP_ */
