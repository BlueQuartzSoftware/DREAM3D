// -*- Mode: c++; c-basic-offset: 4; tab-width: 4; -*-

/****************************************************************************** 
 * 
 *  file:  DocBookOutput.h
 * 
 *  Copyright (c) 2004, Michael E. Smoot
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

#ifndef TCLAP_DOCBOOKOUTPUT_H
#define TCLAP_DOCBOOKOUTPUT_H

#include <string>
#include <vector>
#include <list>
#include <iostream>
#include <algorithm>

#include <tclap/CmdLineInterface.h>
#include <tclap/CmdLineOutput.h>
#include <tclap/XorHandler.h>
#include <tclap/Arg.h>

namespace TCLAP {

/**
 * A class that generates DocBook output for usage() method for the 
 * given CmdLine and its Args.
 */
class DocBookOutput : public CmdLineOutput
{

	public:

		/**
		 * Prints the usage to stdout.  Can be overridden to 
		 * produce alternative behavior.
		 * \param c - The CmdLine object the output is generated for. 
		 */
		virtual void usage(CmdLineInterface& c);

		/**
		 * Prints the version to stdout. Can be overridden 
		 * to produce alternative behavior.
		 * \param c - The CmdLine object the output is generated for. 
		 */
		virtual void version(CmdLineInterface& c);

		/**
		 * Prints (to stderr) an error message, short usage 
		 * Can be overridden to produce alternative behavior.
		 * \param c - The CmdLine object the output is generated for. 
		 * \param e - The ArgException that caused the failure. 
		 */
		virtual void failure(CmdLineInterface& c, 
						     ArgException& e );

	protected:

		/**
		 * Substitutes the char r for string x in string s.
		 * \param s - The string to operate on. 
		 * \param r - The char to replace. 
		 * \param x - What to replace r with. 
		 */
		void substituteSpecialChars( QString& s, char r, QString& x );
		void removeChar( QString& s, char r);
		void basename( QString& s );

		void printShortArg(Arg* it);
		void printLongArg(Arg* it);

		char theDelimiter;
};


inline void DocBookOutput::version(CmdLineInterface& _cmd) 
{ 
	qDebug() << _cmd.getVersion() << "\n";
}

inline void DocBookOutput::usage(CmdLineInterface& _cmd ) 
{
	QList<Arg*> argList = _cmd.getArgList();
	QString progName = _cmd.getProgramName();
	QString version = _cmd.getVersion();
	theDelimiter = _cmd.getDelimiter();
	XorHandler xorHandler = _cmd.getXorHandler();
	QVector< QVector<Arg*> > xorList = xorHandler.getXorList();
	basename(progName);

	qDebug() << "<?xml version='1.0'?>" << "\n";
	qDebug() << "<!DOCTYPE refentry PUBLIC \"-//OASIS//DTD DocBook XML V4.2//EN\"" << "\n";
	qDebug() << "\t\"http://www.oasis-open.org/docbook/xml/4.2/docbookx.dtd\">" << "\n" << "\n";

	qDebug() << "<refentry>" << "\n";

	qDebug() << "<refmeta>" << "\n";
	qDebug() << "<refentrytitle>" << progName << "</refentrytitle>" << "\n";
	qDebug() << "<manvolnum>1</manvolnum>" << "\n";
	qDebug() << "</refmeta>" << "\n";

	qDebug() << "<refnamediv>" << "\n";
	qDebug() << "<refname>" << progName << "</refname>" << "\n";
	qDebug() << "<refpurpose>" << _cmd.getMessage() << "</refpurpose>" << "\n";
	qDebug() << "</refnamediv>" << "\n";

	qDebug() << "<refsynopsisdiv>" << "\n";
	qDebug() << "<cmdsynopsis>" << "\n";

	qDebug() << "<command>" << progName << "</command>" << "\n";

	// xor
	for ( int i = 0; (unsigned int)i < xorList.size(); i++ )
	{
		qDebug() << "<group choice='req'>" << "\n";
		for ( ArgVectorIterator it = xorList[i].begin(); 
						it != xorList[i].end(); it++ )
			printShortArg((*it));

		qDebug() << "</group>" << "\n";
	}
	
	// rest of args
	for (ArgListIterator it = argList.begin(); it != argList.end(); it++)
		if ( !xorHandler.contains( (*it) ) )
			printShortArg((*it));

 	qDebug() << "</cmdsynopsis>" << "\n";
	qDebug() << "</refsynopsisdiv>" << "\n";

	qDebug() << "<refsect1>" << "\n";
	qDebug() << "<title>Description</title>" << "\n";
	qDebug() << "<para>" << "\n";
	qDebug() << _cmd.getMessage() << "\n"; 
	qDebug() << "</para>" << "\n";
	qDebug() << "</refsect1>" << "\n";

	qDebug() << "<refsect1>" << "\n";
	qDebug() << "<title>Options</title>" << "\n";

	qDebug() << "<variablelist>" << "\n";
	
	for (ArgListIterator it = argList.begin(); it != argList.end(); it++)
		printLongArg((*it));

	qDebug() << "</variablelist>" << "\n";
	qDebug() << "</refsect1>" << "\n";

	qDebug() << "<refsect1>" << "\n";
	qDebug() << "<title>Version</title>" << "\n";
	qDebug() << "<para>" << "\n";
	qDebug() << version << "\n"; 
	qDebug() << "</para>" << "\n";
	qDebug() << "</refsect1>" << "\n";
	
	qDebug() << "</refentry>" << "\n";

}

