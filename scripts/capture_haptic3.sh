ffmpeg -y -thread_queue_size 4096 \
       -i rtsp://stream:knot4ewe@10.0.76.11/big -codec copy $1
