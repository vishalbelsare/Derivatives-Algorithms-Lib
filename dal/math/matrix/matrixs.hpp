//
// Created by Cheng Li on 2018/1/13.
//

#pragma once

#include <dal/math/vectors.hpp>
#include <dal/utilities/algorithms.hpp>

namespace Dal {
    template <class E_> class Matrix_ {
    public:
        using I_ = typename Vector_<E_>::iterator;
        using CI_ = typename Vector_<E_>::const_iterator;
        using R_ = typename Vector_<E_>::reference;
        using CR_ = typename Vector_<E_>::const_reference;

    private:
        Vector_<E_> vals_;
        int cols_;
        Vector_<I_> hooks_;
        void SetHook(size_t from = 0);

    public:
        virtual ~Matrix_() = default;
        Matrix_() : cols_(0) {}
        Matrix_(int rows, int cols, E_ val = E_())
            : vals_(static_cast<size_t>((rows + 1) * cols)), cols_(cols), hooks_(static_cast<size_t>(rows)) {
            SetHook();
            vals_.Fill(val);
        }
        Matrix_(const Matrix_& src) : vals_(src.vals_), cols_(src.cols_), hooks_(src.hooks_.size()) { SetHook(); }

        int Rows() const { return static_cast<int>(hooks_.size()); }
        int Cols() const { return cols_; }
        bool Empty() const { return (vals_.size() - cols_) == 0; }
        void Clear() {
            vals_.clear();
            cols_ = 0;
            hooks_.clear();
        }
        CI_ First() const { return vals_.begin(); }
        inline CI_ begin() const { return First(); }
        CI_ Last() const { return vals_.end() - cols_; }
        inline CI_ end() const { return Last(); }

        CR_ operator()(int row, int col) const { return hooks_[row][col]; }
        R_ operator()(int row, int col) { return hooks_[row][col]; }

        // move operators
        void swap(Matrix_& rhs) noexcept {
            std::swap(vals_, rhs.vals_);
            std::swap(cols_, rhs.cols_);
            std::swap(hooks_, rhs.hooks_);
        }

        Matrix_(Matrix_&& rhs) noexcept { swap(rhs); }

        Matrix_& operator=(Matrix_&& rhs) noexcept {
            if (this != &rhs) {
                Matrix_<E_> temp(std::move(rhs));
                swap(temp);
            }
            return *this;
        }

        Matrix_& operator=(const Matrix_& rhs) noexcept {
            if (this != &rhs) {
                vals_ = rhs.vals_;
                cols_ = rhs.cols_;
                hooks_ = Vector_<I_>(rhs.hooks_.size());
                SetHook();
            }
            return *this;
        }

        // slices -- ephemeral containers of rows or columns
        class ConstRow_ {
        protected:
            I_ begin_;
            I_ end_;

        public:
            using value_type = E_;
            using const_iterator = typename Vector_<E_>::const_iterator;

            ConstRow_(I_ begin, I_ end) : begin_(begin), end_(end) {}
            ConstRow_(I_ begin, int size) : begin_(begin), end_(begin + size) {}

            const_iterator begin() const { return begin_; }
            const_iterator end() const { return end_; }
            [[nodiscard]] int size() const { return static_cast<int>(end_ - begin_); }
            const E_& operator[](int col) const { return *(begin_ + col); }
            const E_& front() const { return *begin_; }
            const E_& back() const { return *(end_ - 1); }
            operator Vector_<E_>() const {
                return Vector_<E_>(begin(), end());
            }
        };

        ConstRow_ Row(int i_row) const { return ConstRow_(hooks_[i_row], cols_); }
        ConstRow_ operator[](int i_row) const { return Row(i_row); }

        struct Row_ : ConstRow_ {
            using iterator = I_;
            using const_iterator = typename ConstRow_::const_iterator;
            Row_(I_ begin, I_ end) : ConstRow_(begin, end) {}
            Row_(I_ begin, int size) : ConstRow_(begin, size) {}

            // have to double-implement begin/end, otherwise non-const implementations hide the inherited const
            iterator begin() { return ConstRow_::begin_; }
            const_iterator begin() const { return ConstRow_::begin_; }
            iterator end() { return ConstRow_::end_; }
            const_iterator end() const { return ConstRow_::end_; }
            E_& operator[](int col) { return *(ConstRow_::begin_ + col); }
            const E_& operator[](int col) const { return *(ConstRow_::begin_ + col); }
        };

        Row_ Row(int i_row) { return Row_(hooks_[i_row], cols_); }
        Row_ operator[](int i_row) { return Row(i_row); }

        // Iteration through columns is less efficient
        class ConstCol_ {
        public:
            template <typename RI_>
            struct Iterator_ // column iterator in terms of row iterator
            {
                RI_ val_;
                size_t stride_;
                Iterator_(RI_ val, size_t stride) : val_(val), stride_(stride) {}
                Iterator_& operator++() {
                    val_ += stride_;
                    return *this;
                }
                Iterator_ operator++(int) {
                    Iterator_ ret(*this);
                    val_ += stride_;
                    return ret;
                }
                Iterator_& operator--() {
                    val_ -= stride_;
                    return *this;
                }
                Iterator_ operator--(int) {
                    Iterator_ ret(*this);
                    val_ -= stride_;
                    return ret;
                }
                Iterator_ operator+(size_t inc) {
                    Iterator_ ret(*this);
                    ret.val_ += inc * stride_;
                    return ret;
                }
                typename RI_::reference operator*() { return *val_; }
                bool operator==(const Iterator_& rhs) const {
                    REQUIRE(stride_ == rhs.stride_, "lhs stride size should be same with rhs");
                    return val_ == rhs.val_;
                }
                bool operator!=(const Iterator_& rhs) const { return !this->operator==(rhs); }
                bool operator<(const Iterator_& rhs) const { return val_ < rhs.val_; }
                typename RI_::difference_type operator-(const Iterator_& rhs) const {
                    REQUIRE(stride_ == rhs.stride_, "lhs stride size should be same with rhs");
                    REQUIRE((val_ - rhs.val_) % stride_ == 0, "lhs and rhs should be in same column");
                    return (val_ - rhs.val_) / stride_;
                }
                using iterator_category = typename std::vector<E_>::iterator::iterator_category;
                using difference_type = typename std::vector<E_>::iterator::difference_type;
                using value_type = E_;
                using reference = const E_&;
                using pointer = const E_*;
            };
            using iterator = Iterator_<typename Vector_<E_>::iterator>;

            operator Vector_<E_>() const {
                return Vector_<E_>(begin(), end());
            }

        protected:
            iterator begin_; // non-const to support Column_, below
            size_t size_;

        public:
            using value_type = E_;
            using const_iterator = Iterator_<typename Vector_<E_>::const_iterator>;
            ConstCol_(I_ begin, size_t size, size_t stride) : begin_(begin, stride), size_(size) {}

            const_iterator begin() const { return const_iterator(begin_.val_, begin_.stride_); }
            const_iterator end() const { return const_iterator(begin_.val_ + size_ * begin_.stride_, begin_.stride_); }
            [[nodiscard]] size_t size() const { return size_; }
            const E_& operator[](int row) const { return *(begin_.val_ + row * begin_.stride_); }
        };
        ConstCol_ Col(int i_col) const { return ConstCol_(hooks_[0] + i_col, hooks_.size(), cols_); }

        class Col_ : public ConstCol_ {
            using iterator = typename ConstCol_::iterator;
            using ConstCol_::begin_;
            using ConstCol_::size_;

        public:
            using value_type = E_;
            Col_(I_ begin, size_t size, size_t stride) : ConstCol_(begin, size, stride) {}

            iterator begin() const { return begin_; }
            iterator end() const { return iterator(begin_.val_ + size_ * begin_.stride_, begin_.stride_); }
            E_& operator[](int row) { return *(begin_.val_ + row * begin_.stride_); }

            using ConstCol_::size;
        };
        Col_ Col(int i_col) { return Col_(hooks_[0] + i_col, hooks_.size(), cols_); }

        // POSTPONED -- sub-matrix
        void Swap(Matrix_<E_>* other) {
            REQUIRE(other != nullptr, "can't swap with null");
            vals_.Swap(&other->vals_);
            hooks_.Swap(&other->hooks_);
            std::swap(cols_, other->cols_);
            REQUIRE(hooks_.front() == vals_.begin(), "hooks front should be same with vals begin");
        }

        void Fill(const E_& val) { vals_.Fill(val); }

        template <class T_> void operator*=(const T_& scale) { vals_ *= scale; }

        void Resize(int rows, int cols) {
            const auto old_rows = hooks_.size();
            if (cols == cols_ && rows * old_rows > 0) {
                vals_.Resize((rows + 1) * cols);
                hooks_.Resize(rows);
                SetHook(hooks_[0] == vals_.begin() ? old_rows : 0);
            } else {
                const auto n_copy = std::min(cols, cols_);
                cols_ = cols;
                Vector_<E_> new_vals((rows + 1) * cols);
                for (int ir = 0; ir < rows && ir < old_rows; ++ir) {
                    copy(hooks_[ir], hooks_[ir] + n_copy, new_vals.begin() + ir * cols);
                }
                vals_.Swap(&new_vals);
                hooks_.Resize(rows);
                SetHook();
            }
        }
    };

    template <class E_> void Matrix_<E_>::SetHook(size_t from) {
        for (auto ii = from; ii < hooks_.size(); ++ii)
            hooks_[ii] = vals_.begin() + ii * cols_;
    }
} // namespace Dal
