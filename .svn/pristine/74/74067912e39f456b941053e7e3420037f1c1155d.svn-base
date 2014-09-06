/*
 * region_links.hpp
 *
 *  Created on: Nov 30, 2012
 *      Author: guillem
 */

#ifndef REGION_LINKS_HPP_
#define REGION_LINKS_HPP_

#include <imageplus/core/b_search_tree.hpp>

namespace imageplus {

	enum  LinkType {
		LinkTypeNormal,
		LinkTypeSkip
	};

	//! Link with an associated distance
	template<class RegionModelPtr>
	struct RegionDistanceLink  : public bst_base_node<RegionDistanceLink<RegionModelPtr> > {

		RegionModelPtr neighbor1;
		RegionModelPtr neighbor2;
		float64 distance;
		LinkType type;

		RegionDistanceLink(RegionModelPtr n1 = NULL, RegionModelPtr n2 = NULL, float64 d = 0, LinkType type = LinkTypeNormal) : neighbor1(n1), neighbor2(n2), distance(d), type(LinkTypeNormal) {

		}

		~RegionDistanceLink() {
		}

		struct compare_distance_function {
			bool operator()(RegionDistanceLink *a, RegionDistanceLink *b) {
				if (a->distance == b->distance) {
					if ((uint64)a->neighbor1 == (uint64)b->neighbor1)
						return (uint64)a->neighbor2 < (uint64)b->neighbor2;
					return (uint64)a->neighbor1 < (uint64)b->neighbor1;
				}
				return a->distance < b->distance;
			}
		};

	};

}


#endif /* REGION_LINKS_HPP_ */
