// ---------------------  //
//  Matrix Product Class  //
// ---------------------  //

template
<
typename T1,
typename T2
>
auto MatrixProduct<T1, T2>::multiply(const VectorType &u) const
    -> VectorType
{
    VectorType v = B.multiply(u);
    VectorType w = A.multiply(v);
    return w;
}

template
<
typename T1,
typename T2
>
auto MatrixProduct<T1, T2>::multiply(const MatrixType &C) const
    -> MatrixType
{
    VectorType D = B.multiply(C);
    VectorType E = A.multiply(D);
    return E;
}

template
<
typename T1,
typename T2
>
auto MatrixProduct<T1, T2>::invert() const
    -> MatrixType
{
    MatrixType D = this->operator ResultType();
    MatrixType E = D.invert();
    return E;
}

template
<
typename T1,
typename T2
>
auto MatrixProduct<T1, T2>::solve(const VectorType &u) const
    -> VectorType
{
    VectorType v = B.solve(u);
    VectorType w = A.solve(v);
    return w;
}

template
<
typename T1,
typename T2
>
auto MatrixProduct<T1, T2>::transpose() const
    -> MatrixType
{
    MatrixType C = A.multiply((MatrixType) B);
    MatrixType D = C.transpose();
    return D;
}

template
<
typename T1,
typename T2
>
MatrixProduct<T1, T2>::operator ResultType() const
{
    ResultType C = B;
    ResultType D = A.multiply(C);
    return D;
}

// template
// <
// typename T1,
// typename T2
// >
// MatrixProduct<T1, T2>::operator VectorType() const
// {
//     VectorType C = B;
//     VectorType D = A.multiply(C);
//     return D;
// }
