for seq in 0{1..9} {10..48};do
  sed -e "s|data_bnb_run1_5e19_00|data_bnb_run1_5e19_$seq|g" complete/data_bnb_run1_5e19_00.xml > data_bnb_run1_5e19_$seq.xml
done
