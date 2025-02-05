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

#include <dal/math/aad/aad.hpp>
#include <dal/platform/strict.hpp>

namespace Dal {
    namespace {
        Tape_* CreateGlobalTape() {
            static Tape_ GLOBAL_TAP;
            return &GLOBAL_TAP;
        }
    } // namespace
    size_t Node_::num_adj_ = 1;
    bool Tape_::multi_ = false;

    thread_local Tape_* Number_::tape_ = CreateGlobalTape();
} // namespace Dal
