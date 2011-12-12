/*M///////////////////////////////////////////////////////////////////////////////////////
//
//  IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.
//
//  By downloading, copying, installing or using the software you agree to this license.
//  If you do not agree to this license, do not download, install,
//  copy or use the software.
//
//
//                           License Agreement
//                For Open Source Computer Vision Library
//
// Copyright (C) 2000-2008, Intel Corporation, all rights reserved.
// Copyright (C) 2009, Willow Garage Inc., all rights reserved.
// Third party copyrights are property of their respective owners.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
//   * Redistribution's of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//
//   * Redistribution's in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//
//   * The name of the copyright holders may not be used to endorse or promote products
//     derived from this software without specific prior written permission.
//
// This software is provided by the copyright holders and contributors "as is" and
// any express or implied warranties, including, but not limited to, the implied
// warranties of merchantability and fitness for a particular purpose are disclaimed.
// In no event shall the Intel Corporation or contributors be liable for any direct,
// indirect, incidental, special, exemplary, or consequential damages
// (including, but not limited to, procurement of substitute goods or services;
// loss of use, data, or profits; or business interruption) however caused
// and on any theory of liability, whether in contract, strict liability,
// or tort (including negligence or otherwise) arising in any way out of
// the use of this software, even if advised of the possibility of such damage.
//
//M*/

#include "_cv.h"

/*
    This is stright-forward port v3 of Matlab calibration engine by Jean-Yves Bouguet
    that is (in a large extent) based on the paper:
    Z. Zhang. "A flexible new technique for camera calibration".
    IEEE Transactions on Pattern Analysis and Machine Intelligence, 22(11):1330-1334, 2000.

    The 1st initial port was done by Valery Mosyagin.
*/

using namespace cv;

CvLevMarq::CvLevMarq()
{
    mask = prevParam = param = J = err = JtJ = JtJN = JtErr = JtJV = JtJW = Ptr<CvMat>();
    lambdaLg10 = 0; state = DONE;
    criteria = cvTermCriteria(0,0,0);
    iters = 0;
    completeSymmFlag = false;
}

CvLevMarq::CvLevMarq( int nparams, int nerrs, CvTermCriteria criteria0, bool _completeSymmFlag )
{
    mask = prevParam = param = J = err = JtJ = JtJN = JtErr = JtJV = JtJW = Ptr<CvMat>();
    init(nparams, nerrs, criteria0, _completeSymmFlag);
}

void CvLevMarq::clear()
{
    mask.release();
    prevParam.release();
    param.release();
    J.release();
    err.release();
    JtJ.release();
    JtJN.release();
    JtErr.release();
    JtJV.release();
    JtJW.release();
}

CvLevMarq::~CvLevMarq()
{
    clear();
}

void CvLevMarq::init( int nparams, int nerrs, CvTermCriteria criteria0, bool _completeSymmFlag )
{
    if( !param || param->rows != nparams || nerrs != (err ? err->rows : 0) )
        clear();
    mask = cvCreateMat( nparams, 1, CV_8U );
    cvSet(mask, cvScalarAll(1));
    prevParam = cvCreateMat( nparams, 1, CV_64F );
    param = cvCreateMat( nparams, 1, CV_64F );
    JtJ = cvCreateMat( nparams, nparams, CV_64F );
    JtJN = cvCreateMat( nparams, nparams, CV_64F );
    JtJV = cvCreateMat( nparams, nparams, CV_64F );
    JtJW = cvCreateMat( nparams, 1, CV_64F );
    JtErr = cvCreateMat( nparams, 1, CV_64F );
    if( nerrs > 0 )
    {
        J = cvCreateMat( nerrs, nparams, CV_64F );
        err = cvCreateMat( nerrs, 1, CV_64F );
    }
    prevErrNorm = DBL_MAX;
    lambdaLg10 = -3;
    criteria = criteria0;
    if( criteria.type & CV_TERMCRIT_ITER )
        criteria.max_iter = MIN(MAX(criteria.max_iter,1),1000);
    else
        criteria.max_iter = 30;
    if( criteria.type & CV_TERMCRIT_EPS )
        criteria.epsilon = MAX(criteria.epsilon, 0);
    else
        criteria.epsilon = DBL_EPSILON;
    state = STARTED;
    iters = 0;
    completeSymmFlag = _completeSymmFlag;
}

bool CvLevMarq::update( const CvMat*& _param, CvMat*& matJ, CvMat*& _err )
{
    double change;

    matJ = _err = 0;

    assert( !err.empty() );
    if( state == DONE )
    {
        _param = param;
        return false;
    }

    if( state == STARTED )
    {
        _param = param;
        cvZero( J );
        cvZero( err );
        matJ = J;
        _err = err;
        state = CALC_J;
        return true;
    }

    if( state == CALC_J )
    {
        cvMulTransposed( J, JtJ, 1 );
        cvGEMM( J, err, 1, 0, 0, JtErr, CV_GEMM_A_T );
        cvCopy( param, prevParam );
        step();
        if( iters == 0 )
            prevErrNorm = cvNorm(err, 0, CV_L2);
        _param = param;
        cvZero( err );
        _err = err;
        state = CHECK_ERR;
        return true;
    }

    assert( state == CHECK_ERR );
    errNorm = cvNorm( err, 0, CV_L2 );
    if( errNorm > prevErrNorm )
    {
        lambdaLg10++;
        step();
        _param = param;
        cvZero( err );
        _err = err;
        state = CHECK_ERR;
        return true;
    }

    lambdaLg10 = MAX(lambdaLg10-1, -16);
    if( ++iters >= criteria.max_iter ||
        (change = cvNorm(param, prevParam, CV_RELATIVE_L2)) < criteria.epsilon )
    {
        _param = param;
        state = DONE;
        return true;
    }

    prevErrNorm = errNorm;
    _param = param;
    cvZero(J);
    matJ = J;
    _err = err;
    state = CALC_J;
    return true;
}


bool CvLevMarq::updateAlt( const CvMat*& _param, CvMat*& _JtJ, CvMat*& _JtErr, double*& _errNorm )
{
    double change;

    CV_Assert( err.empty() );
    if( state == DONE )
    {
        _param = param;
        return false;
    }

    if( state == STARTED )
    {
        _param = param;
        cvZero( JtJ );
        cvZero( JtErr );
        errNorm = 0;
        _JtJ = JtJ;
        _JtErr = JtErr;
        _errNorm = &errNorm;
        state = CALC_J;
        return true;
    }

    if( state == CALC_J )
    {
        cvCopy( param, prevParam );
        step();
        _param = param;
        prevErrNorm = errNorm;
        errNorm = 0;
        _errNorm = &errNorm;
        state = CHECK_ERR;
        return true;
    }

    assert( state == CHECK_ERR );
    if( errNorm > prevErrNorm )
    {
        lambdaLg10++;
        step();
        _param = param;
        errNorm = 0;
        _errNorm = &errNorm;
        state = CHECK_ERR;
        return true;
    }

    lambdaLg10 = MAX(lambdaLg10-1, -16);
    if( ++iters >= criteria.max_iter ||
        (change = cvNorm(param, prevParam, CV_RELATIVE_L2)) < criteria.epsilon )
    {
        _param = param;
        state = DONE;
        return false;
    }

    prevErrNorm = errNorm;
    cvZero( JtJ );
    cvZero( JtErr );
    _param = param;
    _JtJ = JtJ;
    _JtErr = JtErr;
    state = CALC_J;
    return true;
}

void CvLevMarq::step()
{
    const double LOG10 = log(10.);
    double lambda = exp(lambdaLg10*LOG10);
    int i, j, nparams = param->rows;

    for( i = 0; i < nparams; i++ )
        if( mask->data.ptr[i] == 0 )
        {
            double *row = JtJ->data.db + i*nparams, *col = JtJ->data.db + i;
            for( j = 0; j < nparams; j++ )
                row[j] = col[j*nparams] = 0;
            JtErr->data.db[i] = 0;
        }

    if( !err )
        cvCompleteSymm( JtJ, completeSymmFlag );
#if 1
    cvCopy( JtJ, JtJN );
    for( i = 0; i < nparams; i++ )
        JtJN->data.db[(nparams+1)*i] *= 1. + lambda;
#else
    cvSetIdentity(JtJN, cvRealScalar(lambda));
    cvAdd( JtJ, JtJN, JtJN );
#endif
    cvSVD( JtJN, JtJW, 0, JtJV, CV_SVD_MODIFY_A + CV_SVD_U_T + CV_SVD_V_T );
    cvSVBkSb( JtJW, JtJV, JtJV, JtErr, param, CV_SVD_U_T + CV_SVD_V_T );
    for( i = 0; i < nparams; i++ )
        param->data.db[i] = prevParam->data.db[i] - (mask->data.ptr[i] ? param->data.db[i] : 0);
}

// reimplementation of dAB.m
CV_IMPL void cvCalcMatMulDeriv( const CvMat* A, const CvMat* B, CvMat* dABdA, CvMat* dABdB )
{
    int i, j, M, N, L;
    int bstep;

    CV_Assert( CV_IS_MAT(A) && CV_IS_MAT(B) );
    CV_Assert( CV_ARE_TYPES_EQ(A, B) &&
        (CV_MAT_TYPE(A->type) == CV_32F || CV_MAT_TYPE(A->type) == CV_64F) );
    CV_Assert( A->cols == B->rows );

    M = A->rows;
    L = A->cols;
    N = B->cols;
    bstep = B->step/CV_ELEM_SIZE(B->type);

    if( dABdA )
    {
        CV_Assert( CV_ARE_TYPES_EQ(A, dABdA) &&
            dABdA->rows == A->rows*B->cols && dABdA->cols == A->rows*A->cols );
    }

    if( dABdB )
    {
        CV_Assert( CV_ARE_TYPES_EQ(A, dABdB) &&
            dABdB->rows == A->rows*B->cols && dABdB->cols == B->rows*B->cols );
    }

    if( CV_MAT_TYPE(A->type) == CV_32F )
    {
        for( i = 0; i < M*N; i++ )
        {
            int i1 = i / N,  i2 = i % N;

            if( dABdA )
            {
                float* dcda = (float*)(dABdA->data.ptr + dABdA->step*i);
                const float* b = (const float*)B->data.ptr + i2;

                for( j = 0; j < M*L; j++ )
                    dcda[j] = 0;
                for( j = 0; j < L; j++ )
                    dcda[i1*L + j] = b[j*bstep];
            }

            if( dABdB )
            {
                float* dcdb = (float*)(dABdB->data.ptr + dABdB->step*i);
                const float* a = (const float*)(A->data.ptr + A->step*i1);

                for( j = 0; j < L*N; j++ )
                    dcdb[j] = 0;
                for( j = 0; j < L; j++ )
                    dcdb[j*N + i2] = a[j];
            }
        }
    }
    else
    {
        for( i = 0; i < M*N; i++ )
        {
            int i1 = i / N,  i2 = i % N;

            if( dABdA )
            {
                double* dcda = (double*)(dABdA->data.ptr + dABdA->step*i);
                const double* b = (const double*)B->data.ptr + i2;

                for( j = 0; j < M*L; j++ )
                    dcda[j] = 0;
                for( j = 0; j < L; j++ )
                    dcda[i1*L + j] = b[j*bstep];
            }

            if( dABdB )
            {
                double* dcdb = (double*)(dABdB->data.ptr + dABdB->step*i);
                const double* a = (const double*)(A->data.ptr + A->step*i1);

                for( j = 0; j < L*N; j++ )
                    dcdb[j] = 0;
                for( j = 0; j < L; j++ )
                    dcdb[j*N + i2] = a[j];
            }
        }
    }
}

// reimplementation of compose_motion.m
CV_IMPL void cvComposeRT( const CvMat* _rvec1, const CvMat* _tvec1,
             const CvMat* _rvec2, const CvMat* _tvec2,
             CvMat* _rvec3, CvMat* _tvec3,
             CvMat* dr3dr1, CvMat* dr3dt1,
             CvMat* dr3dr2, CvMat* dr3dt2,
             CvMat* dt3dr1, CvMat* dt3dt1,
             CvMat* dt3dr2, CvMat* dt3dt2 )
{
    double _r1[3], _r2[3];
    double _R1[9], _d1[9*3], _R2[9], _d2[9*3];
    CvMat r1 = cvMat(3,1,CV_64F,_r1), r2 = cvMat(3,1,CV_64F,_r2);
    CvMat R1 = cvMat(3,3,CV_64F,_R1), R2 = cvMat(3,3,CV_64F,_R2);
    CvMat dR1dr1 = cvMat(9,3,CV_64F,_d1), dR2dr2 = cvMat(9,3,CV_64F,_d2);

    CV_Assert( CV_IS_MAT(_rvec1) && CV_IS_MAT(_rvec2) );

    CV_Assert( CV_MAT_TYPE(_rvec1->type) == CV_32F ||
               CV_MAT_TYPE(_rvec1->type) == CV_64F );

    CV_Assert( _rvec1->rows == 3 && _rvec1->cols == 1 && CV_ARE_SIZES_EQ(_rvec1, _rvec2) );

    cvConvert( _rvec1, &r1 );
    cvConvert( _rvec2, &r2 );

    cvRodrigues2( &r1, &R1, &dR1dr1 );
    cvRodrigues2( &r2, &R2, &dR2dr2 );

    if( _rvec3 || dr3dr1 || dr3dr1 )
    {
        double _r3[3], _R3[9], _dR3dR1[9*9], _dR3dR2[9*9], _dr3dR3[9*3];
        double _W1[9*3], _W2[3*3];
        CvMat r3 = cvMat(3,1,CV_64F,_r3), R3 = cvMat(3,3,CV_64F,_R3);
        CvMat dR3dR1 = cvMat(9,9,CV_64F,_dR3dR1), dR3dR2 = cvMat(9,9,CV_64F,_dR3dR2);
        CvMat dr3dR3 = cvMat(3,9,CV_64F,_dr3dR3);
        CvMat W1 = cvMat(3,9,CV_64F,_W1), W2 = cvMat(3,3,CV_64F,_W2);

        cvMatMul( &R2, &R1, &R3 );
        cvCalcMatMulDeriv( &R2, &R1, &dR3dR2, &dR3dR1 );

        cvRodrigues2( &R3, &r3, &dr3dR3 );

        if( _rvec3 )
            cvConvert( &r3, _rvec3 );

        if( dr3dr1 )
        {
            cvMatMul( &dr3dR3, &dR3dR1, &W1 );
            cvMatMul( &W1, &dR1dr1, &W2 );
            cvConvert( &W2, dr3dr1 );
        }

        if( dr3dr2 )
        {
            cvMatMul( &dr3dR3, &dR3dR2, &W1 );
            cvMatMul( &W1, &dR2dr2, &W2 );
            cvConvert( &W2, dr3dr2 );
        }
    }

    if( dr3dt1 )
        cvZero( dr3dt1 );
    if( dr3dt2 )
        cvZero( dr3dt2 );

    if( _tvec3 || dt3dr2 || dt3dt1 )
    {
        double _t1[3], _t2[3], _t3[3], _dxdR2[3*9], _dxdt1[3*3], _W3[3*3];
        CvMat t1 = cvMat(3,1,CV_64F,_t1), t2 = cvMat(3,1,CV_64F,_t2);
        CvMat t3 = cvMat(3,1,CV_64F,_t3);
        CvMat dxdR2 = cvMat(3, 9, CV_64F, _dxdR2);
        CvMat dxdt1 = cvMat(3, 3, CV_64F, _dxdt1);
        CvMat W3 = cvMat(3, 3, CV_64F, _W3);

        CV_Assert( CV_IS_MAT(_tvec1) && CV_IS_MAT(_tvec2) );
        CV_Assert( CV_ARE_SIZES_EQ(_tvec1, _tvec2) && CV_ARE_SIZES_EQ(_tvec1, _rvec1) );

        cvConvert( _tvec1, &t1 );
        cvConvert( _tvec2, &t2 );
        cvMatMulAdd( &R2, &t1, &t2, &t3 );

        if( _tvec3 )
            cvConvert( &t3, _tvec3 );

        if( dt3dr2 || dt3dt1 )
        {
            cvCalcMatMulDeriv( &R2, &t1, &dxdR2, &dxdt1 );
            if( dt3dr2 )
            {
                cvMatMul( &dxdR2, &dR2dr2, &W3 );
                cvConvert( &W3, dt3dr2 );
            }
            if( dt3dt1 )
                cvConvert( &dxdt1, dt3dt1 );
        }
    }

    if( dt3dt2 )
        cvSetIdentity( dt3dt2 );
    if( dt3dr1 )
        cvZero( dt3dr1 );
}

