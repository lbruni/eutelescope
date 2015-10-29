#!/bin/sh

first="704"
last="704"
RUNLIST="runlist/runlist_longstrip.csv"

for i in 491 493 494 495 496 497 498 499 501 502 503 504 505 506 507 508 509 511
#454 455 460 456 457 478 479 458 459 461 462 463 467 464 468 475 476 477 465 469 466 472 473 470 471 474 480 481 482 483 484 485 486 488 489 490 #position 3 
#417 418 419 420 615 616 421 422 423 424 425 617 618 619 426 428 429 430 431 432 433 434 435 436 620 621 622 623 438 439 440 441 624 625 444 449 do
#position 3
do
jobsub.py -g -c config/config_good_shortstrips.cfg -csv $RUNLIST converter  $i
jobsub.py -g -c config/config_good_shortstrips.cfg -csv $RUNLIST clustering $i
jobsub.py -g -c config/config_good_shortstrips.cfg -csv $RUNLIST hitmaker $i
jobsub.py -g -c config/config_good_shortstrips.cfg -csv $RUNLIST patternRecognition $i
jobsub.py -g -c config/config_good_shortstrips.cfg -csv $RUNLIST GBLTrackFit  $i
done


