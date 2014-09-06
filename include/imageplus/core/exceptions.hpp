// --------------------------------------------------------------
// Copyright (C)
// Universitat Politecnica de Catalunya (UPC) - Barcelona - Spain
// --------------------------------------------------------------

//!
//!  \file exceptions.hpp
//!
//!  Definition of ImagePlus exceptions
//!

#ifndef IMAGEPLUS_CORE_EXCEPTIONS_HPP
#define IMAGEPLUS_CORE_EXCEPTIONS_HPP


//TODO: Backtrace in WIN32?
#ifndef WIN32
#include <execinfo.h>
#include <cxxabi.h>
#endif

#include <stdexcept>
#include <sstream>
#include <stdlib.h>

namespace imageplus
{

    //!
    //! \brief Basic Backtraced exception for ImagePlus, do not use this class but the ones derived from this one
    //!
    //! \author Javier Ruiz Hidalgo <j.ruiz@upc.edu>
    //!
    //! \date 5-feb-2008
    //!
    class BacktracedImagePlusException : public std::exception
    {
        public:

    	//!
    	//! \brief Constructor
    	//!
    	//! \param[in] s : Description of the exception
    	//!
    	BacktracedImagePlusException(const std::string& s)
                : _message (s)
    	{
#ifndef WIN32
#ifndef NDEBUG

    	    this->add_backtrace();

#endif
#endif
    	}
    
    	//!
    	//! \brief returns exception message description
    	//!
    	//! \return exception message description
    	//!
    	virtual const char* what() const throw ()
    	{
    	    return _message.c_str();
    	}
    
    	//!
    	//! \brief Destructor
    	//!
    	~BacktracedImagePlusException() throw ()
    	{
    	}
    
    	//!
    	//! \brief Copy constructor
    	//!
    	//! \param[in] copy : object to copy
    	//!
    	BacktracedImagePlusException( const BacktracedImagePlusException& copy )
    	        : _message( copy._message )
    	{
    	}

        private:

    	//!
    	//! \brief Returns the C+ demangled function name
    	//!
    	//! \param[in] mangled : Function mae as returned by backtrace_symbols()
    	//!
    	//! \return  the C+ demangled function name
    	//!
    	const std::string demangle( const std::string& mangled )
    	{
#ifndef WIN32
    	    
    	    int status;
            char* c_str = abi::__cxa_demangle( mangled.c_str(), 0, 0, &status );
            if ( status != 0 )
            {
                return mangled;
            }
            std::string str( c_str );
            free( c_str );
            return str;
            
#endif
    	}
    
    	//!
    	//! \brief Adds backtrace to _message
    	//!
    	void add_backtrace()
    	{
#ifndef WIN32
    	    
            void * array[25];
            int nSize = backtrace(array, 25);
            char ** symbols = backtrace_symbols(array, nSize);
            std::stringstream sstr;
            
            sstr << std::endl << "DEBUG BACKTRACE: STARTS" << std::endl;
            for (int i = 0; i < nSize; i++)
            {
                std::string str(symbols[i]);
                std::size_t i1 = str.find( '(' );
                std::size_t i2 = str.rfind( '+' );
        
                if ( ( str.substr(i1+1, i2-i1-1) == "__libc_start_main" ) || ( str.substr(i1+1, i2-i1-1) == "__gxx_personality_v0" ) )
                {
                    // do not add this
                }
                else
                {
                    sstr << "DEBUG BACKTRACE:   #" << i << ": " << demangle(str.substr(i1+1, i2-i1-1)) << std::endl;
                }
            }
            sstr << "DEBUG BACKTRACE: ENDS" << std::endl;
            free(symbols);
            _message += sstr.str();
#endif
    	}

    	//! string to store the exception descriptor plus the backtrace
    	std::string _message;
    };


    //!
    //! \brief Basic exception for ImagePlus errors
    //!
    //! \author Javier Ruiz Hidalgo <j.ruiz@upc.edu>
    //!
    //! \date 10-1-2008
    //!
    //!
    class ImagePlusError : public BacktracedImagePlusException
    {
        public:
    
