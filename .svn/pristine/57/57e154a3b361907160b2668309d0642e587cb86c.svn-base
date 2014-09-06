/*
 * hierachical_region.hpp
 *
 *  Created on: Jul 31, 2012
 *      Author: guillem
 */

#ifndef HIERACHICAL_REGION_HPP_
#define HIERACHICAL_REGION_HPP_

#include <vector>
#include <imageplus/core/regions/region.hpp>
#include <imageplus/core/regions/hierarchical_region_iterator.hpp>

namespace imageplus
{
    /*!
     * \brief Class to handle regions (vector of coordinates) with descriptors
     *
     * It can take advantage of a BPT to compute the descriptors recursively.
     * Assumes eigen vectors as coords, but can be changed
     *
     * \author Jordi Pont Tuset - 05-05-2009 - Guillem Palou 2012
     *
     * \todo Store the coordinates in sweeping order: Instead of a vector, use a sorted queue
     */
    template<class coord, class ContainerModel = typename Region<coord>::CoordsContainerType>
    class HierarchicalRegion : public Region<coord, ContainerModel> {
    public:
    	static const uint64 dimensions = Region<coord>::dimensions;

        //! Coordinates Type
        typedef coord				 											coord_type;
        //! Pointer type of the region
        typedef HierarchicalRegion* 											RegionPointer;
        //! Type of Gerometric region (father)
        typedef	Region<coord, ContainerModel>									RegionBaseType;

        //! this class type
        typedef HierarchicalRegion<coord,ContainerModel>					 	RegionType;

        //! iterator of the region coordinates
        typedef RegionIteratorBase<RegionType>									iterator;

        //! const iterator of the region coordinates
        typedef RegionIteratorBase<const RegionType>							const_iterator;

        //! Children container type
        typedef std::vector<RegionPointer>										ChildrenContainerType;

        //! Children iterator type
        typedef typename ChildrenContainerType::iterator						children_iterator;

        //! Children const iterator type
        typedef typename ChildrenContainerType::const_iterator					const_children_iterator;

        //! Id type
        typedef typename RegionBaseType::identifier_type						identifier_type;

        //! default constructor
        inline HierarchicalRegion() {

        }

        /*!
         * Constructor from children
         *
         * \param[in] child1 : Pointer to one of the child regions that formed it via a merging process
         * \param[in] child2 : Pointer to the other child
         * \param[in] label  : Label of the region in the base partition of the Partition (by def. NOT_ASSGNED)
         */
        inline HierarchicalRegion(identifier_type label, RegionPointer  child1, RegionPointer  child2) :  RegionBaseType(label)
        {
        	RegionBaseType::_label = label;
            _init(child1, child2);
            _parent =NULL;
        }

        /*!
         * Constructor from children
         *
         * \param[in] children : container of the childs
         * \param[in] label  : Label of the region in the base partition of the Partition (by def. NOT_ASSGNED)
         */
        inline HierarchicalRegion(identifier_type label, ChildrenContainerType& children) :   RegionBaseType(label)
        {
        	RegionBaseType::_label = label;
        	_init(children);
        	_parent =NULL;
        }

        /*
         * Copy constructor
         * \param[in] copy : copy to construct
         */
        inline HierarchicalRegion(const HierarchicalRegion& copy) : RegionBaseType(copy) {
        	RegionBaseType::_label = copy.label();
        	_init(copy.children());
        	_parent = copy._parent;
        }


        //!
        //! \brief Constructor with the dimension of the Coords
        //!
        //! \param[in] initial_size : Initial number of coordinates
        inline
        HierarchicalRegion(identifier_type label, const coord_type& pos) : RegionBaseType(label,pos) {
        	_parent =NULL;
        }

        //!
        //! \brief Constructor with the dimension of the Coords
        //!
        //! \param[in] initial_size : Initial number of coordinates
        //! \param[in] label  : Label of the region in the base partition of the Partition
        inline
        HierarchicalRegion(identifier_type label) : RegionBaseType(label) {
        	_parent =NULL;
        }

        /*!
         * Constructor from children
         *
         * \param[in] child1 : Pointer to one of the child regions that formed it via a merging process
         * \param[in] child2 : Pointer to the other child
         * \param[in] label  : Label of the region in the base partition of the Partition (by def. NOT_ASSGNED)
         */
        inline HierarchicalRegion(identifier_type label, RegionType&  child1, RegionType& child2) : RegionBaseType(label)
        {
        	RegionBaseType::_label = label;
        	_init(&child1, &child2);
        	_parent = NULL;
        }

        /*!
         * \brief Desctructor
         */
        virtual
        ~HierarchicalRegion()
        {
        }