CV_IMPL int cvRodrigues2( const CvMat* src, CvMat* dst, CvMat* jacobian )
{
    int depth, elem_size;
    int i, k;
    double J[27];
    CvMat matJ = cvMat( 3, 9, CV_64F, J );

    if( !CV_IS_MAT(src) )
        CV_Error( !src ? CV_StsNullPtr : CV_StsBadArg, "Input argument is not a valid matrix" );

    if( !CV_IS_MAT(dst) )
        CV_Error( !dst ? CV_StsNullPtr : CV_StsBadArg,
        "The first output argument is not a valid matrix" );

    depth = CV_MAT_DEPTH(src->type);
    elem_size = CV_ELEM_SIZE(depth);

    if( depth != CV_32F && depth != CV_64F )
        CV_Error( CV_StsUnsupportedFormat, "The matrices must have 32f or 64f data type" );

    if( !CV_ARE_DEPTHS_EQ(src, dst) )
        CV_Error( CV_StsUnmatchedFormats, "All the matrices must have the same data type" );

    if( jacobian )
    {
        if( !CV_IS_MAT(jacobian) )
            CV_Error( CV_StsBadArg, "Jacobian is not a valid matrix" );

        if( !CV_ARE_DEPTHS_EQ(src, jacobian) || CV_MAT_CN(jacobian->type) != 1 )
            CV_Error( CV_StsUnmatchedFormats, "Jacobian must have 32fC1 or 64fC1 datatype" );

        if( (jacobian->rows != 9 || jacobian->cols != 3) &&
            (jacobian->rows != 3 || jacobian->cols != 9))
            CV_Error( CV_StsBadSize, "Jacobian must be 3x9 or 9x3" );
    }

    if( src->cols == 1 || src->rows == 1 )
    {
        double rx, ry, rz, theta;
        int step = src->rows > 1 ? src->step / elem_size : 1;

        if( src->rows + src->cols*CV_MAT_CN(src->type) - 1 != 3 )
            CV_Error( CV_StsBadSize, "Input matrix must be 1x3, 3x1 or 3x3" );

        if( dst->rows != 3 || dst->cols != 3 || CV_MAT_CN(dst->type) != 1 )
            CV_Error( CV_StsBadSize, "Output matrix must be 3x3, single-channel floating point matrix" );

        if( depth == CV_32F )
        {
            rx = src->data.fl[0];
            ry = src->data.fl[step];
            rz = src->data.fl[step*2];
        }
        else
        {
            rx = src->data.db[0];
            ry = src->data.db[step];
            rz = src->data.db[step*2];
        }
        theta = sqrt(rx*rx + ry*ry + rz*rz);

        if( theta < DBL_EPSILON )
        {
            cvSetIdentity( dst );

            if( jacobian )
            {
                memset( J, 0, sizeof(J) );
                J[5] = J[15] = J[19] = -1;
                J[7] = J[11] = J[21] = 1;
            }
        }
        else
        {
            const double I[] = { 1, 0, 0, 0, 1, 0, 0, 0, 1 };

            double c = cos(theta);
            double s = sin(theta);
            double c1 = 1. - c;
            double itheta = theta ? 1./theta : 0.;

            rx *= itheta; ry *= itheta; rz *= itheta;

            double rrt[] = { rx*rx, rx*ry, rx*rz, rx*ry, ry*ry, ry*rz, rx*rz, ry*rz, rz*rz };
            double _r_x_[] = { 0, -rz, ry, rz, 0, -rx, -ry, rx, 0 };
            double R[9];
            CvMat matR = cvMat( 3, 3, CV_64F, R );

            // R = cos(theta)*I + (1 - cos(theta))*r*rT + sin(theta)*[r_x]
            // where [r_x] is [0 -rz ry; rz 0 -rx; -ry rx 0]
            for( k = 0; k < 9; k++ )
                R[k] = c*I[k] + c1*rrt[k] + s*_r_x_[k];

            cvConvert( &matR, dst );

            if( jacobian )
            {
                double drrt[] = { rx+rx, ry, rz, ry, 0, 0, rz, 0, 0,
                                  0, rx, 0, rx, ry+ry, rz, 0, rz, 0,
                                  0, 0, rx, 0, 0, ry, rx, ry, rz+rz };
                double d_r_x_[] = { 0, 0, 0, 0, 0, -1, 0, 1, 0,
                                    0, 0, 1, 0, 0, 0, -1, 0, 0,
                                    0, -1, 0, 1, 0, 0, 0, 0, 0 };
                for( i = 0; i < 3; i++ )
                {
                    double ri = i == 0 ? rx : i == 1 ? ry : rz;
                    double a0 = -s*ri, a1 = (s - 2*c1*itheta)*ri, a2 = c1*itheta;
                    double a3 = (c - s*itheta)*ri, a4 = s*itheta;
                    for( k = 0; k < 9; k++ )
                        J[i*9+k] = a0*I[k] + a1*rrt[k] + a2*drrt[i*9+k] +
                                   a3*_r_x_[k] + a4*d_r_x_[i*9+k];
                }
            }
        }
    }
    else if( src->cols == 3 && src->rows == 3 )
    {
        double R[9], U[9], V[9], W[3], rx, ry, rz;
        CvMat matR = cvMat( 3, 3, CV_64F, R );
        CvMat matU = cvMat( 3, 3, CV_64F, U );
        CvMat matV = cvMat( 3, 3, CV_64F, V );
        CvMat matW = cvMat( 3, 1, CV_64F, W );
        double theta, s, c;
        int step = dst->rows > 1 ? dst->step / elem_size : 1;

        if( (dst->rows != 1 || dst->cols*CV_MAT_CN(dst->type) != 3) &&
            (dst->rows != 3 || dst->cols != 1 || CV_MAT_CN(dst->type) != 1))
            CV_Error( CV_StsBadSize, "Output matrix must be 1x3 or 3x1" );

        cvConvert( src, &matR );
        if( !cvCheckArr( &matR, CV_CHECK_RANGE+CV_CHECK_QUIET, -100, 100 ) )
        {
            cvZero(dst);
            if( jacobian )
                cvZero(jacobian);
            return 0;
        }

        cvSVD( &matR, &matW, &matU, &matV, CV_SVD_MODIFY_A + CV_SVD_U_T + CV_SVD_V_T );
        cvGEMM( &matU, &matV, 1, 0, 0, &matR, CV_GEMM_A_T );

        rx = R[7] - R[5];
        ry = R[2] - R[6];
        rz = R[3] - R[1];

        s = sqrt((rx*rx + ry*ry + rz*rz)*0.25);
        c = (R[0] + R[4] + R[8] - 1)*0.5;
        c = c > 1. ? 1. : c < -1. ? -1. : c;
        theta = acos(c);

        if( s < 1e-5 )
        {
            double t;

            if( c > 0 )
                rx = ry = rz = 0;
            else
            {
                t = (R[0] + 1)*0.5;
                rx = sqrt(MAX(t,0.));
                t = (R[4] + 1)*0.5;
                ry = sqrt(MAX(t,0.))*(R[1] < 0 ? -1. : 1.);
                t = (R[8] + 1)*0.5;
                rz = sqrt(MAX(t,0.))*(R[2] < 0 ? -1. : 1.);
                if( fabs(rx) < fabs(ry) && fabs(rx) < fabs(rz) && (R[5] > 0) != (ry*rz > 0) )
                    rz = -rz;
                theta /= sqrt(rx*rx + ry*ry + rz*rz);
                rx *= theta;
                ry *= theta;
                rz *= theta;
            }

            if( jacobian )
            {
                memset( J, 0, sizeof(J) );
                if( c > 0 )
                {
                    J[5] = J[15] = J[19] = -0.5;
                    J[7] = J[11] = J[21] = 0.5;
                }
            }
        }
        else
        {
            double vth = 1/(2*s);

            if( jacobian )
            {
                double t, dtheta_dtr = -1./s;
                // var1 = [vth;theta]
                // var = [om1;var1] = [om1;vth;theta]
                double dvth_dtheta = -vth*c/s;
                double d1 = 0.5*dvth_dtheta*dtheta_dtr;
                double d2 = 0.5*dtheta_dtr;
                // dvar1/dR = dvar1/dtheta*dtheta/dR = [dvth/dtheta; 1] * dtheta/dtr * dtr/dR
                double dvardR[5*9] =
                {
                    0, 0, 0, 0, 0, 1, 0, -1, 0,
                    0, 0, -1, 0, 0, 0, 1, 0, 0,
                    0, 1, 0, -1, 0, 0, 0, 0, 0,
                    d1, 0, 0, 0, d1, 0, 0, 0, d1,
                    d2, 0, 0, 0, d2, 0, 0, 0, d2
                };
                // var2 = [om;theta]
                double dvar2dvar[] =
                {
                    vth, 0, 0, rx, 0,
                    0, vth, 0, ry, 0,
                    0, 0, vth, rz, 0,
                    0, 0, 0, 0, 1
                };
                double domegadvar2[] =
                {
                    theta, 0, 0, rx*vth,
                    0, theta, 0, ry*vth,
                    0, 0, theta, rz*vth
                };

                CvMat _dvardR = cvMat( 5, 9, CV_64FC1, dvardR );
                CvMat _dvar2dvar = cvMat( 4, 5, CV_64FC1, dvar2dvar );
                CvMat _domegadvar2 = cvMat( 3, 4, CV_64FC1, domegadvar2 );
                double t0[3*5];
                CvMat _t0 = cvMat( 3, 5, CV_64FC1, t0 );

                cvMatMul( &_domegadvar2, &_dvar2dvar, &_t0 );
                cvMatMul( &_t0, &_dvardR, &matJ );

                // transpose every row of matJ (treat the rows as 3x3 matrices)
                CV_SWAP(J[1], J[3], t); CV_SWAP(J[2], J[6], t); CV_SWAP(J[5], J[7], t);
                CV_SWAP(J[10], J[12], t); CV_SWAP(J[11], J[15], t); CV_SWAP(J[14], J[16], t);
                CV_SWAP(J[19], J[21], t); CV_SWAP(J[20], J[24], t); CV_SWAP(J[23], J[25], t);
            }

            vth *= theta;
            rx *= vth; ry *= vth; rz *= vth;
        }

        if( depth == CV_32F )
        {
            dst->data.fl[0] = (float)rx;
            dst->data.fl[step] = (float)ry;
            dst->data.fl[step*2] = (float)rz;
        }
        else
        {
            dst->data.db[0] = rx;
            dst->data.db[step] = ry;
            dst->data.db[step*2] = rz;
        }
    }

    if( jacobian )
    {
        if( depth == CV_32F )
        {
            if( jacobian->rows == matJ.rows )
                cvConvert( &matJ, jacobian );
            else
            {
                float Jf[3*9];
                CvMat _Jf = cvMat( matJ.rows, matJ.cols, CV_32FC1, Jf );
                cvConvert( &matJ, &_Jf );
                cvTranspose( &_Jf, jacobian );
            }
        }
        else if( jacobian->rows == matJ.rows )
            cvCopy( &matJ, jacobian );
        else
            cvTranspose( &matJ, jacobian );
    }

    return 1;
}


CV_IMPL void cvProjectPoints2( const CvMat* objectPoints,
                  const CvMat* r_vec,
                  const CvMat* t_vec,
                  const CvMat* A,
                  const CvMat* distCoeffs,
                  CvMat* imagePoints, CvMat* dpdr,
                  CvMat* dpdt, CvMat* dpdf,
                  CvMat* dpdc, CvMat* dpdk,
                  double aspectRatio )
{
    Ptr<CvMat> matM, _m;
    Ptr<CvMat> _dpdr, _dpdt, _dpdc, _dpdf, _dpdk;

    int i, j, count;
    int calc_derivatives;
    const CvPoint3D64f* M;
    CvPoint2D64f* m;
    double r[3], R[9], dRdr[27], t[3], a[9], k[5] = {0,0,0,0,0}, fx, fy, cx, cy;
    CvMat _r, _t, _a = cvMat( 3, 3, CV_64F, a ), _k;
    CvMat matR = cvMat( 3, 3, CV_64F, R ), _dRdr = cvMat( 3, 9, CV_64F, dRdr );
    double *dpdr_p = 0, *dpdt_p = 0, *dpdk_p = 0, *dpdf_p = 0, *dpdc_p = 0;
    int dpdr_step = 0, dpdt_step = 0, dpdk_step = 0, dpdf_step = 0, dpdc_step = 0;
    bool fixedAspectRatio = aspectRatio > FLT_EPSILON;

    if( !CV_IS_MAT(objectPoints) || !CV_IS_MAT(r_vec) ||
        !CV_IS_MAT(t_vec) || !CV_IS_MAT(A) ||
        /*!CV_IS_MAT(distCoeffs) ||*/ !CV_IS_MAT(imagePoints) )
        CV_Error( CV_StsBadArg, "One of required arguments is not a valid matrix" );

    count = MAX(objectPoints->rows, objectPoints->cols);

    if( CV_IS_CONT_MAT(objectPoints->type) && CV_MAT_DEPTH(objectPoints->type) == CV_64F &&
        ((objectPoints->rows == 1 && CV_MAT_CN(objectPoints->type) == 3) ||
        (objectPoints->rows == count && CV_MAT_CN(objectPoints->type)*objectPoints->cols == 3)))
    {
        matM = cvCloneMat(objectPoints);
    }
    else
    {
        matM = cvCreateMat( 1, count, CV_64FC3 );
        cvConvertPointsHomogeneous( objectPoints, matM );
    }

    if( CV_IS_CONT_MAT(imagePoints->type) && CV_MAT_DEPTH(imagePoints->type) == CV_64F &&
        ((imagePoints->rows == 1 && CV_MAT_CN(imagePoints->type) == 2) ||
        (imagePoints->rows == count && CV_MAT_CN(imagePoints->type)*imagePoints->cols == 2)))
    {
        _m = cvCloneMat(imagePoints);
    }
    else
        _m = cvCreateMat( 1, count, CV_64FC2 );

    M = (CvPoint3D64f*)matM->data.db;
    m = (CvPoint2D64f*)_m->data.db;

    if( (CV_MAT_DEPTH(r_vec->type) != CV_64F && CV_MAT_DEPTH(r_vec->type) != CV_32F) ||
        (((r_vec->rows != 1 && r_vec->cols != 1) ||
        r_vec->rows*r_vec->cols*CV_MAT_CN(r_vec->type) != 3) &&
        ((r_vec->rows != 3 && r_vec->cols != 3) || CV_MAT_CN(r_vec->type) != 1)))
        CV_Error( CV_StsBadArg, "Rotation must be represented by 1x3 or 3x1 "
                  "floating-point rotation vector, or 3x3 rotation matrix" );

    if( r_vec->rows == 3 && r_vec->cols == 3 )
    {
        _r = cvMat( 3, 1, CV_64FC1, r );
        cvRodrigues2( r_vec, &_r );
        cvRodrigues2( &_r, &matR, &_dRdr );
        cvCopy( r_vec, &matR );
    }
    else
    {
        _r = cvMat( r_vec->rows, r_vec->cols, CV_MAKETYPE(CV_64F,CV_MAT_CN(r_vec->type)), r );
        cvConvert( r_vec, &_r );
        cvRodrigues2( &_r, &matR, &_dRdr );
    }

    if( (CV_MAT_DEPTH(t_vec->type) != CV_64F && CV_MAT_DEPTH(t_vec->type) != CV_32F) ||
        (t_vec->rows != 1 && t_vec->cols != 1) ||
        t_vec->rows*t_vec->cols*CV_MAT_CN(t_vec->type) != 3 )
        CV_Error( CV_StsBadArg,
            "Translation vector must be 1x3 or 3x1 floating-point vector" );

    _t = cvMat( t_vec->rows, t_vec->cols, CV_MAKETYPE(CV_64F,CV_MAT_CN(t_vec->type)), t );
    cvConvert( t_vec, &_t );

    if( (CV_MAT_TYPE(A->type) != CV_64FC1 && CV_MAT_TYPE(A->type) != CV_32FC1) ||
        A->rows != 3 || A->cols != 3 )
        CV_Error( CV_StsBadArg, "Instrinsic parameters must be 3x3 floating-point matrix" );

    cvConvert( A, &_a );
    fx = a[0]; fy = a[4];
    cx = a[2]; cy = a[5];

    if( fixedAspectRatio )
        fx = fy*aspectRatio;

    if( distCoeffs )
    {
        if( !CV_IS_MAT(distCoeffs) ||
            (CV_MAT_DEPTH(distCoeffs->type) != CV_64F &&
            CV_MAT_DEPTH(distCoeffs->type) != CV_32F) ||
            (distCoeffs->rows != 1 && distCoeffs->cols != 1) ||
            (distCoeffs->rows*distCoeffs->cols*CV_MAT_CN(distCoeffs->type) != 4 &&
            distCoeffs->rows*distCoeffs->cols*CV_MAT_CN(distCoeffs->type) != 5) )
            CV_Error( CV_StsBadArg,
                "Distortion coefficients must be 1x4, 4x1, 1x5 or 5x1 floating-point vector" );

        _k = cvMat( distCoeffs->rows, distCoeffs->cols,
                    CV_MAKETYPE(CV_64F,CV_MAT_CN(distCoeffs->type)), k );
        cvConvert( distCoeffs, &_k );
    }

    if( dpdr )
    {
        if( !CV_IS_MAT(dpdr) ||
            (CV_MAT_TYPE(dpdr->type) != CV_32FC1 &&
            CV_MAT_TYPE(dpdr->type) != CV_64FC1) ||
            dpdr->rows != count*2 || dpdr->cols != 3 )
            CV_Error( CV_StsBadArg, "dp/drot must be 2Nx3 floating-point matrix" );

        if( CV_MAT_TYPE(dpdr->type) == CV_64FC1 )
        {
            _dpdr = cvCloneMat(dpdr);
        }
        else
            _dpdr = cvCreateMat( 2*count, 3, CV_64FC1 );
        dpdr_p = _dpdr->data.db;
        dpdr_step = _dpdr->step/sizeof(dpdr_p[0]);
    }

    if( dpdt )
    {
        if( !CV_IS_MAT(dpdt) ||
            (CV_MAT_TYPE(dpdt->type) != CV_32FC1 &&
            CV_MAT_TYPE(dpdt->type) != CV_64FC1) ||
            dpdt->rows != count*2 || dpdt->cols != 3 )
            CV_Error( CV_StsBadArg, "dp/dT must be 2Nx3 floating-point matrix" );

        if( CV_MAT_TYPE(dpdt->type) == CV_64FC1 )
        {
            _dpdt = cvCloneMat(dpdt);
        }
        else
            _dpdt = cvCreateMat( 2*count, 3, CV_64FC1 );
        dpdt_p = _dpdt->data.db;
        dpdt_step = _dpdt->step/sizeof(dpdt_p[0]);
    }

    if( dpdf )
    {
        if( !CV_IS_MAT(dpdf) ||
            (CV_MAT_TYPE(dpdf->type) != CV_32FC1 && CV_MAT_TYPE(dpdf->type) != CV_64FC1) ||
            dpdf->rows != count*2 || dpdf->cols != 2 )
            CV_Error( CV_StsBadArg, "dp/df must be 2Nx2 floating-point matrix" );

        if( CV_MAT_TYPE(dpdf->type) == CV_64FC1 )
        {
            _dpdf = cvCloneMat(dpdf);
        }
        else
            _dpdf = cvCreateMat( 2*count, 2, CV_64FC1 );
        dpdf_p = _dpdf->data.db;
        dpdf_step = _dpdf->step/sizeof(dpdf_p[0]);
    }

    if( dpdc )
    {
        if( !CV_IS_MAT(dpdc) ||
            (CV_MAT_TYPE(dpdc->type) != CV_32FC1 && CV_MAT_TYPE(dpdc->type) != CV_64FC1) ||
            dpdc->rows != count*2 || dpdc->cols != 2 )
            CV_Error( CV_StsBadArg, "dp/dc must be 2Nx2 floating-point matrix" );

        if( CV_MAT_TYPE(dpdc->type) == CV_64FC1 )
        {
            _dpdc = cvCloneMat(dpdc);
        }
        else
            _dpdc = cvCreateMat( 2*count, 2, CV_64FC1 );
        dpdc_p = _dpdc->data.db;
        dpdc_step = _dpdc->step/sizeof(dpdc_p[0]);
    }

    if( dpdk )
    {
        if( !CV_IS_MAT(dpdk) ||
            (CV_MAT_TYPE(dpdk->type) != CV_32FC1 && CV_MAT_TYPE(dpdk->type) != CV_64FC1) ||
            dpdk->rows != count*2 || (dpdk->cols != 5 && dpdk->cols != 4 && dpdk->cols != 2) )
            CV_Error( CV_StsBadArg, "dp/df must be 2Nx5, 2Nx4 or 2Nx2 floating-point matrix" );

        if( !distCoeffs )
            CV_Error( CV_StsNullPtr, "distCoeffs is NULL while dpdk is not" );

        if( CV_MAT_TYPE(dpdk->type) == CV_64FC1 )
        {
            _dpdk = cvCloneMat(dpdk);
        }
        else
            _dpdk = cvCreateMat( dpdk->rows, dpdk->cols, CV_64FC1 );
        dpdk_p = _dpdk->data.db;
        dpdk_step = _dpdk->step/sizeof(dpdk_p[0]);
    }

    calc_derivatives = dpdr || dpdt || dpdf || dpdc || dpdk;

    for( i = 0; i < count; i++ )
    {
        double X = M[i].x, Y = M[i].y, Z = M[i].z;
        double x = R[0]*X + R[1]*Y + R[2]*Z + t[0];
        double y = R[3]*X + R[4]*Y + R[5]*Z + t[1];
        double z = R[6]*X + R[7]*Y + R[8]*Z + t[2];
        double r2, r4, r6, a1, a2, a3, cdist;
        double xd, yd;

        z = z ? 1./z : 1;
        x *= z; y *= z;

        r2 = x*x + y*y;
        r4 = r2*r2;
        r6 = r4*r2;
        a1 = 2*x*y;
        a2 = r2 + 2*x*x;
        a3 = r2 + 2*y*y;
        cdist = 1 + k[0]*r2 + k[1]*r4 + k[4]*r6;
        xd = x*cdist + k[2]*a1 + k[3]*a2;
        yd = y*cdist + k[2]*a3 + k[3]*a1;

        m[i].x = xd*fx + cx;
        m[i].y = yd*fy + cy;

        if( calc_derivatives )
        {
            if( dpdc_p )
            {
                dpdc_p[0] = 1; dpdc_p[1] = 0;
                dpdc_p[dpdc_step] = 0;
                dpdc_p[dpdc_step+1] = 1;
                dpdc_p += dpdc_step*2;
            }

            if( dpdf_p )
            {
                if( fixedAspectRatio )
                {
                    dpdf_p[0] = 0; dpdf_p[1] = xd*aspectRatio;
                    dpdf_p[dpdf_step] = 0;
                    dpdf_p[dpdf_step+1] = yd;
                }
                else
                {
                    dpdf_p[0] = xd; dpdf_p[1] = 0;
                    dpdf_p[dpdf_step] = 0;
                    dpdf_p[dpdf_step+1] = yd;
                }
                dpdf_p += dpdf_step*2;
            }

            if( dpdk_p )
            {
                dpdk_p[0] = fx*x*r2;
                dpdk_p[1] = fx*x*r4;
                dpdk_p[dpdk_step] = fy*y*r2;
                dpdk_p[dpdk_step+1] = fy*y*r4;
                if( _dpdk->cols > 2 )
                {
                    dpdk_p[2] = fx*a1;
                    dpdk_p[3] = fx*a2;
                    dpdk_p[dpdk_step+2] = fy*a3;
                    dpdk_p[dpdk_step+3] = fy*a1;
                    if( _dpdk->cols > 4 )
                    {
                        dpdk_p[4] = fx*x*r6;
                        dpdk_p[dpdk_step+4] = fy*y*r6;
                    }
                }
                dpdk_p += dpdk_step*2;
            }

            if( dpdt_p )
            {
                double dxdt[] = { z, 0, -x*z }, dydt[] = { 0, z, -y*z };
                for( j = 0; j < 3; j++ )
                {
                    double dr2dt = 2*x*dxdt[j] + 2*y*dydt[j];
                    double dcdist_dt = k[0]*dr2dt + 2*k[1]*r2*dr2dt + 3*k[4]*r4*dr2dt;
                    double da1dt = 2*(x*dydt[j] + y*dxdt[j]);
                    double dmxdt = fx*(dxdt[j]*cdist + x*dcdist_dt +
                                k[2]*da1dt + k[3]*(dr2dt + 2*x*dxdt[j]));
                    double dmydt = fy*(dydt[j]*cdist + y*dcdist_dt +
                                k[2]*(dr2dt + 2*y*dydt[j]) + k[3]*da1dt);
                    dpdt_p[j] = dmxdt;
                    dpdt_p[dpdt_step+j] = dmydt;
                }
                dpdt_p += dpdt_step*2;
            }

            if( dpdr_p )
            {
                double dx0dr[] =
                {
                    X*dRdr[0] + Y*dRdr[1] + Z*dRdr[2],
                    X*dRdr[9] + Y*dRdr[10] + Z*dRdr[11],
                    X*dRdr[18] + Y*dRdr[19] + Z*dRdr[20]
                };
                double dy0dr[] =
                {
                    X*dRdr[3] + Y*dRdr[4] + Z*dRdr[5],
                    X*dRdr[12] + Y*dRdr[13] + Z*dRdr[14],
                    X*dRdr[21] + Y*dRdr[22] + Z*dRdr[23]
                };
                double dz0dr[] =
                {
                    X*dRdr[6] + Y*dRdr[7] + Z*dRdr[8],
                    X*dRdr[15] + Y*dRdr[16] + Z*dRdr[17],
                    X*dRdr[24] + Y*dRdr[25] + Z*dRdr[26]
                };
                for( j = 0; j < 3; j++ )
                {
                    double dxdr = z*(dx0dr[j] - x*dz0dr[j]);
                    double dydr = z*(dy0dr[j] - y*dz0dr[j]);
                    double dr2dr = 2*x*dxdr + 2*y*dydr;
                    double dcdist_dr = k[0]*dr2dr + 2*k[1]*r2*dr2dr + 3*k[4]*r4*dr2dr;
                    double da1dr = 2*(x*dydr + y*dxdr);
                    double dmxdr = fx*(dxdr*cdist + x*dcdist_dr +
                                k[2]*da1dr + k[3]*(dr2dr + 2*x*dxdr));
                    double dmydr = fy*(dydr*cdist + y*dcdist_dr +
                                k[2]*(dr2dr + 2*y*dydr) + k[3]*da1dr);
                    dpdr_p[j] = dmxdr;
                    dpdr_p[dpdr_step+j] = dmydr;
                }
                dpdr_p += dpdr_step*2;
            }
        }
    }

    if( _m != imagePoints )
        cvConvertPointsHomogeneous( _m, imagePoints );
    if( _dpdr != dpdr )
        cvConvert( _dpdr, dpdr );
    
    if( _dpdt != dpdt )
        cvConvert( _dpdt, dpdt );
    
    if( _dpdf != dpdf )
        cvConvert( _dpdf, dpdf );
    
    if( _dpdc != dpdc )
        cvConvert( _dpdc, dpdc );
    
    if( _dpdk != dpdk )
        cvConvert( _dpdk, dpdk );
}


