#!/bin/sh

while read id
do
  wget "https://www2.eecs.berkeley.edu/Research/Projects/CS/vision/grouping/segbench/BSDS300/html/images/plain/normal/gray/$id.jpg" -P download/test
  wget "https://www2.eecs.berkeley.edu/Research/Projects/CS/vision/grouping/segbench/BSDS300/html/images/human/normal/outline/gray/union/$id.jpg" -P download/test_output
done < iids_test.txt


while read id
do
  wget "https://www2.eecs.berkeley.edu/Research/Projects/CS/vision/grouping/segbench/BSDS300/html/images/plain/normal/gray/$id.jpg" -P download/train
  wget "https://www2.eecs.berkeley.edu/Research/Projects/CS/vision/grouping/segbench/BSDS300/html/images/human/normal/outline/gray/union/$id.jpg" -P download/train_output
done < iids_train.txt
