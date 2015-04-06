

tools/build_detection_db -d ../data -c haptic4 -b april_poster_2in --do-benchmark haptic4_long_detection_benchmarks.txt ../data/datasets/haptic4_calibration/long/haptic4_long.mp4

tols/video_calibration -d ../data --fix-skew -c hapti4 -b april_poster_2in -m angular --calibration-db haptic4_long.kch --save-board-poses all ../data/datasets/haptic4_calibration/long/haptic4_long.mp4


tools/video_calibration -d ../data --fix-skew -c haptic4 -b april_poster_2in -m angular --calibration-db calibration_reference_haptic4_all_two.kch all ../data/datasets/calibration_reference/haptic4.mp4

tools/video_calibration_permutations -d ../data -c haptic4 -b april_poster_2in --fix-skew -m angular --calibration-db calibration_reference_haptic4_new.kch ../data/datasets/calibration_reference/haptic4.mp4

tools/calibration_reproject --reference-db ../data/cache/E9C9740A25558DEB11E29044813121269F328324.kch --calibration-db calibration_reference_haptic4.kch --results-db calibration_reference_haptic4_results.kch 

tools/calibration_db_dump --results-db calibration_reference_haptic4_results.kch  > calibration_reference_against_reference.txt
tools/calibration_db_dump --results-db calibration_reference_haptic4.kch  > calibration_reference_against_self.txt

