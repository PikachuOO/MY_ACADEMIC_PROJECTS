#!/usr/bin/perl -w
#simple test on string concatenation
#subset 3
$test_string = "first Part";
$test_string2 = "second Part";

print $test_string.$test_string2;
print "TEST-CHAR";
print $test_string.$test_string2,"\n";

$test_string .= $test_string2;
print $test_string;
print "TEST-CHAR";
print $test_string,"\n";