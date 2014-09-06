// --------------------------------------------------------------
// Copyright (C)
// Universitat Politecnica de Catalunya (UPC) - Barcelona - Spain
// --------------------------------------------------------------

//!
//!  \file partition.hpp
//!
//!  Region-oriented partitions
//!

#ifndef IMAGEPLUS_CORE_PARTITION_HPP
#define IMAGEPLUS_CORE_PARTITION_HPP

#include <vector>
#include <set>
#include <list>
#include <iterator>

#include <imageplus/core/config.hpp>
#include <imageplus/core/coord2d.hpp>
#include <imageplus/core/imageplus_types.hpp>
#include <imageplus/core/exceptions.hpp>
#include <imageplus/core/geometric_shape.hpp>
#include <imageplus/core/imavol.hpp>
#include <imageplus/core/imagepartition.hpp>

#include <imageplus/core/border.hpp>
#include <imageplus/core/region_contour.hpp>


namespace imageplus
{
    //!
    //! \brief Class to handle region-oriented partitions.
    //!
    //! Class to handle region-oriented partitions, in contrast to ImagePartition, which is pixel-oriented.
    //! Briefly, Partition is a set of regions, and therefore it is more suitable for computing region descriptors
    //!  or performing any algorithm that is mainly based on scanning all the pixels of a region.
    //! 
    //! This class is usually constructed from a image_concept and the pixel-based partition container related. Typically, an Image and an ImagePartition.
    //!
    //! Multiresolution partitions (BPTs) are also accepted, in the sense that there may exist two regions along with their merging (father region).
    //! In this case, the constructor also receives a Merging Sequence.
    //!
    //! \todo Implement a pixel access
    //! 
    //! \author Jordi Pont Tuset <jpont@gps.tsc.upc.edu>
    //!
    //! \date 05-05-2009
    template<class RegionModel = RegionContour<Coord2D<int64> > >
    class Partition
    {
    public:
        typedef RegionModel RegionType;                        //!< Type of coords we are working with
        typedef RegionType*  RegionPointer;  				   //!< Pointer to the Regions
        typedef ImaVol<uint32,1,2> ImagePartitionType;         //!< Type of partition pixel-oriented (usually ImagePartition)
        typedef ImagePartitionType::data_type IdentifierType;  //!< Type of the region identifiers
        typedef typename RegionType::CoordType CoordType;      //!< Type of coords we are working with

    public:
         
        //!
        //! \brief Default constructor
        //!
        IMAGEPLUS_INLINE
        Partition() : _regions(), _leaves_partition(), _roots_partition()
        {
        }
         
        //!
        //! \brief Constructor via a partition_type (typically ImagePartition)
        //!
        //! \param[in] partition : Object describing the partition in a "pixel-based" approach (Typically ImagePartition)
        Partition(const ImagePartitionType& partition) 
                 : _regions(), _leaves_partition(partition), _roots_partition(partition)
        {
            // ASSERT(image.dims(0) == partition.dims(0), "Partition constructor: partition and image must have the same size.");                
            // ASSERT(image.num_elements() == partition.num_elements(), "Partition constructor: partition and image must have the same number of elements.");

            _init();
        }      
        
        //!
        //! \brief Copy constructor
        //! \param copy: Partition to be copied
        //!        
        Partition(const Partition& copy) : _curr_max_label(copy.max_label()), _regions(), _leaves_partition(copy.leaves_partition()), _roots_partition(copy.roots_partition())
        {
            _copy_regions(copy);
        }
        
        //!
        //! \brief Equal operator
        //! \param copy: Partition to be copied
        //!        
        void operator=( const Partition& copy )
        {
            _roots_partition  = copy.roots_partition();
            _leaves_partition = copy.leaves_partition();
            _curr_max_label = copy.max_label();
            _leaves_partition.coll_vd().clear();
            _clear_regions();
            _copy_regions(copy);
        }
        
        //! Destructor
        ~Partition() 
        {
            _clear_regions();
        }

        //!
        //! \brief Returns the number of regions in the partition
        //!
        //! \return Number of regions in the partition
        std::size_t num_regions() const
        {
            return _regions.size();
        }
        
        
        /*!
         * Not kept when pruning
         *
         * \return Maximum region label
         */
        IdentifierType max_label() const
        {
            return _curr_max_label;
        }
         
        //!
        //! \brief Returns a const reference to the base partition of the tree
        //!
        //! \return Const reference to the base partition of the tree
        const ImagePartitionType& leaves_partition() const
        {
            return _leaves_partition;
        }
        