        	//!
        	//! \brief Constructor
        	//!
        	//! \param[in] s : Description of the exception
        	//!
        	ImagePlusError(const std::string& s)
        	        : BacktracedImagePlusException(s)
        	{
        	}
    };
    
    //!
    //! \brief Basic internal exception for ImagePlus. This exception should be used in ASSERT scenarios.
    //!
    //! \author Albert Gil Moreno <agil@tsc.upc.edu>
    //!
    //! \date 12-6-2008
    //!
    //!
    class ImagePlusInternalError : public BacktracedImagePlusException
    {
        public:
    
        //!
        //! \brief Constructor
        //!
        //! \param[in] s : Description of the exception
        //!
        ImagePlusInternalError(const std::string& s)
                : BacktracedImagePlusException("INTERNAL ERROR: "+ s)
        {
        }
    };

    //!
    //! \brief Basic exception for ImagePlus file not found errors
    //!
    //! \author Javier Ruiz Hidalgo <j.ruiz@upc.edu>
    //!
    //! \date 10-1-2008
    //!
    //!
    class ImagePlusFileNotFound : public BacktracedImagePlusException
    {
        public:
    
    	//!
    	//! \brief Constructor
    	//!
    	//! \param[in] filename : Filename of file not found on disk
    	//!
    	ImagePlusFileNotFound(const std::string& filename)
    	        : BacktracedImagePlusException("File " + filename + " not found")
    	{
    	}
    };

    //!
    //! \brief Basic exception for ImagePlus file errors
    //!
    //! \author Javier Ruiz Hidalgo <j.ruiz@upc.edu>
    //!
    //! \date 10-1-2008
    //!
    //!
    class ImagePlusFileError : public BacktracedImagePlusException
    {
        public:
    
    	//!
    	//! \brief Constructor
    	//!
    	//! \param[in] filename : Filename of the name in disk
    	//! \param[in]        s : Description of the error
    	//!
    	ImagePlusFileError(const std::string& filename,const std::string& s)
    	        : BacktracedImagePlusException("Error in file '" + filename + "': " + s)
    	{
    	}
    };

    //!
    //! \brief Basic Exception for Multiview Module. Specially focused on dealing with
    //! visibility exceptions.
    //!
    //! \author Adolfo López <alopez@gps.tsc.upc.es>
    //!
    //! \date 6-02-2008
    //!
    class ImagePlusMultiviewVisibility : public BacktracedImagePlusException
    {
        public:
    	//!
    	//! \brief Basic Exception for Multiview Module. Specially focused to deal with
    	//! visibility exceptions.
        //!
        //! \param[in] s : Description
        //!
      ImagePlusMultiviewVisibility(const std::string& s)
    	        : BacktracedImagePlusException(s)
    	{
    	}
    };

    //!
    //! \brief Exception for flaw in implementation.
    //!
    //! \author Eduardo Mendon�a <eduardo@gps.tsc.upc.es>
    //!
    //! \date 20-05-2008
    //!
    class ImagePlusNotImplemented : public BacktracedImagePlusException
    {
        public:
    	
    	//!
    	//! \brief Constructor
    	//!
    	//! \param[in] s : Description
    	//!
    	ImagePlusNotImplemented(const std::string& s)
                : BacktracedImagePlusException("NOT IMPLEMENTED : " + s)
        {
        }
    };
}

/*!
 * \def IMAGEPLUS_ERROR(msg)
 *
 * \ingroup DebugMacros
 *
 * A helper macro to allow complex and concatenated error messages.
 *
 * \code
 * IMAGEPLUS_ERROR("Error running function var=" << var);
 * \endcode
 *
 *
 * \author Albert Gil Moreno <albert.gil@upc.edu>
 */
#define IMAGEPLUS_ERROR(msg) { std::stringstream ss; ss << "[ImagePlusError]: " << msg; throw ImagePlusError(ss.str()); }

#endif // IMAGEPLUS_CORE_EXCEPTIONS_HPP
