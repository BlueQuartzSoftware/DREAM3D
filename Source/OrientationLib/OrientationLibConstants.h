/* ============================================================================
 * Copyright (c) 2015 BlueQuartz Software, LLC
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice, this
 * list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.
 *
 * Neither the names of any of the BlueQuartz Software contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#ifndef _orientationlibconstants_h_
#define _orientationlibconstants_h_

namespace LambertParametersType
{
  static const double iPi = 0.3183098861837910 ; // 1/pi
  static const double sPi = 1.7724538509055160 ; // sqrt(pi)
  static const double sPio2 = 1.2533141373155000; // sqrt(pi/2)
  static const double sPi2 = 0.8862269254527580; // sqrt(pi)/2
  static const double srt = 0.8660254037844390;  // sqrt(3)/2
  static const double isrt = 0.5773502691896260; // 1/sqrt(3)
  static const double alpha = 1.3467736870885980; // sqrt(pi)/3^(1/4)
  static const double rtt = 1.7320508075688770;  // sqrt(3)
  static const double prea = 0.5250375679043320; // 3^(1/4)/sqrt(2pi)
  static const double preb = 1.0500751358086640; // 3^(1/4)sqrt(2/pi)
  static const double prec = 0.9068996821171090; // pi/2sqrt(3)
  static const double pred = 2.0943951023931950; // 2pi/3
  static const double pree = 0.7598356856515930; // 3^(-1/4)
  static const double pref = 1.3819765978853420; // sqrt(6/pi)
  // ! the following constants are used for the cube to quaternion hemisphere mapping
  static const double a = 1.9257490199582530;    //pi^(5/6)/6^(1/6)
  static const double ap = 2.1450293971110250 ;  //pi^(2/3)
  static const double sc = 0.8977727869612860 ;  //a/ap
  static const double beta = 0.9628745099791260; //pi^(5/6)/6^(1/6)/2
  static const double R1 = 1.3306700394914690 ;  //(3pi/4)^(1/3)
  static const double r2 = 1.4142135623730950 ;  //sqrt(2)
  static const double r22 = 0.7071067811865470;  //1/sqrt(2)
  static const double pi12 = 0.2617993877991490; //pi/12
  static const double pi8 = 0.3926990816987240;  //pi/8
  static const double prek = 1.6434564029725040; //R1 2^(1/4)/beta
  static const double r24 = 4.8989794855663560;  //sqrt(24)
  static const double tfit[16] = {1.00000000000188520, -0.50000000021948470,
                                  -0.0249999921275931260, - 0.0039287015447813740,
                                  -0.00081527015354504380, - 0.00020095004261197120,
                                  -0.000023979867760717560, - 0.000082028689266058410,
                                  +0.000124487150420900920, - 0.00017491142148225770,
                                  +0.00017034819341400540, - 0.000120620650041168280,
                                  +0.0000597197058686608260, - 0.000019807567239656470,
                                  +0.0000039537146842128740, - 0.000000365550014397195440
                                 };
  static const double BP[6] = { 0.0, 1.0, 0.5773502691896260, 0.4142135623730950, 0.0,
                                0.2679491924311230
                              };     //used for Fundamental Zone determination in so3 module
}

// Add some shortened namespace alias
// Condense some of the namespaces to same some typing later on.
namespace LPs = LambertParametersType;


#endif /* _OrientationLibConstants_H_ */
