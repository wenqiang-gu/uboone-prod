# echo $1

INPUT_FILE=$1
LOCAL_DIR=$2
if [ -z "$LOCAL_DIR" ]
then
  echo "local dir: None -> set to files/"
  LOCAL_DIR="files/"
fi

echo "local dir: " $LOCAL_DIR 
mkdir $LOCAL_DIR

count=0
for file in `cat $INPUT_FILE`;do
  timeout 3s ifdh cp $file $LOCAL_DIR/
  echo $count $file
  let count++
done

find $LOCAL_DIR -name "*root" -size +10k > $LOCAL_DIR\_copied.txt
find $LOCAL_DIR -name "*root" -size -10k -printf '%f\n' > $LOCAL_DIR\_uncopied.txt

hadd -f $LOCAL_DIR\_copied.root @$LOCAL_DIR\_copied.txt
