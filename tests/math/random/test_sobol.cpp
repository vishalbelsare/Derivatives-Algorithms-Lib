//
// Created by wegam on 2020/12/26.
//

#include <gtest/gtest.h>
#include <dal/math/random/quasirandom.hpp>
#include <dal/math/random/sobol.hpp>
#include <dal/math/vectors.hpp>
#include <memory>
#include <iostream>

using namespace Dal;

TEST(SobolTest, TestNewSobol) {
    int dim = 443;
    int i_path = 0;
    std::unique_ptr<SequenceSet_> set(NewSobol(dim, i_path));
    Vector_<> dst(dim);

    int num_path = 500000;

    Vector_<> means(dim, 0.);
    Vector_<> vars(dim, 0.);

    for (int i = 0; i < num_path; ++i) {
        set->FillUniform(&dst);
        for (int j = 0; j < dim; ++j) {
            means[j] += dst[j];
            vars[j] += Square(dst[j] - 0.5);
        }
    }

    for (int i = 0; i < dim; ++i) {
        means[i] /= num_path;
        vars[i] /= num_path;
        ASSERT_NEAR(means[i], 0.5, 1e-4);
        ASSERT_NEAR(vars[i], 1. / 12, 1e-4);
    }

    means.Fill(0.);
    vars.Fill(0.);
    for (int i = 0; i < num_path; ++i) {
        set->FillNormal(&dst);
        for (int j = 0; j < dim; ++j) {
            means[j] += dst[j];
            vars[j] += Square(dst[j]);
        }
    }

    for (int i = 0; i < dim; ++i) {
        means[i] /= num_path;
        vars[i] /= num_path;
        ASSERT_NEAR(means[i], 0.0, 1e-4);
        ASSERT_NEAR(vars[i], 1.0, 1e-4);
    }
}
