//
// Refactoring the OpenCV distortion model, starting with the OpencvRadialPolynomial model.
// Based on a heavily hacked version of fisheye.cpp from OpenCV.
//
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
// Copyright (C) 2009-2011, Willow Garage Inc., all rights reserved.
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

#include <math.h>

#include "distortion_radial_polynomial.h"

#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <ceres/ceres.h>
#include <ceres/rotation.h>

#include <iostream>
using namespace std;


static std::ostream& operator <<(std::ostream& stream, const Distortion::OpencvRadialPolynomial &p )
{
  stream << p.f()[0] << " " << p.f()[1] << ", " << p.c()[0] << " " << p.c()[1] << ", " << p.alpha() << ", " << p.distCoeffs()[0] << " " << p.distCoeffs()[1] << " " << p.distCoeffs()[2] << " " << p.distCoeffs()[3];
  return stream;
}

namespace Distortion {

  using namespace cv;
  using namespace std;

  OpencvRadialPolynomial::OpencvRadialPolynomial( void )
    : DistortionModel(), _distCoeffs( 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 )
  {;}

  OpencvRadialPolynomial::OpencvRadialPolynomial( const Vec4d &d )
    : DistortionModel(), _distCoeffs( d[0], d[1], d[2], d[3], 0., 0., 0., 0. )
  {;}

  OpencvRadialPolynomial::OpencvRadialPolynomial( const Vec5d &d )
    : DistortionModel(), _distCoeffs( d[0], d[1], d[2], d[3], d[4], 0., 0., 0. )
  {;}

  OpencvRadialPolynomial::OpencvRadialPolynomial( const Vec8d &distCoeffs )
    : DistortionModel(), _distCoeffs( distCoeffs )
  {;}

  OpencvRadialPolynomial::OpencvRadialPolynomial( const Vec12d &coeffs)
      : DistortionModel( Vec4d( coeffs[0], coeffs[1], coeffs[2], coeffs[3] ) ),
      _distCoeffs( Vec8d( coeffs[4], coeffs[5], coeffs[6], coeffs[7], coeffs[8],
                         coeffs[9], coeffs[10], coeffs[11] ) )
  {;}



  OpencvRadialPolynomial::OpencvRadialPolynomial( const Vec4d &d, const Matx33d &cam )
    : DistortionModel( cam ), _distCoeffs( d[0], d[1], d[2], d[3], 0., 0., 0., 0. )
  {;}

  OpencvRadialPolynomial::OpencvRadialPolynomial( const Vec5d &d, const Matx33d &cam )
    : DistortionModel( cam ), _distCoeffs( d[0], d[1], d[2], d[3], d[4], 0., 0., 0. )
  {;}

  OpencvRadialPolynomial::OpencvRadialPolynomial( const Vec8d &distCoeffs, const Matx33d &cam )
    : DistortionModel( cam ), _distCoeffs( distCoeffs )
  {;}


  //  // Static version uses a reasonable estimate based on image size
  //  OpencvRadialPolynomial OpencvRadialPolynomial::Calibrate( 
  //      const ObjectPointsVecVec &objectPoints, 
  //      const ImagePointsVecVec &imagePoints, 
  //      const Size& image_size,
  //      vector< Vec3d > &rvecs, 
  //      vector< Vec3d > &tvecs,
  //      int flags, 
  //      cv::TermCriteria criteria)
  //  {
  //    OpencvRadialPolynomial fe( InitialDistortionEstimate(), InitialCameraEstimate( image_size ) );
  //    fe.calibrate( objectPoints, imagePoints, image_size, rvecs, tvecs, flags, criteria );
  //    return fe;
  //  }