        //!
        //! \brief Returns a const reference to the base partition of the tree
        //!
        //! \return Const reference to the base partition of the tree
        const ImagePartitionType& roots_partition() const
        {
        	return _roots_partition;
        }

        //!
        //! \brief Sets the base partition of the tree. It resets all the regions
        //!
        //! \param[in] partition: Partition to be set
        void leaves_partition(const ImagePartitionType& partition)
        {
            _roots_partition  = partition;
            _leaves_partition = partition;
            _clear_regions();
            _leaves_partition.coll_vd().clear();
            _init();            
        }
        
        //!
        //! \brief Returns a const reference to the region with a given label
        //!
        //! \param[in] label: Label of the region we want to obtain
        //! \return Const reference to the region with a given label
        const RegionType& operator[](const IdentifierType& label ) const throw (ImagePlusError)
        {
            return _region_by_label(label);
        }
        
        //!
        //! \brief Returns a reference to the region with a given label
        //!
        //! \param[in] label: Label of the region we want to obtain
        //! \return Reference to the region with a given label
        RegionType& operator[](const IdentifierType& label ) throw (ImagePlusError)
        {
            return _region_by_label(label);
        }
        
        
    protected:
        //! Type of map
        typedef typename std::map<IdentifierType, RegionPointer> map_type;

        //! Type of map pair
        typedef typename std::pair<IdentifierType, RegionPointer> map_pair_type;
        
        //! Max label of regions
        IdentifierType _curr_max_label;
        
        //! Map of regions
        map_type _regions;

        //! Copy of the pixel-based partition to implement the pixel access efficiently (todo)
        ImagePartitionType _leaves_partition;
        //! Partition that contains the roots of the three
        ImagePartitionType _roots_partition;
        
        //! Inits the set of regions scanning _leaves_partition
        void _init()
        {
            _curr_max_label = 0;
            
            typename map_type::iterator it, it2, it_end;
            it_end = _regions.end();

            // 2 "up-left" neighbors to be scanned for each pixel
            Neighborhood2D ngb;
            ngb.resize(2);
            ngb[0]=Neighborhood2D::CoordType(0,-1);
            ngb[1]=Neighborhood2D::CoordType(-1,0);
            
            // 2 "down-right" neighbors to be scanned for each pixel (looking for border)
            Neighborhood2D ngb2;
            ngb2.resize(2);
            ngb2[0]=Neighborhood2D::CoordType(0,1);
            ngb2[1]=Neighborhood2D::CoordType(1,0);
            
            boost::array<int64, 2> borders2d;
            borders2d[0]=1;
            borders2d[1]=1;
            
            const Border<ImagePartitionType>& part_borders = calc_descriptor(new Border<ImagePartitionType>(borders2d, not_assigned<IdentifierType>()), _leaves_partition);

            RegionPointer curr_region;
            
            // Scan all regions
            for(typename Border<ImagePartitionType>::template const_iterator<> part_it = part_borders.colors_begin(), part_it_end = part_borders.colors_end() ; part_it!= part_it_end; ++part_it)
            {
                typename Border<ImagePartitionType>::template const_iterator<>::pixel_type curr_pixel = *part_it;
                it = _regions.find(curr_pixel.value(PARTITION_CHANNEL));
                if(it==it_end)
                {
                    curr_region = new RegionType(curr_pixel.position(), curr_pixel.value(PARTITION_CHANNEL));
                    _regions.insert(map_pair_type(curr_pixel.value(PARTITION_CHANNEL), curr_region));
                    _curr_max_label = std::max(_curr_max_label, curr_pixel.value(PARTITION_CHANNEL));
                }
                else
                {
                    curr_region = it->second;
                    curr_region->push_back(curr_pixel.position());
                }
            }

            CoordType offset(1,1);

            // Scan neighbors
            for(typename Border<ImagePartitionType>::template const_iterator<> part_it = part_borders.colors_begin(), part_it_end = part_borders.colors_end() ; part_it!= part_it_end; ++part_it)
            {
                typename Border<ImagePartitionType>::template const_iterator<>::pixel_type curr_pixel = *part_it;
                curr_region = _regions.find(curr_pixel.value(PARTITION_CHANNEL))->second;

                // Up-left
                for(typename Border<ImagePartitionType>::template const_iterator<>::pixel_type::neighbor_iterator nit = curr_pixel.local_begin(ngb); nit != curr_pixel.local_end(ngb); ++nit)
                {
                    if ((*nit).value(PARTITION_CHANNEL)==not_assigned<IdentifierType>())
                    {
                        CoordType border_coord;
                        border_coord = (*nit).position() + curr_pixel.position() + offset;
                            
                        // Add a neighbor with the border of the image (RegionPointer())     
                        curr_region->neighbors_insert(NULL, border_coord);
                    }
                    else if ((*nit).value(PARTITION_CHANNEL)!=curr_pixel.value(PARTITION_CHANNEL))
                    {
                        it2 = _regions.find((*nit).value(PARTITION_CHANNEL));
                        
                        ASSERT(it2!= it_end, "Label not found in LUT");
                        
                        RegionPointer neighb_reg = it2->second;
                        
                        ASSERT(curr_region!=0, "curr_region is NULL");
  
                        CoordType border_coord;
                        border_coord = (*nit).position() + curr_pixel.position() + offset;

                        // Add a new neighbor
                        curr_region->neighbors_insert(neighb_reg, border_coord);
                    }
                }
                
                // Down-right, just for borders
                for(typename Border<ImagePartitionType>::template const_iterator<>::pixel_type::neighbor_iterator nit = curr_pixel.local_begin(ngb2); nit != curr_pixel.local_end(ngb2); ++nit)
                {
                    if ((*nit).value(PARTITION_CHANNEL)==not_assigned<IdentifierType>())
                    {
                        CoordType border_coord;
                        border_coord = (*nit).position() + curr_pixel.position() + offset;
                          
                        // Add a neighbor with the border of the image (RegionPointer())     
                        curr_region->neighbors_insert(NULL, border_coord);
                    }
                }
            }
        }

        
        //!
        //! \brief Returns a reference to the region with a given label
        //!
        //! \param[in] label: Label of the region we want to obtain
        //! \return Reference to the region with a given label
        RegionType& _region_by_label(const IdentifierType& label ) throw (ImagePlusError)
        {
            typename map_type::iterator it = _regions.find(label);
            if (it==_regions.end())
            {
                throw ImagePlusError("Partition: region index not found.");
            }
            
            return *(it->second);
        }
        