inline void DocBookOutput::failure( CmdLineInterface& _cmd,
				    ArgException& e ) 
{ 
	static_cast<void>(_cmd); // unused
	qDebug() << e.what() << "\n";
	throw ExitException(1);
}

inline void DocBookOutput::substituteSpecialChars( QString& s,
				                                   char r,
												   QString& x )
{
	size_t p;
	while ( (p = s.find_first_of(r)) != QString::npos )
	{
		s.erase(p,1);
		s.insert(p,x);
	}
}

inline void DocBookOutput::removeChar( QString& s, char r)
{
	size_t p;
	while ( (p = s.find_first_of(r)) != QString::npos )
	{
		s.erase(p,1);
	}
}

inline void DocBookOutput::basename( QString& s )
{
	size_t p = s.find_last_of('/');
	if ( p != QString::npos )
	{
		s.erase(0, p + 1);
	}
}

inline void DocBookOutput::printShortArg(Arg* a)
{
	QString lt = "&lt;"; 
	QString gt = "&gt;"; 

	QString id = a->shortID();
	substituteSpecialChars(id,'<',lt);
	substituteSpecialChars(id,'>',gt);
	removeChar(id,'[');
	removeChar(id,']');
	
	QString choice = "opt";
	if ( a->isRequired() )
		choice = "plain";

	qDebug() << "<arg choice='" << choice << '\'';
	if ( a->acceptsMultipleValues() )
		qDebug() << " rep='repeat'";


	qDebug() << '>';
	if ( !a->getFlag().empty() )
		qDebug() << a->flagStartChar() << a->getFlag();
	else
		qDebug() << a->nameStartString() << a->getName();
	if ( a->isValueRequired() )
	{
		QString arg = a->shortID();
		removeChar(arg,'[');
		removeChar(arg,']');
		removeChar(arg,'<');
		removeChar(arg,'>');
		arg.erase(0, arg.find_last_of(theDelimiter) + 1);
		qDebug() << theDelimiter;
		qDebug() << "<replaceable>" << arg << "</replaceable>";
	}
	qDebug() << "</arg>" << "\n";

}

inline void DocBookOutput::printLongArg(Arg* a)
{
	QString lt = "&lt;"; 
	QString gt = "&gt;"; 

	QString desc = a->getDescription();
	substituteSpecialChars(desc,'<',lt);
	substituteSpecialChars(desc,'>',gt);

	qDebug() << "<varlistentry>" << "\n";

	if ( !a->getFlag().empty() )
	{
		qDebug() << "<term>" << "\n";
		qDebug() << "<option>";
		qDebug() << a->flagStartChar() << a->getFlag();
		qDebug() << "</option>" << "\n";
		qDebug() << "</term>" << "\n";
	}

	qDebug() << "<term>" << "\n";
	qDebug() << "<option>";
	qDebug() << a->nameStartString() << a->getName();
	if ( a->isValueRequired() )
	{
		QString arg = a->shortID();
		removeChar(arg,'[');
		removeChar(arg,']');
		removeChar(arg,'<');
		removeChar(arg,'>');
		arg.erase(0, arg.find_last_of(theDelimiter) + 1);
		qDebug() << theDelimiter;
		qDebug() << "<replaceable>" << arg << "</replaceable>";
	}
	qDebug() << "</option>" << "\n";
	qDebug() << "</term>" << "\n";

	qDebug() << "<listitem>" << "\n";
	qDebug() << "<para>" << "\n";
	qDebug() << desc << "\n";
	qDebug() << "</para>" << "\n";
	qDebug() << "</listitem>" << "\n";

	qDebug() << "</varlistentry>" << "\n";
}

} //namespace TCLAP
#endif 
