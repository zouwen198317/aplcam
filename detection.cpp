
#include "detection.h"
#include "file_utils.h"

using namespace std;
using namespace cv;

void Detection::calculateCorners( const Board &board )
{ 
  corners.resize(0);

  switch(board.pattern)
  {
    case CHESSBOARD:
    case CIRCLES_GRID:
      for( int i = 0; i < board.size().height; i++ )
        for( int j = 0; j < board.size().width; j++ )
          corners.push_back(Point3f(float(j*board.squareSize),
                float(i*board.squareSize), 0));
      break;

    case ASYMMETRIC_CIRCLES_GRID:
      for( int i = 0; i < board.size().height; i++ )
        for( int j = 0; j < board.size().width; j++ )
          corners.push_back(Point3f(float((2*j + i % 2)*board.squareSize),
                float(i*board.squareSize), 0));
      break;

    default:
      CV_Error(CV_StsBadArg, "Unknown pattern type\n");
  }
}

void Detection::drawCorners( const Board &board, Mat &view ) const
{
  cout << "Drawing " << corners.size() << " corners" << endl;
  drawChessboardCorners( view, board.size(), Mat(points), found );
}

//============================================================================
//  Serialization/unserialization methods
//============================================================================

void Detection::writeCache( const Board &board, const string &cacheFile )
{
  mkdir_p( cacheFile );

  FileStorage fs( cacheFile, FileStorage::WRITE );

  fs << "board_type" << board.patternString();
  fs << "board_name" << board.name;
  fs << "image_points" << Mat( points );
  fs << "world_points" << Mat( corners );
}


Detection *Detection::load( const string &cacheFile )
{
  if( !file_exists( cacheFile ) ) {
    cout << "Unable to find cache file \"" << cacheFile << "\"" << endl;
    return NULL;
  }

  FileStorage fs( cacheFile, FileStorage::READ );

  Detection *detection = detection = new Detection();

  // Load and validate data
  Mat pts;
  fs["image_points"] >> pts;

  // Should be able to do this in-place, but ...
  for( int i = 0; i < pts.rows; ++i ) {
    detection->points.push_back( Point2f(pts.at<float>(i,0), pts.at<float>(i,1) ) );
  }

  fs["world_points"] >> pts;

  // Should be able to do this in-place, but ...
  for( int i = 0; i < pts.rows; ++i ) {
    detection->corners.push_back( Point3f(pts.at<float>(i,0), pts.at<float>(i,1), pts.at<float>(i,2) ) );
  }

  return detection;
}


//============================================================================
//  AprilTagDetection
//============================================================================

void AprilTagsDetection::calculateCorners( const AprilTagsBoard &board )
{
  // Go for a simple model here, assume all tags are unique on the board
  
  points.clear();
  corners.clear();

  for( int i = 0; i < _det.size(); ++i ) {


    Point2i loc;
    if( board.find( _det[i].id, loc ) ) {
    cout << "Found  tag id " << _det[i].id << endl;

      points.push_back( Point2f( _det[i].cxy.first, _det[i].cxy.second ) );
      corners.push_back( board.worldLocation( loc ) );

    } else {
      cerr << "Couldn't find tag \'" << _det[i].id << "\'" << endl;
    }
  }
}