        //!
        //! \brief Relabels the roots_partition
        //! \param[in] reg : Father region to update
        //! \return Const reference to the base partition of the tree
        void _update_roots_partition(RegionPointer reg) 
        {
            typename RegionType::position_iterator pixels;
            for (pixels = reg->begin(); pixels != reg->end(); ++pixels)
            { 
                // Iterate along all the pixels and relabel
                const Coord<int64,2>& c = *pixels;
                _roots_partition[c[0]][c[1]] = reg->label();
            }
        }
        
        //! Clear and deletes all regions
        void _clear_regions() 
        {
            // Clears everything added by neighbors (if something has been allocated)
            RegionType::clear_regions(_regions);
            
            // Delete regions
            typename map_type::iterator it = _regions.begin();
            for(; it!=_regions.end(); ++it)
                delete it->second;
            
            _regions.clear();
        }
        
        //! Copies all regions
        //! \param copy: Partition to be copied
        void _copy_regions(const Partition& copy)
        {
            const_global_iterator glob_it = copy.begin();
            const_global_iterator glob_it_end = copy.end();

            // Copy the regions
            for(; glob_it!=glob_it_end; ++glob_it)
            {
                const RegionType& curr_reg = *(glob_it);
                RegionPointer new_region(new RegionType(curr_reg));
                _regions.insert(map_pair_type(curr_reg.label(), new_region));
            }

            // Modify the necessary region traits (tree structure, neighbors, etc.)
            typename map_type::iterator reg_it     = _regions.begin();
            typename map_type::iterator reg_it_end = _regions.end();
            for(; reg_it!=reg_it_end; ++reg_it)
            {
                reg_it->second->update_traits(*this);
            }
        }
        
    public:
        
        //! Class that iterates through the regions of the tree that fulfil a condition given by the functor "condition"
        //! 
        //! \todo document and explain how to create new ones
        //! 
        template<class condition, class iterator_type=typename map_type::iterator, class region_type=RegionType>
        class conditional_iterator : public std::iterator<std::input_iterator_tag, RegionType>
        {
        public:
            //! Type of region
            typedef region_type RegionType;
        protected:
            //! Iterator pointing to the current region
            iterator_type _map_it;
            
            //! Iterator pointing to the end of the region map
            iterator_type _map_end;
            
            //! Functor that defines through which regions we will iterate
            condition _condition;
            
        public:
            //! Default constructor
            conditional_iterator() : _map_it(), _map_end(), _condition() {}
            
