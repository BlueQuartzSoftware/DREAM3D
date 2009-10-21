/*
 * Grains.cpp
 *
 *  Created on: Oct 21, 2009
 *      Author: mjackson
 */

#include "Grain.h"
#include <iostream>


static int something = 0;


Grain::Grain()
{
  m_uid = ++something;
  std::cout << "Grain(): " << m_uid << std::endl;
}

Grain::~Grain()
{
  std::cout << "~Grain()" << std::endl;
}