CV_IMPL void cvFindExtrinsicCameraParams2( const CvMat* objectPoints,
                  const CvMat* imagePoints, const CvMat* A,
                  const CvMat* distCoeffs, CvMat* rvec, CvMat* tvec,
                  int useExtrinsicGuess )
{
    const int max_iter = 20;
    Ptr<CvMat> matM, _Mxy, _m, _mn, matL, matJ;

    int i, count;
    double a[9], ar[9]={1,0,0,0,1,0,0,0,1}, R[9];
    double MM[9], U[9], V[9], W[3];
    CvScalar Mc;
    double param[6];
    CvMat matA = cvMat( 3, 3, CV_64F, a );
    CvMat _Ar = cvMat( 3, 3, CV_64F, ar );
    CvMat matR = cvMat( 3, 3, CV_64F, R );
    CvMat _r = cvMat( 3, 1, CV_64F, param );
    CvMat _t = cvMat( 3, 1, CV_64F, param + 3 );
    CvMat _Mc = cvMat( 1, 3, CV_64F, Mc.val );
    CvMat _MM = cvMat( 3, 3, CV_64F, MM );
    CvMat matU = cvMat( 3, 3, CV_64F, U );
    CvMat matV = cvMat( 3, 3, CV_64F, V );
    CvMat matW = cvMat( 3, 1, CV_64F, W );
    CvMat _param = cvMat( 6, 1, CV_64F, param );
    CvMat _dpdr, _dpdt;

    CV_Assert( CV_IS_MAT(objectPoints) && CV_IS_MAT(imagePoints) &&
        CV_IS_MAT(A) && CV_IS_MAT(rvec) && CV_IS_MAT(tvec) );

    count = MAX(objectPoints->cols, objectPoints->rows);
    matM = cvCreateMat( 1, count, CV_64FC3 );
    _m = cvCreateMat( 1, count, CV_64FC2 );

    cvConvertPointsHomogeneous( objectPoints, matM );
    cvConvertPointsHomogeneous( imagePoints, _m );
    cvConvert( A, &matA );

    CV_Assert( (CV_MAT_DEPTH(rvec->type) == CV_64F || CV_MAT_DEPTH(rvec->type) == CV_32F) &&
        (rvec->rows == 1 || rvec->cols == 1) && rvec->rows*rvec->cols*CV_MAT_CN(rvec->type) == 3 );

    CV_Assert( (CV_MAT_DEPTH(tvec->type) == CV_64F || CV_MAT_DEPTH(tvec->type) == CV_32F) &&
        (tvec->rows == 1 || tvec->cols == 1) && tvec->rows*tvec->cols*CV_MAT_CN(tvec->type) == 3 );

    _mn = cvCreateMat( 1, count, CV_64FC2 );
    _Mxy = cvCreateMat( 1, count, CV_64FC2 );

    // normalize image points
    // (unapply the intrinsic matrix transformation and distortion)
    cvUndistortPoints( _m, _mn, &matA, distCoeffs, 0, &_Ar );

    if( useExtrinsicGuess )
    {
        CvMat _r_temp = cvMat(rvec->rows, rvec->cols,
            CV_MAKETYPE(CV_64F,CV_MAT_CN(rvec->type)), param );
        CvMat _t_temp = cvMat(tvec->rows, tvec->cols,
            CV_MAKETYPE(CV_64F,CV_MAT_CN(tvec->type)), param + 3);
        cvConvert( rvec, &_r_temp );
        cvConvert( tvec, &_t_temp );
    }
    else
    {
        Mc = cvAvg(matM);
        cvReshape( matM, matM, 1, count );
        cvMulTransposed( matM, &_MM, 1, &_Mc );
        cvSVD( &_MM, &matW, 0, &matV, CV_SVD_MODIFY_A + CV_SVD_V_T );

        // initialize extrinsic parameters
        if( W[2]/W[1] < 1e-3 || count < 4 )
        {
            // a planar structure case (all M's lie in the same plane)
            double tt[3], h[9], h1_norm, h2_norm;
            CvMat* R_transform = &matV;
            CvMat T_transform = cvMat( 3, 1, CV_64F, tt );
            CvMat matH = cvMat( 3, 3, CV_64F, h );
            CvMat _h1, _h2, _h3;

            if( V[2]*V[2] + V[5]*V[5] < 1e-10 )
                cvSetIdentity( R_transform );

            if( cvDet(R_transform) < 0 )
                cvScale( R_transform, R_transform, -1 );

            cvGEMM( R_transform, &_Mc, -1, 0, 0, &T_transform, CV_GEMM_B_T );

            for( i = 0; i < count; i++ )
            {
                const double* Rp = R_transform->data.db;
                const double* Tp = T_transform.data.db;
                const double* src = matM->data.db + i*3;
                double* dst = _Mxy->data.db + i*2;

                dst[0] = Rp[0]*src[0] + Rp[1]*src[1] + Rp[2]*src[2] + Tp[0];
                dst[1] = Rp[3]*src[0] + Rp[4]*src[1] + Rp[5]*src[2] + Tp[1];
            }

            cvFindHomography( _Mxy, _mn, &matH );

            cvGetCol( &matH, &_h1, 0 );
            _h2 = _h1; _h2.data.db++;
            _h3 = _h2; _h3.data.db++;
            h1_norm = sqrt(h[0]*h[0] + h[3]*h[3] + h[6]*h[6]);
            h2_norm = sqrt(h[1]*h[1] + h[4]*h[4] + h[7]*h[7]);

            cvScale( &_h1, &_h1, 1./h1_norm );
            cvScale( &_h2, &_h2, 1./h2_norm );
            cvScale( &_h3, &_t, 2./(h1_norm + h2_norm));
            cvCrossProduct( &_h1, &_h2, &_h3 );

            cvRodrigues2( &matH, &_r );
            cvRodrigues2( &_r, &matH );
            cvMatMulAdd( &matH, &T_transform, &_t, &_t );
            cvMatMul( &matH, R_transform, &matR );
            cvRodrigues2( &matR, &_r );
        }
        else
        {
            // non-planar structure. Use DLT method
            double* L;
            double LL[12*12], LW[12], LV[12*12], sc;
            CvMat _LL = cvMat( 12, 12, CV_64F, LL );
            CvMat _LW = cvMat( 12, 1, CV_64F, LW );
            CvMat _LV = cvMat( 12, 12, CV_64F, LV );
            CvMat _RRt, _RR, _tt;
            CvPoint3D64f* M = (CvPoint3D64f*)matM->data.db;
            CvPoint2D64f* mn = (CvPoint2D64f*)_mn->data.db;

            matL = cvCreateMat( 2*count, 12, CV_64F );
            L = matL->data.db;

            for( i = 0; i < count; i++, L += 24 )
            {
                double x = -mn[i].x, y = -mn[i].y;
                L[0] = L[16] = M[i].x;
                L[1] = L[17] = M[i].y;
                L[2] = L[18] = M[i].z;
                L[3] = L[19] = 1.;
                L[4] = L[5] = L[6] = L[7] = 0.;
                L[12] = L[13] = L[14] = L[15] = 0.;
                L[8] = x*M[i].x;
                L[9] = x*M[i].y;
                L[10] = x*M[i].z;
                L[11] = x;
                L[20] = y*M[i].x;
                L[21] = y*M[i].y;
                L[22] = y*M[i].z;
                L[23] = y;
            }

            cvMulTransposed( matL, &_LL, 1 );
            cvSVD( &_LL, &_LW, 0, &_LV, CV_SVD_MODIFY_A + CV_SVD_V_T );
            _RRt = cvMat( 3, 4, CV_64F, LV + 11*12 );
            cvGetCols( &_RRt, &_RR, 0, 3 );
            cvGetCol( &_RRt, &_tt, 3 );
            if( cvDet(&_RR) < 0 )
                cvScale( &_RRt, &_RRt, -1 );
            sc = cvNorm(&_RR);
            cvSVD( &_RR, &matW, &matU, &matV, CV_SVD_MODIFY_A + CV_SVD_U_T + CV_SVD_V_T );
            cvGEMM( &matU, &matV, 1, 0, 0, &matR, CV_GEMM_A_T );
            cvScale( &_tt, &_t, cvNorm(&matR)/sc );
            cvRodrigues2( &matR, &_r );
        }
    }

    cvReshape( matM, matM, 3, 1 );
    cvReshape( _mn, _mn, 2, 1 );

    // refine extrinsic parameters using iterative algorithm
    CvLevMarq solver( 6, count*2, cvTermCriteria(CV_TERMCRIT_EPS+CV_TERMCRIT_ITER,max_iter,FLT_EPSILON), true);
    cvCopy( &_param, solver.param );

    for(;;)
    {
        CvMat *matJ = 0, *_err = 0;
        const CvMat *__param = 0;
        bool proceed = solver.update( __param, matJ, _err );
        cvCopy( __param, &_param );
        if( !proceed || !_err )
            break;
        cvReshape( _err, _err, 2, 1 );
        if( matJ )
        {
            cvGetCols( matJ, &_dpdr, 0, 3 );
            cvGetCols( matJ, &_dpdt, 3, 6 );
            cvProjectPoints2( matM, &_r, &_t, &matA, distCoeffs,
                              _err, &_dpdr, &_dpdt, 0, 0, 0 );
        }
        else
        {
            cvProjectPoints2( matM, &_r, &_t, &matA, distCoeffs,
                              _err, 0, 0, 0, 0, 0 );
        }
        cvSub(_err, _m, _err);
        cvReshape( _err, _err, 1, 2*count );
    }
    cvCopy( solver.param, &_param );

    _r = cvMat( rvec->rows, rvec->cols,
        CV_MAKETYPE(CV_64F,CV_MAT_CN(rvec->type)), param );
    _t = cvMat( tvec->rows, tvec->cols,
        CV_MAKETYPE(CV_64F,CV_MAT_CN(tvec->type)), param + 3 );

    cvConvert( &_r, rvec );
    cvConvert( &_t, tvec );
}


