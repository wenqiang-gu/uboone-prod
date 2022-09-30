#!/bin/bash

js=(run3b-wmyz \
run3b-wmthxz \
run3b-wmthyz \
run3b-wmdedx \
run3b-sce \
run3b-recomb2)
# run3b-lyattn \
# run3b-lyrayleigh \
# run3b-lydown \
# run3b-cv \
# run3b-wmx \

for idx in "${!js[@]}"; do
  j=${js[$idx]}
  echo $j

  pushd $j
  ../hadd/hadd-md5.sh offline_path.txt $j.root.1
  popd

done

