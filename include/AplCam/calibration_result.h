#ifndef __CALIBRATION_RESULT_H__
#define __CALIBRATION_RESULT_H__

#include <opencv2/core/core.hpp>

#include <vector>
#include <string>

#include "AplCam/types.h"

namespace AplCam {

  using cv::FileStorage;


  struct Result {
    Result( size_t sz = 0 )
      : rms(-1), reprojErrors( sz ), good( true ),
      numPoints(-1), numImages(-1)
    {;}

    virtual void resize( size_t sz )
    {
      rms = -1.0;
      numPoints = -1;
      numImages = -1;
      reprojErrors.resize( sz );
    }

    virtual void serialize( FileStorage &fs )
    {
      fs << "success" << good;
      fs << "numPoints" << numPoints;
      fs << "numImages" << numImages;
      fs << "rms" << rms;
      fs << "reprojErrors" << reprojErrors;
    }

    std::string toString(  void )
    {
      FileStorage fs("foo.yml", FileStorage::WRITE | FileStorage::MEMORY );
      serialize( fs );
      return fs.releaseAndGetString();
    }

    double rms;
    ReprojErrorsVecVec reprojErrors;
    bool good;
    int numPoints, numImages;


  };


  struct CalibrationResult : public Result {
    CalibrationResult( size_t sz = 0)
      : Result( sz ),
      totalTime(-1.0), residual(-1.0),
      rvecs( sz, Vec3d(0,0,0) ),
      tvecs( sz, Vec3d(0,0,0) ),
      status( sz, false )
    { good = false; }

    virtual void resize( size_t sz )
    {
      good = false;
      totalTime = -1.0;
      residual = -1.0;
      rvecs.resize( sz, Vec3d(0,0,0) );
      tvecs.resize( sz, Vec3d(0,0,0) );
      status.resize( sz, false );
    }

    virtual void serialize( FileStorage &fs )
    {
      Result::serialize(fs);

      fs << "totalTime" << totalTime;
      fs << "residual" << residual;
    }

    double totalTime, residual;

    RotVec rvecs;
    TransVec tvecs;
    vector< bool > status;
  };

}

#endif
