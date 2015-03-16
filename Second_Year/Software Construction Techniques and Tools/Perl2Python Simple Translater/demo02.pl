#!/usr/bin/perl -w
# simple loop test with undeclared variables
$answer = 1;
while (!$stop) {
    if ($answer < 1) {
        print "answer = $answer\n";
    } elsif ($answer == 1) {
        print "answer = $answer\n";
        $answer += 1;
    } else {
        print "Stopping Now\n";
        $stop = 1;
    }
}
