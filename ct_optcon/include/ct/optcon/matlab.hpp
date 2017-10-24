/**********************************************************************************************************************
This file is part of the Control Toobox (https://adrlab.bitbucket.io/ct), copyright by ETH Zurich, Google Inc.
Authors:  Michael Neunert, Markus Giftthaler, Markus Stäuble, Diego Pardo, Farbod Farshidian
Lincensed under Apache2 license (see LICENSE file in main directory)
**********************************************************************************************************************/

#pragma once

#ifdef MATLAB_FULL_LOG
#define MATLAB
#endif  // MATLAB_FULL_LOG

#ifdef MATLAB
#include <matlabCppInterface/MatFile.hpp>
#else   //MATLAB
namespace matlab {
//! a dummy class which is created for compatibility reasons if the MATLAB flag is not set.
class MatFile
{
public:
    MatFile() {}
};
}  // namespace MATLAB
#endif  //MATLAB
