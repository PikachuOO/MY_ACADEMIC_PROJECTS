#!/usr/bin/perl
#
# jche804
# Created by Jiashu Chen on 02/10/13.
# Copyright (c) 2013 University of New South Wales.
# All rights reserved.
# program takes in a perl program
# translate it into a python program
# and then output to stdout

# keep tracking of indentations in python
$curr_indent = '';
$tab_shift = '    ';

# Array that stores the output code
@output = ();
@declared_var_list = ();
$hashbang_location = 0;
$importFlag = 0;
@import_lists = ();
$out_of_domain = 0;
# indent with 4 spaces
sub indent {
    $curr_indent .= $tab_shift;
}
# $unindent with 4 spaces
sub un_indent {
    $curr_indent =~ s/$tab_shift//;
}

while ($line = <>) {
    $line = string_conc($line) if ($line =~ m/([\S\$\"]\.[\S\$\"]|\s*\.\=\s*)/ and $line !~ /\.\./);
    # if statement deal with special variable such as @ARGV
    $line = IO_interpreter($line) if ($line =~ /\<.*\>/ or $line =~ /ARGV/);
    $line = func_basic_python_functions($line) if ($line =~ /(chomp|split|join|shift|push|pop|unshift|reverse)/);
    $line = last_next($line) if ($line =~ m/\s*(next|last)\s*;/);
    $line = func_basic_regex($line)if ($line !~ /^#!/);
    if ($line =~ /^#!/) {
        # translate #! line
        push @output, output_with_indentation("#!/usr/bin/python2.7 -u\n");
        $hashbang_location = $#output;
        @declared_var_list = ();
        @import_lists = ();
        $importFlag = 0;
    } elsif ($line =~ /^\s*#/ || $line =~ /^\s*$/) {
        # Blank & comment lines can be passed unchanged
        push @output, output_with_indentation($line);
    } elsif ($line =~ /^\s*print/) {
        # special case for closing if () {   print "\$answer = $answer\n";}
        if ($line =~ m/^\s*print[^\}]+\}\s*$/) {
            $line =~ s/\}\s*$/\n/;
            push @output, output_with_indentation(func_print($line));
            un_indent();
        }else {
            push @output, output_with_indentation(func_print($line));
        }
    } elsif ($line =~ /\s*(while|for|foreach|if|elsif|else)\s+\(?/) {
        $line = control_structures($line);
        $line = logic_arithmetic_operators(check_variables($line));
        push @output, output_with_indentation($line);
        indent();
    } elsif ($line =~ /^\s*\{\s*$/){
        # case for opening the loop, on a different line
        # don't do anything
        indent();
    } elsif ($line =~ m/\s*([^\}]*)\}\s*$/){
        # case for closing the loop, and unindent the notation
        # print "\$line is $line";
        $line = $1 =~ s/\}\s*//;
        un_indent();
        $line = logic_arithmetic_operators(check_variables($line));
        $line = $line."\n";
        push @output, output_with_indentation($line);
    } elsif ($line =~ /[\$\@\%]/ and $line !~ /(while|for|foreach|if|elsif|else)/) {
        $line = check_variables(logic_arithmetic_operators($line));
        push @output, output_with_indentation($line);
    } else {
        push @output, "#$line" if (!$out_of_domain);
        push @output, "$line" if ($out_of_domain);
    }
    $out_of_domain = 0;
}

print @output,"\n";

