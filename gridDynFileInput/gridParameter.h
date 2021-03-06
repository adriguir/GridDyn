/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil;  eval: (c-set-offset 'innamespace 0); -*- */
/*
* LLNS Copyright Start
* Copyright (c) 2016, Lawrence Livermore National Security
* This work was performed under the auspices of the U.S. Department
* of Energy by Lawrence Livermore National Laboratory in part under
* Contract W-7405-Eng-48 and in part under Contract DE-AC52-07NA27344.
* Produced at the Lawrence Livermore National Laboratory.
* All rights reserved.
* For details, see the LICENSE file.
* LLNS Copyright End
*/

#ifndef GRIDPARAMETER_H_
#define GRIDPARAMETER_H_

#include "units.h"
#include <string>
#include <vector>
/** @brief helper class for representing a parameter of various types*/
class gridParameter
{
public:
  std::string field;        //!< the field of the parameter
  std::string strVal;        //!< a string value of the parameter
  std::vector<int> applyIndex;        //!< a vector of indices the parameter should be applied to
  double value = 0.0;        //!< the numerical value of the parameter
  gridUnits::units_t paramUnits = gridUnits::defUnit;        //!< the units used for the parameter
  bool valid = false;        //!< indicator if the parameter is valid
  bool stringType = false;        //!< indicator that the parameter is using the string property
  /** @brief constructor*/
  gridParameter ();
  /** @brief alternate constructor
  @param[in] str  call the from string constructor
  */
  gridParameter (const std::string &str);
  /** @brief alternate constructor
  @param[in] fld  the field of the parameter
  @param[in] val  the value of the parameter
  */
  gridParameter (std::string fld, double val);
  /** @brief alternate constructor
  @param[in] fld  the field of the parameter
  @param[in] val  the string value of the parameter
  */
  gridParameter (std::string fld, std::string val);
  /** @brief reset the parameter*/
  void reset ();
  /** @brief load a gridParameter from a string*
  @param[in] str the string to load from
  @return FUNCTION_EXECUTION_SUCCESS on successful convergence FUNCTION_EXECUTION_FAILURE otherwise
  */
  int fromString (const std::string &str);
};

#endif