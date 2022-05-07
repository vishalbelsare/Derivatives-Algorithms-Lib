#include <cmath>
#include <dal/math/matrix/matrixarithmetic.hpp>
#include <dal/math/matrix/matrixs.hpp>
#include <dal/platform/strict.hpp>
#include <functional>

namespace Dal {
    namespace Matrix {
        Vector_<> Vols(const Matrix_<>& cov, Matrix_<>* corr) {
            const int n = cov.Rows();
            REQUIRE(cov.Cols() == n, "Covariance matrix should be square matrix");
            Vector_<> ret_val(n);
            if (corr)
                *corr = cov;
            for (int i = 0; i != n; ++i) {
                ret_val[i] = sqrt(cov(i, i));
                if (corr) {
                    auto scale = [&ret_val, &i](double x) {
                        return x / Max(Dal::EPSILON, ret_val[i]);
                    };
                    auto r = corr->Row(i);
                    Transform(&r, scale);
                    auto c = corr->Col(i);
                    Transform(&c, scale);
                }
            }
            return ret_val;
        }
    } // namespace Matrix
} // namespace Dal