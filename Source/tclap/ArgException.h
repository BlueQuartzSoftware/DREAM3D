// -*- Mode: c++; c-basic-offset: 4; tab-width: 4; -*-

/****************************************************************************** 
 * 
 *  file:  ArgException.h
 * 
 *  Copyright (c) 2003, Michael E. Smoot .
 *  All rights reverved.
 * 
 *  See the file COPYING in the top directory of this distribution for
 *  more information.
 *  
 *  THE SOFTWARE IS PROVIDED _AS IS_, WITHOUT WARRANTY OF ANY KIND, EXPRESS 
 *  OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL 
 *  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
 *  DEALINGS IN THE SOFTWARE.  
 *  
 *****************************************************************************/ 


#ifndef TCLAP_ARG_EXCEPTION_H
#define TCLAP_ARG_EXCEPTION_H

#include <string>
#include <exception>

namespace TCLAP {

/**
 * A simple class that defines and argument exception.  Should be caught
 * whenever a CmdLine is created and parsed.
 */
class ArgException : public std::exception
{
	public:
	
		/**
		 * Constructor.
		 * \param text - The text of the exception.
		 * \param id - The text identifying the argument source.
		 * \param td - Text describing the type of ArgException it is.
		 * of the exception.
		 */
		ArgException( const QString& text = "undefined exception", 
					  const QString& id = "undefined",
					  const QString& td = "Generic ArgException")
			: std::exception(), 
			  _errorText(text), 
			  _argId( id ), 
			  _typeDescription(td)
		{ } 
		
		/**
		 * Destructor.
		 */
		virtual ~ArgException() throw() { }

		/**
		 * Returns the error text.
		 */
		QString error() const { return ( _errorText ); }

		/**
		 * Returns the argument id.
		 */
		QString argId() const  
		{ 
			if ( _argId == "undefined" )
				return " ";
			else
				return ( "Argument: " + _argId ); 
		}

		/**
		 * Returns the arg id and error text. 
		 */
		const char* what() const throw() 
		{
			static QString ex; 
			ex = _argId + " -- " + _errorText;
			return ex.c_str();
		}

		/**
		 * Returns the type of the exception.  Used to explain and distinguish
		 * between different child exceptions.
		 */
		QString typeDescription() const
		{
			return _typeDescription; 
		}


	private:

		/**
		 * The text of the exception message.
		 */
		QString _errorText;

		/**
		 * The argument related to this exception.
		 */
		QString _argId;

		/**
		 * Describes the type of the exception.  Used to distinguish
		 * between different child exceptions.
		 */
		QString _typeDescription;

};

/**
 * Thrown from within the child Arg classes when it fails to properly
 * parse the argument it has been passed.
 */
class ArgParseException : public ArgException
{ 
	public:
		/**
		 * Constructor.
		 * \param text - The text of the exception.
		 * \param id - The text identifying the argument source 
		 * of the exception.
		 */
		ArgParseException( const QString& text = "undefined exception", 
					       const QString& id = "undefined" )
			: ArgException( text, 
			                id, 
							QString( "Exception found while parsing " ) + 
							QString( "the value the Arg has been passed." ))
			{ }
};

/**
 * Thrown from CmdLine when the arguments on the command line are not
 * properly specified, e.g. too many arguments, required argument missing, etc.
 */
class CmdLineParseException : public ArgException
{
	public:
		/**
		 * Constructor.
		 * \param text - The text of the exception.
		 * \param id - The text identifying the argument source 
		 * of the exception.
		 */
		CmdLineParseException( const QString& text = "undefined exception", 
					           const QString& id = "undefined" )
			: ArgException( text, 
			                id,
							QString( "Exception found when the values ") +
							QString( "on the command line do not meet ") +
							QString( "the requirements of the defined ") +
							QString( "Args." ))
		{ }
};

/**
 * Thrown from Arg and CmdLine when an Arg is improperly specified, e.g. 
 * same flag as another Arg, same name, etc.
 */
class SpecificationException : public ArgException
{
	public:
		/**
		 * Constructor.
		 * \param text - The text of the exception.
		 * \param id - The text identifying the argument source 
		 * of the exception.
		 */
		SpecificationException( const QString& text = "undefined exception",
					            const QString& id = "undefined" )
			: ArgException( text, 
			                id,
							QString("Exception found when an Arg object ")+
							QString("is improperly defined by the ") +
							QString("developer." )) 
		{ }

};

class ExitException {
public:
	ExitException(int estat) : _estat(estat) {}

	int getExitStatus() const { return _estat; }

private:
	int _estat;
};

} // namespace TCLAP

#endif

