SPEC_HOME=~/Desktop/spec-cpu2006/benchspec/CPU2006
bzip="$SPEC_HOME/401.bzip2/exe/bzip2_base.amd64-m64-gcc42-nn $SPEC_HOME/401.bzip2/data/test/input/dryer.jpg"
mcf="$SPEC_HOME/429.mcf/exe/mcf_base.amd64-m64-gcc42-nn $SPEC_HOME/429.mcf/data/test/input/inp.in"

./collect_by_pirate.sh "bzip2" $bzip
./collect_by_pirate.sh "mcf" $mcf