  bool OpencvRadialPolynomial::doCalibrate(
      const ObjectPointsVecVec &objectPoints, 
      const ImagePointsVecVec &imagePoints, 
      const Size& imageSize,
      CalibrationResult &result,
      int flags, 
      cv::TermCriteria criteria)
  {

    Mat camera( mat() );
    Mat dist( _distCoeffs );

    vector<Mat> _rvecs, _tvecs;

    ObjectPointsVecVec _objPts;
    ImagePointsVecVec _imgPts;

    for( size_t i = 0; i < objectPoints.size(); ++i ) {
      if( result.status[i] ) {
        _objPts.push_back( objectPoints[i] );
        _imgPts.push_back( imagePoints[i] );
      }
    }

    //for( int i = 0; i < objectPoints.size(); ++i ) 
    //  cout << i << " " << objectPoints[i].size() << " " << imagePoints[i].size() << endl;

    int before = getTickCount();
    result.rms = calibrateCamera( _objPts, _imgPts, imageSize, camera, dist, _rvecs, _tvecs, flags, criteria );
    result.totalTime = (getTickCount() - before)/getTickFrequency();


    setCamera( camera );

    // _distCoeffs will be variable length
    double *d = dist.ptr<double>(0);

    for( unsigned int i = 0; i < 4; ++i ) _distCoeffs[i] = d[i];
    if( (dist.rows*dist.cols) == 5 )  _distCoeffs[4] = d[4];
    if( (dist.rows*dist.cols) == 8 ) {
      _distCoeffs[5] = d[5];
      _distCoeffs[6] = d[6];
      _distCoeffs[7] = d[7];
    }

    for( size_t i =0, k = 0; i < objectPoints.size(); ++i ) {
      if( result.status[i] ) {
        result.rvecs[i] = _rvecs[k];
        result.tvecs[i] = _tvecs[k];
        ++k;
      }
    }

    result.good = true;
    return result.good;
  }

  void OpencvRadialPolynomial::projectPoints( const ObjectPointsVec &objectPoints, 
      const Vec3d &rvec, const Vec3d &tvec, ImagePointsVec &imagePoints ) const
  {

    cv::projectPoints( objectPoints, rvec, tvec, mat(), Mat( _distCoeffs ), imagePoints );
  }


  ImagePoint OpencvRadialPolynomial::distort( const ObjectPoint &w ) const
  {
    double theta = atan2( sqrt( w[0]*w[0] + w[1]*w[1] ), w[2] );
    double psi = atan2( w[1], w[0] );

    double theta2 = theta*theta, theta4 = theta2*theta2, theta6 = theta4*theta2, theta8 = theta4*theta4;
    double theta_d = theta * (1 + _distCoeffs[0]*theta2 + _distCoeffs[1]*theta4 + _distCoeffs[2]*theta6 + _distCoeffs[3]*theta8);

    return Vec2d( theta_d*cos( psi ), theta_d*sin(psi) );
  }

  ImagePoint OpencvRadialPolynomial::undistort( const ImagePoint &pw ) const
  {
    double scale = 1.0;

    double theta_d = sqrt(pw[0]*pw[0] + pw[1]*pw[1]);
    if (theta_d > 1e-8)
    {
      // compensate distortion iteratively
      double theta = theta_d;
      for(int j = 0; j < 10; j++ )
      {
        double theta2 = theta*theta, theta4 = theta2*theta2, theta6 = theta4*theta2, theta8 = theta6*theta2;
        theta = theta_d / (1 + _distCoeffs[0] * theta2 + _distCoeffs[1] * theta4 + _distCoeffs[2] * theta6 + _distCoeffs[3] * theta8);
      }

      scale = std::tan(theta) / theta_d;
    }

    Vec2d pu = pw * scale; //undistorted point

    return pu;
  }

  FileStorage &OpencvRadialPolynomial::write( FileStorage &out ) const
  {
    DistortionModel::write( out );
    out << "distortion_coefficients" << _distCoeffs;

    return out;
  }

  OpencvRadialPolynomial *OpencvRadialPolynomial::Load( cv::FileStorage &in )
  {
    Mat kmat, distmat;
    Vec8d dist;

    in["camera_matrix"] >> kmat;
    in["distortion_coefficients"] >> dist;

    Matx33d k;

    kmat.copyTo( k, CV_64F );
    //distmat.copyTo( dist, CV_64F );

    return new OpencvRadialPolynomial( dist, k );

  }


  DistortionModel *OpencvRadialPolynomial::estimateMeanCamera( vector< DistortionModel *> cameras )
  {
    Mat mean( Mat::zeros(12,1,CV_64F) );
    for( size_t i = 0; i < cameras.size(); ++i ) {
      assert( cameras[i]->name() == name() );
      mean += cameras[i]->coeffMat();
    }

    mean /= cameras.size();

    Vec12d vecCoeff;
    mean.copyTo( vecCoeff );

    return new OpencvRadialPolynomial( vecCoeff );
  }

}


