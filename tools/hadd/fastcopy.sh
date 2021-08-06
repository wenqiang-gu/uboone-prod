# echo $1

count=0
for file in `cat $1`;do
  timeout 3s ifdh cp $file files/ 
  echo $count $file
  let count++
done
