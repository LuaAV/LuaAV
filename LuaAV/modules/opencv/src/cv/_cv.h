/*M///////////////////////////////////////////////////////////////////////////////////////
//
//  IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.
//
//  By downloading, copying, installing or using the software you agree to this license.
//  If you do not agree to this license, do not download, install,
//  copy or use the software.
//
//
//                          License Agreement
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

#ifndef _OPENCV_CV_INTERNAL_H_
#define _OPENCV_CV_INTERNAL_H_

#if defined _MSC_VER && _MSC_VER >= 1200
    // disable warnings related to inline functions
    #pragma warning( disable: 4251 4711 4710 4514 )
#endif

#include "cv.h"
#include "cxmisc.h"
#include <math.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <float.h>

#include "cvinternal.h"

/* helper tables */
extern const uchar icvSaturate8u_cv[];
#define CV_FAST_CAST_8U(t)  (assert(-256 <= (t) || (t) <= 512), icvSaturate8u_cv[(t)+256])
#define CV_CALC_MIN_8U(a,b) (a) -= CV_FAST_CAST_8U((a) - (b))
#define CV_CALC_MAX_8U(a,b) (a) += CV_FAST_CAST_8U((b) - (a))

// -256.f ... 511.f
extern const float icv8x32fTab_cv[];
#define CV_8TO32F(x)  icv8x32fTab_cv[(x)+256]

// (-128.f)^2 ... (255.f)^2
extern const float icv8x32fSqrTab[];
#define CV_8TO32F_SQR(x)  icv8x32fSqrTab[(x)+128]

namespace cv
{

enum { INTER_BITS=5, INTER_BITS2=INTER_BITS*2,
       INTER_TAB_SIZE=(1<<INTER_BITS),
       INTER_TAB_SIZE2=INTER_TAB_SIZE*INTER_TAB_SIZE };

static inline Point normalizeAnchor( Point anchor, Size ksize )
{
    if( anchor == Point(-1,-1) )
        anchor = Point(ksize.width/2, ksize.height/2);
    else
        CV_Assert( anchor.inside(Rect(0, 0, ksize.width, ksize.height)) );
    return anchor;
}

void preprocess2DKernel( const Mat& kernel, vector<Point>& coords, vector<uchar>& coeffs );
void crossCorr( const Mat& src, const Mat& templ, Mat& dst,
                Point anchor=Point(0,0), double delta=0,
                int borderType=BORDER_REFLECT_101 );

}

typedef struct CvPyramid
{
    uchar **ptr;
    CvSize *sz;
    double *rate;
    int *step;
    uchar *state;
    int level;
}
CvPyramid;

#include "_cvgeom.h"
#include "_cvimgproc.h"

#endif /*__OPENCV_CV_INTERNAL_H_*/
