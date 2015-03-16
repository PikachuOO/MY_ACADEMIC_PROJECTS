#!/usr/bin/perl -w
#simple test on variable type detections
#subset 4
$num = 101;
$num = $num % 2;
if ($num != 0){
    print "RIGHT\n";
}

$num = 2.5;
$num = $num**4;

if ($num == 39.0625) {
    print $num;
}