        /*!
         * Constructor receiving the first coordinate of the region and a reference to the partition
         * where it belongs (optional).
         *
         * \param[in] first_position : First position (coordinate) of the region
         * \param[in] label          : Label of the region in the base partition of the Partition
         *                            (by def. NOT_ASSGNED)
         */
        HierarchicalRegion(coord_type first_position, identifier_type label = -1) :   RegionBaseType()
        {
        	RegionBaseType::_label = label;
            RegionBaseType::coordinates().push_back(first_position);
            _parent =NULL;
        }

    public:

        /*!
         * \returns true because the region can have child
         */
        bool is_composite() const
        {
            return true;
        }

        /*!
         * \brief Function to iterate along the region coordinates. The coordinates may be given in any order
         * \return iterator to the beginning of the coordinates
         */
        iterator begin()
        {
            return iterator(this);
        }

        /*!
         * \brief Function to iterate along the region coordinates. The coordinates may be given in any order
         * \return iterator to the end of the coordinates
         */
        iterator end()
        {
            return iterator(this,true);
        }

        /*!
         * \brief Function to iterate along the region coordinates. The coordinates may be given in any order
         * \return const iterator to the beginning of the coordinates
         */
        const_iterator begin() const
        {
            return const_iterator(this);
        }

        /*!
         * \brief Function to iterate along the region coordinates. The coordinates may be given in any order
         * \return iterator to the end of the coordinates
         */
        const_iterator end() const
        {
            return const_iterator(this,true);
        }

        /*!
         *  Returns a pointer the father of the regions
         *  \return the parent of the region
         */
        RegionPointer parent() {
        	return _parent;
        }

        /*!
         *  Sets a pointer to the father of the regions
         *  \param[in] the parent of the region
         */
        void parent(RegionPointer father) {
        	_parent = father;
        }

        /*!
         * Returns the vector of childrenx
         * \return the children
         */
        ChildrenContainerType& children() {
        	return _children;
        }

        /*!
         * Returns the vector of childrenx
         * \return the children
         */
        const ChildrenContainerType& children() const {
        	return _children;
        }

        /*!
         * Returns a specific child
         * \param[in] child_num : the child to retrieve
         * \return child
         */
        RegionPointer child(uint64 child_num) {
        	return _children[child_num];
        }

        /*!
         * Clears the children vector. Be careful, it only clears the pointers, it does not deallocate memory
         */
        void clear_children() {
        	_children.clear();
        }

        //! TODO operator= not implemented because it is not clear if only the region should be copied or all its hierarchy.
        /*RegionType& operator=(const RegionType& r) {
        	_children.clear();
        	_label = r.label();
        	_parent = r.parent();

        	_init(r.children());
        }*/

    protected:

        /*!
         * Initializes the class
         *
         * \param[in] child1 : Pointer to one of the child regions that formed it via a merging process
         * \param[in] child2 : Pointer to the other child
         */
        void _init(RegionPointer child1, RegionPointer child2)
        {
        	std::size_t disp = 0;
            if(child1!=0)
            {
                this->_children.push_back(child1);
                child1->parent(this);
                disp = child1->coordinates().size();

                for (iterator it = child1->begin(); it != child1->end(); ++it) {
                	RegionBaseType::coordinates().push_back(*it);
                }

                if (child1->children().size() != 0) child1->coordinates().clear(); // we only maintain a copy of the coordinates at the roots and leaves
            }

            if(child2!=0)
            {
                this->_children.push_back(child2);
                child2->parent(this);

                for (iterator it = child2->begin(); it != child2->end(); ++it) {
                	RegionBaseType::coordinates().push_back(*it);
                }

                if (child2->children().size() != 0) child2->coordinates().clear(); // we only maintain a copy of the coordinates at the roots and leaves
             }
        }

        /*!
         * Initializes the class
         *
         * \param[in] children : container
         */
        void _init(const ChildrenContainerType& children)
        {
        	_children = children;

        	for (typename ChildrenContainerType::iterator c = _children.begin(); c != _children.end(); ++c) {

        		for (iterator it = (*c)->begin(); it != (*c)->end(); ++it) {
        			RegionBaseType::coordinates().push_back(*it);
        		}

        		if ((*c)->children().size() != 0) (*c)->coordinates().clear(); // we only maintain a copy of the coordinates at the roots
        		(*c)->parent(this);
        	}
        }

    protected:

        //!Region father
        RegionPointer _parent;

        //! Region childs
        ChildrenContainerType _children;
    };

}

#endif /* HIERACHICAL_REGION_HPP_ */
