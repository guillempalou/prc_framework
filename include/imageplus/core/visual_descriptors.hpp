// --------------------------------------------------------------
// Copyright (C)
// Universitat Politecnica de Catalunya (UPC) - Barcelona - Spain
// --------------------------------------------------------------

//!
//!  \file visual_descriptors.hpp
//!
//!

#ifndef IMAGEPLUS_CORE_VISUAL_DESCRIPTORS_HPP
#define IMAGEPLUS_CORE_VISUAL_DESCRIPTORS_HPP

#include <vector>
#include <map>
#include <imageplus/core/imageplus_types.hpp>
#include <imageplus/core/exceptions.hpp>
#include <boost/shared_ptr.hpp>
#include <iostream>

#include <boost/lexical_cast.hpp>
#include <string>

#ifdef USE_XML
#include <xercesc/dom/DOM.hpp>
#endif

namespace imageplus
{   
    // Forward declaration
    class CollaborativeDescriptors;
   
    //! Base class of all the Visual Descriptors, allowing us to store pointers to this class but 
    //!  running the derived class functions thanks to the virtual functions.
    //!
    //! \author Jordi Pont <jordi.pont@upc.edu>
    //!
    //! \date 10-9-2009
    class DescriptorBase
    {
    public:
              
    	//! Constructor receiving its id and whether the descriptor is recursive or not
    	//!
        //! \param[in] id : Strinf identifier
    	//! \param[in] recursive : Boolean that defines whether the descriptor is recursive or not
        DescriptorBase(const std::string& id, bool recursive=false) 
                : _id(id), _recursive(recursive)
        {
        };
        
        //! Virtual destructor
        virtual ~DescriptorBase(){};
        
        //! Returns the string identifier of the descriptor 
        //! \return The identifier string of the desciptor (for instance "Area")
        virtual const std::string& id() const
        {
            return _id;
        }
       
        //! \brief Calculates the descriptor of the parent region given its siblings.
        //!
        //! \param[in] son1_descs : CollaborativeDescriptors of the son 1
        //! \param[in] son2_descs : CollaborativeDescriptors of the son 2
        //! \param[in] peer_descs : Pointer to CollaborativeDescriptors, in principle is never 0x0 but it is a good idea to ASSERT it
        void recursive_calculate(CollaborativeDescriptors& son1_descs, CollaborativeDescriptors& son2_descs, CollaborativeDescriptors* peer_descs)
        {
            throw ImagePlusError("Descriptor '" + id() + "' is not implemented recursively.");
        }
        
    #ifdef USE_XML
        //! \returns the name of the descriptor to be written to the XML descriptors file
        virtual
        std::string xml_name() const{return "Unassigned";};
        
        //! Writes the value of the descriptor. This function is mandatory to be defined. 
        //! \param[in] doc: Pointer to the XML document we are writing 
        //! \param[in] vdElem: Pointer to the XML element referring to the region whose descriptors we are writing
        virtual
        void xml_write(XERCES_CPP_NAMESPACE::DOMDocument *doc, XERCES_CPP_NAMESPACE::DOMElement *vdElem) const{};
        
        /*!
         * Reads the value of the descriptor. This function is mandatory to be defined.
         *
         * \param[in] vdNode: Pointer to the XML element referring to the region whose descriptors we are reading
         *
         * \return true if the descriptor have been read.
         */
        virtual
        bool xml_read(XERCES_CPP_NAMESPACE::DOMNode *vdNode) const{return false;};
    #endif
        
        //! Returns whether the descriptor can be computed recursively.
        //! \return True if the descriptor may be computed recursively and false if not.
        virtual bool is_recursive() const
        {
            return _recursive;
        };
        
    protected:
        //! String identifier
        std::string _id;
        
        //! String identifier
        bool _recursive;
    };
    
    
    //! Shared pointer to the base class of the descriptors (DescriptorBase)
    typedef DescriptorBase* VDBasePtr;


    //! Collaborative Visual Descriptor
    //! 
    //! \note To compute descriptors, please refer to the helper functions available below
    //!
    //! \author Jordi Pont <jordi.pont@upc.edu>
    //!
    //! \date 10-9-2009
    class CollaborativeDescriptors
    {
    public:
        //! Constructor
        //!
        //! \param[in] global_desc : Pointer to the global CollaborativeDescriptors (for instance, of the whole image)
        CollaborativeDescriptors(CollaborativeDescriptors* global_desc=0x0) : _vdescs()
        {
            _global_desc = global_desc;
        }
        
        ~CollaborativeDescriptors() {
        	clear();
        }