sub output_with_indentation {
    my $input = shift @_;
    $input = $curr_indent.$input;
    return $input;
}
# string concatenation
sub string_conc {
    my $input = shift @_;
    # protect decimal numbers
    $input =~ s/(\d+)\.(\d+)/$1>->-<-<$2/g;
    $out_of_domain = 1;
    $input =~ s/\.\=/\+\=/;
    my @temp = split ('\.', $input);
    $input = join ("+", @temp);
    $input =~ s/>->-<-</\./g;
    return $input;
}
# deal with last and next
sub last_next {
    my $input = shift @_;
    $out_of_domain = 1;
    $input =~ s/next\s*\;/continue/g;
    $input =~ s/last\s*\;/break/g;
    return $input;
}
# deal with functions such as chomp, join, split
sub func_basic_python_functions {
    my $input = shift @_;
    $out_of_domain = 1;
    $input =~ s/chomp\s*\(?\s*\$([\w\_]+)\s*\)?\s*\;/$1 = $1.rstrip()/ig;
    $input =~ s/join\s*\(?([\'\"].+?[\'\"])\,\s*(.+?)\)/$1.join(map(str,$2))/;
    $input =~ s/split\(?([\'\"].+?[\'\"])\,\s*(\$[\w\_]+)\)?/$2.split($1)/;

    # push , pop, shift, unshift
    $input =~ s/unshift\s*\(?\s*([\@][^\s\.\\\(\)\{\}\[\]\;\+\-\"\'\:\=]+),\s*([\$][^\s\.\\\(\)\{\}\[\]\;\+\-\"\'\:\=]+)\s*\)?\s*/$1.insert(0, $2)/;
    if ($input =~ /\s*([^\s]*)unshift\s*\(?\s*([\@][^\s\.\\\(\)\{\}\[\]\;\+\-\"\'\:\=]+),\s*([\@][^\s\.\\\(\)\{\}\[\]\;\+\-\"\'\:\=]+)\s*\)?\s*/) {
        python_import_package('copy');
        $input = "_temp_merge_list = []\n";
        $input = $input."_temp_merge_list.extend(".$3.")\n";
        $input = $input."_temp_merge_list.extend(".$2.")\n";
        $input = $input.$2." = copy.deepcopy(_temp_merge_list)\n";
    }

    $input =~ s/\s*([^\s]*)shift\s*\(?\s*([\@][^\s\.\\\(\)\{\}\[\]\;\+\-\"\'\:\=]+)\s*\)?\s*/$1 $2.pop(0)/;

    $input =~ s/\s*([^\s]*)pop\s*\(?\s*([\@][^\s\.\\\(\)\{\}\[\]\;\+\-\"\'\:\=]+)\s*\)?\s*/$1 $2.pop()/;

    $input =~ s/\s*([^\s]*)push\s*\(?\s*([\@][^\s\.\\\(\)\{\}\[\]\;\+\-\"\'\:\=]+),\s*([\$][^\s\.\\\(\)\{\}\[\]\;\+\-\"\'\:\=]+)\s*\)?\s*/$1 $2.append($3)/;
    $input =~ s/\s*([^\s]*)push\s*\(?\s*([\@][^\s\.\\\(\)\{\}\[\]\;\+\-\"\'\:\=]+),\s*([\@][^\s\.\\\(\)\{\}\[\]\;\+\-\"\'\:\=]+)\s*\)?\s*/$1 $2.extend($3)/;

    $input =~ s/\s*([^\s]*)reverse\s*\(?\s*([\@][^\s\.\\\(\)\{\}\[\]\;\+\-\"\'\:\=]+)\s*\)?\s*/$1 $2.reverse()/;
    $input =~ s/([\S]+)\s*=\s*([\S]+)(.reverse\(\))(.*)/$2$3\n$1 = $2$4\n/;

    return $input;
}

sub func_basic_regex {
    $input = shift @_;
    if($input =~ /\s*\$.+?\s+\=\~\s*m?\/.+\/\;?/) {
        $input =~ s/\$(.+?)(\s+)\=\~\s*\/(.+)\/\;?/$1$2= re.search('$3', $1)/g;
        python_import_package('re');
    } elsif($input =~ /\s*\$.+?\s+\=\~\s*s\/.+\/.*?\/[gi]*\;?/) {
        $input =~ s/\$(.+?)(\s+)\=\~\s*s\/(.+)\/(.*?)\/[gi]*\;?/$1$2= re.sub(r'$3', '$4', $1)/g;
        python_import_package('re');
    }
    return $input;
}

sub python_import_package {
    my $request_package = shift @_;
    $out_of_domain = 1;
    push @import_lists , $request_package;
    my %var_hash = map {$_,1} @import_lists;
    @import_lists = keys %var_hash;
    my $packages = join(",", @import_lists);
    $packages = "import ".$packages."\n";
    if ($importFlag) {
        $output[$hashbang_location+1] = $packages;
    } else {
        splice(@output, ($hashbang_location+1), 0, $packages);
        $importFlag = 1;
    }
}

sub IO_interpreter {
    my $input = shift @_;
    if ($input =~ s/while\s*\(?\$([\S]+)\s*=\s*(\<[\w]*\>)?\)\s*\{?/for $1 in $2:/g){
        push @declared_var_list, "\$".$1;
        if ($2 =~ /\<STDIN\>/) {
            python_import_package('sys') if ($input =~ s/\<STDIN\>/sys.stdin/);
        }
    }
    python_import_package('sys') if ($input =~ s/\<STDIN\>\s*/sys.stdin.readline()/g);
    python_import_package('sys') if ($input =~ s/\$ARGV\[(.+?)\]/sys.argv[$1]/g);
    python_import_package('sys') if ($input =~ s/\@ARGV/sys.argv[1:]/g);
    python_import_package('sys') if ($input =~ s/\$\#ARGV/(len(sys.argv)-1)/g);
    python_import_package('fileinput') if ($input =~ s/\s*\<\>/ fileinput.input()/g);
    return $input;
}

sub control_structures {
    my $input = shift @_;
    $input =~ s/\{\s*$/:\n/;
    un_indent() if ($input =~ s/}\s*elsif/elif/);
    un_indent() if ($input =~ s/}\s*else/else/);
    # case deal with foreach loop
    if ($input =~ m/^\s*foreach\s+\$([^\s\.\\\(\)\{\}\[\]\;\+\-\"\'\:\=]+)\s+(.+)\s+\:/) {
        my $var = $1;
        my $list = $2;
        push @declared_var_list, "\$".$var;
        $list =~ s/^\((.+)\)$/$1/g;
        $input = "for ".$var." in ".$list.":\n";
    }

    return $input;
}

sub logic_arithmetic_operators {
    my $input = shift @_;
    # bitwise operators are not needed to be translated
    my %operators = (' \|\| ' => ' or ',
                    ' \&\& ' => ' and ',
                    ' lt ' => ' < ',
                    ' le ' => ' <= ',
                    ' gt ' => ' > ',
                    ' ge ' => ' >= ',
                    ' eq ' => ' == ',
                    ' ne ' => ' != ');
    foreach my $operator (keys %operators) {
        $input =~ s/$operator/$operators{$operator}/g;
    }
    # special case ++ --
    if ($input =~ s/\s*(\$[^\+\-\"\']+)(\+\+|\-\-)\s*;?/$1 $2= 1/g) {
        $input =~ s/\+\+=/\+=/g;
        $input =~ s/\-\-=/\-=/g;
    }
    # special case ! to not
    $input =~ s/(.+?)!([^\=])/$1 not $2/;
    $input =~ s/^\s*//;     # get rid of useless spaces
    $input =~ s/;\s*$/\n/;    # get rid of semi-column
    # detect number variable number = number ** 2;
    $input =~ s/\s*([\w\_\-]+)\s*(\%|\*\*)/float($1) $2/;
    # detect number variable 2 if (a <= 2)
    $input =~ s/([^\\\(\)\=]?[\$\@]?[^\s\.\\\(\)\{\}\[\]\;\+\-\"\'\:\=]+)\s*(<=|>=|<|>|==|!=)\s*([\d\.]+)/float($1) $2 $3/;
    return $input;
}

sub func_print {
    my$input = shift @_;
    my $result = "";
    # If alternate comma notation exists, remove and recursivly call function again.
    if($input =~ m/^\s*(print\s*.*?)[\'\"]?\,\s*[\'\"]?.*?/
        # special case deal with map
        && $input !~ /map\(str,[^\)]+\)[^\"\']+$/){
        $input =~ s/(\))\,\s*[\'\"].+[\'\"]/$1/g;
        $input =~ s/[\'\"]\,\s*([a-zA-Z0-9_\$\@])/$1/g;
        $input =~ s/([\w_\$\@])\,\s*[\'\"].+[\'\"]/$1/g;
        $input =~ s/([\w_])\,\s*([\$\@])/$1$2/g;
        $input =~ s/[\'\"]\,\s*[\'\"]//g;
        $result = func_print($input);
   } elsif($input =~ /^\s*print\s*\"?([^\"]*)(\\n)+\"[\s;]*$/) {
        $result = "print ".func_print_sub($1)."\n";
        $result =~ s/\+/,/g;
        # deal with extra space python  ", str(var) creates
        $result =~ s/\s\"\s*,\s*(str\([^\)]*\))/\",$1/g;
        # print "input after tranlate special cases = $input\n\n";

   } elsif($line =~ /^\s*print\s*\(?"(.*)"\)?[\s;]*$/) {
        python_import_package('sys');
        $result = "sys.stdout.write(".func_print_sub($1).")\n";
   } else {
        if($line =~ /^\s*print([^\"\']+);/) {
            # special case for print variables only;
            python_import_package('sys');
            $result = "sys.stdout.write(".func_print_sub($1).")\n";
        }else {
           $result = check_variables($input);
        }
   }
   return $result;
}

sub func_print_sub {
   my ($input) = @_;
   my $ret_string = '';
   # print "input original value is $input\n";
   if($input =~ /\s+/) {

        $input =~ s/\"\s*,\s*\"//g;
        $input =~ s/(\s*)([\w_\$\:\=\\]+)(\s*)/"$2"/g;
        # print "input after tranlate 1 quote around = $input\n";
        $input =~ s/(\"\"\$)/ $1/g;
        # print "input after tranlate 2 preseve spacing = $input\n";
        $input =~ s/(\"\")([\w_\:\=])/$1 $2/g;
        # print "input after tranlate 3 preseve spacing pt2 = $input\n";
        $input =~ s/\"\"/"+"/g;
        # print "input after tranlate 4 string conc = $input\n";
        $input =~ s/\"(\s*)([^\\]?[\$\@][^\s\.\\\(\)\{\}\[\]\;\+\-\"\'\:\=]+)(\s*)\"/+"$1"+str($2)+"$3"+/g;
        # print "input after tranlate 5 variable interpretation = $input\n";
        $input =~ s/^\++(.*)\++$/$1/;
        # print "input after tranlate 6 = $input\n";
        # trim the +""+ at the start and end of the line
        $input =~ s/\+\"\"\+?\s*$//;
        # print "input after tranlate 7 extra END +\"\" struct = $input\n";
        $input =~ s/^\s*\+?\"\"\+//;
        # print "input after tranlate 8 extra START \"\"+ struct = $input\n";
        $input =~ s/\+\"\"\+/+/g;
        # print "input after tranlate 9 extra +\"\"+ struct = $input\n";
        # trim the unwanted multiple +
        $input =~ s/\+{2,}/\+/g;

        $input =~ s/\"\+\"//g;
        # print "input after tranlate 10 = $input\n";

        $ret_string = check_variables($input);
    } else {
        if ($input =~ /\$/) {
        # variable
            $ret_string = check_variables($input);
        } else {
            $ret_string = "\"$input\"";
        }
    }

   return $ret_string;
}

sub check_variables {
    my $input = shift @_;
    # check for undeclared variables
    check_undeclared_variables($input);
    # scalars
    if ($input =~ s/([^\\]?)[\$]([^\s\.\\\(\)\{\}\[\]\;\+\-\"\'\:\=]+)/$1$2/g) {
       $input =~ tr/{}/[]/; # Translate brackes to scalers
    }

    # array
    if ($input =~ s/([^\\]?)[\@]([^\s\.\\\(\)\{\}\[\]\;\+\-\"\'\:\=]+)/$1$2/g) {
        # clean up the variable declearation
        $input =~ s/\s*\=\s*\(([\w\,\s\"\']+)\)/ = [$1]/;
    }
    # deal with (something .. something)
    $input =~ s/[^\s\:]*([\S]+)\s*\.\.\s*([^\s\:]+)[\'\"]*/xrange($1,$2)/g;
    # special case with xrange
    $input =~ s/(xrange.*)\(len\(sys.argv\)-1\)/$1(len(sys.argv))/;
    $input =~ s/^\s*//;     # get rid of useless spaces
    $input =~ s/;\s*$/\n/;    # get rid of semi-column

    return $input;
}

sub check_undeclared_variables {
    my $input = shift @_;
    # check for undeclared variables
    return if ($input =~ m/(if|elsif|foreach|for|else|ARGV)/);
    my @check_new_Var = $input =~ m/[^\\]?([\$\@\%][^\s\.\\\(\)\{\}\[\]\;\+\-\"\'\:\=]+)/g;
    foreach my $x (@check_new_Var) {
        if (!(grep {$_ eq $x} @declared_var_list)
            and $input !~ /^\\/
            and $input !~ /\s*=\s*/
            and $input !~ /\{\$/) {
            $x =~ s/[\$\@\%]//;
            push @output, output_with_indentation("$x = None\n");
        }
    }
    push @declared_var_list, @check_new_Var;
    @check_new_Var = $input =~ m/[^\\]?(\$[^\s\.\\\(\)\{\}\[\]\;\+\-\"\'\:\=]+\{)/g;
    foreach my $x (@check_new_Var) {
        if (!(grep {$_ eq $x} @declared_var_list)
            and $input !~ /^\\/) {
            my $temp = $x;
            $temp =~ s/[\$\@\%]//;
            $temp =~ s/\{\s*$//;
            push @output, output_with_indentation("$temp = {}\n");
        }
    }
    push @declared_var_list, @check_new_Var;
    my %var_hash = map {$_,1} @declared_var_list;
    @declared_var_list = keys %var_hash;

}