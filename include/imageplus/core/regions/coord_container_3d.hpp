/*
 * coord_container_3d.hpp
 *
 *  Created on: Dec 18, 2012
 *      Author: gpalou
 */

#ifndef COORD_CONTAINER_3D_HPP_
#define COORD_CONTAINER_3D_HPP_

#include <imageplus/core/imageplus_types.hpp>
#include <imageplus/core/iterators/coord3d_iterator.hpp>
#include <vector>
#include <map>

namespace imageplus {

	template<class coord_model>
	class CoordContainer3D {

		typedef coord_model coord_type;
		typedef std::deque<coord_type>						coord_container_type;
		typedef std::map<uint64,coord_container_type>		frame_container_type;

	public:

		typedef coord3d_iterator<frame_container_type, coord_container_type, coord_type>				iterator;
		typedef coord3d_iterator<frame_container_type, coord_container_type, const coord_type>			const_iterator;

		CoordContainer3D() : _N(0) {

		}

		void push_back(const coord_type& c) {
			_N++;
			_coordinates[c(2)].push_back(c);
		}

		uint64 size() {
			return _N;
		}

		void clear() {
			_coordinates.clear();
		}

		iterator begin() {
			return iterator(_coordinates,false);
		}

		iterator end() {
			return iterator(_coordinates,true);
		}

		const_iterator begin() const {
			return iterator(_coordinates,false);
		}

		const_iterator end() const {
			return iterator(_coordinates,true);
		}

	protected:

		frame_container_type _coordinates;
		uint64 _N;
	};

}

#endif /* COORD_CONTAINER_3D_HPP_ */