CV_IMPL void cvInitIntrinsicParams2D( const CvMat* objectPoints,
                         const CvMat* imagePoints, const CvMat* npoints,
                         CvSize imageSize, CvMat* cameraMatrix,
                         double aspectRatio )
{
    Ptr<CvMat> matA, _b, _allH, _allK;

    int i, j, pos, nimages, total, ni = 0;
    double a[9] = { 0, 0, 0, 0, 0, 0, 0, 0, 1 };
    double H[9], f[2];
    CvMat _a = cvMat( 3, 3, CV_64F, a );
    CvMat matH = cvMat( 3, 3, CV_64F, H );
    CvMat _f = cvMat( 2, 1, CV_64F, f );

    assert( CV_MAT_TYPE(npoints->type) == CV_32SC1 &&
            CV_IS_MAT_CONT(npoints->type) );
    nimages = npoints->rows + npoints->cols - 1;

    if( (CV_MAT_TYPE(objectPoints->type) != CV_32FC3 &&
        CV_MAT_TYPE(objectPoints->type) != CV_64FC3) ||
        (CV_MAT_TYPE(imagePoints->type) != CV_32FC2 &&
        CV_MAT_TYPE(imagePoints->type) != CV_64FC2) )
        CV_Error( CV_StsUnsupportedFormat, "Both object points and image points must be 2D" );

    if( objectPoints->rows != 1 || imagePoints->rows != 1 )
        CV_Error( CV_StsBadSize, "object points and image points must be a single-row matrices" );

    matA = cvCreateMat( 2*nimages, 2, CV_64F );
    _b = cvCreateMat( 2*nimages, 1, CV_64F );
    a[2] = (imageSize.width - 1)*0.5;
    a[5] = (imageSize.height - 1)*0.5;
    _allH = cvCreateMat( nimages, 9, CV_64F );

    total = cvRound(cvSum(npoints).val[0]);

    // extract vanishing points in order to obtain initial value for the focal length
    for( i = 0, pos = 0; i < nimages; i++, pos += ni )
    {
        double* Ap = matA->data.db + i*4;
        double* bp = _b->data.db + i*2;
        ni = npoints->data.i[i];
        double h[3], v[3], d1[3], d2[3];
        double n[4] = {0,0,0,0};
        CvMat _m, matM;
        cvGetCols( objectPoints, &matM, pos, pos + ni );
        cvGetCols( imagePoints, &_m, pos, pos + ni );

        cvFindHomography( &matM, &_m, &matH );
        memcpy( _allH->data.db + i*9, H, sizeof(H) );

        H[0] -= H[6]*a[2]; H[1] -= H[7]*a[2]; H[2] -= H[8]*a[2];
        H[3] -= H[6]*a[5]; H[4] -= H[7]*a[5]; H[5] -= H[8]*a[5];

        for( j = 0; j < 3; j++ )
        {
            double t0 = H[j*3], t1 = H[j*3+1];
            h[j] = t0; v[j] = t1;
            d1[j] = (t0 + t1)*0.5;
            d2[j] = (t0 - t1)*0.5;
            n[0] += t0*t0; n[1] += t1*t1;
            n[2] += d1[j]*d1[j]; n[3] += d2[j]*d2[j];
        }

        for( j = 0; j < 4; j++ )
            n[j] = 1./sqrt(n[j]);

        for( j = 0; j < 3; j++ )
        {
            h[j] *= n[0]; v[j] *= n[1];
            d1[j] *= n[2]; d2[j] *= n[3];
        }

        Ap[0] = h[0]*v[0]; Ap[1] = h[1]*v[1];
        Ap[2] = d1[0]*d2[0]; Ap[3] = d1[1]*d2[1];
        bp[0] = -h[2]*v[2]; bp[1] = -d1[2]*d2[2];
    }

    cvSolve( matA, _b, &_f, CV_NORMAL + CV_SVD );
    a[0] = sqrt(fabs(1./f[0]));
    a[4] = sqrt(fabs(1./f[1]));
    if( aspectRatio != 0 )
    {
        double tf = (a[0] + a[4])/(aspectRatio + 1.);
        a[0] = aspectRatio*tf;
        a[4] = tf;
    }

    cvConvert( &_a, cameraMatrix );
}


/* finds intrinsic and extrinsic camera parameters
   from a few views of known calibration pattern */
CV_IMPL double cvCalibrateCamera2( const CvMat* objectPoints,
                    const CvMat* imagePoints, const CvMat* npoints,
                    CvSize imageSize, CvMat* cameraMatrix, CvMat* distCoeffs,
                    CvMat* rvecs, CvMat* tvecs, int flags )
{
    const int NINTRINSIC = 9;
    Ptr<CvMat> matM, _m, _Ji, _Je, _err;
    CvLevMarq solver;
    double reprojErr = 0;

    double A[9], k[5] = {0,0,0,0,0};
    CvMat matA = cvMat(3, 3, CV_64F, A), _k;
    int i, nimages, maxPoints = 0, ni = 0, pos, total = 0, nparams, npstep, cn;
    double aspectRatio = 0.;

    // 0. check the parameters & allocate buffers
    if( !CV_IS_MAT(objectPoints) || !CV_IS_MAT(imagePoints) ||
        !CV_IS_MAT(npoints) || !CV_IS_MAT(cameraMatrix) || !CV_IS_MAT(distCoeffs) )
        CV_Error( CV_StsBadArg, "One of required vector arguments is not a valid matrix" );

    if( imageSize.width <= 0 || imageSize.height <= 0 )
        CV_Error( CV_StsOutOfRange, "image width and height must be positive" );

    if( CV_MAT_TYPE(npoints->type) != CV_32SC1 ||
        (npoints->rows != 1 && npoints->cols != 1) )
        CV_Error( CV_StsUnsupportedFormat,
            "the array of point counters must be 1-dimensional integer vector" );

    nimages = npoints->rows*npoints->cols;
    npstep = npoints->rows == 1 ? 1 : npoints->step/CV_ELEM_SIZE(npoints->type);

    if( rvecs )
    {
        cn = CV_MAT_CN(rvecs->type);
        if( !CV_IS_MAT(rvecs) ||
            (CV_MAT_DEPTH(rvecs->type) != CV_32F && CV_MAT_DEPTH(rvecs->type) != CV_64F) ||
            ((rvecs->rows != nimages || (rvecs->cols*cn != 3 && rvecs->cols*cn != 9)) &&
            (rvecs->rows != 1 || rvecs->cols != nimages || cn != 3)) )
            CV_Error( CV_StsBadArg, "the output array of rotation vectors must be 3-channel "
                "1xn or nx1 array or 1-channel nx3 or nx9 array, where n is the number of views" );
    }

    if( tvecs )
    {
        cn = CV_MAT_CN(tvecs->type);
        if( !CV_IS_MAT(tvecs) ||
            (CV_MAT_DEPTH(tvecs->type) != CV_32F && CV_MAT_DEPTH(tvecs->type) != CV_64F) ||
            ((tvecs->rows != nimages || tvecs->cols*cn != 3) &&
            (tvecs->rows != 1 || tvecs->cols != nimages || cn != 3)) )
            CV_Error( CV_StsBadArg, "the output array of translation vectors must be 3-channel "
                "1xn or nx1 array or 1-channel nx3 array, where n is the number of views" );
    }

    if( (CV_MAT_TYPE(cameraMatrix->type) != CV_32FC1 &&
        CV_MAT_TYPE(cameraMatrix->type) != CV_64FC1) ||
        cameraMatrix->rows != 3 || cameraMatrix->cols != 3 )
        CV_Error( CV_StsBadArg,
            "Intrinsic parameters must be 3x3 floating-point matrix" );

    if( (CV_MAT_TYPE(distCoeffs->type) != CV_32FC1 &&
        CV_MAT_TYPE(distCoeffs->type) != CV_64FC1) ||
        (distCoeffs->cols != 1 && distCoeffs->rows != 1) ||
        (distCoeffs->cols*distCoeffs->rows != 4 &&
        distCoeffs->cols*distCoeffs->rows != 5) )
        CV_Error( CV_StsBadArg,
            "Distortion coefficients must be 4x1, 1x4, 5x1 or 1x5 floating-point matrix" );

    for( i = 0; i < nimages; i++ )
    {
        ni = npoints->data.i[i*npstep];
        if( ni < 4 )
        {
            char buf[100];
            sprintf( buf, "The number of points in the view #%d is < 4", i );
            CV_Error( CV_StsOutOfRange, buf );
        }
        maxPoints = MAX( maxPoints, ni );
        total += ni;
    }

    matM = cvCreateMat( 1, total, CV_64FC3 );
    _m = cvCreateMat( 1, total, CV_64FC2 );

    cvConvertPointsHomogeneous( objectPoints, matM );
    cvConvertPointsHomogeneous( imagePoints, _m );

    nparams = NINTRINSIC + nimages*6;
    _Ji = cvCreateMat( maxPoints*2, NINTRINSIC, CV_64FC1 );
    _Je = cvCreateMat( maxPoints*2, 6, CV_64FC1 );
    _err = cvCreateMat( maxPoints*2, 1, CV_64FC1 );
    cvZero( _Ji );

    _k = cvMat( distCoeffs->rows, distCoeffs->cols, CV_MAKETYPE(CV_64F,CV_MAT_CN(distCoeffs->type)), k);
    if( distCoeffs->rows*distCoeffs->cols*CV_MAT_CN(distCoeffs->type) == 4 )
        flags |= CV_CALIB_FIX_K3;

    // 1. initialize intrinsic parameters & LM solver
    if( flags & CV_CALIB_USE_INTRINSIC_GUESS )
    {
        cvConvert( cameraMatrix, &matA );
        if( A[0] <= 0 || A[4] <= 0 )
            CV_Error( CV_StsOutOfRange, "Focal length (fx and fy) must be positive" );
        if( A[2] < 0 || A[2] >= imageSize.width ||
            A[5] < 0 || A[5] >= imageSize.height )
            CV_Error( CV_StsOutOfRange, "Principal point must be within the image" );
        if( fabs(A[1]) > 1e-5 )
            CV_Error( CV_StsOutOfRange, "Non-zero skew is not supported by the function" );
        if( fabs(A[3]) > 1e-5 || fabs(A[6]) > 1e-5 ||
            fabs(A[7]) > 1e-5 || fabs(A[8]-1) > 1e-5 )
            CV_Error( CV_StsOutOfRange,
                "The intrinsic matrix must have [fx 0 cx; 0 fy cy; 0 0 1] shape" );
        A[1] = A[3] = A[6] = A[7] = 0.;
        A[8] = 1.;

        if( flags & CV_CALIB_FIX_ASPECT_RATIO )
            aspectRatio = A[0]/A[4];
        cvConvert( distCoeffs, &_k );
    }
    else
    {
        CvScalar mean, sdv;
        cvAvgSdv( matM, &mean, &sdv );
        if( fabs(mean.val[2]) > 1e-5 || fabs(sdv.val[2]) > 1e-5 )
            CV_Error( CV_StsBadArg,
            "For non-planar calibration rigs the initial intrinsic matrix must be specified" );
        for( i = 0; i < total; i++ )
            ((CvPoint3D64f*)matM->data.db)[i].z = 0.;

        if( flags & CV_CALIB_FIX_ASPECT_RATIO )
        {
            aspectRatio = cvmGet(cameraMatrix,0,0);
            aspectRatio /= cvmGet(cameraMatrix,1,1);
            if( aspectRatio < 0.01 || aspectRatio > 100 )
                CV_Error( CV_StsOutOfRange,
                    "The specified aspect ratio (=A[0][0]/A[1][1]) is incorrect" );
        }
        cvInitIntrinsicParams2D( matM, _m, npoints, imageSize, &matA, aspectRatio );
    }

    solver.init( nparams, 0, cvTermCriteria(CV_TERMCRIT_ITER+CV_TERMCRIT_EPS,30,DBL_EPSILON) );

    {
    double* param = solver.param->data.db;
    uchar* mask = solver.mask->data.ptr;

    param[0] = A[0]; param[1] = A[4]; param[2] = A[2]; param[3] = A[5];
    param[4] = k[0]; param[5] = k[1]; param[6] = k[2]; param[7] = k[3];
    param[8] = k[4];

    if( flags & CV_CALIB_FIX_FOCAL_LENGTH )
        mask[0] = mask[1] = 0;
    if( flags & CV_CALIB_FIX_PRINCIPAL_POINT )
        mask[2] = mask[3] = 0;
    if( flags & CV_CALIB_ZERO_TANGENT_DIST )
    {
        param[6] = param[7] = 0;
        mask[6] = mask[7] = 0;
    }
    if( flags & CV_CALIB_FIX_K1 )
        mask[4] = 0;
    if( flags & CV_CALIB_FIX_K2 )
        mask[5] = 0;
    if( flags & CV_CALIB_FIX_K3 )
        mask[8] = 0;
    }

    // 2. initialize extrinsic parameters
    for( i = 0, pos = 0; i < nimages; i++, pos += ni )
    {
        CvMat _Mi, _mi, _ri, _ti;
        ni = npoints->data.i[i*npstep];

        cvGetRows( solver.param, &_ri, NINTRINSIC + i*6, NINTRINSIC + i*6 + 3 );
        cvGetRows( solver.param, &_ti, NINTRINSIC + i*6 + 3, NINTRINSIC + i*6 + 6 );

        cvGetCols( matM, &_Mi, pos, pos + ni );
        cvGetCols( _m, &_mi, pos, pos + ni );

        cvFindExtrinsicCameraParams2( &_Mi, &_mi, &matA, &_k, &_ri, &_ti );
    }

    // 3. run the optimization
    for(;;)
    {
        const CvMat* _param = 0;
        CvMat *_JtJ = 0, *_JtErr = 0;
        double* _errNorm = 0;
        bool proceed = solver.updateAlt( _param, _JtJ, _JtErr, _errNorm );
        double *param = solver.param->data.db, *pparam = solver.prevParam->data.db;

        if( flags & CV_CALIB_FIX_ASPECT_RATIO )
        {
            param[0] = param[1]*aspectRatio;
            pparam[0] = pparam[1]*aspectRatio;
        }

        A[0] = param[0]; A[4] = param[1];
        A[2] = param[2]; A[5] = param[3];
        k[0] = param[4]; k[1] = param[5]; k[2] = param[6];
        k[3] = param[7];
        k[4] = param[8];

        if( !proceed )
            break;
        
        reprojErr = 0;

        for( i = 0, pos = 0; i < nimages; i++, pos += ni )
        {
            CvMat _Mi, _mi, _ri, _ti, _dpdr, _dpdt, _dpdf, _dpdc, _dpdk, _mp, _part;
            ni = npoints->data.i[i*npstep];

            cvGetRows( solver.param, &_ri, NINTRINSIC + i*6, NINTRINSIC + i*6 + 3 );
            cvGetRows( solver.param, &_ti, NINTRINSIC + i*6 + 3, NINTRINSIC + i*6 + 6 );

            cvGetCols( matM, &_Mi, pos, pos + ni );
            cvGetCols( _m, &_mi, pos, pos + ni );

            _Je->rows = _Ji->rows = _err->rows = ni*2;
            cvGetCols( _Je, &_dpdr, 0, 3 );
            cvGetCols( _Je, &_dpdt, 3, 6 );
            cvGetCols( _Ji, &_dpdf, 0, 2 );
            cvGetCols( _Ji, &_dpdc, 2, 4 );
            cvGetCols( _Ji, &_dpdk, 4, NINTRINSIC );
            cvReshape( _err, &_mp, 2, 1 );

            if( _JtJ || _JtErr )
            {
                cvProjectPoints2( &_Mi, &_ri, &_ti, &matA, &_k, &_mp, &_dpdr, &_dpdt,
                                  (flags & CV_CALIB_FIX_FOCAL_LENGTH) ? 0 : &_dpdf,
                                  (flags & CV_CALIB_FIX_PRINCIPAL_POINT) ? 0 : &_dpdc, &_dpdk,
                                  (flags & CV_CALIB_FIX_ASPECT_RATIO) ? aspectRatio : 0);
            }
            else
                cvProjectPoints2( &_Mi, &_ri, &_ti, &matA, &_k, &_mp );

            cvSub( &_mp, &_mi, &_mp );

            if( _JtJ || _JtErr )
            {
                cvGetSubRect( _JtJ, &_part, cvRect(0,0,NINTRINSIC,NINTRINSIC) );
                cvGEMM( _Ji, _Ji, 1, &_part, 1, &_part, CV_GEMM_A_T );

                cvGetSubRect( _JtJ, &_part, cvRect(NINTRINSIC+i*6,NINTRINSIC+i*6,6,6) );
                cvGEMM( _Je, _Je, 1, 0, 0, &_part, CV_GEMM_A_T );

                cvGetSubRect( _JtJ, &_part, cvRect(NINTRINSIC+i*6,0,6,NINTRINSIC) );
                cvGEMM( _Ji, _Je, 1, 0, 0, &_part, CV_GEMM_A_T );

                cvGetRows( _JtErr, &_part, 0, NINTRINSIC );
                cvGEMM( _Ji, _err, 1, &_part, 1, &_part, CV_GEMM_A_T );

                cvGetRows( _JtErr, &_part, NINTRINSIC + i*6, NINTRINSIC + (i+1)*6 );
                cvGEMM( _Je, _err, 1, 0, 0, &_part, CV_GEMM_A_T );
            }

            double errNorm = cvNorm( &_mp, 0, CV_L2 );
            reprojErr += errNorm*errNorm;
        }
        if( _errNorm )
            *_errNorm = reprojErr;
    }

    // 4. store the results
    cvConvert( &matA, cameraMatrix );
    cvConvert( &_k, distCoeffs );

    for( i = 0; i < nimages; i++ )
    {
        CvMat src, dst;
        if( rvecs )
        {
            src = cvMat( 3, 1, CV_64F, solver.param->data.db + NINTRINSIC + i*6 );
            if( rvecs->rows == nimages && rvecs->cols*CV_MAT_CN(rvecs->type) == 9 )
            {
                dst = cvMat( 3, 3, CV_MAT_DEPTH(rvecs->type),
                    rvecs->data.ptr + rvecs->step*i );
                cvRodrigues2( &src, &matA );
                cvConvert( &matA, &dst );
            }
            else
            {
                dst = cvMat( 3, 1, CV_MAT_DEPTH(rvecs->type), rvecs->rows == 1 ?
                    rvecs->data.ptr + i*CV_ELEM_SIZE(rvecs->type) :
                    rvecs->data.ptr + rvecs->step*i );
                cvConvert( &src, &dst );
            }
        }
        if( tvecs )
        {
            src = cvMat( 3, 1, CV_64F, solver.param->data.db + NINTRINSIC + i*6 + 3 );
            dst = cvMat( 3, 1, CV_MAT_TYPE(tvecs->type), tvecs->rows == 1 ?
                    tvecs->data.ptr + i*CV_ELEM_SIZE(tvecs->type) :
                    tvecs->data.ptr + tvecs->step*i );
            cvConvert( &src, &dst );
         }
    }
    
    return reprojErr;
}