            //! Constructor receiving an iterator to a region, to the end, and a condition functor.
            //! It iterates untill it finds a region fulfilling the condition
            //!
            //! \param[in] map_it: Iterator to the current region
            //! \param[in] map_it_end: Iterator to the end of the region map
            //! \param[in] cond :  Functor defining the condition to iterate (by default uses the default constructor)
            conditional_iterator(iterator_type map_it, iterator_type map_it_end, condition cond = condition()) : _map_it(map_it), _map_end(map_it_end), _condition(cond)
            {
                while(_map_it!=_map_end)
                {
                    if(_condition(*(_map_it->second)))
                        break;
                    else
                        ++_map_it;
                }
            }
            
            //! Copy contructor
            //!
            //! \param[in] copy: Iterator to be copied
            template<class iterator_model>
            conditional_iterator(const iterator_model& copy)
            {
                _map_it = copy.map_it();
                _map_end = copy.map_end();
                 
                while(_map_it!=_map_end)
                {
                    if(_condition(*(_map_it->second)))
                        break;
                    else
                        ++_map_it;
                }
            }
            
            
            //! map_it const accessor (for the copy constructor between different types of iterators)
            //! \return Const reference to map_it
            const iterator_type& map_it() const
            {
                return _map_it;
            }
            
            //! map_end const accessor (for the copy constructor between different types of iterators)
            //! \return Const reference to map_end
            const iterator_type& map_end() const
            {
                return _map_end;
            }
            
            //! Operator ++ to move through regions. It looks for the next region fulfilling condition
            //! \return The iterator pointing to the new position
            conditional_iterator& operator++()
            {
                ++_map_it;
                
                while(_map_it!=_map_end)
                {
                    if(_condition(*(_map_it->second)))
                        break;
                    else
                        ++_map_it;
                }
                
                return *this;
            }
            
            
            //! Operator ++ to move through regions. It looks for the next region fulfilling condition
            //! \return The iterator pointing to the new position
            conditional_iterator& operator--()
            {
                --_map_it;
                
                while(_map_it!=_map_end)
                {
                    if(_condition(*(_map_it->second)))
                        break;
                    else
                        --_map_it;
                }
                
                return *this;
            }
            
            
            /*!
             * \param[in] it : Iterator to be compared with
             *
             * \returns true if the iterators are equal
             */
            template<class iterator_model>
            bool operator==(const iterator_model& it) 
            {
                return _map_it==it.map_it();
            }

            /*!
             * \param[in] it : Iterator to be compared with
             *
             * \returns true if the iterators are different
             */
            template<class iterator_model>
            bool operator!=(const iterator_model& it) 
            {
                return _map_it!=it.map_it();
            }
            
            //! Dereference operator
            //! \return The region that it is pointing at            
            region_type& operator*() const
            {
                return *(_map_it->second);
            }
            
            //! Copy operator
            //!
            //! \param[in] other : The iterator to copy
            //! \return Reference to this, to be able to do a=b=c
            template<class iterator_model>
            conditional_iterator& operator=(const iterator_model& other)
            {
                _map_it = other.map_it();
                _map_end = other.map_end();
                
                while(_map_it!=_map_end)
                {
                    if(_condition(*(_map_it->second)))
                        break;
                    else
                        ++_map_it;
                }
                
                return *this;
            }

        };
        
        //! Condition functor to iterate through all regions
        struct global
        {
            //! Returns true, to iterate through all regions
            //! \param[in] curr_region : region to be checked
            //! \return True
            bool operator()(const RegionType& curr_region) const
            {  
                return true;
            }
        };

        //! Condition functor to iterate through leaf regions
        struct leaves
        {
            //! Checks whether the region is a leaf, i.e., if it does not have parts
            //! \param[in] curr_region : region to be checked
            //! \return Whether the region is a leaf
            bool operator()(const RegionType& curr_region) const
            {  
                return curr_region.parts().size()==0;
            }
        };
        
        //! Condition functor to iterate through non-leaf regions
        struct non_leaves
        {
            //! Checks whether the region is not a leaf, i.e., if it does have parts
            //! \param[in] curr_region : region to be checked
            //! \return Whether the region is not a leaf
            bool operator()(const RegionType& curr_region) const
            {  
                return curr_region.parts().size()>0;
            }
        };

        //! Condition functor to iterate through root regions
        struct roots
        {
            //! Checks whether the region is a root, i.e., if it does not have father
            //! \param[in] curr_region : region to be checked
            //! \return Whether the region is a root
            bool operator()(const RegionType& curr_region) const
            {  
                return curr_region.parent()==0;
            }
        };

