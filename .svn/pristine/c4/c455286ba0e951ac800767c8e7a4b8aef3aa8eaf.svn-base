// --------------------------------------------------------------
// Copyright (C)
// Universitat Politecnica de Catalunya (UPC) - Barcelona - Spain
// --------------------------------------------------------------

//!
//!  \file imageplus_types.hpp
//!
//!  Portable definition of basic types
//!


#ifndef IMAGEPLUS_CORE_IMAGEPLUS_TYPES_HPP
#define IMAGEPLUS_CORE_IMAGEPLUS_TYPES_HPP


namespace imageplus
{
    // Define integer types (8, 16, 32, 64)

    /*!
     * \defgroup ImagePlusTypes Basic Data Types
     */

    //! Unsigned 8 bits integer \ingroup ImagePlusTypes
    typedef unsigned char         uint8;
    
    //! Signed 8 bits integer   \ingroup ImagePlusTypes
    typedef char                  int8;
    
    //! Unsigned 16 bits integer \ingroup ImagePlusTypes
    typedef unsigned short        uint16;
    
    //! Signed 16 bits integer   \ingroup ImagePlusTypes
    typedef short int             int16;
    
    //! Unsigned 32 bits integer  \ingroup ImagePlusTypes
    typedef unsigned int          uint32;
    
    //! Signed 32 bits integer    \ingroup ImagePlusTypes
    typedef int                   int32;
    
    //! Unsigned 64 bits integer  \ingroup ImagePlusTypes
    typedef unsigned long long    uint64;
    
    //! Signed 64 bits integer    \ingroup ImagePlusTypes
    typedef long long             int64;
    
    //! Signed 32 bits float      \ingroup ImagePlusTypes
    typedef float                 float32;
    
    //! Signed 64 bits float      \ingroup ImagePlusTypes
    typedef double                float64;
    
    //! Type for size and dims variables \ingroup ImagePlusTypes
    typedef uint64                size_type;

   
    //! Alias for the largest floating point type in current architecture. Use only to safely contain data before type conversions in some templated functions.
    //! Do not use this type if not absolutely necessary. 
    //! \ingroup ImagePlusTypes
    typedef long double                largest_float;

    //! Alias for the largest integer type in current architecture. Use only to safely contain data before type conversions in some templated functions. 
    //! In the following example, a function to round to nearest integer uses largest_integer before converting to return type
    //!
    //! \code
    //! template<typename T1, typename T2>
    //! IMAGEPLUS_INLINE
    //! T1 mnint (T2 inval)
    //! {
    //!     largest_integer outval = (inval < 0) ? static_cast<largest_integer>(inval-0.5) : static_cast<largest_integer>(inval+0.5);
    //!    
    //!     ASSERT (outval <= std::numeric_limits<T1>::max(), "Conversion causes data loss");
    //!     ASSERT (outval >= std::numeric_limits<T1>::min(), "Conversion causes data loss");
    //!
    //!     return (static_cast<T1>(outval));
    //! }
    //! \endcode
    //! Do not use this type if not absolutely necessary
    //! \ingroup ImagePlusTypes
    typedef long long int              largest_integer;

}

#endif