void cvCalibrationMatrixValues( const CvMat *calibMatr, CvSize imgSize,
    double apertureWidth, double apertureHeight, double *fovx, double *fovy,
    double *focalLength, CvPoint2D64f *principalPoint, double *pasp )
{
    double alphax, alphay, mx, my;
    int imgWidth = imgSize.width, imgHeight = imgSize.height;

    /* Validate parameters. */

    if(calibMatr == 0)
        CV_Error(CV_StsNullPtr, "Some of parameters is a NULL pointer!");

    if(!CV_IS_MAT(calibMatr))
        CV_Error(CV_StsUnsupportedFormat, "Input parameters must be a matrices!");

    if(calibMatr->cols != 3 || calibMatr->rows != 3)
        CV_Error(CV_StsUnmatchedSizes, "Size of matrices must be 3x3!");

    alphax = cvmGet(calibMatr, 0, 0);
    alphay = cvmGet(calibMatr, 1, 1);
    assert(imgWidth != 0 && imgHeight != 0 && alphax != 0.0 && alphay != 0.0);

    /* Calculate pixel aspect ratio. */
    if(pasp)
        *pasp = alphay / alphax;

    /* Calculate number of pixel per realworld unit. */

    if(apertureWidth != 0.0 && apertureHeight != 0.0) {
        mx = imgWidth / apertureWidth;
        my = imgHeight / apertureHeight;
    } else {
        mx = 1.0;
        my = *pasp;
    }

    /* Calculate fovx and fovy. */

    if(fovx)
        *fovx = 2 * atan(imgWidth / (2 * alphax)) * 180.0 / CV_PI;

    if(fovy)
        *fovy = 2 * atan(imgHeight / (2 * alphay)) * 180.0 / CV_PI;

    /* Calculate focal length. */

    if(focalLength)
        *focalLength = alphax / mx;

    /* Calculate principle point. */

    if(principalPoint)
        *principalPoint = cvPoint2D64f(cvmGet(calibMatr, 0, 2) / mx, cvmGet(calibMatr, 1, 2) / my);
}


//////////////////////////////// Stereo Calibration ///////////////////////////////////

static int dbCmp( const void* _a, const void* _b )
{
    double a = *(const double*)_a;
    double b = *(const double*)_b;

    return (a > b) - (a < b);
}


double cvStereoCalibrate( const CvMat* _objectPoints, const CvMat* _imagePoints1,
                        const CvMat* _imagePoints2, const CvMat* _npoints,
                        CvMat* _cameraMatrix1, CvMat* _distCoeffs1,
                        CvMat* _cameraMatrix2, CvMat* _distCoeffs2,
                        CvSize imageSize, CvMat* matR, CvMat* matT,
                        CvMat* matE, CvMat* matF,
                        CvTermCriteria termCrit,
                        int flags )
{
    const int NINTRINSIC = 9;
    Ptr<CvMat> npoints, err, J_LR, Je, Ji, imagePoints[2], objectPoints, RT0;
    CvLevMarq solver;
    double reprojErr = 0;

    double A[2][9], dk[2][5]={{0,0,0,0,0},{0,0,0,0,0}}, rlr[9];
    CvMat K[2], Dist[2], om_LR, T_LR;
    CvMat R_LR = cvMat(3, 3, CV_64F, rlr);
    int i, k, p, ni = 0, ofs, nimages, pointsTotal, maxPoints = 0;
    int nparams;
    bool recomputeIntrinsics = false;
    double aspectRatio[2] = {0,0};

    CV_Assert( CV_IS_MAT(_imagePoints1) && CV_IS_MAT(_imagePoints2) &&
               CV_IS_MAT(_objectPoints) && CV_IS_MAT(_npoints) &&
               CV_IS_MAT(matR) && CV_IS_MAT(matT) );

    CV_Assert( CV_ARE_TYPES_EQ(_imagePoints1, _imagePoints2) &&
               CV_ARE_DEPTHS_EQ(_imagePoints1, _objectPoints) );

    CV_Assert( (_npoints->cols == 1 || _npoints->rows == 1) &&
               CV_MAT_TYPE(_npoints->type) == CV_32SC1 );

    nimages = _npoints->cols + _npoints->rows - 1;
    npoints = cvCreateMat( _npoints->rows, _npoints->cols, _npoints->type );
    cvCopy( _npoints, npoints );

    for( i = 0, pointsTotal = 0; i < nimages; i++ )
    {
        maxPoints = MAX(maxPoints, npoints->data.i[i]);
        pointsTotal += npoints->data.i[i];
    }

    objectPoints = cvCreateMat( _objectPoints->rows, _objectPoints->cols,
                                CV_64FC(CV_MAT_CN(_objectPoints->type)));
    cvConvert( _objectPoints, objectPoints );
    cvReshape( objectPoints, objectPoints, 3, 1 );

    for( k = 0; k < 2; k++ )
    {
        const CvMat* points = k == 0 ? _imagePoints1 : _imagePoints2;
        const CvMat* cameraMatrix = k == 0 ? _cameraMatrix1 : _cameraMatrix2;
        const CvMat* distCoeffs = k == 0 ? _distCoeffs1 : _distCoeffs2;

        int cn = CV_MAT_CN(_imagePoints1->type);
        CV_Assert( (CV_MAT_DEPTH(_imagePoints1->type) == CV_32F ||
                CV_MAT_DEPTH(_imagePoints1->type) == CV_64F) &&
               ((_imagePoints1->rows == pointsTotal && _imagePoints1->cols*cn == 2) ||
                (_imagePoints1->rows == 1 && _imagePoints1->cols == pointsTotal && cn == 2)) );

        K[k] = cvMat(3,3,CV_64F,A[k]);
        Dist[k] = cvMat(1,5,CV_64F,dk[k]);

        imagePoints[k] = cvCreateMat( points->rows, points->cols, CV_64FC(CV_MAT_CN(points->type)));
        cvConvert( points, imagePoints[k] );
        cvReshape( imagePoints[k], imagePoints[k], 2, 1 );

        if( flags & (CV_CALIB_FIX_INTRINSIC|CV_CALIB_USE_INTRINSIC_GUESS|
            CV_CALIB_FIX_ASPECT_RATIO|CV_CALIB_FIX_FOCAL_LENGTH) )
            cvConvert( cameraMatrix, &K[k] );

        if( flags & (CV_CALIB_FIX_INTRINSIC|CV_CALIB_USE_INTRINSIC_GUESS|
            CV_CALIB_FIX_K1|CV_CALIB_FIX_K2|CV_CALIB_FIX_K3) )
        {
            CvMat tdist = cvMat( distCoeffs->rows, distCoeffs->cols,
                CV_MAKETYPE(CV_64F,CV_MAT_CN(distCoeffs->type)), Dist[k].data.db );
            cvConvert( distCoeffs, &tdist );
        }

        if( !(flags & (CV_CALIB_FIX_INTRINSIC|CV_CALIB_USE_INTRINSIC_GUESS)))
        {
            cvCalibrateCamera2( objectPoints, imagePoints[k],
                npoints, imageSize, &K[k], &Dist[k], 0, 0, flags );
        }
    }

    if( flags & CV_CALIB_SAME_FOCAL_LENGTH )
    {
        static const int avg_idx[] = { 0, 4, 2, 5, -1 };
        for( k = 0; avg_idx[k] >= 0; k++ )
            A[0][avg_idx[k]] = A[1][avg_idx[k]] = (A[0][avg_idx[k]] + A[1][avg_idx[k]])*0.5;
    }

    if( flags & CV_CALIB_FIX_ASPECT_RATIO )
    {
        for( k = 0; k < 2; k++ )
            aspectRatio[k] = A[k][0]/A[k][4];
    }

    recomputeIntrinsics = (flags & CV_CALIB_FIX_INTRINSIC) == 0;

    err = cvCreateMat( maxPoints*2, 1, CV_64F );
    Je = cvCreateMat( maxPoints*2, 6, CV_64F );
    J_LR = cvCreateMat( maxPoints*2, 6, CV_64F );
    Ji = cvCreateMat( maxPoints*2, NINTRINSIC, CV_64F );
    cvZero( Ji );

    // we optimize for the inter-camera R(3),t(3), then, optionally,
    // for intrinisic parameters of each camera ((fx,fy,cx,cy,k1,k2,p1,p2) ~ 8 parameters).
    nparams = 6*(nimages+1) + (recomputeIntrinsics ? NINTRINSIC*2 : 0);

    // storage for initial [om(R){i}|t{i}] (in order to compute the median for each component)
    RT0 = cvCreateMat( 6, nimages, CV_64F );

    solver.init( nparams, 0, termCrit );
    if( recomputeIntrinsics )
    {
        uchar* imask = solver.mask->data.ptr + nparams - NINTRINSIC*2;
        if( flags & CV_CALIB_FIX_ASPECT_RATIO )
            imask[0] = imask[NINTRINSIC] = 0;
        if( flags & CV_CALIB_FIX_FOCAL_LENGTH )
            imask[0] = imask[1] = imask[NINTRINSIC] = imask[NINTRINSIC+1] = 0;
        if( flags & CV_CALIB_FIX_PRINCIPAL_POINT )
            imask[2] = imask[3] = imask[NINTRINSIC+2] = imask[NINTRINSIC+3] = 0;
        if( flags & CV_CALIB_ZERO_TANGENT_DIST )
            imask[6] = imask[7] = imask[NINTRINSIC+6] = imask[NINTRINSIC+7] = 0;
        if( flags & CV_CALIB_FIX_K1 )
            imask[4] = imask[NINTRINSIC+4] = 0;
        if( flags & CV_CALIB_FIX_K2 )
            imask[5] = imask[NINTRINSIC+5] = 0;
        if( flags & CV_CALIB_FIX_K3 )
            imask[8] = imask[NINTRINSIC+8] = 0;
    }

    /*
       Compute initial estimate of pose

       For each image, compute:
          R(om) is the rotation matrix of om
          om(R) is the rotation vector of R
          R_ref = R(om_right) * R(om_left)'
          T_ref_list = [T_ref_list; T_right - R_ref * T_left]
          om_ref_list = {om_ref_list; om(R_ref)]

       om = median(om_ref_list)
       T = median(T_ref_list)
    */
    for( i = ofs = 0; i < nimages; ofs += ni, i++ )
    {
        ni = npoints->data.i[i];
        CvMat objpt_i;
        double _om[2][3], r[2][9], t[2][3];
        CvMat om[2], R[2], T[2], imgpt_i[2];

        objpt_i = cvMat(1, ni, CV_64FC3, objectPoints->data.db + ofs*3);
        for( k = 0; k < 2; k++ )
        {
            imgpt_i[k] = cvMat(1, ni, CV_64FC2, imagePoints[k]->data.db + ofs*2);
            om[k] = cvMat(3, 1, CV_64F, _om[k]);
            R[k] = cvMat(3, 3, CV_64F, r[k]);
            T[k] = cvMat(3, 1, CV_64F, t[k]);

            // FIXME: here we ignore activePoints[k] because of
            // the limited API of cvFindExtrnisicCameraParams2
            cvFindExtrinsicCameraParams2( &objpt_i, &imgpt_i[k], &K[k], &Dist[k], &om[k], &T[k] );
            cvRodrigues2( &om[k], &R[k] );
            if( k == 0 )
            {
                // save initial om_left and T_left
                solver.param->data.db[(i+1)*6] = _om[0][0];
                solver.param->data.db[(i+1)*6 + 1] = _om[0][1];
                solver.param->data.db[(i+1)*6 + 2] = _om[0][2];
                solver.param->data.db[(i+1)*6 + 3] = t[0][0];
                solver.param->data.db[(i+1)*6 + 4] = t[0][1];
                solver.param->data.db[(i+1)*6 + 5] = t[0][2];
            }
        }
        cvGEMM( &R[1], &R[0], 1, 0, 0, &R[0], CV_GEMM_B_T );
        cvGEMM( &R[0], &T[0], -1, &T[1], 1, &T[1] );
        cvRodrigues2( &R[0], &T[0] );
        RT0->data.db[i] = t[0][0];
        RT0->data.db[i + nimages] = t[0][1];
        RT0->data.db[i + nimages*2] = t[0][2];
        RT0->data.db[i + nimages*3] = t[1][0];
        RT0->data.db[i + nimages*4] = t[1][1];
        RT0->data.db[i + nimages*5] = t[1][2];
    }

    // find the medians and save the first 6 parameters
    for( i = 0; i < 6; i++ )
    {
        qsort( RT0->data.db + i*nimages, nimages, CV_ELEM_SIZE(RT0->type), dbCmp );
        solver.param->data.db[i] = nimages % 2 != 0 ? RT0->data.db[i*nimages + nimages/2] :
            (RT0->data.db[i*nimages + nimages/2 - 1] + RT0->data.db[i*nimages + nimages/2])*0.5;
    }

    if( recomputeIntrinsics )
        for( k = 0; k < 2; k++ )
        {
            double* iparam = solver.param->data.db + (nimages+1)*6 + k*NINTRINSIC;
            if( flags & CV_CALIB_ZERO_TANGENT_DIST )
                dk[k][2] = dk[k][3] = 0;
            iparam[0] = A[k][0]; iparam[1] = A[k][4]; iparam[2] = A[k][2]; iparam[3] = A[k][5];
            iparam[4] = dk[k][0]; iparam[5] = dk[k][1]; iparam[6] = dk[k][2];
            iparam[7] = dk[k][3]; iparam[8] = dk[k][4];
        }

    om_LR = cvMat(3, 1, CV_64F, solver.param->data.db);
    T_LR = cvMat(3, 1, CV_64F, solver.param->data.db + 3);

    for(;;)
    {
        const CvMat* param = 0;
        CvMat tmpimagePoints;
        CvMat *JtJ = 0, *JtErr = 0;
        double *_errNorm = 0;
        double _omR[3], _tR[3];
        double _dr3dr1[9], _dr3dr2[9], /*_dt3dr1[9],*/ _dt3dr2[9], _dt3dt1[9], _dt3dt2[9];
        CvMat dr3dr1 = cvMat(3, 3, CV_64F, _dr3dr1);
        CvMat dr3dr2 = cvMat(3, 3, CV_64F, _dr3dr2);
        //CvMat dt3dr1 = cvMat(3, 3, CV_64F, _dt3dr1);
        CvMat dt3dr2 = cvMat(3, 3, CV_64F, _dt3dr2);
        CvMat dt3dt1 = cvMat(3, 3, CV_64F, _dt3dt1);
        CvMat dt3dt2 = cvMat(3, 3, CV_64F, _dt3dt2);
        CvMat om[2], T[2], imgpt_i[2];
        CvMat dpdrot_hdr, dpdt_hdr, dpdf_hdr, dpdc_hdr, dpdk_hdr;
        CvMat *dpdrot = &dpdrot_hdr, *dpdt = &dpdt_hdr, *dpdf = 0, *dpdc = 0, *dpdk = 0;

        if( !solver.updateAlt( param, JtJ, JtErr, _errNorm ))
            break;
        reprojErr = 0;

        cvRodrigues2( &om_LR, &R_LR );
        om[1] = cvMat(3,1,CV_64F,_omR);
        T[1] = cvMat(3,1,CV_64F,_tR);

        if( recomputeIntrinsics )
        {
            double* iparam = solver.param->data.db + (nimages+1)*6;
            double* ipparam = solver.prevParam->data.db + (nimages+1)*6;
            dpdf = &dpdf_hdr;
            dpdc = &dpdc_hdr;
            dpdk = &dpdk_hdr;
            if( flags & CV_CALIB_SAME_FOCAL_LENGTH )
            {
                iparam[NINTRINSIC] = iparam[0];
                iparam[NINTRINSIC+1] = iparam[1];
                ipparam[NINTRINSIC] = ipparam[0];
                ipparam[NINTRINSIC+1] = ipparam[1];
            }
            if( flags & CV_CALIB_FIX_ASPECT_RATIO )
            {
                iparam[0] = iparam[1]*aspectRatio[0];
                iparam[NINTRINSIC] = iparam[NINTRINSIC+1]*aspectRatio[1];
                ipparam[0] = ipparam[1]*aspectRatio[0];
                ipparam[NINTRINSIC] = ipparam[NINTRINSIC+1]*aspectRatio[1];
            }
            for( k = 0; k < 2; k++ )
            {
                A[k][0] = iparam[k*NINTRINSIC+0];
                A[k][4] = iparam[k*NINTRINSIC+1];
                A[k][2] = iparam[k*NINTRINSIC+2];
                A[k][5] = iparam[k*NINTRINSIC+3];
                dk[k][0] = iparam[k*NINTRINSIC+4];
                dk[k][1] = iparam[k*NINTRINSIC+5];
                dk[k][2] = iparam[k*NINTRINSIC+6];
                dk[k][3] = iparam[k*NINTRINSIC+7];
                dk[k][4] = iparam[k*NINTRINSIC+8];
            }
        }

        for( i = ofs = 0; i < nimages; ofs += ni, i++ )
        {
            ni = npoints->data.i[i];
            CvMat objpt_i, _part;

            om[0] = cvMat(3,1,CV_64F,solver.param->data.db+(i+1)*6);
            T[0] = cvMat(3,1,CV_64F,solver.param->data.db+(i+1)*6+3);

            if( JtJ || JtErr )
                cvComposeRT( &om[0], &T[0], &om_LR, &T_LR, &om[1], &T[1], &dr3dr1, 0,
                             &dr3dr2, 0, 0, &dt3dt1, &dt3dr2, &dt3dt2 );
            else
                cvComposeRT( &om[0], &T[0], &om_LR, &T_LR, &om[1], &T[1] );

            objpt_i = cvMat(1, ni, CV_64FC3, objectPoints->data.db + ofs*3);
            err->rows = Je->rows = J_LR->rows = Ji->rows = ni*2;
            cvReshape( err, &tmpimagePoints, 2, 1 );

            cvGetCols( Ji, &dpdf_hdr, 0, 2 );
            cvGetCols( Ji, &dpdc_hdr, 2, 4 );
            cvGetCols( Ji, &dpdk_hdr, 4, NINTRINSIC );
            cvGetCols( Je, &dpdrot_hdr, 0, 3 );
            cvGetCols( Je, &dpdt_hdr, 3, 6 );

            for( k = 0; k < 2; k++ )
            {
                double maxErr, l2err;
                imgpt_i[k] = cvMat(1, ni, CV_64FC2, imagePoints[k]->data.db + ofs*2);

                if( JtJ || JtErr )
                    cvProjectPoints2( &objpt_i, &om[k], &T[k], &K[k], &Dist[k],
                            &tmpimagePoints, dpdrot, dpdt, dpdf, dpdc, dpdk,
                            (flags & CV_CALIB_FIX_ASPECT_RATIO) ? aspectRatio[k] : 0);
                else
                    cvProjectPoints2( &objpt_i, &om[k], &T[k], &K[k], &Dist[k], &tmpimagePoints );
                cvSub( &tmpimagePoints, &imgpt_i[k], &tmpimagePoints );

                l2err = cvNorm( &tmpimagePoints, 0, CV_L2 );
                maxErr = cvNorm( &tmpimagePoints, 0, CV_C );

                if( JtJ || JtErr )
                {
                    int iofs = (nimages+1)*6 + k*NINTRINSIC, eofs = (i+1)*6;
                    assert( JtJ && JtErr );

                    if( k == 1 )
                    {
                        // d(err_{x|y}R) ~ de3
                        // convert de3/{dr3,dt3} => de3{dr1,dt1} & de3{dr2,dt2}
                        for( p = 0; p < ni*2; p++ )
                        {
                            CvMat de3dr3 = cvMat( 1, 3, CV_64F, Je->data.ptr + Je->step*p );
                            CvMat de3dt3 = cvMat( 1, 3, CV_64F, de3dr3.data.db + 3 );
                            CvMat de3dr2 = cvMat( 1, 3, CV_64F, J_LR->data.ptr + J_LR->step*p );
                            CvMat de3dt2 = cvMat( 1, 3, CV_64F, de3dr2.data.db + 3 );
                            double _de3dr1[3], _de3dt1[3];
                            CvMat de3dr1 = cvMat( 1, 3, CV_64F, _de3dr1 );
                            CvMat de3dt1 = cvMat( 1, 3, CV_64F, _de3dt1 );

                            cvMatMul( &de3dr3, &dr3dr1, &de3dr1 );
                            cvMatMul( &de3dt3, &dt3dt1, &de3dt1 );

                            cvMatMul( &de3dr3, &dr3dr2, &de3dr2 );
                            cvMatMulAdd( &de3dt3, &dt3dr2, &de3dr2, &de3dr2 );

                            cvMatMul( &de3dt3, &dt3dt2, &de3dt2 );

                            cvCopy( &de3dr1, &de3dr3 );
                            cvCopy( &de3dt1, &de3dt3 );
                        }

                        cvGetSubRect( JtJ, &_part, cvRect(0, 0, 6, 6) );
                        cvGEMM( J_LR, J_LR, 1, &_part, 1, &_part, CV_GEMM_A_T );

                        cvGetSubRect( JtJ, &_part, cvRect(eofs, 0, 6, 6) );
                        cvGEMM( J_LR, Je, 1, 0, 0, &_part, CV_GEMM_A_T );

                        cvGetRows( JtErr, &_part, 0, 6 );
                        cvGEMM( J_LR, err, 1, &_part, 1, &_part, CV_GEMM_A_T );
                    }

                    cvGetSubRect( JtJ, &_part, cvRect(eofs, eofs, 6, 6) );
                    cvGEMM( Je, Je, 1, &_part, 1, &_part, CV_GEMM_A_T );

                    cvGetRows( JtErr, &_part, eofs, eofs + 6 );
                    cvGEMM( Je, err, 1, &_part, 1, &_part, CV_GEMM_A_T );

                    if( recomputeIntrinsics )
                    {
                        cvGetSubRect( JtJ, &_part, cvRect(iofs, iofs, NINTRINSIC, NINTRINSIC) );
                        cvGEMM( Ji, Ji, 1, &_part, 1, &_part, CV_GEMM_A_T );
                        cvGetSubRect( JtJ, &_part, cvRect(iofs, eofs, NINTRINSIC, 6) );
                        cvGEMM( Je, Ji, 1, &_part, 1, &_part, CV_GEMM_A_T );
                        if( k == 1 )
                        {
                            cvGetSubRect( JtJ, &_part, cvRect(iofs, 0, NINTRINSIC, 6) );
                            cvGEMM( J_LR, Ji, 1, &_part, 1, &_part, CV_GEMM_A_T );
                        }
                        cvGetRows( JtErr, &_part, iofs, iofs + NINTRINSIC );
                        cvGEMM( Ji, err, 1, &_part, 1, &_part, CV_GEMM_A_T );
                    }
                }

                reprojErr += l2err*l2err;
            }
        }
        if(_errNorm)
            *_errNorm = reprojErr;
    }

    cvRodrigues2( &om_LR, &R_LR );
    if( matR->rows == 1 || matR->cols == 1 )
        cvConvert( &om_LR, matR );
    else
        cvConvert( &R_LR, matR );
    cvConvert( &T_LR, matT );

    if( recomputeIntrinsics )
    {
        cvConvert( &K[0], _cameraMatrix1 );
        cvConvert( &K[1], _cameraMatrix2 );

        for( k = 0; k < 2; k++ )
        {
            CvMat* distCoeffs = k == 0 ? _distCoeffs1 : _distCoeffs2;
            CvMat tdist = cvMat( distCoeffs->rows, distCoeffs->cols,
                CV_MAKETYPE(CV_64F,CV_MAT_CN(distCoeffs->type)), Dist[k].data.db );
            cvConvert( &tdist, distCoeffs );
        }
    }

    if( matE || matF )
    {
        double* t = T_LR.data.db;
        double tx[] =
        {
            0, -t[2], t[1],
            t[2], 0, -t[0],
            -t[1], t[0], 0
        };
        CvMat Tx = cvMat(3, 3, CV_64F, tx);
        double e[9], f[9];
        CvMat E = cvMat(3, 3, CV_64F, e);
        CvMat F = cvMat(3, 3, CV_64F, f);
        cvMatMul( &Tx, &R_LR, &E );
        if( matE )
            cvConvert( &E, matE );
        if( matF )
        {
            double ik[9];
            CvMat iK = cvMat(3, 3, CV_64F, ik);
            cvInvert(&K[1], &iK);
            cvGEMM( &iK, &E, 1, 0, 0, &E, CV_GEMM_A_T );
            cvInvert(&K[0], &iK);
            cvMatMul(&E, &iK, &F);
            cvConvertScale( &F, matF, fabs(f[8]) > 0 ? 1./f[8] : 1 );
        }
    }
    
    return reprojErr;
}


