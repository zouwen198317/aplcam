#ifndef __DETECTION_H__
#define __DETECTION_H__

#include <string>
#include <vector>

#include <opencv2/core/core.hpp>

#include "board.h"
#include "distortion_model.h"

#include <kchashdb.h>

struct SharedPoints
{
  Distortion::ImagePointsVec imagePoints[2];
  Distortion::ObjectPointsVec worldPoints;
};

struct Detection 
{
  Detection(  )
    : found(false), points(), corners(), ids() {;}

  bool found;
  Distortion::ImagePointsVec points;
  Distortion::ObjectPointsVec corners;
  std::vector< int > ids;

  int size( void ) const { return points.size(); }

  virtual void calculateCorners( const Board &board );
  virtual void drawCorners(  const Board &board, cv::Mat &view ) const;

  virtual void writeCache( const Board &board, const std::string &cacheFile ) const;
  virtual void serialize( std::string &str ) const; 
  virtual void serializeToFileStorage( cv::FileStorage &fs ) const;

  static Detection *unserialize( const std::string &str );
  static Detection *loadCache( const std::string &cacheFile );
  static Detection *unserializeFromFileStorage( const cv::FileStorage &fs );
  static SharedPoints sharedWith( Detection &a, Detection &b );
};

#ifdef USE_APRILTAGS
struct AprilTagsDetection : public Detection
{
  AprilTagsDetection( vector< AprilTags::TagDetection > det )
    : Detection(), _det(det) {;}

  vector< AprilTags::TagDetection > _det;

  virtual void calculateCorners( const AprilTagsBoard &board );


};
#endif


using kyotocabinet::HashDB;

class DetectionDb {
  public:

    DetectionDb();

    ~DetectionDb();

    bool open( const string &dbFile, bool writer = false );
    bool open( const string &dbDir, const string &videoFile, bool writer = false );

    bool save( const int frame, const Detection &detection );
    bool has( const int frame );
    Detection *load( const int frame );
    Detection *loadAdvanceCursor( void );

    kyotocabinet::BasicDB::Error error( void ) { return _db.error(); }


    static std::string FrameToKey( const int frame );

  protected:

    kyotocabinet::DB::Cursor *cursor( void );


    HashDB _db;
    kyotocabinet::DB::Cursor *_cursor;

};

#endif
