RUNLIST=/afs/cern.ch/user/l/lbruni/Eutelescope/v01-17-05/Eutelescope/trunk/jobsub/examples/GBL/SCT/runlist/runlist_longstrip.csv
CONFIGFILE=/afs/cern.ch/user/l/lbruni/Eutelescope/v01-17-05/Eutelescope/trunk/jobsub/examples/GBL/SCT/config/config.cfg


 


#jobsub.py -g -c $CONFIGFILE -csv $RUNLIST converter $1
#jobsub.py -g -c $CONFIGFILE -csv $RUNLIST clustering              $1
#jobsub.py -g $DRY -c $CONFIGFILE -csv $RUNLIST hitmaker                $1
#jobsub.py -g  -c $CONFIGFILE -csv $RUNLIST patternRecognition      $1
#jobsub.py -g  -c $CONFIGFILE -csv $RUNLIST GBLAlign      $1
jobsub.py -g $DRY -c $CONFIGFILE -csv $RUNLIST patternRecognition      $1
jobsub.py -g $DRY -c $CONFIGFILE -csv $RUNLIST GBLTrackFit           $1
