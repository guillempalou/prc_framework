/*
 * region_neighbor_container.hpp
 *
 *  Created on: Aug 28, 2012
 *      Author: guillem
 */

#ifndef REGION_NEIGHBOR_CONTAINER_HPP_
#define REGION_NEIGHBOR_CONTAINER_HPP_

#include <imageplus/core/regions/region_links.hpp>
#include <map>

#include <iostream>

namespace imageplus {

	//! Basic container for the neighbor regions
	template <class RegionModelPtr, class RegionLinkModel = RegionDistanceLink<RegionModelPtr> >
	class RegionNeighborContainer {

		typedef RegionModelPtr								RegionPointer;
		typedef std::map<RegionPointer,RegionLinkModel*> 	neighbor_map_type;
		typedef std::pair<RegionPointer,RegionLinkModel*> 	neighbor_pair_type;

	public:

		typedef RegionLinkModel								RegionLinkType;

		RegionNeighborContainer(RegionPointer r) : _region(r) {

		}

		RegionNeighborContainer(const RegionNeighborContainer& copy, RegionPointer new_reg = NULL) : _region(copy._region), _neighbors(copy._neighbors){
			if (new_reg != NULL) {
				_region = new_reg;
			}
		}

		class neighbor_iterator {
		public:
			neighbor_iterator() {
			}

			neighbor_iterator(typename neighbor_map_type::iterator map_it) : _map_it(map_it) {
			}

			bool operator==(const neighbor_iterator& it) {
				return _map_it==it._map_it;
			}

			bool operator!=(const neighbor_iterator& it) {
				return _map_it!=it._map_it;
			}

			RegionPointer operator*() const {
				return _map_it->first;
			}

			RegionLinkType* link_data() const {
				return _map_it->second;
			}

			neighbor_iterator& operator++() {
				++_map_it;
				return *this;
			}

		private:
			typename neighbor_map_type::iterator _map_it;

		};

		neighbor_iterator neighbors_begin() {
			return neighbor_iterator(_neighbors.begin());
		}

		neighbor_iterator neighbors_end() {
			return neighbor_iterator(_neighbors.end());
		}

		neighbor_iterator neighbors_find(RegionPointer reg) {
			return neighbor_iterator(_neighbors.find(reg));
		}

		RegionLinkType* neighbors_insert(RegionPointer reg, RegionLinkType* link = NULL )
		{
			std::pair<typename neighbor_map_type::iterator, bool> p = _neighbors.insert(neighbor_pair_type(reg,(RegionLinkType*)NULL));

			typename neighbor_map_type::iterator it = p.first;

			if(p.second == true)
			{
				RegionLinkType* link_data;
				if (link == NULL) {
					link_data = new RegionLinkType(_region, reg);
					reg->neighbors().neighbors_insert(_region, link_data);
				}
				else
					link_data = link;

				it->second = link_data;
			}

			return it->second;
		}

		inline void neighbors_erase(RegionPointer reg, bool delete_link = true) {
			neighbor_iterator neigh_it = this->neighbors_find(reg);

			// We do this check to allow for merging of non-neighboring regions
			if(neigh_it != this->neighbors_end()) {
				if (delete_link) {
					delete neigh_it.link_data();
					reg->neighbors().neighbors_erase(_region, false);
				}
				_neighbors.erase(reg);
			}
		}

		inline void neighbors_clear() {
			neighbor_iterator neigh_it = this->neighbors_begin();
			for(; neigh_it!=this->neighbors_end(); ++neigh_it) {
				neighbors_erase(*neigh_it);
			}
		}

	protected:

		//! region where the container belongs
		RegionPointer _region;

		//! Pointer to the neighbor regions
		neighbor_map_type _neighbors;
	};


}


#endif /* REGION_NEIGHBOR_CONTAINER_HPP_ */
