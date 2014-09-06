// --------------------------------------------------------------
// Copyright (C)
// Universitat Politecnica de Catalunya (UPC) - Barcelona - Spain
// --------------------------------------------------------------

//!
//!  \file b_search_tree.hpp
//!
//!  \brief Implementation of the Binary Search Tress known as AVL trees (Hierarchical queues)
//!
//!
#ifndef IMAGEPLUS_CORE_B_SEARCH_TREE_HPP
#define IMAGEPLUS_CORE_B_SEARCH_TREE_HPP

// Hack to avoid compile errors
#undef BOOST_DISABLE_ASSERTS

#include <boost/config.hpp>
#include <boost/intrusive/avltree_algorithms.hpp>
#include <boost/shared_ptr.hpp>


namespace imageplus
{

    //!
    //! \brief This class implements a very efficient auto-balanced Binary Search Tree structure known as AVL tree.
    //!
    //! This class implements a very efficient auto-balanced Binary Search Tree structure known as AVL tree.
    //!
    //! The objective of this class (and generally all the Binary Search Tree structures) is to create a sorted queue
    //!  with a very efficient insertion, deletion, and search operations. Since this particular implementation is autobalanced,
    //!  log(n) complexity is always guaranteed in these operations. 
    //! 
    //! It is important to note that the tree represents a single index of sorting the queue, so the search operations can be performed only 
    //!  on this index. For instance, if we store the results of a race by time spent and we also put the name of the runner, 
    //!  the queue will only be able to search by time and not by name. If you are interested in creating a table that can perform multiindex 
    //!  search, you can use the Multi-Index package of Boost (http://www.boost.org/doc/libs/1_36_0/libs/multi_index/doc/index.html).
    //!
    //! This class is mainly a clean wrapper of the Boost avltree_algorithms functions:
    //! http://www.boost.org/doc/libs/1_36_0/doc/html/boost/intrusive/avltree_algorithms.html.\n
    //! So if you are interested in having some funcionality that is available in Boost but it is not in this class, it can be easily added.
    //!
    //! In order to use it, you have to define the container in the nodes (which fields you want to store in the nodes) 
    //!  and the comparison function that defines the order in which the tree is sorted. 
    //!
    //! For instance, if we want to define a tree containing a list of countries sorted by their population, we should define first
    //!  the structure contained in the nodes:
    //! 
    //! \code
    //!     struct my_node : public bst_base_node<my_node>
    //!     {
    //!         uint64 population_;
    //!         std::string country_;
    //!            
    //!         // Constructor
    //!         node(uint64 population=0, std::string country="")
    //!         {
    //!             population_ = population;
    //!             country_ = country;
    //!         }   
    //!     };
    //! \endcode
    //!
    //! For the algorithm to work properly, our struct \b must inherit from \b bst_base_node<my_node>, 
    //!  and a constructor with default values must be defined.
    //!
    //! The definition of the comparison function is as follows:
    //!
    //! \code
    //!    struct compare_function
    //!    {
    //!        bool operator()(my_node *a, my_node *b)
    //!        {  
    //!            return a->population_ < b->population_;  
    //!        }    
    //!    };
    //! \endcode
    //!
    //! Where the parameters of the () operator must be of the type we have previously defined for the nodes.
    //!
    //! Finally, if we want to create a BST, the code is found below, where the two structures defined above are passed by template parameter:
    //!
    //! \code
    //!    BST<node, compare_function> queue;
    //! \endcode
    //! 
    //! You may find a fully defined example of the use of this class in the examples section (bst-example). 
    //!
    //! \author Jordi Pont Tuset <jpont@gps.tsc.upc.edu>
    //!
    //! \date 05-11-2008
    //!
    template<class container, class compare_function>
    class BST
    {
    public:
        
    	//! container pointer
        typedef container* container_pointer;
//        typedef boost::shared_ptr<container> container_pointer;
        
        //! Default constructor
        BST()
        {
            _avl_handler::init_header(&_avl_header);  
        }

        //!
        //! \brief Puts an element into the BST.
        //!
        //! \param[in] data : Pointer to the new data we want to put
        //!
        void put(container_pointer data)
        {
            _avl_handler::insert_equal_lower_bound(&_avl_header, data, _compare_function);
        }
        
        //!
        //! \brief Deletes an element from the BST.
        //!
        //! \param[in] pointer : Pointer to the data we want to remove
        //!
        void erase(container_pointer pointer)
        {
            _avl_handler::erase(&_avl_header, pointer);
        }        
        
        //!
        //! \brief Gets the first element of the BST.
        //!
        //! \return Pointer to the first element
        //!
        container_pointer get_first() const
        {
            return _avl_header.left_;
        }

        //!
        //! \brief Returns the next element of the BST.
        //!
        //! \param[in] key : Pointer to the data whose next element we are looking for
        //! \return Pointer to the next element in the queue (0x0 if it's the last element)
        //!
        container_pointer next(container_pointer key) const
        {
            container * temp = _avl_handler::next_node(key);
            if (temp == &_avl_header)
            {
                return 0x0;
            }
            else
            {
                return temp;   
            }
        }
        

