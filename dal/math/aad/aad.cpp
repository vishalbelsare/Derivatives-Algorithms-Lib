/*
 * Modified by wegamekinglc on 2020/12/13.
 * Written by Antoine Savine in 2018
 * This code is the strict IP of Antoine Savine
 * License to use and alter this code for personal and commercial applications
 * is freely granted to any person or company who purchased a copy of the book
 * Modern Computational Finance: AAD and Parallel Simulations
 * Antoine Savine
 * Wiley, 2018
 * As long as this comment is preserved at the top of the file
 */

#include <dal/platform/strict.hpp>
#include <dal/math/aad/aad.hpp>

namespace Dal::AAD {
    size_t TapNode_::numAdj_ = 1;
    bool Tape_::multi_ = false;
}
