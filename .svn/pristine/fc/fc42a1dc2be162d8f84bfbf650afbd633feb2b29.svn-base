/*
 * region_iterator.hpp
 *
 *  Created on: Nov 26, 2012
 *      Author: guillem
 */

#ifndef REGION_ITERATOR_HPP_
#define REGION_ITERATOR_HPP_

namespace imageplus {

	//! Class to iterator accross a region in the signal
	template<class Signal, class RegionModel>
	class region_iterator_type : public std::iterator<std::forward_iterator_tag, typename Signal::value_type> {

		typedef typename Signal::coord_type coord_type;

	public:

		region_iterator_type(Signal* signal, RegionModel& region, bool end = false) : _signal(signal) {
			_end = end;
			_iterator = region.begin();
			_end_iterator = region.end();
		}

		region_iterator_type& operator++() {
			if (_end) return *this;

			++_iterator;

			if (_iterator == _end_iterator) {_end = true;}

			return *this;
		}

		bool operator!=(const region_iterator_type& a) {
			if (a._end == _end) return false;
			if ((a._end || _end) == true && a._end != _end) return true;

			return ((*a._iterator - *_iterator).isZero() != 0);
		}

		typename Signal::value_ret_type operator*() {
			return _signal->operator()(*_iterator);
		}

		const coord_type& pos() {
			return *_iterator;
		}

	private:

		//! region iterator
		typename RegionModel::iterator _iterator;

		//! region end iterator
		typename RegionModel::iterator _end_iterator;

		//! signal
		Signal *_signal;

		//! end iterator
		bool _end;
	};

}


#endif /* REGION_ITERATOR_HPP_ */
