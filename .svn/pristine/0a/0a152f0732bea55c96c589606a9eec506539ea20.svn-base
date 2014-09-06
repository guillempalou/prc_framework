/*
 * global_iterator.hpp
 *
 *  Created on: Nov 26, 2012
 *      Author: guillem
 */

#ifndef GLOBAL_ITERATOR_HPP_
#define GLOBAL_ITERATOR_HPP_

#include <imageplus/math/math_types.hpp>

namespace imageplus {

	//! Class to iterator accross the whole signal
	template<class Signal>
	class global_iterator_type : public std::iterator<std::forward_iterator_tag, typename Signal::value_type> {

		typedef typename Signal::coord_type coord_type;

	public:

		global_iterator_type(Signal* signal, bool end) : _signal(signal) {
			_end = end;

			for (uint64 i = 0; i < Signal::coord_dimensions; i++) _order(i) = i;
			_sizes = _signal->sizes();
			_current_coord.fill(0);
		}

		global_iterator_type(Signal* signal, const coord_type& initial_coord, bool end, const coord_type& order) {
			_end = end;
			_signal = signal;
			_order = order;

			_sizes = _signal->sizes();
			_current_coord = initial_coord;
		}

		global_iterator_type(Signal* signal, const coord_type& initial_coord, bool end) {
			_end = end;
			_signal = signal;
			for (uint64 i = 0; i < Signal::coord_dimensions; i++) _order(i) = i;

			_sizes = _signal->sizes();
			_current_coord = initial_coord;
		}

		global_iterator_type(Signal* signal, bool end, const coord_type& order) {
			_end = end;
			_signal = signal;
			_order = order;

			_current_coord.fill(0);
			_sizes = _signal->sizes();
		}

		global_iterator_type& operator++() {
			if (_end) return *this;

			_current_coord(_order(0))++;

			uint64 i = 0;

			while (_current_coord(_order(i)) >= _sizes(_order(i))) {
				_current_coord(_order(i)) = 0;
				i++;
				_current_coord(_order(i))++;

				if (i == Signal::coord_dimensions-1 && _current_coord(_order(i)) == _sizes(_order(i))) {_end = true; return *this;}
			}

			return *this;
		}

		bool operator!=(const global_iterator_type& a) {
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

		//! signal
		Signal *_signal;

		//! size for every dimension
		coord_type _sizes;

		//! current coordinate
		coord_type _current_coord;

		//! order to traverse the dimensions
		coord_type _order;

		//! end iterator
		bool _end;
	};

}

#endif /* GLOBAL_ITERATOR_HPP_ */