        //! Condition functor to iterate through non-root regions
        struct non_roots
        {
            //! Checks whether the region is not a root, i.e., if it does have father
            //! \param[in] curr_region : region to be checked
            //! \return Whether the region is not a root
            bool operator()(const RegionType& curr_region) const
            {  
                return curr_region.parent()!=0;
            }
        };
        
        typedef conditional_iterator<global>         global_iterator;   //!< Type to refer to iterators through all regions
        typedef conditional_iterator<leaves>         leaves_iterator;   //!< Type to refer to iterators through leaf regions
        typedef conditional_iterator<non_leaves> non_leaves_iterator;   //!< Type to refer to iterators through non-leaf regions
        typedef conditional_iterator<roots>           roots_iterator;   //!< Type to refer to iterators through root regions
        typedef conditional_iterator<non_roots>   non_roots_iterator;   //!< Type to refer to iterators through non-root regions
        
        typedef conditional_iterator<global, typename map_type::const_iterator, const RegionType>         const_global_iterator; //!< Type to refer to const iterators through all regions
        typedef conditional_iterator<leaves, typename map_type::const_iterator, const RegionType>         const_leaves_iterator; //!< Type to refer to const iterators through leaf regions
        typedef conditional_iterator<non_leaves, typename map_type::const_iterator, const RegionType> const_non_leaves_iterator; //!< Type to refer to const iterators through non-leaf regions
        typedef conditional_iterator<roots, typename map_type::const_iterator, const RegionType>           const_roots_iterator; //!< Type to refer to const iterators through root regions
        typedef conditional_iterator<non_roots, typename map_type::const_iterator, const RegionType>   const_non_roots_iterator; //!< Type to refer to const iterators through non-root regions
        
        //! \returns an iterator to the first region of the Partition.
        //! Note that it returns a global_iterator, but in the assignment to another type of iterator, the type is changed
        global_iterator begin()     { return global_iterator(_regions.begin(), _regions.end()); }

        //! \returns an iterator to the end of the Partition.
        //! Note that it returns a global_iterator, but in the assignment to another type of iterator, the type is changed
        global_iterator end()       { return global_iterator(_regions.end(), _regions.end()); }
        
        //! \returns an iterator to region with a given label.
        //! Note that it returns a global_iterator, but in the assignment to another type of iterator, the type is changed
        //! \param id : Identifier of the region we are looking for
        global_iterator find(IdentifierType id)    { return global_iterator(_regions.find(id), _regions.end()); }

        
        
        //! \returns a const iterator to the first region of the Partition.
        //! Note that it returns a global_iterator, but in the assignment to another type of iterator, the type is changed        
        const_global_iterator begin() const    { return const_global_iterator(_regions.begin(), _regions.end()); }
        
        //! \returns a const iterator to the end of the Partition.
        //! Note that it returns a global_iterator, but in the assignment to another type of iterator, the type is changed        
        const_global_iterator end() const      { return const_global_iterator(_regions.end(), _regions.end()); }

        //! \returns a const iterator to region with a given label.
        //! Note that it returns a global_iterator, but in the assignment to another type of iterator, the type is changed
        //! \param id : Identifier of the region we are looking for
        const_global_iterator find(IdentifierType id) const      { return const_global_iterator(_regions.find(id), _regions.end()); }

        /*! 
         * \cond SKIP_DOC 
         * 
         * Friends 
         */
        template<class PartitionModel>
        friend void merge_regions(typename PartitionModel::RegionType&, typename PartitionModel::RegionType&, typename PartitionModel::IdentifierType, PartitionModel&);
        
        template<class PartitionModel>
        friend void merge_regions(typename PartitionModel::RegionType&, typename PartitionModel::RegionType&, typename PartitionModel::IdentifierType, PartitionModel&, std::list<typename PartitionModel::RegionType::NeighborLinkPointer>&, std::vector<std::list<typename PartitionModel::RegionType::NeighborLinkPointer> >&);
        
        template<class PM>
        friend void cut_partition_tree_down(std::size_t, PM&);
        
        template<class PM>
        friend void prune(typename PM::RegionType&, PM&);
        
        template<class RM>
        friend bool operator== (const Partition<RM>& bpt1, const Partition<RM>& bpt2);
        
        template<class PM>
        friend void divide_partition_tree(std::size_t, PM&, PM&);
        
        template<class PM>
        friend void relabel(PM&);
        
        friend class create_bpt;
        /*! 
         * \endcond SKIP_DOC 
         */
    };
}


#endif