static void
icvGetRectangles( const CvMat* cameraMatrix, const CvMat* distCoeffs,
                 const CvMat* R, const CvMat* newCameraMatrix, CvSize imgSize,
                 cv::Rect_<float>& inner, cv::Rect_<float>& outer )
{
    const int N = 9;
    int x, y, k;
    cv::Ptr<CvMat> _pts = cvCreateMat(1, N*N, CV_32FC2);
    CvPoint2D32f* pts = (CvPoint2D32f*)(_pts->data.ptr);
    
    for( y = k = 0; y < N; y++ )
        for( x = 0; x < N; x++ )
            pts[k++] = cvPoint2D32f((float)x*imgSize.width/(N-1),
                                    (float)y*imgSize.height/(N-1));
    
    cvUndistortPoints(_pts, _pts, cameraMatrix, distCoeffs, R, newCameraMatrix);
    
    float iX0=-FLT_MAX, iX1=FLT_MAX, iY0=-FLT_MAX, iY1=FLT_MAX;
    float oX0=FLT_MAX, oX1=-FLT_MAX, oY0=FLT_MAX, oY1=-FLT_MAX;
    // find the inscribed rectangle.
    // the code will likely not work with extreme rotation matrices (R) (>45%)
    for( y = k = 0; y < N; y++ )
        for( x = 0; x < N; x++ )
        {
            CvPoint2D32f p = pts[k++];
            oX0 = MIN(oX0, p.x);
            oX1 = MAX(oX1, p.x);
            oY0 = MIN(oY0, p.y);
            oY1 = MAX(oY1, p.y);
            
            if( x == 0 )
                iX0 = MAX(iX0, p.x);
            if( x == N-1 )
                iX1 = MIN(iX1, p.x); 
            if( y == 0 )
                iY0 = MAX(iY0, p.y);
            if( y == N-1 )
                iY1 = MIN(iY1, p.y);
        }
    inner = cv::Rect_<float>(iX0, iY0, iX1-iX0, iY1-iY0);
    outer = cv::Rect_<float>(oX0, oY0, oX1-oX0, oY1-oY0);
}  


void cvStereoRectify( const CvMat* _cameraMatrix1, const CvMat* _cameraMatrix2,
                      const CvMat* _distCoeffs1, const CvMat* _distCoeffs2,
                      CvSize imageSize, const CvMat* matR, const CvMat* matT,
                      CvMat* _R1, CvMat* _R2, CvMat* _P1, CvMat* _P2,
                      CvMat* matQ, int flags, double alpha, CvSize newImgSize,
                      CvRect* roi1, CvRect* roi2 )
{
    double _om[3], _t[3], _uu[3]={0,0,0}, _r_r[3][3], _pp[3][4];
    double _ww[3], _wr[3][3], _z[3] = {0,0,0}, _ri[3][3];
    cv::Rect_<float> inner1, inner2, outer1, outer2;
    
    CvMat om  = cvMat(3, 1, CV_64F, _om);
    CvMat t   = cvMat(3, 1, CV_64F, _t);
    CvMat uu  = cvMat(3, 1, CV_64F, _uu);
    CvMat r_r = cvMat(3, 3, CV_64F, _r_r);
    CvMat pp  = cvMat(3, 4, CV_64F, _pp);
    CvMat ww  = cvMat(3, 1, CV_64F, _ww); // temps
    CvMat wR  = cvMat(3, 3, CV_64F, _wr);
    CvMat Z   = cvMat(3, 1, CV_64F, _z);
    CvMat Ri  = cvMat(3, 3, CV_64F, _ri);
    double nx = imageSize.width, ny = imageSize.height;
    int i, k;

    if( matR->rows == 3 && matR->cols == 3 )
        cvRodrigues2(matR, &om);          // get vector rotation
    else
        cvConvert(matR, &om); // it's already a rotation vector
    cvConvertScale(&om, &om, -0.5); // get average rotation
    cvRodrigues2(&om, &r_r);        // rotate cameras to same orientation by averaging
    cvMatMul(&r_r, matT, &t);

    int idx = fabs(_t[0]) > fabs(_t[1]) ? 0 : 1;
    double c = _t[idx], nt = cvNorm(&t, 0, CV_L2);
    _uu[idx] = c > 0 ? 1 : -1;

    // calculate global Z rotation
    cvCrossProduct(&t,&uu,&ww);
    double nw = cvNorm(&ww, 0, CV_L2);
    cvConvertScale(&ww, &ww, acos(fabs(c)/nt)/nw);
    cvRodrigues2(&ww, &wR);

    // apply to both views
    cvGEMM(&wR, &r_r, 1, 0, 0, &Ri, CV_GEMM_B_T);
    cvConvert( &Ri, _R1 );
    cvGEMM(&wR, &r_r, 1, 0, 0, &Ri, 0);
    cvConvert( &Ri, _R2 );
    cvMatMul(&r_r, matT, &t);

    // calculate projection/camera matrices
    // these contain the relevant rectified image internal params (fx, fy=fx, cx, cy)
    double fc_new = DBL_MAX;
    CvPoint2D64f cc_new[2] = {{0,0}, {0,0}};

	for( k = 0; k < 2; k++ ) {
        const CvMat* A = k == 0 ? _cameraMatrix1 : _cameraMatrix2;
        const CvMat* Dk = k == 0 ? _distCoeffs1 : _distCoeffs2;
        double dk1 = Dk ? cvmGet(Dk, 0, 0) : 0;
        double fc = cvmGet(A,idx^1,idx^1);
		if( dk1 < 0 ) {
 			fc *= 1 + dk1*(nx*nx + ny*ny)/(4*fc*fc);
		}
        fc_new = MIN(fc_new, fc);
	}

    for( k = 0; k < 2; k++ )
    {
        const CvMat* A = k == 0 ? _cameraMatrix1 : _cameraMatrix2;
        const CvMat* Dk = k == 0 ? _distCoeffs1 : _distCoeffs2;
        CvPoint2D32f _pts[4];
        CvPoint3D32f _pts_3[4];
        CvMat pts = cvMat(1, 4, CV_32FC2, _pts);
        CvMat pts_3 = cvMat(1, 4, CV_32FC3, _pts_3);

        for( i = 0; i < 4; i++ )
        {
			int j = (i<2) ? 0 : 1;
            _pts[i].x = (float)((i % 2)*(nx-1));
	        _pts[i].y = (float)(j*(ny-1));
        }
        cvUndistortPoints( &pts, &pts, A, Dk, 0, 0 );
        cvConvertPointsHomogeneous( &pts, &pts_3 );

		//Change camera matrix to have cc=[0,0] and fc = fc_new
		double _a_tmp[3][3];
		CvMat A_tmp  = cvMat(3, 3, CV_64F, _a_tmp);
		_a_tmp[0][0]=fc_new;
		_a_tmp[1][1]=fc_new;
		_a_tmp[0][2]=0.0;
		_a_tmp[1][2]=0.0;
		cvProjectPoints2( &pts_3, k == 0 ? _R1 : _R2, &Z, &A_tmp, 0, &pts );
        CvScalar avg = cvAvg(&pts);
        cc_new[k].x = (nx-1)/2 - avg.val[0];
        cc_new[k].y = (ny-1)/2 - avg.val[1];
    }

    // vertical focal length must be the same for both images to keep the epipolar constraint
    // (for horizontal epipolar lines -- TBD: check for vertical epipolar lines)
    // use fy for fx also, for simplicity

    // For simplicity, set the principal points for both cameras to be the average
    // of the two principal points (either one of or both x- and y- coordinates)
    if( flags & CV_CALIB_ZERO_DISPARITY )
    {
        cc_new[0].x = cc_new[1].x = (cc_new[0].x + cc_new[1].x)*0.5;
        cc_new[0].y = cc_new[1].y = (cc_new[0].y + cc_new[1].y)*0.5;
    }
    else if( idx == 0 ) // horizontal stereo
        cc_new[0].y = cc_new[1].y = (cc_new[0].y + cc_new[1].y)*0.5;
    else // vertical stereo
        cc_new[0].x = cc_new[1].x = (cc_new[0].x + cc_new[1].x)*0.5;

    cvZero( &pp );
    _pp[0][0] = _pp[1][1] = fc_new;
    _pp[0][2] = cc_new[0].x;
    _pp[1][2] = cc_new[0].y;
    _pp[2][2] = 1;
    cvConvert(&pp, _P1);

    _pp[0][2] = cc_new[1].x;
    _pp[1][2] = cc_new[1].y;
    _pp[idx][3] = _t[idx]*fc_new; // baseline * focal length
    cvConvert(&pp, _P2);
    
    alpha = MIN(alpha, 1.);
    
    icvGetRectangles( _cameraMatrix1, _distCoeffs1, _R1, _P1, imageSize, inner1, outer1 );
    icvGetRectangles( _cameraMatrix2, _distCoeffs2, _R2, _P2, imageSize, inner2, outer2 );
    
    {
    newImgSize = newImgSize.width*newImgSize.height != 0 ? newImgSize : imageSize;
    double cx1_0 = cc_new[0].x;
    double cy1_0 = cc_new[0].y;
    double cx2_0 = cc_new[1].x;
    double cy2_0 = cc_new[1].y;
    double cx1 = newImgSize.width*cx1_0/imageSize.width;
    double cy1 = newImgSize.height*cy1_0/imageSize.height;
    double cx2 = newImgSize.width*cx2_0/imageSize.width;
    double cy2 = newImgSize.height*cy2_0/imageSize.height;
    double s = 1.;
    
    if( alpha >= 0 )
    {
        double s0 = std::max(std::max(std::max((double)cx1/(cx1_0 - inner1.x), (double)cy1/(cy1_0 - inner1.y)),
                            (double)(newImgSize.width - cx1)/(inner1.x + inner1.width - cx1_0)),
                        (double)(newImgSize.height - cy1)/(inner1.y + inner1.height - cy1_0));
        s0 = std::max(std::max(std::max(std::max((double)cx2/(cx2_0 - inner2.x), (double)cy2/(cy2_0 - inner2.y)),
                         (double)(newImgSize.width - cx2)/(inner2.x + inner2.width - cx2_0)),
                     (double)(newImgSize.height - cy2)/(inner2.y + inner2.height - cy2_0)),
                 s0);
        
        double s1 = std::min(std::min(std::min((double)cx1/(cx1_0 - outer1.x), (double)cy1/(cy1_0 - outer1.y)),
                            (double)(newImgSize.width - cx1)/(outer1.x + outer1.width - cx1_0)),
                        (double)(newImgSize.height - cy1)/(outer1.y + outer1.height - cy1_0));
        s1 = std::min(std::min(std::min(std::min((double)cx2/(cx2_0 - outer2.x), (double)cy2/(cy2_0 - outer2.y)),
                         (double)(newImgSize.width - cx2)/(outer2.x + outer2.width - cx2_0)),
                     (double)(newImgSize.height - cy2)/(outer2.y + outer2.height - cy2_0)),
                 s1);
        
        s = s0*(1 - alpha) + s1*alpha;
    }
        
    fc_new *= s;
    cc_new[0] = cvPoint2D64f(cx1, cy1);
    cc_new[1] = cvPoint2D64f(cx2, cy2);
    
    cvmSet(_P1, 0, 0, fc_new);
    cvmSet(_P1, 1, 1, fc_new);
    cvmSet(_P1, 0, 2, cx1);
    cvmSet(_P1, 1, 2, cy1);
    
    cvmSet(_P2, 0, 0, fc_new);
    cvmSet(_P2, 1, 1, fc_new);
    cvmSet(_P2, 0, 2, cx2);
    cvmSet(_P2, 1, 2, cy2);
    cvmSet(_P2, idx, 3, s*cvmGet(_P2, idx, 3));
    
    if(roi1)
    {
        *roi1 = cv::Rect(cvCeil((inner1.x - cx1_0)*s + cx1),
                     cvCeil((inner1.y - cy1_0)*s + cy1),
                     cvFloor(inner1.width*s), cvFloor(inner1.height*s))
            & cv::Rect(0, 0, newImgSize.width, newImgSize.height);
    }
    
    if(roi2)
    {
        *roi2 = cv::Rect(cvCeil((inner2.x - cx2_0)*s + cx2),
                     cvCeil((inner2.y - cy2_0)*s + cy2),
                     cvFloor(inner2.width*s), cvFloor(inner2.height*s))
            & cv::Rect(0, 0, newImgSize.width, newImgSize.height);
    }
    }

    if( matQ )
    {
        double q[] =
        {
            1, 0, 0, -cc_new[0].x,
            0, 1, 0, -cc_new[0].y,
            0, 0, 0, fc_new,
            0, 0, 1./_t[idx],
            (idx == 0 ? cc_new[0].x - cc_new[1].x : cc_new[0].y - cc_new[1].y)/_t[idx]
        };
        CvMat Q = cvMat(4, 4, CV_64F, q);
        cvConvert( &Q, matQ );
    }
}
                        