        //!
        //! \brief Returns the previous element of the BST.
        //!
        //! \param[in] key : Pointer to the data whose next element we are looking for
        //! \return Pointer to the previous element in the queue (0x0 if it's the first element)
        //!
        container_pointer prev(container_pointer key) const
        {
            container_pointer temp = _avl_handler::prev_node(key);
            if (temp == &_avl_header)
            {
                return 0x0;
            }
            else
            {
                return temp;   
            }
        }

        
        //!
        //! \brief Finds the element that is equal to key, according to the comparison function defined to create the tree.
        //!
        //! \param[in] key : Pointer to the data we want to find
        //! \return Pointer to the found element or 0x0 if it was not found
        //!
        container_pointer find(container_pointer key) const
        {
            container_pointer temp = _avl_handler::find(&_avl_header , key, _compare_function);
            if (temp==&_avl_header)
                return 0x0;
            else
                return temp;
        }
        
        //!
        //! \brief Finds the element that is stricly bigger than key, according to the comparison function defined to create the tree.
        //!
        //! \param[in] key : Pointer to the data we want to compare
        //! \return Pointer to the found element or 0x0 if it was not found
        //!
        container_pointer strict_upper_bound(container_pointer key) const
        {
            container_pointer temp = _avl_handler::upper_bound(&_avl_header , key, _compare_function);
            if (temp==&_avl_header)
                return 0x0;
            else
                return temp;
        }
        
        //!
        //! \brief Finds the element that is bigger than or equal to key, according to the comparison function defined to create the tree.
        //!
        //! \param[in] key : Pointer to the data we want to compare
        //! \return Pointer to the found element or 0x0 if it was not found
        //!
        container_pointer upper_bound(container_pointer key) const
        {
            container_pointer temp = _avl_handler::lower_bound(&_avl_header , key, _compare_function);
            if (temp==&_avl_header)
                return 0x0;
            else
                return temp;
        }

        //!
        //! \brief Returns the number of elements of the tree
        //!
        //! \note The cost of this function is logarithmic
        //! \return The size of the tree
        //!
        std::size_t size() const
        {
            return _avl_handler::size(&_avl_header);
        }
        
        //!
        //! \brief Returns the number of elements of the tree
        //!
        //! \note The cost of this function is constant
        //! \return Whether the tree is empty
        //!
        bool is_empty() const
        {
            return _avl_header.left_==&(_avl_header);
        }
        

    private:
        //! Functor to compare nodes
        compare_function _compare_function;
        
        //! Functions and types to work with the AVL tree
        struct avltree_node_traits
        {
           //! AVL node type 
           typedef container           node;
           //! AVL node pointer type
           typedef container *         node_ptr;
           //! AVL const node pointer type
           typedef const container *   const_node_ptr;
           //! Type to represent the balance factor of a node of the the AVL
           typedef int                 balance;

           //! Gets a pointer to the parent node
           //! \param[in] n : Pointer to the current node
           //! \return    Pointer to the parent node
           static node_ptr get_parent(const_node_ptr n)       {  return n->parent_;   }  

           //! Sets the pointer to the parent node
           //! \param[in] n : Pointer to the current node
           //! \param[in] parent : Pointer to the parent node
           static void set_parent(node_ptr n, node_ptr parent){  n->parent_ = parent; }
           
           //! Gets a pointer to the node at the left
           //! \param[in] n : Pointer to the current node
           //! \return    Pointer to the left node
           static node_ptr get_left(const_node_ptr n)         {  return n->left_;     }  

           //! Sets the pointer to the node at the left
           //! \param[in] n : Pointer to the current node
           //! \param[in] left : Pointer to the left node
           static void set_left(node_ptr n, node_ptr left)    {  n->left_ = left;     }
           
           //! Gets a pointer to the node at the right
           //! \param[in] n : Pointer to the current node
           //! \return    Pointer to the right node
           static node_ptr get_right(const_node_ptr n)        {  return n->right_;    }  
           
           //! Sets the pointer to the node at the right
           //! \param[in] n : Pointer to the current node
           //! \param[in] right : Pointer to the right node
           static void set_right(node_ptr n, node_ptr right)  {  n->right_ = right;   }  
           
           //! Gets the balance factor of a node
           //! \param[in] n : Pointer to the current node
           //! \return Balance of the node
           static balance get_balance(const_node_ptr n)       {  return n->balance_;  }  

           //! Sets the balance factor of a node
           //! \param[in] n : Pointer to the current node
           //! \param[in] b : Balance of the node
           static void set_balance(node_ptr n, balance b)     {  n->balance_ = b;     }
           
           //! \returns the value to represent a negative balance
           static balance negative()                          {  return -1; }
           //! \returns the value to represent a zero balance
           static balance zero()                              {  return  0;  }
           //! \returns the value to represent a positive balance
           static balance positive()                          {  return  1;  }
        };
    public:
        //! Class storing the algorithms involved in the AVL tree handling
        typedef boost::intrusive::avltree_algorithms<avltree_node_traits> _avl_handler;
    private:
        //! Header of the tree
        container _avl_header;
    };
    
    
    
    
    
    
    //! Mandatory data stored in the AVL tree for the structure to work properly
    template<class node>
    struct bst_base_node
    {
        //! Default constructor
        bst_base_node(){}
       
        //! Pointer to parent node
        node *parent_;
        //! Pointer to the left node
        node *left_;
        //! Pointer to the right node
        node *right_;
        
        //! Balance factor of the node
        int balance_;
    };
}



#endif
