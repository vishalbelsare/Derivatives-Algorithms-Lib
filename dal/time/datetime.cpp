//
// Created by Cheng Li on 2018/2/4.
//

#include <dal/platform/strict.hpp>
#include <dal/time/datetime.hpp>
#include <dal/utilities/algorithms.hpp>

namespace Dal {

    namespace {
        void FracToHMS(double frac, int* h, int* m, int* s) {
            ASSIGN(h, static_cast<int>(24 * frac));
            ASSIGN(m, static_cast<int>(1440 * frac) % 60);
            ASSIGN(s, static_cast<int>(86400 * frac) % 60);
        }
    } // namespace

    DateTime_::DateTime_(const Date_& date, double frac) : date_(date), frac_(frac) {
        REQUIRE(frac_ < 1., "DateTime fraction exceeds or equal to 1");
    }

    DateTime_::DateTime_(const Date_& date, int h, int m, int s) : date_(date) {
        const auto secs = 60 * (60 * h + m) + s;
        REQUIRE(secs >= 0 && secs < 86400, "DateTime fraction exceeds maximum seconds in one day");
        frac_ = secs / 86400.;
    }

    DateTime_::DateTime_(long long msec) {
        const auto whole = msec / 86400000;
        frac_ = static_cast<double>(msec - 86400000 * whole) / 86400000.;
        REQUIRE(frac_ < 1., "DateTime fraction exceeds maximum seconds in one day");
        date_ = Date::Minimum().AddDays(static_cast<int>(whole));
    }

    DateTime_& DateTime_::operator+=(double frac) {
        REQUIRE(frac > 0.0, "frac must be positive");
        frac_ += frac;
        const auto dt = static_cast<int>(std::floor(frac_));
        if (dt > 0) {
            date_ = date_.AddDays(dt);
            frac_ -= dt;
        }
        return *this;
    }

    DateTime_ DateTime_::operator+(double frac) {
        DateTime_ new_dt(date_, frac_);
        new_dt += frac;
        return new_dt;
    }

    double operator-(const DateTime_& lhs, const DateTime_& rhs) {
        return lhs.Date() - rhs.Date() + lhs.Frac() - rhs.Frac();
    }

    bool operator<(const DateTime_& lhs, const DateTime_& rhs) {
        return lhs.Date() < rhs.Date() || (lhs.Date() == rhs.Date() && lhs.Frac() < rhs.Frac());
    }

    namespace DateTime {
        int Hour(const DateTime_& dt) {
            int h;
            FracToHMS(dt.Frac(), &h, nullptr, nullptr);
            return h;
        }

        int Minute(const DateTime_& dt) {
            int m;
            FracToHMS(dt.Frac(), nullptr, &m, nullptr);
            return m;
        }

        String_ TimeString(const DateTime_& dt) {
            int h, m, s;
            FracToHMS(dt.Frac(), &h, &m, &s);
            String_ ret_val("00:00:00");
            sprintf(&ret_val[0], "%02d:%02d:%02d", h, m, s);
            return ret_val;
        }

        DateTime_ Minimum() { return DateTime_(Date::Minimum(), 0.); }

        long long MSec(const DateTime_& dt) {
            long long days = dt.Date() - Date::Minimum();
            return 86400000 * days + static_cast<long long>(dt.Frac() * 86400000);
        }
    } // namespace DateTime
} // namespace Dal