        //!
        //! \note You shouldn't use this function directly, use helper functions instead
        //!
        //! \param[in] desc : Pointer to the descriptor to be calculated
        //! \param[in] first : Iterator to the beginning of the region
        //! \param[in] last : Iterator to the end of the region
        //! \param[in] global_desc : Pointer to the global CollaborativeDescriptors (for instance, of the whole image)
        //!
        //! \returns the calculated descriptor of a region given the iterators to visit it
        //!
        template<class VDModel, class IterModel>
        VDModel& calculate(VDModel* desc, IterModel first, IterModel last, CollaborativeDescriptors* global_desc=0x0)
        {
            if((global_desc!=0x0) && (_global_desc==0x0))
                _global_desc = global_desc;
            else if (_global_desc==0x0)
                _global_desc = this;
            
            std::string id = desc->id();
            std::map<std::string,VDBasePtr>::iterator it = _vdescs.find(id);
            if( it!=_vdescs.end() )
            {
                delete desc;
                desc = (VDModel*)((it->second));
            }
            else
            {
                _vdescs[id]=desc;
                desc->calculate(first, last, this);
            }
            return *desc;
        }
    
        //!
        //! \note You shouldn't use this function directly, use helper functions instead
        //!
        //! \param[in] desc : Pointer to the descriptor to be calculated
        //! \param[in] son1_desc : CollaborativeDescriptors of one son
        //! \param[in] son2_desc : CollaborativeDescriptors of the other son
        //! \param[in] global_desc : Pointer to the global CollaborativeDescriptors (for instance, of the whole image)
        //!
        //! \returns the calculated descriptor of a region given their sons CollaborativeDescriptors
        //!
        template<class VDModel>
        VDModel& recursive_calculate(VDModel* desc, CollaborativeDescriptors& son1_desc, CollaborativeDescriptors& son2_desc, CollaborativeDescriptors* global_desc=0x0)
        {
            if((global_desc!=0x0) && (_global_desc==0x0))
                _global_desc = global_desc;
            
            std::string id = desc->id();
            std::map<std::string,VDBasePtr>::iterator it = _vdescs.find(id);
            if( it!=_vdescs.end() )
            {
                delete desc;
                desc = (VDModel*)((it->second));
            }
            else
            {
                _vdescs[id]=VDBasePtr(desc);
                desc->recursive_calculate(son1_desc, son2_desc, this);

                son1_desc.delete_descriptor(id);
                son2_desc.delete_descriptor(id);
            }
            return *desc;
        }
    
        
        //!
        //! It creates a descriptors without the need to be computed. This function should just be used in 
        //! special cases such as VDCommonPerim
        //!
        //! \param[in] desc: Pointer to the descriptor to be created
        //!
        //! \returns *desc
        //!
        //! \todo We need a link to an example about how and why use this function
        //!
        template<class VDModel>
        VDModel& create(VDModel* desc)
        {                     
            // ASSERT not calculated
            std::string id = desc->id();

            _vdescs[id] = desc;
            return *desc;
        }
        
        //! Gets the descriptor of the type defined by desc
        //!
        //! \note The descriptor must be already calculated, so make sure it is
        //! 
        //! \param[in] desc: Object of the type of the descriptor to be retrieved
        //! \return The stored descriptor
        template<class VDModel>
        VDModel& get(const VDModel& desc)
        {           
            std::string id = desc.id();
            
            //ASSERT(is_calculated(id), "You called 'get' of a descriptor "+ id +" that was not previously calculated.");
            
            std::map<std::string,VDBasePtr>::iterator it = _vdescs.find(id);
            return *((VDModel*)(it->second));
        }
        
        //! Gets the descriptor of the type defined by desc
        //!
        //! \note The descriptor must be already calculated, so make sure it is
        //! 
        //! \param[in] desc: Object of the type of the descriptor to be retrieved
        //! \return The stored descriptor
        template<class VDModel>
        const VDModel& get(const VDModel& desc) const
        {           
            std::string id = desc.id();
            
            //ASSERT(is_calculated(id), "You called 'get' of a descriptor "+ id +" that was not previously calculated.");
            
            std::map<std::string,VDBasePtr>::const_iterator it = _vdescs.find(id);
            return *((VDModel*)(it->second));
        }

        //! Gets the pointer to the descriptor of the type defined by id. Note that the pointer 
        //!   is to a base class, so if you want to execute functions that are not virtual, you
        //!   will have to cast the pointer to the specific derived class.
        //!
        //! \note The descriptor must be already calculated, so make sure it is
        //! 
        //! \param[in] id: String identifier
        //! \return A pointer to the stored descriptor (but with the type of the base class)        
        const VDBasePtr get_id(const std::string& id) const
        {
            //ASSERT(is_calculated(id), "You called 'get' of a descriptor "+ id +" that was not previously calculated.");
            
            std::map<std::string,VDBasePtr>::const_iterator it = _vdescs.find(id);
            return (it->second);
        }
        
