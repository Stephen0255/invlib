template<typename T1>
MatrixInverse<T1>::MatrixInverse(const T1 &A_)
    : A(A_)
{
    // Nothing to do here.
}

template<typename T1>
auto MatrixInverse<T1>::multiply(const VectorType &v) const
    -> VectorType
{
    VectorType w = A.solve(v);
    return w;
}

template<typename T1>
auto MatrixInverse<T1>::multiply(const MatrixType &B) const
    -> MatrixType
{
    MatrixType C = A.invert();
    return C.multiply(B);
}

template<typename T1>
auto MatrixInverse<T1>::solve(const VectorType &v) const
    -> VectorType
{
    MatrixType B = A;
    return B.multiply(v);
}

template<typename T1>
auto MatrixInverse<T1>::invert() const
    -> MatrixType
{
    MatrixType B = A;
    return B;
}

template<typename T1>
    template <typename T2>
auto MatrixInverse<T1>::operator*(T2 &&B) const
    -> Product<T2>
{
    return Product<T2>(*this, B);
}

template<typename T1>
    template <typename T2>
auto MatrixInverse<T1>::operator-(T2 &&B) const
    -> Difference<T2>
{
    return Difference<T2>(*this, B);
}

template<typename T1>
    template <typename T2>
auto MatrixInverse<T1>::operator+(T2 &&B) const
    -> Sum<T2>
{
    return Sum<T2>(*this, B);
}

template<typename T1>
MatrixInverse<T1>::operator ResultType() const
{
    MatrixType B = A.invert();
    return B;
}

template <typename T2>
MatrixInverse<T2> inv(T2 &&A)
{
    return MatrixInverse<T2>(A);
}
