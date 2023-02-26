//
// Created by wegam on 2023/2/25.
//

#include <dal/math/pde/finitedifference.hpp>

namespace Dal::PDE {

    void FiniteDifference_::Dx(int wind, const Vector_<>& x, Matrix_<>& dx) {
        int n = x.size();
        int m = n - 1;

        dx.Resize(n, 3);
        if (!n)
            return;

        int i, j;
        double dxl, dxu, dxm;

        if (wind < 0) {
            for (j = 0; j < 3; ++j)
                dx(0, j) = 0.0;
            for (i = 1; i <= m; ++i) {
                dxl = x(i) - x(i - 1);
                dx(i, 0) = -1.0 / dxl;
                dx(i, 1) = 1.0 / dxl;
                dx(i, 2) = 0.0;
            }
        } else if (wind == 0) {
            dxu = x(1) - x(0);
            dx(0, 0) = 0.0;
            dx(0, 1) = -1.0 / dxu;
            dx(0, 2) = 1.0 / dxu;
            for (i = 1; i < m; ++i) {
                dxl = x(i) - x(i - 1);
                dxu = x(i + 1) - x(i);
                dxm = dxl + dxu;
                dx(i, 0) = -dxu / dxl / dxm;
                dx(i, 1) = (dxu / dxl - dxl / dxu) / dxm;
                dx(i, 2) = dxl / dxu / dxm;
            }
            dxl = x(m) - x(m - 1);
            dx(m, 0) = -1.0 / dxl;
            dx(m, 1) = 1.0 / dxl;
            dx(m, 2) = 0.0;
        } else if (wind > 0) {
            for (i = 0; i < m; ++i) {
                dxu = x(i + 1) - x(i);
                dx(i, 0) = 0.0;
                dx(i, 1) = -1.0 / dxu;
                dx(i, 2) = 1.0 / dxu;
            }
            for (j = 0; j < 3; ++j)
                dx(m, j) = 0.0;
        }
    }

    void FiniteDifference_::Dxx(const Vector_<>& x, Matrix_<>& dxx) {
        int n = x.size();
        int m = n - 1;

        dxx.Resize(n, 3);
        if (!n)
            return;

        int i, j;
        double dxl, dxu, dxm;

        for (j = 0; j < 3; ++j)
            dxx(0, j) = 0.0;
        for (i = 1; i < m; ++i) {
            dxl = x(i) - x(i - 1);
            dxu = x(i + 1) - x(i);
            dxm = 0.5 * (dxl + dxu);
            dxx(i, 0) = 1.0 / dxl / dxm;
            dxx(i, 1) = -(1.0 / dxl + 1.0 / dxu) / dxm;
            dxx(i, 2) = 1.0 / dxu / dxm;
        }
        for (j = 0; j < 3; ++j)
            dxx(m, j) = 0.0;
    }

} // namespace Dal::PDE