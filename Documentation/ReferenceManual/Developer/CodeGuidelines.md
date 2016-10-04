Best Practices for Portable Code {#codeguidelines}
========

# Source Code Line Endings #
 All source code line endings should be in "Unix" style "\n". Every text editor understands these line endings on every platform **except** the "Notepad" application on Windows.

# Use of Tabs #
Spaces should be used instead of hard tabs. This helps file portability across different editors. DREAM.3D uses a standard whereby all indents  use two spaces.

#  Always Use an Include Guard #
Always use an "include guard" in your headers:

    #ifndef _MY_HEADER_H_
    #define _MY_HEADER_H_
    
    Your Code Goes Here

    #endif

Include guards are _#define_ preprocessor directives used in header files that ensure
the header is only ever included **once** during the compiling process for a
given source file. If these guards are not included, it is up to the programmer to make sure that headers are only included a single time, which can become tedious or impossible in some situations with larger projects.

# Use Only ANSI-C99 Integer Types #
The ANSI-C99 standard defines typedefs for 8, 16, 32 and 64 bit integer types. These should be used if possible. Platform specific types such as **int64** and ambiguous types such as

    long int
    signed long int
    unsigned long int

should be avoided at all costs. This usage is important since the number of bytes that a "long" variable occupies will change depending on CPU type and compiler used. This can have an impact on many important operations, such as pointer arithmetic and binary file IO. All Unix  systems have a *stdint.h*  file included with the compiler that defines the proper types. The Microsoft Visual Studio Compiler does not have this file included. There are lots of examples of creating one on the internet. An exemplar file is  also included with the Electronic Imaging software distribution that is generated using CMake.

## C99 Integer Types ##
    int8_t    8  Bit Singed Integer
    uint8_t   8  Bit UnSinged Integer
    int16_t   16 Bit Singed Integer
    uint16_t  16 Bit UnSinged Integer
    int32_t   32 Bit Singed Integer
    uint32_t  32 Bit UnSinged Integer
    int64_t   64 Bit Singed Integer
    uint64_t  64 Bit UnSinged Integer

# Special Constants #
nullptr should be used in conjunction with only raw pointers and 0 (zero) should be used in conjunction with numeric values:

    int* value = nullptr; // Good
    int* value = 0; // Bad

    int i = 0; // Good
    int i = nullptr; // Bad 

# Variable Initialization #
*Never* rely on the compiler to initialize variables or pointers for you. Always initialize your variables before first use:

    int i; // Bad, unless this is pure "C" then you have NO choice
    i = 0; // Good. The variable has a known starting value
    float* f = nullptr; // Good (if the compiler will let you do this)
    char buffer[512]; // Allocation of char buffer but will be filled with junk
    memset(buffer, 0, 512); // Good. Splat Zeros across the array

## Rationale ##
Initializing your own variables ensures that you are starting from a known state when execution of the code reaches that point. Subtle bugs can occur if you do not initialize your variables and then perform checks on them to see if they are valid. A classic example is the following:

    int* ptr;
    if (!ptr) { initializePointer(ptr); }

When compiled in Debug on *some* compilers this will execute as you would expect. The problems come when the code is compiled in *Release* mode. The _ptr_ variable will **NOT** be set to _NULL_ but instead some random memory address, which can cause problems because the initialization code that should have been running will be skipped due to the conditional _if (!ptr) _ will be true. Remembering to always initialize variables avoids these issues.

# Array Initialization #

    define MY_ARRAY_SIZE  10;
    int array[10];              // Good - Will compile on all platforms
    int array[MY_ARRAY_SIZE];   // Good - Will compile on all platforms
    const int size = 10;
    int array[size];            // Bad - Will NOT compile on all platforms

An array is declared as *datatype name[constant-size]* and groups one or more instances of a data type into  one addressable place. Constant-size may be an expression,  but the expression must evaluate to a constant.

# The Dereference operator `*` and the Address-Of Operator `&` Should Be Directly Connected with the Type-Specifier #

    int32*   p; // Correct
    int32   *p; // Incorrect
    int32*   p, q; // Probably error

The _int32*  p;_ form emphasizes type over syntax while the _int32 *p;_ form emphasizes syntax over type. Although both forms are equally valid C++, the heavy emphasis on types in C++ suggests that _int32* p;_ is the preferable form.

# Never Assume CPU Capabilities #
When writing code, never assume that a CPU has a specific capability such as SSE. Use a _#define_ block to either test for the capability at runtime or write a generic version of your code will work on any machine and then write the specific implementation of the code that is bracketed by a _#ifdef_ block.

    #ifdef HAS_SSE_3
      SSE3 specific code
    #else
       Generic Code or error message stating the cpu is missing the functionality
    #endif


# Copyright & License Information #
Always include a copyright and/or license block at the start of **every** source file. It is good practice to document in the source code any copyright or licensing restrictions in every file that you write. An example license block is shown below.

    ///////////////////////////////////////////////////////////////////////////////
    //
    //  Copyright (c) 2007, 2013 Michael A. Jackson for BlueQuartz Software
    //  All rights reserved.
    //  BSD License: http://www.opensource.org/licenses/bsd-license.html
    //
    ///////////////////////////////////////////////////////////////////////////////

    More stringent notices can also be written into the source code.
    /***************************************************************************
    **
    ** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
    ** All rights reserved.
    ** Contact: Nokia Corporation (qt-info@nokia.com)
    **
    ** This file is part of the QtCore module of the Qt Toolkit.
    **
    ** $QT\_BEGIN\_LICENSE:LGPL$
    ** Commercial Usage
    ** Licensees holding valid Qt Commercial licenses may use this file in
    ** accordance with the Qt Commercial License Agreement provided with the
    ** Software or, alternatively, in accordance with the terms contained in
    ** a written agreement between you and Nokia.
    **
    ** GNU Lesser General Public License Usage
    ** Alternatively, this file may be used under the terms of the GNU Lesser
    ** General Public License version 2.1 as published by the Free Software
    ** Foundation and appearing in the file LICENSE.LGPL included in the
    ** packaging of this file.  Please review the following information to
    ** ensure the GNU Lesser General Public License version 2.1 requirements
    ** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
    **
    ** In addition, as a special exception, Nokia gives you certain additional
    ** rights.  These rights are described in the Nokia Qt LGPL Exception
    ** version 1.1, included in the file LGPL\_EXCEPTION.txt in this package.
    **
    ** GNU General Public License Usage
    ** Alternatively, this file may be used under the terms of the GNU
    ** General Public License version 3.0 as published by the Free Software
    ** Foundation and appearing in the file LICENSE.GPL included in the
    ** packaging of this file.  Please review the following information to
    ** ensure the GNU General Public License version 3.0 requirements will be
    ** met: http://www.gnu.org/copyleft/gpl.html.
    **
    ** If you have questions regarding the use of this file, please contact
    ** Nokia at qt-info@nokia.com.
    ** $QT\_END\_LICENSE$
    **
    ***************************************************************************/


# Always Implement the "Big Three" in C++ Classes #
When writing C++ classes the programmer will always define the "Big Three" which are defined as:

- copy constructor
- copy assignment operator
- destructor

The programmer should **never** allow the compiler to implement these methods. This will happen if they are not explicitly defined in the class declaration. Further more if the destructor is declared _public_ then it **will** have the **virtual** modifier applied to the declaration.

    class A
    {
      public:
         A();
         virtual ~A();
      private:
       A(const A&);               //Copy Constructor Not Implemented
       void operator=(const A&);  //Copy Assignment Not Implemented
    };

Note that with C++11, programmers now have the ability to inform the compiler which of these operations can be constructed by default and which can be ignored using the `default` and `delete` keywords. 

# String Constants #
String constants in C++ should be declared as:

    const std::string MyFile("SomeFile.dat");

Like constants should be grouped into a namespace.

    namespace MyConstants {
        const std::string MyFile("SoimeFile");
    }

When using ANSI C one should use a char* for constant strings:

    const char* MyFile "SomeFile.dat";
    const char MyFile[5] = { 'a', '.', 'd', 'a', 't'};

Using this type of approach allows for quicker code updates when constant values need to be changed.