        //! Gets the pointer to the descriptor of the type defined by id. Note that the pointer 
        //!   is to a base class, so if you want to execute functions that are not virtual, you
        //!   will have to cast the pointer to the specific derived class.
        //!
        //! \note The descriptor must be already calculated, so make sure it is
        //! 
        //! \param[in] id: String identifier
        //! \return A pointer to the stored descriptor (but with the type of the base class)
        template<class VDModel>
        const VDModel& get_id(const std::string& id) const
        {
            //ASSERT(is_calculated(id), "You called 'get' of a descriptor "+ id +" that was not previously calculated.");
            
            std::map<std::string,VDBasePtr>::const_iterator it = _vdescs.find(id);
            return *((VDModel*)(it->second));
        }

        //! Checks whether a descriptor is already calculated
        //! 
        //! \param[in] id: String identifier
        //! \return Whether the descriptor is calculated or not      
        bool is_calculated(const std::string& id) const
        {
            std::map<std::string,VDBasePtr>::const_iterator it = _vdescs.find(id);
            if( it!=_vdescs.end() ) 
                return true;
            else
                return false;        
        }
    
        
        //! Accessor to global descriptors
        //! 
        //! \return Pointer to global descriptors     
        CollaborativeDescriptors* global_desc()
        {
            return _global_desc;
        }

        //! Const accessor to global descriptors
        //! 
        //! \return Const pointer to global descriptors
        const CollaborativeDescriptors* global_desc() const
        {
            return _global_desc;
        }
        
        void delete_descriptor(std::string id) {
        	/*Erase the recursive descriptors*/
        	delete _vdescs[id];
        	_vdescs.erase(_vdescs.find(id));
        }

        //! Deletes all calculated descriptors
        void clear()
        {
        	for (std::map<std::string,VDBasePtr>::iterator it = _vdescs.begin(); it != _vdescs.end(); ++it)
        		delete it->second;

            _vdescs.clear();
        }

    private:
        //! Ponter to global collaborative descriptors
        CollaborativeDescriptors* _global_desc;

        //! Set of pointers to visual descriptor, indexed by its identifiers
        std::map<std::string,VDBasePtr> _vdescs;
    };
    
    
    //! Traits to contain collaborative descriptors. Your class has to inherit from this one to be describable
    class collaborative_descriptors_traits
    {
    public:
        //! Constructor receiving a pointer to the global CollaborativeDescriptors.
    	//! \param[in] global_vd : Pointer to the global CollaborativeDescriptors
        collaborative_descriptors_traits(CollaborativeDescriptors* global_vd=NULL)
            : _coll_vd(global_vd)
        {
        }
        
        //! Collaborative Visual Descriptors accessor
        //! \return Reference to CollaborativeDescriptors
        CollaborativeDescriptors& coll_vd()
        {
            return _coll_vd;
        }

        //! Collaborative Visual Descriptors const accessor
        //! \return Const reference to CollaborativeDescriptors
        const CollaborativeDescriptors& coll_vd() const
        {
            return _coll_vd;
        }
        
    protected:
        //! Collaborative descriptors of the regions
        CollaborativeDescriptors _coll_vd;
    };
    
    
    
    // //////////////////////////////////////////
    //            Helper functions
    // //////////////////////////////////////////
    
    //! Check whether a descriptor is calculated
    //! \param[in] id : String identifying the descriptor
    //! \param[in] input : Object being described
    //!
    //! \return true if the descriptor is already calculated
    //!
    template<class InputModel>
    bool is_calculated(std::string id, InputModel& input)
    {
        return input.coll_vd().is_calculated(id);
    }
    
    //! Stores a descriptor in the collaborative descriptors of input
    //!
    //! \param[in] desc : Pointer to the descriptor to be stored
    //! \param[in] input : Object being described
    //!
    //! \returns the stored descriptor
    //!
    template<class VDModel, class InputModel>
    VDModel& store_descriptor(VDModel* desc, InputModel& input)
    {
        return input.coll_vd().create(desc);
    }

    //! Gets a descriptor that has been previously calculated
    //!
    //! \param[in] id : String identifying the descriptor
    //! \param[in] input : Object being described
    //!
    //! \returns the desired descriptor
    //!
    template<class VDModel, class InputModel>
    VDModel& get_descriptor(std::string id, InputModel& input)
    {
        return *((VDModel*)(input.coll_vd().get_id(id)));
    }
    
    //! Helper function to compute a visual descriptor from an InputModel
    //!
    //! \param[in] desc: Pointer to a new descriptor object of the type we want to compute
    //! \param[in] input: The descriptor will be computed on this object 
    //!
    //! \returns the calculated descriptor
    //!
    template<class VDModel, class InputModel>
    VDModel& calc_descriptor(VDModel* desc, InputModel& input )
    {
        return input.coll_vd().calculate(desc, input.colors_begin(), input.colors_end(), &input.coll_vd());
    }

