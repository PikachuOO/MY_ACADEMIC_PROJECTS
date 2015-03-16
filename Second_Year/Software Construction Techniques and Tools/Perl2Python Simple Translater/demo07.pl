#!/usr/bin/perl -w
@var = (1, 2, 3);
$var3= "12";
@var2 = (1, 2, 3);
unshift @var, $var3;
$test = join (",", @var);
print $test, "\n";

unshift @var, @var2;
$test = join (",", @var);
print $test, "\n";

$test2 = shift @var;
print $test2, "\n";

$test2 = pop @var;
print $test2, "\n";

$last = 3;
push @var, $last;
$test = join (",", @var);
print $test, "\n";

push @var, @var2;
$test = join (",", @var);
print $test, "\n";

@var = reverse @var;
$test = join (",", @var);
print $test, "\n";

