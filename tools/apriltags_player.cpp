
#include <iostream>
#include <sstream>
#include <glog/logging.h>


#include "AprilTags/TagDetector.h"
#include "AprilTags/Tag36h11.h"
using namespace AprilTags;

#include "AplCam/splitter_common.h"

using namespace std;
using namespace AplCam;


class AprilTagSplitterApp : public SplitterApp {
    public:
      AprilTagSplitterApp( SplitterOpts options )
        : SplitterApp( options )
      {;}

      virtual bool processSplitFrame( Mat &img, Mat &toDisplay )
      {
        TagDetector detector( AprilTags::tagCodes36h11 );

        Mat grey;
        cvtColor( img, grey, CV_BGR2GRAY );
        toDisplay = grey;


        Mat equalized( grey.size(), grey.type() );
        Ptr<CLAHE> clahe = createCLAHE( 40, Size(4,4));
        clahe->apply( grey, equalized );

        vector<TagDetection> tags = detector.extractTags( equalized );

        for( size_t i = 0; i < tags.size(); ++i ) {
          tags[i].draw( toDisplay );
        }

        imshow( "equalized", equalized );

        return true;
      }

    protected:

  };


  int main( int argc, char **argv )
  {
    google::InitGoogleLogging( argv[0] );
    FLAGS_logtostderr = true;
    FLAGS_minloglevel = 0;

    SplitterOpts opts;
    stringstream msg;
    if( !opts.parseArgs( argc, argv, msg ) ) {
      cout << msg.str() << endl;
      exit(-1);
    }

    // Just force this on..
    opts.doDisplay = true;

    AprilTagSplitterApp main( opts );

    exit( main.run() ? 0 : -1 );

  }
