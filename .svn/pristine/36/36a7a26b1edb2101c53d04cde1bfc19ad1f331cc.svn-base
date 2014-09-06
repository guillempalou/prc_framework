/*
 * coord3d_iterator.hpp
 *
 *  Created on: Dec 19, 2012
 *      Author: gpalou
 */

#ifndef COORD3D_ITERATOR_HPP_
#define COORD3D_ITERATOR_HPP_

namespace imageplus {

	template<class FrameContainer, class CoordContainer, class coord>
	class coord3d_iterator {

			typedef FrameContainer	FrameContainerType;
			typedef CoordContainer	CoordContainerType;
			typedef coord			coord_type;

	public:

			coord3d_iterator() {

			}

			coord3d_iterator(FrameContainer& coordinates, bool end) {
				if (end == true) {
					_current_frame_it = coordinates.end();
				} else {
					_current_frame_it = coordinates.begin();
					_current_coord_it = _current_frame_it->second.begin();
					_frame_end = _current_frame_it->second.end();
				}
				_end = coordinates.end();
			}

			coord3d_iterator(FrameContainer& coordinates, uint64 frame, bool end) {
				if (end == true) {
					_current_frame_it = coordinates.find(frame)++;
					_current_coord_it = coordinates[frame].end();
				} else {
					_current_frame_it = coordinates.find(frame);
					_current_coord_it = _current_frame_it->second.begin();
				}
				_end = coordinates.find(frame)++;;
				_frame_end = coordinates[frame].end();
			}

			coord_type& operator*() {
				return *_current_coord_it;
			}

			const coord_type& operator*() const {
				return *_current_coord_it;
			}

			coord3d_iterator& operator++() {
				if (_current_frame_it == _end)
					return *this;

				_current_coord_it++;
				if (_current_coord_it == _frame_end) {
					_current_frame_it++;
					if (_current_frame_it == _end) return *this;
					_current_coord_it = _current_frame_it->second.begin();
				}
				return *this;
			}

			bool operator==(const coord3d_iterator& a) {
				if (_current_frame_it == a._current_frame_it) {
					if (_current_frame_it == _end) return true;
					if (_current_coord_it == a._current_coord_it) return true;
					return false;
				} else return false;
			}

			bool operator!=(const coord3d_iterator& a) {
				if (_current_frame_it != a._current_frame_it) {
					return true;
				} else {
					if (_current_coord_it == a._current_coord_it) return false;
					return true;
				}
			}

		protected:

			typename FrameContainerType::iterator 					_current_frame_it;
			typename FrameContainerType::iterator 					_end;

			typename CoordContainerType::iterator					 _current_coord_it;
			typename CoordContainerType::iterator					 _frame_end;
	};
}



#endif /* COORD3D_ITERATOR_HPP_ */
