//
// Created by wegam on 2022/1/29.
//

#pragma once

#include <dal/math/vectors.hpp>
#include <dal/utilities/exceptions.hpp>

/*IF--------------------------------------------------------------------------
enumeration CollateralType
    Quantities for which discount curves are defined
switchable
alternative OIS
    Collateral appropriate for OIS or similar rate
alternative GC
    General government collateral
alternative NONE
-IF-------------------------------------------------------------------------*/

namespace Dal {
    class String_;

#include <dal/auto/MG_CollateralType_enum.hpp>
} // namespace Dal