    //! Helper function to compute a visual descriptor from an InputModel and a RegionModel
    //!
    //! \param[in] desc: Pointer to a new descriptor object of the type we want to compute
    //! \param[in] input: The descriptor will be computed using this data
    //! \param[in] region: The descriptor will be computed on this region
    //!
    //! \returns the calculated descriptor
    //!
    template<class VDModel, class InputModel, class RegionModel >
    VDModel& calc_descriptor(VDModel* desc, InputModel& input, RegionModel& region )
    {
        typedef typename RegionModel::RegionPointer RegPointer;
        if((!desc->is_recursive()) || (!region.is_composite()))
        {
            return region.coll_vd().calculate(desc, input.colors_begin(region), input.colors_end(region), &input.coll_vd());
        }
        else if(!(region.parts().size()))
        {
            return region.coll_vd().calculate(desc, input.colors_begin(region), input.colors_end(region), &input.coll_vd());
        }
        else if(region.coll_vd().is_calculated(desc->id()))
        {
        	VDModel& descriptor = region.coll_vd().get(*desc);
        	delete desc;
            return descriptor;
        }
        else
        {
            std::vector<RegPointer> directly, recursive, to_look;
            std::size_t n_to_look = 0;
            for(std::size_t ii=0; ii<region.parts().size(); ii++)
            {
                to_look.push_back(region.parts()[ii]);
                n_to_look++;
            }
             
            while(n_to_look>0)
            {
                RegPointer curr = to_look.back();
                to_look.pop_back();
                 
                if(curr->coll_vd().is_calculated(desc->id()))
                {
                    n_to_look--;
                }
                else
                {
                    std::size_t n_children = curr->parts().size();
                    if(n_children)
                    {
                        recursive.push_back(curr);
                        for(std::size_t ii=0; ii<n_children; ii++)
                        {
                            to_look.push_back(curr->parts()[ii]);
                        }
                        n_to_look = n_to_look + n_children - 1;
                    }
                    else
                    {
                        directly.push_back(curr);
                        n_to_look--;
                    }
                }
            }
             
            typename std::vector<RegPointer>::iterator it = directly.begin();
            typename std::vector<RegPointer>::iterator it_end = directly.end();
            for(; it!=it_end; ++it)
            {
                (*it)->coll_vd().calculate(new VDModel(*desc), input.colors_begin(**it), input.colors_end(**it), &input.coll_vd());
            }
         
            typename std::vector<RegPointer>::reverse_iterator it2 = recursive.rbegin();
            typename std::vector<RegPointer>::reverse_iterator it2_end = recursive.rend();
            for(; it2!=it2_end; ++it2)
            {
                (*it2)->coll_vd().recursive_calculate(new VDModel(*desc), (*it2)->parts()[0]->coll_vd(), (*it2)->parts()[1]->coll_vd(), &input.coll_vd());
            }
            return region.coll_vd().recursive_calculate(desc, region.parts()[0]->coll_vd(), region.parts()[1]->coll_vd(), &input.coll_vd());
        }
    }
    
#ifdef USE_XML
    //! Helper function to read a visual descriptor from a RegionModel in a XML node. If the descriptor
    //! can't be read from the XML, then it is computed.
    //!
    //! \param[in] desc  : Pointer to a new descriptor object of the type we want to compute
    //! \param[in] vdNode: Pointer to the "StillRegion" node of the XML file
    //! \param[in] input : The descriptor will be computed using this data if can't be read from vdNode
    //! \param[in] region: The descriptor will be read/computed on this region
    template<class VDModel, class InputModel, class RegionModel >
    void read_or_calc_descriptor(VDModel* desc, XERCES_CPP_NAMESPACE::DOMNode *vdNode, InputModel& input, RegionModel& region )
    {
        bool read = desc->xml_read(vdNode);
        if(read)
        {
            store_descriptor(desc,region);
        }
        else
        {
            calc_descriptor(desc,input,region);
        }
    }
    
    //! Helper function to read a visual descriptor from a RegionModel in a XML node
    //!
    //! \param[in] desc: Pointer to a new descriptor object of the type we want to read
    //! \param[in] vdNode: Pointer to the "StillRegion" node of the XML file
    //! \param[in] region: The descriptor will be read from this region
    template<class VDModel, class RegionModel >
    void read_descriptor(VDModel* desc, XERCES_CPP_NAMESPACE::DOMNode *vdNode, RegionModel& region )
    {
        bool read = desc->xml_read(vdNode);
        if(read)
        {
            store_descriptor(desc,region);
        }
        else
        {
            throw ImagePlusError(desc->id() + " can't be read from XML file for the given region");
        }
    }
#endif
    
}

#endif