void cvGetOptimalNewCameraMatrix( const CvMat* cameraMatrix, const CvMat* distCoeffs,
                                  CvSize imgSize, double alpha,
                                  CvMat* newCameraMatrix, CvSize newImgSize,
                                  CvRect* validPixROI )
{
    cv::Rect_<float> inner, outer;
    icvGetRectangles( cameraMatrix, distCoeffs, 0, cameraMatrix, imgSize, inner, outer );
    
    newImgSize = newImgSize.width*newImgSize.height != 0 ? newImgSize : imgSize;
    
    double M[3][3];
    CvMat matM = cvMat(3, 3, CV_64F, M);
    cvConvert(cameraMatrix, &matM);
    
    double cx0 = M[0][2];
    double cy0 = M[1][2];
    double cx = (newImgSize.width-1)*0.5;
    double cy = (newImgSize.height-1)*0.5;
    
    double s0 = std::max(std::max(std::max((double)cx/(cx0 - inner.x), (double)cy/(cy0 - inner.y)),
                        (double)cx/(inner.x + inner.width - cx0)),
                    (double)cy/(inner.y + inner.height - cy0));
    double s1 = std::min(std::min(std::min((double)cx/(cx0 - outer.x), (double)cy/(cy0 - outer.y)),
                        (double)cx/(outer.x + outer.width - cx0)),
                    (double)cy/(outer.y + outer.height - cy0));
    double s = s0*(1 - alpha) + s1*alpha;
    
    M[0][0] *= s;
    M[1][1] *= s;
    M[0][2] = cx;
    M[1][2] = cy;
    cvConvert(&matM, newCameraMatrix);
    
    if( validPixROI )
    {
        inner = cv::Rect_<float>((float)((inner.x - cx0)*s + cx),
                             (float)((inner.y - cy0)*s + cy),
                             (float)(inner.width*s),
                             (float)(inner.height*s));
        cv::Rect r(cvCeil(inner.x), cvCeil(inner.y), cvFloor(inner.width), cvFloor(inner.height));
        r &= cv::Rect(0, 0, newImgSize.width, newImgSize.height);
        *validPixROI = r;
    }
}


CV_IMPL int cvStereoRectifyUncalibrated(
    const CvMat* _points1, const CvMat* _points2,
    const CvMat* F0, CvSize imgSize,
    CvMat* _H1, CvMat* _H2, double threshold )
{
    Ptr<CvMat> _m1, _m2, _lines1, _lines2;

    int i, j, npoints;
    double cx, cy;
    double u[9], v[9], w[9], f[9], h1[9], h2[9], h0[9], e2[3];
    CvMat E2 = cvMat( 3, 1, CV_64F, e2 );
    CvMat U = cvMat( 3, 3, CV_64F, u );
    CvMat V = cvMat( 3, 3, CV_64F, v );
    CvMat W = cvMat( 3, 3, CV_64F, w );
    CvMat F = cvMat( 3, 3, CV_64F, f );
    CvMat H1 = cvMat( 3, 3, CV_64F, h1 );
    CvMat H2 = cvMat( 3, 3, CV_64F, h2 );
    CvMat H0 = cvMat( 3, 3, CV_64F, h0 );

    CvPoint2D64f* m1;
    CvPoint2D64f* m2;
    CvPoint3D64f* lines1;
    CvPoint3D64f* lines2;

    CV_Assert( CV_IS_MAT(_points1) && CV_IS_MAT(_points2) &&
        (_points1->rows == 1 || _points1->cols == 1) &&
        (_points2->rows == 1 || _points2->cols == 1) &&
        CV_ARE_SIZES_EQ(_points1, _points2) );

    npoints = _points1->rows * _points1->cols * CV_MAT_CN(_points1->type) / 2;

    _m1 = cvCreateMat( _points1->rows, _points1->cols, CV_64FC(CV_MAT_CN(_points1->type)) );
    _m2 = cvCreateMat( _points2->rows, _points2->cols, CV_64FC(CV_MAT_CN(_points2->type)) );
    _lines1 = cvCreateMat( 1, npoints, CV_64FC3 );
    _lines2 = cvCreateMat( 1, npoints, CV_64FC3 );

    cvConvert( F0, &F );

    cvSVD( (CvMat*)&F, &W, &U, &V, CV_SVD_U_T + CV_SVD_V_T );
    W.data.db[8] = 0.;
    cvGEMM( &U, &W, 1, 0, 0, &W, CV_GEMM_A_T );
    cvMatMul( &W, &V, &F );

    cx = cvRound( (imgSize.width-1)*0.5 );
    cy = cvRound( (imgSize.height-1)*0.5 );

    cvZero( _H1 );
    cvZero( _H2 );

    cvConvert( _points1, _m1 );
    cvConvert( _points2, _m2 );
    cvReshape( _m1, _m1, 2, 1 );
    cvReshape( _m2, _m2, 2, 1 );

    m1 = (CvPoint2D64f*)_m1->data.ptr;
    m2 = (CvPoint2D64f*)_m2->data.ptr;
    lines1 = (CvPoint3D64f*)_lines1->data.ptr;
    lines2 = (CvPoint3D64f*)_lines2->data.ptr;

    if( threshold > 0 )
    {
        cvComputeCorrespondEpilines( _m1, 1, &F, _lines1 );
        cvComputeCorrespondEpilines( _m2, 2, &F, _lines2 );

        // measure distance from points to the corresponding epilines, mark outliers
        for( i = j = 0; i < npoints; i++ )
        {
            if( fabs(m1[i].x*lines2[i].x +
                     m1[i].y*lines2[i].y +
                     lines2[i].z) <= threshold &&
                fabs(m2[i].x*lines1[i].x +
                     m2[i].y*lines1[i].y +
                     lines1[i].z) <= threshold )
            {
                if( j > i )
                {
                    m1[j] = m1[i];
                    m2[j] = m2[i];
                }
                j++;
            }
        }

        npoints = j;
        if( npoints == 0 )
            return 0;
    }

    _m1->cols = _m2->cols = npoints;
    memcpy( E2.data.db, U.data.db + 6, sizeof(e2));
    cvScale( &E2, &E2, e2[2] > 0 ? 1 : -1 );

    double t[] =
    {
        1, 0, -cx,
        0, 1, -cy,
        0, 0, 1
    };
    CvMat T = cvMat(3, 3, CV_64F, t);
    cvMatMul( &T, &E2, &E2 );

    int mirror = e2[0] < 0;
    double d = MAX(sqrt(e2[0]*e2[0] + e2[1]*e2[1]),DBL_EPSILON);
    double alpha = e2[0]/d;
    double beta = e2[1]/d;
    double r[] =
    {
        alpha, beta, 0,
        -beta, alpha, 0,
        0, 0, 1
    };
    CvMat R = cvMat(3, 3, CV_64F, r);
    cvMatMul( &R, &T, &T );
    cvMatMul( &R, &E2, &E2 );
    double invf = fabs(e2[2]) < 1e-6*fabs(e2[0]) ? 0 : -e2[2]/e2[0];
    double k[] =
    {
        1, 0, 0,
        0, 1, 0,
        invf, 0, 1
    };
    CvMat K = cvMat(3, 3, CV_64F, k);
    cvMatMul( &K, &T, &H2 );
    cvMatMul( &K, &E2, &E2 );

    double it[] =
    {
        1, 0, cx,
        0, 1, cy,
        0, 0, 1
    };
    CvMat iT = cvMat( 3, 3, CV_64F, it );
    cvMatMul( &iT, &H2, &H2 );

    memcpy( E2.data.db, U.data.db + 6, sizeof(e2));
    cvScale( &E2, &E2, e2[2] > 0 ? 1 : -1 );

    double e2_x[] =
    {
        0, -e2[2], e2[1],
       e2[2], 0, -e2[0],
       -e2[1], e2[0], 0
    };
    double e2_111[] =
    {
        e2[0], e2[0], e2[0],
        e2[1], e2[1], e2[1],
        e2[2], e2[2], e2[2],
    };
    CvMat E2_x = cvMat(3, 3, CV_64F, e2_x);
    CvMat E2_111 = cvMat(3, 3, CV_64F, e2_111);
    cvMatMulAdd(&E2_x, &F, &E2_111, &H0 );
    cvMatMul(&H2, &H0, &H0);
    CvMat E1=cvMat(3, 1, CV_64F, V.data.db+6);
    cvMatMul(&H0, &E1, &E1);

    cvPerspectiveTransform( _m1, _m1, &H0 );
    cvPerspectiveTransform( _m2, _m2, &H2 );
    CvMat A = cvMat( 1, npoints, CV_64FC3, lines1 ), BxBy, B;
    double a[9], atb[3], x[3];
    CvMat AtA = cvMat( 3, 3, CV_64F, a );
    CvMat AtB = cvMat( 3, 1, CV_64F, atb );
    CvMat X = cvMat( 3, 1, CV_64F, x );
    cvConvertPointsHomogeneous( _m1, &A );
    cvReshape( &A, &A, 1, npoints );
    cvReshape( _m2, &BxBy, 1, npoints );
    cvGetCol( &BxBy, &B, 0 );
    cvGEMM( &A, &A, 1, 0, 0, &AtA, CV_GEMM_A_T );
    cvGEMM( &A, &B, 1, 0, 0, &AtB, CV_GEMM_A_T );
    cvSolve( &AtA, &AtB, &X, CV_SVD_SYM );

    double ha[] =
    {
        x[0], x[1], x[2],
        0, 1, 0,
        0, 0, 1
    };
    CvMat Ha = cvMat(3, 3, CV_64F, ha);
    cvMatMul( &Ha, &H0, &H1 );
    cvPerspectiveTransform( _m1, _m1, &Ha );

    if( mirror )
    {
        double mm[] = { -1, 0, cx*2, 0, -1, cy*2, 0, 0, 1 };
        CvMat MM = cvMat(3, 3, CV_64F, mm);
        cvMatMul( &MM, &H1, &H1 );
        cvMatMul( &MM, &H2, &H2 );
    }

    cvConvert( &H1, _H1 );
    cvConvert( &H2, _H2 );

    return 1;
}


CV_IMPL void cvReprojectImageTo3D(
    const CvArr* disparityImage,
    CvArr* _3dImage, const CvMat* matQ,
    int handleMissingValues )
{
    const double bigZ = 10000.;
    double q[4][4];
    CvMat Q = cvMat(4, 4, CV_64F, q);
    CvMat sstub, *src = cvGetMat( disparityImage, &sstub );
    CvMat dstub, *dst = cvGetMat( _3dImage, &dstub );
    int stype = CV_MAT_TYPE(src->type), dtype = CV_MAT_TYPE(dst->type);
    int x, y, rows = src->rows, cols = src->cols;
    float* sbuf = (float*)cvStackAlloc( cols*sizeof(sbuf[0]) );
    float* dbuf = (float*)cvStackAlloc( cols*3*sizeof(dbuf[0]) );
    double minDisparity = FLT_MAX;

    CV_Assert( CV_ARE_SIZES_EQ(src, dst) &&
        (CV_MAT_TYPE(stype) == CV_8UC1 || CV_MAT_TYPE(stype) == CV_16SC1 ||
         CV_MAT_TYPE(stype) == CV_32SC1 || CV_MAT_TYPE(stype) == CV_32FC1) &&
        (CV_MAT_TYPE(dtype) == CV_16SC3 || CV_MAT_TYPE(dtype) == CV_32SC3 ||
        CV_MAT_TYPE(dtype) == CV_32FC3) );

    cvConvert( matQ, &Q );

    // NOTE: here we quietly assume that at least one pixel in the disparity map is not defined.
    // and we set the corresponding Z's to some fixed big value.
    if( handleMissingValues )
        cvMinMaxLoc( disparityImage, &minDisparity, 0, 0, 0 ); 

    for( y = 0; y < rows; y++ )
    {
        const float* sptr = (const float*)(src->data.ptr + src->step*y);
        float* dptr0 = (float*)(dst->data.ptr + dst->step*y), *dptr = dptr0;
        double qx = q[0][1]*y + q[0][3], qy = q[1][1]*y + q[1][3];
        double qz = q[2][1]*y + q[2][3], qw = q[3][1]*y + q[3][3];

        if( stype == CV_8UC1 )
        {
            const uchar* sptr0 = (const uchar*)sptr;
            for( x = 0; x < cols; x++ )
                sbuf[x] = (float)sptr0[x];
            sptr = sbuf;
        }
        else if( stype == CV_16SC1 )
        {
            const short* sptr0 = (const short*)sptr;
            for( x = 0; x < cols; x++ )
                sbuf[x] = (float)sptr0[x];
            sptr = sbuf;
        }
        else if( stype == CV_32SC1 )
        {
            const int* sptr0 = (const int*)sptr;
            for( x = 0; x < cols; x++ )
                sbuf[x] = (float)sptr0[x];
            sptr = sbuf;
        }
                
        if( dtype != CV_32FC3 )
            dptr = dbuf;

        for( x = 0; x < cols; x++, qx += q[0][0], qy += q[1][0], qz += q[2][0], qw += q[3][0] )
        {
            double d = sptr[x];
            double iW = 1./(qw + q[3][2]*d);
            double X = (qx + q[0][2]*d)*iW;
            double Y = (qy + q[1][2]*d)*iW;
            double Z = (qz + q[2][2]*d)*iW;
            if( fabs(d-minDisparity) <= FLT_EPSILON )
                Z = bigZ;

            dptr[x*3] = (float)X;
            dptr[x*3+1] = (float)Y;
            dptr[x*3+2] = (float)Z;
        }

        if( dtype == CV_16SC3 )
        {
            for( x = 0; x < cols*3; x++ )
            {
                int ival = cvRound(dptr[x]);
                ((short*)dptr0)[x] = CV_CAST_16S(ival);
            }
        }
        else if( dtype == CV_32SC3 )
        {
            for( x = 0; x < cols*3; x++ )
            {
                int ival = cvRound(dptr[x]);
                ((int*)dptr0)[x] = ival;
            }
        }
    }
}


namespace cv
{

static void collectCalibrationData( const vector<vector<Point3f> >& objectPoints,
                                    const vector<vector<Point2f> >& imagePoints,
                                    const vector<vector<Point2f> >& imagePoints2,
                                    Mat& objPtMat, Mat& imgPtMat, Mat* imgPtMat2,
                                    Mat& npoints )
{
    size_t i, j = 0, ni = 0, nimages = objectPoints.size(), total = 0;
    CV_Assert(nimages > 0 && nimages == imagePoints.size() &&
        (!imgPtMat2 || nimages == imagePoints2.size()));

    for( i = 0; i < nimages; i++ )
    {
        ni = objectPoints[i].size();
        CV_Assert(ni == imagePoints[i].size() && (!imgPtMat2 || ni == imagePoints2[i].size()));
        total += ni;
    }

    npoints.create(1, (int)nimages, CV_32S);
    objPtMat.create(1, (int)total, DataType<Point3f>::type);
    imgPtMat.create(1, (int)total, DataType<Point2f>::type);
    Point2f* imgPtData2 = 0;
    
    if( imgPtMat2 )
    {
        imgPtMat2->create(1, (int)total, DataType<Point2f>::type);
        imgPtData2 = imgPtMat2->ptr<Point2f>();
    }

    Point3f* objPtData = objPtMat.ptr<Point3f>();
    Point2f* imgPtData = imgPtMat.ptr<Point2f>();
    
    for( i = 0; i < nimages; i++, j += ni )
    {
        ni = objectPoints[i].size();
        ((int*)npoints.data)[i] = (int)ni;
        std::copy(objectPoints[i].begin(), objectPoints[i].end(), objPtData + j);
        std::copy(imagePoints[i].begin(), imagePoints[i].end(), imgPtData + j);
        if( imgPtMat2 )
            std::copy(imagePoints2[i].begin(), imagePoints2[i].end(), imgPtData2 + j);
    }
}
    
static Mat prepareCameraMatrix(Mat& cameraMatrix0, int rtype)
{
    Mat cameraMatrix = Mat::eye(3, 3, rtype);
    if( cameraMatrix0.size() == cameraMatrix.size() )
        cameraMatrix0.convertTo(cameraMatrix, rtype);
    return cameraMatrix;
}

static Mat prepareDistCoeffs(Mat& distCoeffs0, int rtype)
{
    Mat distCoeffs = Mat::zeros(distCoeffs0.cols == 1 ? Size(1, 5) : Size(5, 1), rtype);
    if( distCoeffs0.size() == Size(1, 4) ||
       distCoeffs0.size() == Size(1, 5) ||
       distCoeffs0.size() == Size(4, 1) ||
       distCoeffs0.size() == Size(5, 1) )
    {
        Mat dstCoeffs(distCoeffs, Rect(0, 0, distCoeffs0.cols, distCoeffs0.rows));
        distCoeffs0.convertTo(dstCoeffs, rtype);
    }
    return distCoeffs;
}
    
}


