#ifndef BOOST_TEST_MODULE
#define BOOST_TEST_MODULE "Forward Models, Linear"
#endif

#include <boost/test/included/unit_test.hpp>
#include <iostream>

#include "invlib/algebra.h"
#include "invlib/algebra/solvers.h"
#include "invlib/map.h"
#include "invlib/optimization.h"

#include "forward_models/linear.h"
#include "utility.h"
#include "test_types.h"

using namespace invlib;

// Use a random linear forward model to test the equivalence of the
// standard, n-form and m-form when using the Gauss-Newton optimizer
// and the standard form using Levenberg-Marquardt and Gauss-Newton
// optimization.
template
<
typename T
>
void linear_test(unsigned int n)
{
    using RealType   = typename T::RealType;
    using VectorType = typename T::VectorType;
    using MatrixType = typename T::MatrixType;
    using Id     = invlib::MatrixIdentity<MatrixType>;
    using Model  = invlib::Linear<MatrixType>;

    MatrixType Se = random_positive_definite<MatrixType>(n);
    MatrixType Sa = random_positive_definite<MatrixType>(n);
    VectorType xa = random<VectorType>(n);
    VectorType y  = random<VectorType>(n);

    Model F(n,n);
    MAP<Model, MatrixType, MatrixType, MatrixType, Formulation::STANDARD>
        std(F, xa, Sa, Se);
    MAP<Model, MatrixType, MatrixType, MatrixType, Formulation::NFORM>
        nform(F, xa, Sa, Se);
    MAP<Model, MatrixType, MatrixType, MatrixType, Formulation::MFORM>
        mform(F, xa, Sa, Se);

    // Test inversion using standard solver.
    Id I{};
    GaussNewton<RealType> GN{};
    GN.tolerance() = 1e-9; GN.maximum_iterations() = 1000;
    LevenbergMarquardt<RealType, Id> LM(I);
    LM.tolerance() = 1e-9; LM.maximum_iterations() = 1000;

    VectorType x_std_lm, x_std_gn, x_n_gn, x_m_gn;
    std.compute(x_std_lm, y, LM);
    std.compute(x_std_gn, y, GN);
    nform.compute(x_n_gn, y, GN);
    mform.compute(x_m_gn, y, GN);

    RealType e1, e2, e3;
    e1 = maximum_error(x_std_lm, x_std_gn);
    e2 = maximum_error(x_std_gn, x_n_gn);
    e3 = maximum_error(x_std_gn, x_m_gn);

    BOOST_TEST((e1 < EPS), "Error STD - NFORM = " << e1);
    BOOST_TEST((e2 < EPS), "Error STD - MFORM = " << e2);
    BOOST_TEST((e3 < EPS), "Error STD - MFORM = " << e3);

    // Test inversion using CG solver.
    ConjugateGradient cg(1e-5);
    GaussNewton<RealType, ConjugateGradient> GN_CG(cg);
    GN_CG.tolerance() = 1e-9; GN.maximum_iterations() = 1000;
    LevenbergMarquardt<RealType, Id, ConjugateGradient> LM_CG(I, cg);
    LM_CG.tolerance() = 1e-9; LM.maximum_iterations() = 1000;

    std.compute(x_std_lm, y, LM_CG);
    std.compute(x_std_gn, y, GN_CG);
    nform.compute(x_n_gn, y, GN_CG);
    mform.compute(x_m_gn, y, GN_CG);

    BOOST_TEST((e1 < EPS), "Error STD - NFORM CG = " << e1);
    BOOST_TEST((e2 < EPS), "Error STD - MFORM CG = " << e2);
    BOOST_TEST((e3 < EPS), "Error STD - MFORM CG = " << e3);
}

// Same test as above but applying the NormalizingDiagonal transform.
template
<
typename T
>
void linear_test_transformed(unsigned int n)
{
    using RealType   = typename T::RealType;
    using VectorType = typename T::VectorType;
    using MatrixType = typename T::MatrixType;
    using Id     = invlib::MatrixIdentity<MatrixType>;
    using Model  = invlib::Linear<MatrixType>;

    MatrixType Se = random_positive_definite<MatrixType>(n);
    MatrixType Sa = random_positive_definite<MatrixType>(n);
    VectorType xa = random<VectorType>(n);
    VectorType y  = random<VectorType>(n);

    Model F(n,n);
    MAP<Model, MatrixType, MatrixType, MatrixType, Formulation::STANDARD>
        std(F, xa, Sa, Se);
    MAP<Model, MatrixType, MatrixType, MatrixType, Formulation::NFORM>
        nform(F, xa, Sa, Se);
    MAP<Model, MatrixType, MatrixType, MatrixType, Formulation::MFORM>
        mform(F, xa, Sa, Se);
    NormalizeDiagonal<MatrixType> trans(Sa);

    // Test inversion using standard solver.
    Id I{};
    GaussNewton<RealType> GN{};
    GN.tolerance() = 1e-9; GN.maximum_iterations() = 1000;
    LevenbergMarquardt<RealType, Id> LM(I);
    LM.tolerance() = 1e-9; LM.maximum_iterations() = 1000;

    VectorType x_std_lm, x_std_gn, x_n_gn, x_m_gn;
    std.compute(x_std_lm, y, LM);
    std.compute(x_std_gn, y, GN);
    nform.compute(x_n_gn, y, GN);
    mform.compute(x_m_gn, y, GN);

    RealType e1, e2, e3;
    e1 = maximum_error(x_std_lm, x_std_gn);
    e2 = maximum_error(x_std_gn, x_n_gn);
    e3 = maximum_error(x_std_gn, x_m_gn);

    BOOST_TEST((e1 < EPS), "Error STD - NFORM = " << e1);
    BOOST_TEST((e2 < EPS), "Error STD - MFORM = " << e2);
    BOOST_TEST((e3 < EPS), "Error STD - MFORM = " << e3);

    // Test inversion using CG solver.
    ConjugateGradient cg(1e-5);
    GaussNewton<RealType, ConjugateGradient> GN_CG(cg);
    GN_CG.tolerance() = 1e-9; GN.maximum_iterations() = 1000;
    LevenbergMarquardt<RealType, Id, ConjugateGradient> LM_CG(I, cg);
    LM_CG.tolerance() = 1e-9; LM.maximum_iterations() = 1000;

    std.compute(x_std_lm, y, LM_CG);
    std.compute(x_std_gn, y, GN_CG);
    nform.compute(x_n_gn, y, GN_CG);
    mform.compute(x_m_gn, y, GN_CG);

    BOOST_TEST((e1 < EPS), "Error STD - NFORM CG = " << e1);
    BOOST_TEST((e2 < EPS), "Error STD - MFORM CG = " << e2);
    BOOST_TEST((e3 < EPS), "Error STD - MFORM CG = " << e3);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(linear, T, matrix_types)
{
    srand(time(NULL));
    for (unsigned int i = 0; i < ntests; i++)
    {
        unsigned int n = 1 + rand() % 20;
        linear_test<T>(10);
    }
}

BOOST_AUTO_TEST_CASE_TEMPLATE(linear_transformed, T, matrix_types)
{
    srand(time(NULL));
    for (unsigned int i = 0; i < ntests; i++)
    {
        unsigned int n = 1 + rand() % 20;
        linear_test_transformed<T>(10);
    }
}
