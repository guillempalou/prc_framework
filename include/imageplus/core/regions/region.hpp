/*
 * region.hpp
 *
 *  Created on: Jul 31, 2012
 *      Author: guillem
 */

#ifndef REGION_HPP_
#define REGION_HPP_

#include <imageplus/core/imageplus_types.hpp>
#include <deque>
#include <set>
#include <vector>

#include <imageplus/core/regions/region_neighbor_container.hpp>

namespace imageplus {

	//!
	//! \brief Class to handle a geometric region, i.e., a vector of coordinates
	//!
	template<class coord, class ContainerModel = std::deque<coord> >
	class Region
	{
	public:

		//! dimensions of the coords
		const static std::size_t dimensions = coord::RowsAtCompileTime; /*!< Coords dimensions */

		//! coordinates of the regions
		typedef			 coord										coord_type;

		/*!< Type of container used  */
		typedef         ContainerModel					         	CoordsContainerType;

		//! neighbor container
		typedef RegionNeighborContainer<Region*>					NeighborContainerType;

		typedef typename NeighborContainerType::RegionLinkType		RegionLinkType;

		//! identifier type
		typedef uint64												identifier_type;

		//!< Region iterator
		typedef typename CoordsContainerType::iterator        		iterator;

		//!< Const region iterator
		typedef typename CoordsContainerType::const_iterator  		const_iterator;

		//! typedef for the neighbor iterators
		typedef typename NeighborContainerType::neighbor_iterator 	neighbor_iterator;

		//!
		//! \brief Default constructor
		//!
		inline Region() : _coordinates(), _neighbors(this) {
		}

		//!
		//! \brief Default constructor
		//!
		inline Region(const Region& copy) : _coordinates(copy._coordinates), _label(copy._label), _neighbors(copy._neighbors, this) {
		}

		//!
		//! \brief Constructor with the dimension of the Coords
		//!
		//! \param[in] initial_size : Initial number of coordinates
		inline Region(identifier_type label) : _coordinates(), _label(label), _neighbors(this) {
		}

		//!
		//! \brief Constructor with 1 Coord
		//!
		//! \param[in] initial_size : Initial number of coordinates
		inline Region(identifier_type label, const coord_type& pos) : _coordinates(1,pos), _label(label), _neighbors(this) {
		}

		//!
		//! \brief Constructor with the dimension of the Coords
		//!
		virtual ~Region() {
		}

        /*!
         * \returns the label of the region in the context of a Partition
         */
        uint64 label() const
        {
            return _label;
        }

		/*! \brief Function to iterate along this region coordinates. You should not work with this method directly see Region and RegionContour instead
		 *
		 * \return iterator to the first coordinate contained in the region
		 */
		iterator begin() {
			return _coordinates.begin();
		}

		/*! \brief Function to iterate along this region coordinates. You should not work with this method directly see Region and RegionContour instead
		 *
		 * \return iterator to the end of the coordinates
		 */
		iterator end() {
			return _coordinates.end();
		}

		/*! \brief Function to iterate along this region coordinates. You should not work with this method directly see Region and RegionContour instead
		 *
		 * \return const iterator to the first coordinate contained in the region
		 */
		const_iterator begin() const {
			return _coordinates.begin();
		}

		/*! \brief Function to iterate along this region coordinates. You should not work with this method directly see Region and RegionContour instead
		 *
		 * \return const iterator to the end of the coordinates
		 */
		const_iterator end() const {
			return _coordinates.end();
		}

		/*! \brief Function to add a coordinate to the region
		 *
		 * \param[in] coord : coordinate to add
		 */
		void add_coordinate(const coord_type& c) {
			_coordinates.push_back(c);
		}

		const CoordsContainerType& coordinates() const {
			return _coordinates;
		}

		CoordsContainerType& coordinates() {
			return _coordinates;
		}

		//!
		//! Copy operator from base class
		//!
		//! \param[in] copy: Object to be copied
		//!
		//! \return copy, to concatenate
		//!
		const Region& operator=(const Region& copy) {
			_coordinates = copy.coordinates();
			_label = copy.label();
			new (&_neighbors) NeighborContainerType(copy._neighbors, this);
			return *this;
		}

		//! Method that returns a vector with pointers to neighboring regions
		//! \return a map with region pointers
		NeighborContainerType& neighbors() {
			return _neighbors;
		}

		//! Method to add a neighbor
		//! \param[in] r : neighboring region
		inline void add_neighbor(Region* r) {
			_neighbors.neighbors_insert(r);
		}

		//! Method to erase a neighbor
		//! \param[in] r : neighboring region
		inline void erase_neighbor(Region *r) {
			_neighbors.neighbors_erase(r);
		}

		//! Clear neighbors
		inline void clear_neighbors() {
			_neighbors.neighbors_clear();
		}

		//! Check if a reigon is a neighbor
		//! \param[in] r : pointer to the region
		inline bool is_neighbor(Region* r) {
			return (_neighbors.neighbors_find(r) != _neighbors.neighbors_end());
		}

		//! begin to the neighbors
		//! iterator to the neighbors beginning
		neighbor_iterator neighbors_begin()
		{
			return _neighbors.neighbors_begin();
		}

		//! end to the neighbors
		//! iterator to the neighbors ending
		neighbor_iterator neighbors_end()
		{
			return _neighbors.neighbors_end();
		}

		//! find a particular neighbor
		//! \param[in] reg : region to find
		//! \return iterator to the neighbor. end if it doesn't exist
		inline neighbor_iterator neighbors_find(Region* reg)
		{
			return _neighbors.neighbors_find(reg);
		}

	protected:

		//! coordinates
		CoordsContainerType	_coordinates;

		//! label
		identifier_type _label;

		//! neighbors
		NeighborContainerType _neighbors;

	};

}


#endif /* REGION_HPP_ */