void cv::Rodrigues(const Mat& src, Mat& dst)
{
    bool v2m = src.cols == 1 || src.rows == 1;
    dst.create(3, v2m ? 3 : 1, src.depth());
    CvMat _src = src, _dst = dst;
    bool ok = cvRodrigues2(&_src, &_dst, 0) > 0;
    if( !ok )
        dst = Scalar(0);
}

void cv::Rodrigues(const Mat& src, Mat& dst, Mat& jacobian)
{
    bool v2m = src.cols == 1 || src.rows == 1;
    dst.create(3, v2m ? 3 : 1, src.depth());
    jacobian.create(v2m ? Size(9, 3) : Size(3, 9), src.depth());
    CvMat _src = src, _dst = dst, _jacobian = jacobian;
    bool ok = cvRodrigues2(&_src, &_dst, &_jacobian) > 0;
    if( !ok )
        dst = Scalar(0);
}

void cv::matMulDeriv( const Mat& A, const Mat& B, Mat& dABdA, Mat& dABdB )
{
    dABdA.create(A.rows*B.cols, A.rows*A.cols, A.type());
    dABdB.create(A.rows*B.cols, B.rows*B.cols, A.type());
    CvMat matA = A, matB = B, _dABdA = dABdA, _dABdB = dABdB;
    cvCalcMatMulDeriv(&matA, &matB, &_dABdA, &_dABdB);
}

void cv::composeRT( const Mat& rvec1, const Mat& tvec1,
                    const Mat& rvec2, const Mat& tvec2,
                    Mat& rvec3, Mat& tvec3 )
{
    rvec3.create(rvec1.size(), rvec1.type());
    tvec3.create(tvec1.size(), tvec1.type());
    CvMat _rvec1 = rvec1, _tvec1 = tvec1, _rvec2 = rvec2,
    _tvec2 = tvec2, _rvec3 = rvec3, _tvec3 = tvec3;
    cvComposeRT(&_rvec1, &_tvec1, &_rvec2, &_tvec2, &_rvec3, &_tvec3, 0, 0, 0, 0, 0, 0, 0, 0);
}


void cv::composeRT( const Mat& rvec1, const Mat& tvec1,
                    const Mat& rvec2, const Mat& tvec2,
                    Mat& rvec3, Mat& tvec3,
                    Mat& dr3dr1, Mat& dr3dt1,
                    Mat& dr3dr2, Mat& dr3dt2,
                    Mat& dt3dr1, Mat& dt3dt1,
                    Mat& dt3dr2, Mat& dt3dt2 )
{
    int rtype = rvec1.type();
    rvec3.create(rvec1.size(), rtype);
    tvec3.create(tvec1.size(), rtype);
    dr3dr1.create(3, 3, rtype); dr3dt1.create(3, 3, rtype);
    dr3dr2.create(3, 3, rtype); dr3dt2.create(3, 3, rtype);
    dt3dr1.create(3, 3, rtype); dt3dt1.create(3, 3, rtype);
    dt3dr2.create(3, 3, rtype); dt3dt2.create(3, 3, rtype);
    
    CvMat _rvec1 = rvec1, _tvec1 = tvec1, _rvec2 = rvec2,
    _tvec2 = tvec2, _rvec3 = rvec3, _tvec3 = tvec3;
    CvMat _dr3dr1 = dr3dr1, _dr3dt1 = dr3dt1, _dr3dr2 = dr3dr2, _dr3dt2 = dr3dt2;
    CvMat _dt3dr1 = dt3dr1, _dt3dt1 = dt3dt1, _dt3dr2 = dt3dr2, _dt3dt2 = dt3dt2;
    cvComposeRT(&_rvec1, &_tvec1, &_rvec2, &_tvec2, &_rvec3, &_tvec3,
                &_dr3dr1, &_dr3dt1, &_dr3dr2, &_dr3dt2,
                &_dt3dr1, &_dt3dt1, &_dt3dr2, &_dt3dt2);
}


void cv::projectPoints( const Mat& opoints,
                        const Mat& rvec, const Mat& tvec,
                        const Mat& cameraMatrix,
                        const Mat& distCoeffs,
                        vector<Point2f>& ipoints )
{
    CV_Assert(opoints.isContinuous() && opoints.depth() == CV_32F &&
              ((opoints.rows == 1 && opoints.channels() == 3) ||
               opoints.cols*opoints.channels() == 3));
    
    ipoints.resize(opoints.cols*opoints.rows*opoints.channels()/3);
    CvMat _objectPoints = opoints, _imagePoints = Mat(ipoints);
    CvMat _rvec = rvec, _tvec = tvec, _cameraMatrix = cameraMatrix, _distCoeffs = distCoeffs;
    
    cvProjectPoints2( &_objectPoints, &_rvec, &_tvec, &_cameraMatrix, &_distCoeffs,
                     &_imagePoints, 0, 0, 0, 0, 0, 0 );
}

void cv::projectPoints( const Mat& opoints,
                        const Mat& rvec, const Mat& tvec,
                        const Mat& cameraMatrix,
                        const Mat& distCoeffs,
                        vector<Point2f>& ipoints,
                        Mat& dpdrot, Mat& dpdt, Mat& dpdf,
                        Mat& dpdc, Mat& dpddist,
                        double aspectRatio )
{
    CV_Assert(opoints.isContinuous() && opoints.depth() == CV_32F &&
              ((opoints.rows == 1 && opoints.channels() == 3) ||
               opoints.cols*opoints.channels() == 3));
    
    int npoints = opoints.cols*opoints.rows*opoints.channels()/3;
    ipoints.resize(npoints);
    dpdrot.create(npoints*2, 3, CV_64F);
    dpdt.create(npoints*2, 3, CV_64F);
    dpdf.create(npoints*2, 2, CV_64F);
    dpdc.create(npoints*2, 2, CV_64F);
    dpddist.create(npoints*2, distCoeffs.rows + distCoeffs.cols - 1, CV_64F);
    CvMat _objectPoints = opoints, _imagePoints = Mat(ipoints);
    CvMat _rvec = rvec, _tvec = tvec, _cameraMatrix = cameraMatrix, _distCoeffs = distCoeffs;
    CvMat _dpdrot = dpdrot, _dpdt = dpdt, _dpdf = dpdf, _dpdc = dpdc, _dpddist = dpddist;
    
    cvProjectPoints2( &_objectPoints, &_rvec, &_tvec, &_cameraMatrix, &_distCoeffs,
                      &_imagePoints, &_dpdrot, &_dpdt, &_dpdf, &_dpdc, &_dpddist, aspectRatio );
}

void cv::solvePnP( const Mat& opoints, const Mat& ipoints,
                   const Mat& cameraMatrix, const Mat& distCoeffs,
                   Mat& rvec, Mat& tvec, bool useExtrinsicGuess )
{
    CV_Assert(opoints.isContinuous() && opoints.depth() == CV_32F &&
              ((opoints.rows == 1 && opoints.channels() == 3) ||
               opoints.cols*opoints.channels() == 3) &&
              ipoints.isContinuous() && ipoints.depth() == CV_32F &&
              ((ipoints.rows == 1 && ipoints.channels() == 2) ||
              ipoints.cols*ipoints.channels() == 2));
    
    rvec.create(3, 1, CV_64F);
    tvec.create(3, 1, CV_64F);
    CvMat _objectPoints = opoints, _imagePoints = ipoints;
    CvMat _cameraMatrix = cameraMatrix, _distCoeffs = distCoeffs;
    CvMat _rvec = rvec, _tvec = tvec;
    cvFindExtrinsicCameraParams2(&_objectPoints, &_imagePoints, &_cameraMatrix,
                                 &_distCoeffs, &_rvec, &_tvec, useExtrinsicGuess );
}


cv::Mat cv::initCameraMatrix2D( const vector<vector<Point3f> >& objectPoints,
                                const vector<vector<Point2f> >& imagePoints,
                                Size imageSize, double aspectRatio )
{
    Mat objPt, imgPt, npoints, cameraMatrix(3, 3, CV_64F);
    collectCalibrationData( objectPoints, imagePoints, vector<vector<Point2f> >(),
                            objPt, imgPt, 0, npoints );
    CvMat _objPt = objPt, _imgPt = imgPt, _npoints = npoints, _cameraMatrix = cameraMatrix;
    cvInitIntrinsicParams2D( &_objPt, &_imgPt, &_npoints,
                             imageSize, &_cameraMatrix, aspectRatio );
    return cameraMatrix;
}


double cv::calibrateCamera( const vector<vector<Point3f> >& objectPoints,
                          const vector<vector<Point2f> >& imagePoints,
                          Size imageSize, Mat& cameraMatrix, Mat& distCoeffs,
                          vector<Mat>& rvecs, vector<Mat>& tvecs, int flags )
{
    int rtype = CV_64F;
    cameraMatrix = prepareCameraMatrix(cameraMatrix, rtype);
    distCoeffs = prepareDistCoeffs(distCoeffs, rtype);

    size_t i, nimages = objectPoints.size();
    CV_Assert( nimages > 0 );
    Mat objPt, imgPt, npoints, rvecM((int)nimages, 3, CV_64FC1), tvecM((int)nimages, 3, CV_64FC1);
    collectCalibrationData( objectPoints, imagePoints, vector<vector<Point2f> >(),
                            objPt, imgPt, 0, npoints );
    CvMat _objPt = objPt, _imgPt = imgPt, _npoints = npoints;
    CvMat _cameraMatrix = cameraMatrix, _distCoeffs = distCoeffs;
    CvMat _rvecM = rvecM, _tvecM = tvecM;

    double reprojErr = cvCalibrateCamera2(
        &_objPt, &_imgPt, &_npoints, imageSize, &_cameraMatrix,
        &_distCoeffs, &_rvecM, &_tvecM, flags );
    rvecs.resize(nimages);
    tvecs.resize(nimages);
    for( i = 0; i < nimages; i++ )
    {
        rvecM.row((int)i).copyTo(rvecs[i]);
        tvecM.row((int)i).copyTo(tvecs[i]);
    }
    return reprojErr;
}

void cv::calibrationMatrixValues( const Mat& cameraMatrix, Size imageSize,
                                  double apertureWidth, double apertureHeight,
                                  double& fovx, double& fovy, double& focalLength,
                                  Point2d& principalPoint, double& aspectRatio )
{
    CvMat _cameraMatrix = cameraMatrix;
    cvCalibrationMatrixValues( &_cameraMatrix, imageSize, apertureWidth, apertureHeight,
        &fovx, &fovy, &focalLength, (CvPoint2D64f*)&principalPoint, &aspectRatio );
}

double cv::stereoCalibrate( const vector<vector<Point3f> >& objectPoints,
                          const vector<vector<Point2f> >& imagePoints1,
                          const vector<vector<Point2f> >& imagePoints2,
                          Mat& cameraMatrix1, Mat& distCoeffs1,
                          Mat& cameraMatrix2, Mat& distCoeffs2,
                          Size imageSize, Mat& R, Mat& T,
                          Mat& E, Mat& F, TermCriteria criteria,
                          int flags )
{
    int rtype = CV_64F;
    cameraMatrix1 = prepareCameraMatrix(cameraMatrix1, rtype);
    cameraMatrix2 = prepareCameraMatrix(cameraMatrix2, rtype);
    distCoeffs1 = prepareDistCoeffs(distCoeffs1, rtype);
    distCoeffs2 = prepareDistCoeffs(distCoeffs2, rtype);
    R.create(3, 3, rtype);
    T.create(3, 1, rtype);
    E.create(3, 3, rtype);
    F.create(3, 3, rtype);

    Mat objPt, imgPt, imgPt2, npoints;

    collectCalibrationData( objectPoints, imagePoints1, imagePoints2,
                            objPt, imgPt, &imgPt2, npoints );
    CvMat _objPt = objPt, _imgPt = imgPt, _imgPt2 = imgPt2, _npoints = npoints;
    CvMat _cameraMatrix1 = cameraMatrix1, _distCoeffs1 = distCoeffs1;
    CvMat _cameraMatrix2 = cameraMatrix2, _distCoeffs2 = distCoeffs2;
    CvMat matR = R, matT = T, matE = E, matF = F;

    return cvStereoCalibrate(&_objPt, &_imgPt, &_imgPt2, &_npoints, &_cameraMatrix1,
        &_distCoeffs1, &_cameraMatrix2, &_distCoeffs2, imageSize,
        &matR, &matT, &matE, &matF, criteria, flags );
}

void cv::stereoRectify( const Mat& cameraMatrix1, const Mat& distCoeffs1,
                        const Mat& cameraMatrix2, const Mat& distCoeffs2,
                        Size imageSize, const Mat& R, const Mat& T,
                        Mat& R1, Mat& R2, Mat& P1, Mat& P2, Mat& Q,
                        int flags )
{
    int rtype = CV_64F;
    R1.create(3, 3, rtype);
    R2.create(3, 3, rtype);
    P1.create(3, 4, rtype);
    P2.create(3, 4, rtype);
    Q.create(4, 4, rtype);
    CvMat _cameraMatrix1 = cameraMatrix1, _distCoeffs1 = distCoeffs1;
    CvMat _cameraMatrix2 = cameraMatrix2, _distCoeffs2 = distCoeffs2;
    CvMat matR = R, matT = T, _R1 = R1, _R2 = R2, _P1 = P1, _P2 = P2, matQ = Q;
    cvStereoRectify( &_cameraMatrix1, &_cameraMatrix2, &_distCoeffs1, &_distCoeffs2,
        imageSize, &matR, &matT, &_R1, &_R2, &_P1, &_P2, &matQ, flags );
}

void cv::stereoRectify( const Mat& cameraMatrix1, const Mat& distCoeffs1,
                        const Mat& cameraMatrix2, const Mat& distCoeffs2,
                        Size imageSize, const Mat& R, const Mat& T,
                        Mat& R1, Mat& R2, Mat& P1, Mat& P2, Mat& Q,
                        double alpha, Size newImageSize,
                        Rect* validPixROI1, Rect* validPixROI2,
                        int flags )
{
    int rtype = CV_64F;
    R1.create(3, 3, rtype);
    R2.create(3, 3, rtype);
    P1.create(3, 4, rtype);
    P2.create(3, 4, rtype);
    Q.create(4, 4, rtype);
    CvMat _cameraMatrix1 = cameraMatrix1, _distCoeffs1 = distCoeffs1;
    CvMat _cameraMatrix2 = cameraMatrix2, _distCoeffs2 = distCoeffs2;
    CvMat matR = R, matT = T, _R1 = R1, _R2 = R2, _P1 = P1, _P2 = P2, matQ = Q;
    cvStereoRectify( &_cameraMatrix1, &_cameraMatrix2, &_distCoeffs1, &_distCoeffs2,
                    imageSize, &matR, &matT, &_R1, &_R2, &_P1, &_P2, &matQ, flags,
                    alpha, newImageSize, (CvRect*)validPixROI1, (CvRect*)validPixROI2);
}

bool cv::stereoRectifyUncalibrated( const Mat& points1, const Mat& points2,
                                    const Mat& F, Size imgSize,
                                    Mat& H1, Mat& H2, double threshold )
{
    int rtype = CV_64F;
    H1.create(3, 3, rtype);
    H2.create(3, 3, rtype);
    CvMat _pt1 = points1, _pt2 = points2, matF, *pF=0, _H1 = H1, _H2 = H2;
    if( F.size() == Size(3, 3) )
        pF = &(matF = F);
    return cvStereoRectifyUncalibrated(&_pt1, &_pt2, pF, imgSize, &_H1, &_H2, threshold) > 0;
}

void cv::reprojectImageTo3D( const Mat& disparity,
                             Mat& _3dImage, const Mat& Q,
                             bool handleMissingValues )
{
    _3dImage.create(disparity.size(), CV_32FC3);
    CvMat _disparity = disparity, __3dImage = _3dImage, matQ = Q;
    cvReprojectImageTo3D( &_disparity, &__3dImage, &matQ, handleMissingValues );
}


cv::Mat cv::getOptimalNewCameraMatrix( const Mat& cameraMatrix, const Mat& distCoeffs,
                                   Size imgSize, double alpha, Size newImgSize,
                                   Rect* validPixROI )
{
    Mat newCameraMatrix(3, 3, cameraMatrix.type());
    CvMat _cameraMatrix = cameraMatrix,
        _distCoeffs = distCoeffs,
        _newCameraMatrix = newCameraMatrix;
    cvGetOptimalNewCameraMatrix(&_cameraMatrix, &_distCoeffs, imgSize,
                                alpha, &_newCameraMatrix,
                                newImgSize, (CvRect*)validPixROI);
    return newCameraMatrix;
}


/* End of file. */
