//
// Created by Cheng Li on 17-12-19.
//

#pragma once

#include <algorithm>
#include <dal/platform/platform.hpp>
#include <dal/utilities/exceptions.hpp>
#include <type_traits>

#define ASSIGN(p, v)                                                                                                   \
    if (!(p))                                                                                                          \
        ;                                                                                                              \
    else                                                                                                               \
        *(p) = (v)

#define DEREFERENCE(p, v) ((p) ? *(p) : (v))

namespace Dal {

    template <class T> struct vector_of { using type = Vector_<std::remove_reference_t<std::remove_const_t<T>>>; };

    template <class CS_, class OP_, class CD_> void Transform(const CS_& src, OP_ op, CD_* dst) {
        REQUIRE(dst && src.size() == dst->size(), "dst is null or src size is not compatible with dst size");
        std::transform(src.begin(), src.end(), dst->begin(), op);
    }

    template <class CS1_, class CS2_, class OP_, class CD_>
    void Transform(const CS1_& src1, const CS2_& src2, OP_ op, CD_* dst) {
        REQUIRE(dst && src1.size() == dst->size() && src2.size() == dst->size(),
                "dst is null or src size is not compatible with dst size");
        std::transform(src1.begin(), src1.end(), src2.begin(), dst->begin(), op);
    }

    template <class C_, class OP_> void Transform(C_* to_change, OP_ op) {
        REQUIRE(to_change != nullptr, "dst is null");
        std::transform(to_change->begin(), to_change->end(), to_change->begin(), op);
    }

    template <class C_, class CI_, class OP_> void Transform(C_* to_change, const CI_& other, OP_ op) {
        REQUIRE(to_change != nullptr && to_change->size() == other.size(),
                "dst is null or src size is not compatible with dst size");
        std::transform(other.begin(), other.end(), to_change->begin(), op);
    }

    template <class CS_, class CD_> void Copy(const CS_& src, CD_* dst) {
        REQUIRE(dst && src.size() == dst->size(), "dst is null or src size is not compatible with dst size");
        std::copy(src.begin(), src.end(), dst->begin());
    }

    template <class C_, class OP_> auto Apply(OP_ op, const C_& src) {
        using vector_t = typename vector_of<decltype(op(*src.begin()))>::type;
        vector_t ret_val(src.size());
        Transform(src, op, &ret_val);
        return ret_val;
    }

    template <class C1_, class C2_, class OP_> auto Apply(OP_ op, const C1_& src1, const C2_& src2) {
        REQUIRE(src1.size() == src2.size(), "src1 and src2 type is compatible");
        using vector_t = typename vector_of<decltype(op(*src1.begin(), *src2.begin()))>::type;
        vector_t ret_val(src1.size());
        Transform(src1, src2, op, &ret_val);
        return ret_val;
    }

    template <class C1_, class C2_> void Append(C1_* c1, const C2_& c2) { c1->insert(c1->end(), c2.begin(), c2.end()); }

    template <class E_, class C_> void Append(Vector_<E_>* c1, const C_& c2) { c1->Append(c2); }

    template <class C1_, class C2_> C1_ Concatenate(const C1_& c1, const C2_& c2) {
        C1_ ret_val(c1);
        Append(&ret_val, c2);
        return ret_val;
    }

    template <class C_, class E_> void Fill(C_* range, const E_& val) { std::fill(range->begin(), range->end(), val); }

    template <class C_> Vector_<typename C_::value_type> Copy(const C_& src) {
        using vector_t = Vector_<typename C_::value_type>;
        vector_t ret_val(src.size());
        std::copy(src.begin(), src.end(), ret_val.begin());
        return ret_val;
    }

    template <class C_> Vector_<typename C_::value_type> Reverse(const C_& src) {
        using vector_t = Vector_<typename C_::value_type>;
        vector_t ret_val(src);
        std::reverse(ret_val.begin(), ret_val.end());
        return ret_val;
    }

    template <class C_, class P_> C_ Filter(const C_& src, const P_& pred) {
        using element_t = decltype(*src.begin());
        C_ ret_val(src);
        auto stop = std::remove_if(ret_val.begin(), ret_val.end(), [&](const element_t& e) { return !pred(e); });
        ret_val.erase(stop, ret_val.end());
        return ret_val;
    }

    template <class C_, class LT_> C_ Unique(const C_& src, const LT_& less) {
        C_ ret_val(src);
        std::sort(ret_val.begin(), ret_val.end(), less);
        ret_val.erase(std::unique(ret_val.begin(), ret_val.end()), ret_val.end());
        return ret_val;
    }

    template <class C_> C_ Unique(const C_& src) { return Unique(src, std::less<typename C_::value_type>()); }

    template <class C_> typename C_::const_iterator LowerBound(const C_& src, const typename C_::value_type& x) {
        return std::lower_bound(src.begin(), src.end(), x);
    }

    template <class C_> typename C_::const_iterator UpperBound(const C_& src, const typename C_::value_type& x) {
        return std::upper_bound(src.begin(), src.end(), x);
    }

    template <class C_, class E_> bool BinarySearch(const C_& c, const E_& val) {
        return std::binary_search(c.begin(), c.end(), val);
    }

    template <typename T_> T_ Next(const T_& src) {
        T_ ret_val(src);
        return ++ret_val;
    }

    template <class T_> T_ Previous(const T_& src) {
        T_ ret_val(src);
        return --ret_val;
    }

    template <class C_> typename C_::const_iterator MaxElement(const C_& src) {
        return std::max_element(src.begin(), src.end());
    }

    template <class C_> typename C_::const_iterator MinElement(const C_& src) {
        return std::min_element(src.begin(), src.end());
    }

    template <class C_, class L_> bool IsMonotonic(const C_& c, const L_& pred) {
        if (c.size() < 2)
            return true;
        auto pvs = c.begin();
        for (auto nxt = Next(pvs); nxt != c.end(); pvs = nxt++) {
            if (!pred(*pvs, *nxt))
                return false;
        }
        return true;
    }

    template <class C_> bool IsMonotonic(const C_& c) { return IsMonotonic(c, std::less<typename C_::value_type>()); }

    template <class C_> auto Keys(const C_& src) {
        using vector_t = typename vector_of<typename C_::key_type>::type;
        using value_type = typename C_::value_type;
        auto op = [](const value_type& kv) { return kv.first; };
        vector_t ret_val(src.size());
        Transform(src, op, &ret_val);
        return ret_val;
    }

    template <class C_> auto MapValues(const C_& src) {
        using vector_t = typename vector_of<typename C_::mapped_type>::type;
        using value_type = typename C_::value_type;
        auto op = [](const value_type& kv) { return kv.second; };
        vector_t ret_val(src.size());
        Transform(src, op, &ret_val);
        return ret_val;
    }

    template <class C_, class P_> bool AllOf(const C_& val, const P_& pred) {
        return std::all_of(val.begin(), val.end(), pred);
    }

} // namespace Dal