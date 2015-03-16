#!/usr/bin/perl -w
# written by Jiashu Chen
# http://www.cse.unsw.edu.au/~cs2041/assignments/mekong/

use CGI qw/:all/;

$debug = 0;
$| = 1;

if (!@ARGV) {
    # run as a CGI script
    cgi_main();
    
} else {
    # for debugging purposes run from the command line
    console_main();
}
exit 0;

# This is very simple CGI code that goes straight
# to the search screen and it doesn't format the
# search results at all

# This is very simple CGI code that goes straight
# to the search screen and it doesn't format the
# search results at all

sub cgi_main {
    print page_header();
    
    set_global_variables();
    read_books($books_file);

    my $action = param('action');
    my $search_terms = param('search_terms');
    my $lostpass = param('lostpass');
    my $purchaser = param('purchaser');
    if (!defined $action) {
        if (defined $lostpass) {
            print p,start_form;
            print reset_password_page($lostpass);
            print end_form,p;
        }elsif (defined $purchaser) {
            my $login = param('purchaser');
            my $credit_card_number = param('credit_card_number');
            my $expiry_date = param('expiry_date');
            finalize_order($login,$credit_card_number,$expiry_date);
            $last_error = "Order confirmed, Login to view your order";
            print p,start_form;
            print login_form($login);
            print end_form,p;
        } else {
            print '<h3 align="center">Welcome To Mekong.com.au</h3>';
            print p,start_form;
            print login_form();
            print end_form,p;
        }
    } else {
        if ($action eq 'Login') {
            my $login = param('login');
            my $password = param('password');
            if (legal_login($login)
                && legal_password($password)
                && authenticate($login, $password)) {
                print p,start_form,search_form($login),end_form,p;
                print website_option($action, $login);
            }else {
                print start_form;
                print login_form();
                print end_form;
            }
        } elsif ($action eq "Create New Account") {
            print '<h3 align="center">Register A New Account</h3>';
            print start_form, create_new_account_page(), end_form;
        } elsif ($action eq "Create Account") {
            my $login = param('login');
            my $password = param('password');
            my $name = param('name');
            my $street = param('street');
            my $city = param('city');
            my $state = param('state');
            my $postCode = param('postcode');
            my $email = param('email');

            if (create_account($login, $password, $name, $street, $city, $state, $postCode, $email)) {
                print '<h3 align="center">Account Created</h3>';
                print p,start_form,search_form($login),end_form,p;
                print website_option($action, $login);
            } else {
                print '<h3 align="center">Register A New Account</h3>';
                print start_form, create_new_account_page($login, $password, $name, $street, $city, $state, $postCode, $email), end_form;
            }
        } elsif ($action eq 'Search') {
            my $search_terms = param('search_terms');
            my $login = param('login');
            print p,start_form,search_form($login),end_form,p;
            print search_results($login,$search_terms);
            print website_option($action, $login);

        } elsif ($action eq 'Add') {
            my $search_terms = param('search_terms');
            my $isbn = param('requested_book');
            my $login = param('login');
            
            add_basket($login, $isbn);

            print '<h2 align="center">Book has been added to your basket.</h2>';         
            print p,start_form,search_form($login),end_form,p;
            print search_results($login, $search_terms);
            print website_option($action, $login);

        } elsif ($action eq 'Details') {
            my $search_terms = param('search_terms');
            my $isbn = param('requested_book');
            my $login = param('login');

            # get_book_descriptions($isbn); # this will set the global variable for $book_details
            print book_details($isbn);         
            print website_option($action, $login, $isbn, $search_terms);

        }elsif ($action eq 'Basket'){
            my $login = param('login');
            print '<h2 align="center">Shopping basket for '.$login.'</h2>';
            print p,start_form,search_form($login),end_form,p;
            print basket_page($login);
            print website_option($action, $login);

        } elsif ($action eq 'Drop') {
            my $isbn = param('requested_book');
            my $login = param('login');

            delete_basket($login, $isbn);

            print '<h2 align="center">Shopping basket for '.$login.'</h2>';
            print p,start_form,search_form($login),end_form,p;
            print basket_page($login);
            print website_option($action, $login);

        } elsif ($action eq 'Check Out') {
            my $login = param('login');
            print '<h2 align="center">Check out as '.$login.' with the following items.</h2>';
            print p,start_form,search_form($login),end_form,p;
            print basket_page($login);
            print checkout_page($login) if read_basket($login);
            print website_option($action, $login);
        } elsif ($action eq 'Confirm Order') {
            my $login = param('login');
            my $credit_card_number = param('credit_card_number');            
            my $expiry_date = param('expiry_date');

            if (legal_credit_card_number($credit_card_number)
                && legal_expiry_date($expiry_date)) {
                $message = $ENV{REDIRECT_SCRIPT_URI}.'?purchaser='.$login.'&credit_card_number='.$credit_card_number.'&expiry_date='.$expiry_date;
                print send_email($login, $message, $action);

                # finalize_order($login,$credit_card_number,$expiry_date);
                print '<h2 align="center">Shopping basket for '.$login.'. Check Email for validation</h2>';
                print p,start_form,search_form($login),end_form,p;
                print basket_page($login);
                print website_option('Basket', $login);
            } else {
                print '<h2 align="center">Check out as '.$login.'.</h2>';
                print p,start_form,search_form($login),end_form,p;
                print basket_page($login);
                print checkout_page($login);
                print website_option('Check Out', $login);
            }
        } elsif ($action eq 'View Orders') {
            my $login = param('login');
            print p,start_form,search_form($login),end_form,p;
            print order_page($login);
            print website_option($action, $login);
        } elsif ($action eq 'Logout') {
            print start_form;
            print login_form();
            print end_form;
        } elsif ($action eq 'Forgot Password') {
            my $login = param('login');
            if ($login) {
                print '<h2 align="center">Email has sent to you for password change.</h2>';
                $message = $ENV{SCRIPT_URI}.'?lostpass='.$login;
                print send_email($login, $message, $action);
                print start_form;
                print login_form();
                print end_form;
            } else {
                $last_error = "Please type your user name in the login field, and then click Forgot Password Button";
                print start_form;
                print login_form();
                print end_form;            
            }
        } elsif ($action eq 'Reset') {
            my $login = param('login');
            my $password = param('password');
            reset_password($login, $password);
            if (legal_password($password)) {
                print start_form;
                print login_form();
                print end_form;
            } else {
                print p,start_form;
                print reset_password_page($lostpass);
                print end_form,p;

            }
        } else {
            print nof_found();
        }
    }
    
    print page_trailer();
}

sub reset_password {
    my ($login,$password) = @_;
    %user_info = get_user_info($login);

    if (!legal_login($login)) {
    } elsif (!legal_password($password)) {
    } elsif (!(-r "$users_dir/$login")) {
        $last_error = "Invalid user name: login doesn't exists.\n";
    } elsif (!open(new_user, ">$users_dir/$login")) {
        $last_error = "Can not create user file $users_dir/$login: $!";
    } else {
        print new_user "password=$password\n";
        print new_user "name=".$user_info{name}."\n";
        print new_user "street=".$user_info{street}."\n";
        print new_user "city=".$user_info{city}."\n";
        print new_user "state=".$user_info{state}."\n";
        print new_user "postcode=".$user_info{postcode}."\n";
        print new_user "email=".$user_info{email}."\n";
        close(new_user);
        return 1;
    }
}

sub reset_password_page {
    my ($login) = @_;
    return '
        <p>
            <table align="center">
               <caption><font color=red>'.$last_error.'</font></caption>
             <tr>
                <td>New Password:</td> 
                <td><input type="password" name="password"  width="10" /></td>
             </tr>
             <tr>
                <td align="center" colspan="2"> 
                    <input type="hidden" name="login" value="'.$login.'" />
                    <input class="btn" type="submit" name="action" value="Reset"/>
                </td>
             </tr>
          </table>
        <p>
    ';

}

sub send_email {
    my ($login, $mail_message, $subject) = @_;
    my %user_info = get_user_info($login);
    my $mail_address = $user_info{email};
    my $mailprog = "/usr/lib/sendmail";
    open my $mail, '|-', "$mailprog -t -oi" or die $!;
    print $mail "To: $mail_address\n";
    print $mail "From: noreply\@mekong.com.au\n";
    print $mail "Subject: $subject\n";
    print $mail "\n";
    print $mail "$mail_message";
    close $mail;
    return ;
}

sub order_page {
    my ($login) = @_;
    my $result = '<div>';
    foreach $order (login_to_orders($login)) {
        my ($order_time, $credit_card_number, $expiry_date, @isbns) = read_order($order);
        my @descriptions = get_book_descriptions(@isbns);
        my $total_cost = total_books(@isbns);
        $order_time = localtime($order_time);
        $result .= '<p></p>
                        <table align="center" border=1>';
        $result .= '<tr align="center" bgcolor:green>
                        <td colspan="4">
                            <span style="font-family:Cursive;font-size:18px;font-style:normal;font-weight:normal;text-decoration:underline;text-transform:capitalize;color:FFFFF;">
                            Order #'.$order.' - '.$order_time.' <br />
                            Credit Card Number = '.$credit_card_number.' (Expiry '.$expiry_date.')
                        </span>
                        </td>
                    </tr>
                    <tr>
                        <th>Thumbnail</th>
                        <th>Title/Author</th>
                        <th>Price</th>
                        <th>Option</th>
                    </tr>';
        foreach my $x (@descriptions) {

            my ($isbn, $price, $title, $author, $smallImageURL) = split('\&\&', $x);
            $result .= '<tr>
                        <td width="56">
                            <img src="'.$smallImageURL.'">
                        </td>
                        <td width="450">
                            <i>'.$title.'</i><br>
                            '.$author.'<br>
                        </td>
                        <td align="right" width="60">
                            <tt>'.$price.'</tt>
                        </td>
                        <td valign="middle">
                            <form method="post" enctype="multipart/form-data">
                                <input type="hidden" name="requested_book" value="'.$isbn.'" />
                                <input type="hidden" name="search_terms" value="" />
                                <input type="hidden" name="login" value="'.$login.'" />
                                <input class="btn" type="submit" name="action" value="Details"><br>
                            </form>
                        </td>
                    </tr>';
        }
        $result .= '
                    <tr>
                        <td>Total</td>
                        <td></td>
                        <td>$'.$total_cost.'</td>
                        <td></td>
                    </tr>
                </table>
            <p></p>
        ';

    }

    $result .= '</div>';
    return $result;
}

sub checkout_page {
    my ($login) = @_;
    my %user_info = get_user_info($login);
    return '
            <b>Shipping Details:</b>
            <br />
            <table align=center border=1px>
            <tr>
                <th>
                    Name:
                </th>
                <th>
                    <span style="text-transform:capitalize">'.$user_info{name}.'</span>
                </th>
            </tr>
            <tr>
                <th>
                    Street:
                </th>
                <th>
                    <span style="text-transform:capitalize">'.$user_info{street}.'</span>
                </th>
            </tr>
            <tr>
                <th>
                    City:
                </th>
                <th>
                    <span style="text-transform:capitalize">'.$user_info{city}.'</span>
                </th>
            </tr>
            <tr>
                <th>
                    State:
                </th>            
                <th>
                    <span style="text-transform:uppercase">'.$user_info{state}.'</span>
                </th>
            </tr>
            <tr>
                <th>
                    Postcode:
                </th>
                <th>
                    <span style="text-transform:capitalize">'.$user_info{postcode}.'</span>
                </th>
            </tr>
            </table>
            <p></p>
            <p></p>
             <form method="post" enctype="multipart/form-data">
                <input type="hidden" name="login" value="'.$login.'" />   
                   <table align="center">
                        <tr>
                            <td>Credit Card Number:</td>
                            <td><input type="text" name="credit_card_number" /></td>
                        </tr>
                        <tr>
                            <td>Expiry date (mm/yy):</td>
                            <td><input type="text" name="expiry_date" /></td>
                        </tr>
                        <tr>
                            <td colspan="2" align="center">
                            <input class="btn" type="submit" name="action" value="Confirm Order">
                        </td>
                     </tr>
                  </table>
            </form>
    ';
}

sub get_user_info {
    my ($login) = @_;
    my %user_info;   
    if(-r "$users_dir/$login") {
        if (open("USER", "<$users_dir/$login")) {
            while($line = <USER>) {
                chomp($line);
                ($attr, $info) = split('\=', $line);
                $user_info{$attr} = $info;
            }
            close(USER);
        } else {
           print "Can not read user file $users_dir/$login: $!";
        }   
    } else {
        print "Invalid user name: login doesn't exist.\n";
    }
    return %user_info;
}

sub basket_page {
    my ($login) = @_;
    my @isbn = read_basket($login);
    my $total_cost = total_books(@isbn);
    if (!@isbn) {
        return '
            <table align="center"> 
                <tr>
                    <td colspan="4">Your shopping basket is empty</td>
                </tr>
            </table>
        ';
    }

    $result .= '<table align="center" border="1">
                <tr>
                    <th>Thumbnail</th>
                    <th>Title/Author</th>
                    <th>Price</th>
                    <th>Option</th>
                </tr>';

    foreach my $x (@isbn) {
        $result .= '<tr>
                        <td>
                            <img src="'.$book_details{$x}{smallimageurl}.'">
                        </td>
                        <td width="500">
                            <i>'.$book_details{$x}{title}.'</i><br>
                            '.$book_details{$x}{authors}.'</br>
                        </td>
                        <td align="right">
                            <tt>'.$book_details{$x}{price}.'</tt>
                        </td>
                        <td valign="baseline">
                            <form method="post" enctype="multipart/form-data">
                                <input type="hidden" name="requested_book" value="'.$x.'" />
                                <input type="hidden" name="search_terms" value="" />
                                <input type="hidden" name="login" value="'.$login.'" />
                                <input class="btn" type="submit" name="action" value="Drop"><br>
                                <input class="btn" type="submit" name="action" value="Details"><br>
                            </form>
                        </td>
                    </tr>';
    }

    $result .= '
            <tr>
                <td>Total</td>
                <td></td>
                <td>$'.$total_cost.'</td>
                <td></td>
            </tr>
        </table>
    ';

    return $result;
}

sub book_details {  
    my ($isbn) = @_;
    my $result ='
               <h2 align="center">'.$book_details{$isbn}{title}.'</h2>
                <table bgcolor="white" align="center">
                    <tr>
                        <td width="30%"><div><img src="'.$book_details{$isbn}{largeimageurl}.'" /></div></td>
                        <td width="70%"><div>'.$book_details{$isbn}{productdescription}.'</div></td>
                    </tr>                    
                </table></tr></table>           
                <table bgcolor="white" align="center">
            ';
    foreach my $x (sort keys %{$book_details{$isbn}}) {
        if ($x ne 'title'
            && $x ne 'productdescription'
            && $x ne 'largeimageurl'
            && $x !~ m/image/) {

            $result .= '<tr>
                        <td><b>'.$x.'</b></td>
                        <td>'.$book_details{$isbn}{$x}.'</td>
                        </tr>';
        }
    }
    $result .= '</table></tr></table>';        
         
   return $result;
}

sub website_option {
    my ($options, $login) = @_;

    $result = '<form method="post" enctype="multipart/form-data"><p></p>';

    if ($options eq 'Login' ) {
        $result .= '
            <table align="center">
            <tr>
               <td align="center" colspan="3">
                    <input type="hidden" name="login" value="'.$login.'" />
                    <input class="btn" type="submit" name="action" value="Basket">
                    <input class="btn" type="submit" name="action" value="Check Out">
                    <input class="btn" type="submit" name="action" value="View Orders">
                    <input class="btn" type="submit" name="action" value="Logout">
               </td>
            </tr>
            </table>
        ';
    } elsif ($options eq 'Add' 
            || $options eq 'Search'
            || $options eq 'Create Account'
            || $options eq 'Basket'
            || $options eq 'Drop'
            || $options eq 'Confirm Order') {
        $result .= '
            <table align="center">
            <tr>
               <td align="center" colspan="4"> 
                    <input type="hidden" name="login" value="'.$login.'" />
                    <input class="btn" type="submit" name="action" value="Basket">
                    <input class="btn" type="submit" name="action" value="Check Out">
                    <input class="btn" type="submit" name="action" value="View Orders">
                    <input class="btn" type="submit" name="action" value="Logout">
               </td>
            </tr>
            </table>
        ';
    } elsif ($options eq 'View Orders') {
        $result .= '
            <table align="center">
            <tr>
               <td align="center" colspan="4"> 
                    <input type="hidden" name="login" value="'.$login.'" />
                    <input class="btn" type="submit" name="action" value="Basket">
                    <input class="btn" type="submit" name="action" value="Check Out">
                    <input class="btn" type="submit" name="action" value="Logout">
               </td>
            </tr>
            </table>
        ';
    } elsif ($options eq 'Check Out') {
        $result .= '
        <table align="center">
            <tr>
                <td align="center" colspan="3">
                    <input type="hidden" name="login" value="'.$login.'" />
                    <input class="btn" type="submit" name="action" value="Basket">
                    <input class="btn" type="submit" name="action" value="View Orders">
                    <input class="btn" type="submit" name="action" value="Logout">
               </td>
            </tr>
        </table>';
    } elsif ($options eq 'Details') {
        # if the option is Deatils the function takes in
        my ($options, $login,$isbn,$search_terms) = @_;        
        $result .= '
        <table align="center">
            <tr>
               <td align="center" colspan="4">
                    <input type="hidden" name="login" value="'.$login.'" />
                    <input type="hidden" name="requested_book" value="'.$isbn.'" />
                    <input type="hidden" name="search_terms" value="'.$search_terms.'" />
                    <input class="btn" type="submit" name="action" value="Add">
                    <input class="btn" type="submit" name="action" value="Basket">
                    <input class="btn" type="submit" name="action" value="Check Out">
                    <input class="btn" type="submit" name="action" value="Logout">
               </td>
            </tr>
         </table>
        ';
    }

    $result .= '</form><p></p>';
    return $result;
}

# create new account page
sub create_account {
    my ($login, $password, $name, $street, $city, $state, $postCode, $email) = @_;
    if (!legal_login($login)) {
    } elsif (!legal_password($password)) {
    } elsif (!legal_email($email)) {
    } elsif (!legal_postcode($postCode)) {
    } elsif (-r "$users_dir/$login") {
        $last_error = "Invalid user name: login already exists.\n";
    } elsif (!open(new_user, ">$users_dir/$login")) {
        $last_error = "Can not create user file $users_dir/$login: $!";
    } else {
        print new_user "password=$password\n";
        print new_user "name=$name\n";
        print new_user "street=$street\n";
        print new_user "city=$city\n";
        print new_user "state=$state\n";
        print new_user "postcode=$postCode\n";
        print new_user "email=$email\n";
        close(new_user);
        return 1;
    }

    return 0;
}

sub legal_email {
   my ($email) = @_;
   our ($last_error);

   if ($email !~ /^[^@]+\@[^@]+$/) {
      $last_error = "Invalid email '".$email."': emails must be in the following format example\@you.com";
      return 0;
   }
   return 1;
}

sub legal_postcode {
   my ($postCode) = @_;
   our ($last_error);

   if ($postCode !~ /^\d+$/) {
      $last_error = "Invalid postcode '".$postCode."': postcode must be digits only";
      return 0;
   }
   return 1;
}
sub create_new_account_page {
    my @info_row = @_;
    my @iformation = (
          'text|login|Login:|10',
          'password|password|Password:|10',
          'text|name|Full Name:|50',
          'text|street|Street:|50',
          'text|city|City/Suburb:|25',
          'text|state|State:|25',
          'text|postcode|Postcode:|25',
          'text|email|Email Address:|35'
          );
    my $result = '<table align="center">
                        <caption><font color=red>'.$last_error.'</font></caption>
                  ';
    foreach my $x (@iformation) {
        $attr_val = shift @info_row if (defined $info_row[0]);
        ($attr_type,$attr_name,$attr_title,$attr_width) = split ('\|', $x);
        if (defined $attr_val) {
            $result .= '<tr>
                        <td>'.$attr_title.'</td>
                        <td><input type="'.$attr_type.'" name="'.$attr_name.'" width="'.$attr_width.'" value="'.$attr_val.'" /></td>
                     </tr>';        
        } else {
            $result .= '<tr>
                        <td>'.$attr_title.'</td>
                        <td><input type="'.$attr_type.'" name="'.$attr_name.'" width="'.$attr_width.'" /></td>
                     </tr>';
        }
        
    }
    $result .= '<tr>
                        <td align="center" colspan="1">
                        <input class="btn" type="submit" name="action" value="Create Account">
                        </td>
                     </tr>
                 </table>
              ';
    return $result;
}

# simple login form without authentication  
sub login_form {
    return return '
    <p>
        <table align="center">
           <caption><font color=red>'.$last_error.'</font></caption>
           <tr>
              <td>Login:</td> 
              <td><input type="text" name="login"  width="10" /></td>
           </tr>
         <tr>
            <td>Password:</td> 
            <td><input type="password" name="password"  width="10" /></td>
         </tr>
         <tr>
            <td align="center" colspan="2"> 
               <input class="btn" type="submit" name="action" value="Login"/>
               <input class="btn" type="submit" name="action" value="Create New Account"/>
               <input class="btn" type="submit" name="action" value="Forgot Password"/>
            </td>
         </tr>
      </table>
    <p>
';
}

# simple search form
sub search_form {
    my ($login) = @_;
    return <<eof
    <p>You're currently logged in as $login!</p>
    <table align="center">
        <caption><font color=red>$last_error</font></caption>
        <tr>
        <td valign="middle">
        Search: 
        </td>
        <td valign="middle">
        <input type="text" name="search_terms" width=50 value="Search Here"></input>
        </td>
        <td valign="top">
        <input class="btn" type="submit" name="action" value="Search"/>
        </td>
        </tr>
    </table>
    <input type="hidden" name="login" value="$login" />
eof
}

# ascii display of search results
sub search_results {
    my ($login,$search_terms) = @_;
    return 0 if !legal_login($login);
    my @matching_isbns = search_books($search_terms);
    my @descriptions = get_book_descriptions(@matching_isbns);

    $results = '<table align="center" border=1>
                    <tr>
                        <th>Thumbnail</th>
                        <th>Title/Author</th>
                        <th>Price</th>
                        <th>Option</th>
                    </tr>';

    foreach my $x (@descriptions) {
       my ($isbn, $price, $title, $author, $smallImageURL) = split('\&\&', $x);
           
        $results .= '<tr>
                        <td>
                            <img src="'.$smallImageURL.'">
                        </td>
                        <td>
                            <i>'.$title.'</i><br>
                            '.$author.'<br>
                        </td>
                        <td align="right">
                            <tt>'.$price.'</tt>
                        </td>
                        <td>
                            <form method="post" enctype="multipart/form-data">
                                <input type="hidden" name="requested_book" value="'.$isbn.'" />
                                <input type="hidden" name="search_terms" value="'.$search_terms.'" />
                                <input type="hidden" name="login" value="'.$login.'" />
                                <input class="btn" type="submit" name="action" value="Add"><br>
                                <input class="btn" type="submit" name="action" value="Details"><br>
                            </form>
                        </td>
                    </tr>';
    }

    $results .= '</table>';
    return $results;
}

#
# HTML at top of every screen
#
sub page_header() {
    return <<eof;
Content-Type: text/html

<!DOCTYPE html>
<html lang="en">
<head>
<title>mekong.com.au</title>
<link href="//netdna.bootstrapcdn.com/twitter-bootstrap/2.3.1/css/bootstrap-combined.min.css" rel="stylesheet">
<script src="//netdna.bootstrapcdn.com/twitter-bootstrap/2.3.1/js/bootstrap.min.js"></script>
<style>
body
{
background-size: 100% 100%;
/* Mozilla Firefox */ 
background-image: -moz-linear-gradient(bottom left, #FFFFFF 0%, #00A3EF 100%);

/* Opera */ 
background-image: -o-linear-gradient(bottom left, #FFFFFF 0%, #00A3EF 100%);

/* Webkit (Safari/Chrome 10) */ 
background-image: -webkit-gradient(linear, left bottom, right top, color-stop(0, #FFFFFF), color-stop(1, #00A3EF));

/* Webkit (Chrome 11+) */ 
background-image: -webkit-linear-gradient(bottom left, #FFFFFF 0%, #00A3EF 100%);
min-height: 1080px;
}
h2
{
/* Mozilla Firefox */ 
background-image: -moz-radial-gradient(center, ellipse closest-corner, #30CFFF 0%, #E8EF16 100%);

/* Opera */ 
background-image: -o-radial-gradient(center, ellipse closest-corner, #30CFFF 0%, #E8EF16 100%);

/* Webkit (Safari/Chrome 10) */ 
background-image: -webkit-gradient(radial, center center, 0, center center, 497, color-stop(0, #30CFFF), color-stop(1, #E8EF16));

/* Webkit (Chrome 11+) */ 
background-image: -webkit-radial-gradient(center, ellipse closest-corner, #30CFFF 0%, #E8EF16 100%);
}
h3
{
background: rgb(30,87,153); /* Old browsers */
background: -moz-radial-gradient(center, ellipse cover, rgb(30,87,153) 0%, rgb(41,137,216) 50%, rgb(32,124,202) 51%, rgb(125,185,232) 100%); /* FF3.6+ */
background: -webkit-gradient(radial, center center, 0px, center center, 100%, color-stop(0%,rgb(30,87,153)), color-stop(50%,rgb(41,137,216)), color-stop(51%,rgb(32,124,202)), color-stop(100%,rgb(125,185,232))); /* Chrome,Safari4+ */
background: -webkit-radial-gradient(center, ellipse cover, rgb(30,87,153) 0%,rgb(41,137,216) 50%,rgb(32,124,202) 51%,rgb(125,185,232) 100%); /* Chrome10+,Safari5.1+ */
background: -o-radial-gradient(center, ellipse cover, rgb(30,87,153) 0%,rgb(41,137,216) 50%,rgb(32,124,202) 51%,rgb(125,185,232) 100%); /* Opera 12+ */
background: -ms-radial-gradient(center, ellipse cover, rgb(30,87,153) 0%,rgb(41,137,216) 50%,rgb(32,124,202) 51%,rgb(125,185,232) 100%); /* IE10+ */
background: radial-gradient(ellipse at center, rgb(30,87,153) 0%,rgb(41,137,216) 50%,rgb(32,124,202) 51%,rgb(125,185,232) 100%); /* W3C */
filter: progid:DXImageTransform.Microsoft.gradient( startColorstr='#1e5799', endColorstr='#7db9e8',GradientType=1 ); /* IE6-9 fallback on horizontal gradient */
}
th
{
background-color:green;
color:white;
}
.centererer
{
    border-radius: 30px;
    -moz-border-radius: 30px;
    -khtml-border-radius: 30px;
    -webkit-border-radius: 30px;
    height:400px;
    width:1280px;
    margin:auto;
    background: url('http://alumni.virginia.edu/wp-content/uploads/2012/04/banner-big-lawn-1280x400.jpg');
}
</style></head>
<body>
<div class="centererer"/>
</div>
<div class="container">
eof
}

#
# HTML at bottom of every screen
#
sub page_trailer() {
#     my $debugging_info = debugging_info();
    
#     return <<eof;
#     $debugging_info
#     </div>
# <body>
# </html>
# eof
    return <<eof;
    </div>
<body>
</html>
eof
}

#
# Print out information for debugging purposes
#
sub debugging_info() {
    my $params = "";
    foreach $p (param()) {
        $params .= "param($p)=".param($p)."\n"
    }

    return <<eof;
<hr>
<h4>Debugging information - parameter values supplied to $0</h4>
<pre>
$params
</pre>
<hr>
eof
}




###
### Below here are utility functions
### Most are unused by the code above, but you will 
### need to use these functions (or write your own equivalent functions)
### 
###

# return true if specified string can be used as a login

sub legal_login {
    my ($login) = @_;
    our ($last_error);

    if ($login !~ /^[a-zA-Z][a-zA-Z0-9]*$/) {
        $last_error = "Invalid login '$login': logins must start with a letter and contain only letters and digits.";
        return 0;
    }
    if (length $login < 3 || length $login > 8) {
        $last_error = "Invalid login: logins must be 3-8 characters long.";
        return 0;
    }
    return 1;
}

# return true if specified string can be used as a password

sub legal_password {
    my ($password) = @_;
    our ($last_error);
    
    if ($password =~ /\s/) {
        $last_error = "Invalid password: password can not contain white space.";
        return 0;
    }
    if (length $password < 5) {
        $last_error = "Invalid password: passwords must contain at least 5 characters.";
        return 0;
    }
    return 1;
}


# return true if specified string could be an ISBN

sub legal_isbn {
    my ($isbn) = @_;
    our ($last_error);
    
    return 1 if $isbn =~ /^\d{9}(\d|X)$/;
    $last_error = "Invalid isbn '$isbn' : an isbn must be exactly 10 digits.";
    return 0;
}


# return true if specified string could be an credit card number

sub legal_credit_card_number {
    my ($number) = @_;
    our ($last_error);
    
    return 1 if $number =~ /^\d{16}$/;
    $last_error = "Invalid credit card number - must be 16 digits.\n";
    return 0;
}

# return true if specified string could be an credit card expiry date

sub legal_expiry_date {
    my ($expiry_date) = @_;
    our ($last_error);
    
    return 1 if $expiry_date =~ /^\d\d\/\d\d$/;
    $last_error = "Invalid expiry date - must be mm/yy, e.g. 11/04.\n";
    return 0;
}



# return total cost of specified books

sub total_books {
    my @isbns = @_;
    our %book_details;
    $total = 0;
    foreach $isbn (@isbns) {
        die "Internal error: unknown isbn $isbn  in total_books" if !$book_details{$isbn}; # shouldn't happen
        my $price = $book_details{$isbn}{price};
        $price =~ s/[^0-9\.]//g;
        $total += $price;
    }
    return $total;
}

# return true if specified login & password are correct
# user's details are stored in hash user_details

sub authenticate {
    my ($login, $password) = @_;
    our (%user_details, $last_error);
    
    return 0 if !legal_login($login);
    if (!open(USER, "$users_dir/$login")) {
        $last_error = "User '$login' does not exist.";
        return 0;
    }
    my %details =();
    while (<USER>) {
        next if !/^([^=]+)=(.*)/;
        $details{$1} = $2;
    }
    close(USER);
    foreach $field (qw(name street city state postcode password)) {
        if (!defined $details{$field}) {
            $last_error = "Incomplete user file: field $field missing";
            return 0;
        }
    }
    if ($details{"password"} ne $password) {
        $last_error = "Incorrect password.";
        return 0;
     }
     %user_details = %details;
     return 1;
}

# read contents of files in the books dir into the hash book
# a list of field names in the order specified in the file
 
sub read_books {
    my ($books_file) = @_;
    our %book_details;
    print STDERR "read_books($books_file)\n" if $debug;
    open BOOKS, $books_file or die "Can not open books file '$books_file'\n";
    my $isbn;
    while (<BOOKS>) {
        if (/^\s*"(\d+X?)"\s*:\s*{\s*$/) {
            $isbn = $1;
            next;
        }
        next if !$isbn;
        my ($field, $value);
        if (($field, $value) = /^\s*"([^"]+)"\s*:\s*"(.*)",?\s*$/) {
            $attribute_names{$field}++;
            print STDERR "$isbn $field-> $value\n" if $debug > 1;
            $value =~ s/([^\\]|^)\\"/$1"/g;
            $book_details{$isbn}{$field} = $value;
        } elsif (($field) = /^\s*"([^"]+)"\s*:\s*\[\s*$/) {
            $attribute_names{$1}++;
            my @a = ();
            while (<BOOKS>) {
                last if /^\s*\]\s*,?\s*$/;
                push @a, $1 if /^\s*"(.*)"\s*,?\s*$/;
            }
            $value = join("\n", @a);
            $value =~ s/([^\\]|^)\\"/$1"/g;
            $book_details{$isbn}{$field} = $value;
            print STDERR "book{$isbn}{$field}=@a\n" if $debug > 1;
        }
    }
    close BOOKS;
}

# return books matching search string

sub search_books {
    my ($search_string) = @_;
    $search_string =~ s/\s*$//;
    $search_string =~ s/^\s*//;
    return search_books1(split /\s+/, $search_string);
}

# return books matching search terms

sub search_books1 {
    my (@search_terms) = @_;
    our %book_details;
    print STDERR "search_books1(@search_terms)\n" if $debug;
    my @unknown_fields = ();
    foreach $search_term (@search_terms) {
        push @unknown_fields, "'$1'" if $search_term =~ /([^:]+):/ && !$attribute_names{$1};
    }
    printf STDERR "$0: warning unknown field%s: @unknown_fields\n", (@unknown_fields > 1 ? 's' : '') if @unknown_fields;
    my @matches = ();
    BOOK: foreach $isbn (sort keys %book_details) {
        my $n_matches = 0;
        if (!$book_details{$isbn}{'=default_search='}) {
            $book_details{$isbn}{'=default_search='} = ($book_details{$isbn}{title} || '')."\n".($book_details{$isbn}{authors} || '');
            print STDERR "$isbn default_search -> '".$book_details{$isbn}{'=default_search='}."'\n" if $debug;
        }
        print STDERR "search_terms=@search_terms\n" if $debug > 1;
        foreach $search_term (@search_terms) {
            my $search_type = "=default_search=";
            my $term = $search_term;
            if ($search_term =~ /([^:]+):(.*)/) {
                $search_type = $1;
                $term = $2;
            }
            print STDERR "term=$term\n" if $debug > 1;
            while ($term =~ s/<([^">]*)"[^"]*"([^>]*)>/<$1 $2>/g) {}
            $term =~ s/<[^>]+>/ /g;
            next if $term !~ /\w/;
            $term =~ s/^\W+//g;
            $term =~ s/\W+$//g;
            $term =~ s/[^\w\n]+/\\b +\\b/g;
            $term =~ s/^/\\b/g;
            $term =~ s/$/\\b/g;
            next BOOK if !defined $book_details{$isbn}{$search_type};
            print STDERR "search_type=$search_type term=$term book=$book_details{$isbn}{$search_type}\n" if $debug;
            my $match;
            eval {
                my $field = $book_details{$isbn}{$search_type};
                # remove text that looks like HTML tags (not perfect)
                while ($field =~ s/<([^">]*)"[^"]*"([^>]*)>/<$1 $2>/g) {}
                $field =~ s/<[^>]+>/ /g;
                $field =~ s/[^\w\n]+/ /g;
                $match = $field !~ /$term/i;
            };
            if ($@) {
                $last_error = $@;
                $last_error =~ s/;.*//;
                return (); 
            }
            next BOOK if $match;
            $n_matches++;
        }
        push @matches, $isbn if $n_matches > 0;
    }
    
    sub bySalesRank {
        my $max_sales_rank = 100000000;
        my $s1 = $book_details{$a}{SalesRank} || $max_sales_rank;
        my $s2 = $book_details{$b}{SalesRank} || $max_sales_rank;
        return $a cmp $b if $s1 == $s2;
        return $s1 <=> $s2;
    }
    
    return sort bySalesRank @matches;
}


# return books in specified user's basket

sub read_basket {
    my ($login) = @_;
    our %book_details;
    open F, "$baskets_dir/$login" or return ();
    my @isbns = <F>;

    close(F);
    chomp(@isbns);
    !$book_details{$_} && die "Internal error: unknown isbn $_ in basket\n" foreach @isbns;
    return @isbns;
}


# delete specified book from specified user's basket
# only first occurance is deleted

sub delete_basket {
    my ($login, $delete_isbn) = @_;
    my @isbns = read_basket($login);
    open F, ">$baskets_dir/$login" or die "Can not open $baskets_dir/$login: $!";
    foreach $isbn (@isbns) {
        if ($isbn eq $delete_isbn) {
            $delete_isbn = "";
            next;
        }
        print F "$isbn\n";
    }
    close(F);
    unlink "$baskets_dir/$login" if ! -s "$baskets_dir/$login";
}


# add specified book to specified user's basket

sub add_basket {
    my ($login, $isbn) = @_;
    open F, ">>$baskets_dir/$login" or die "Can not open $baskets_dir/$login::$! \n";
    print F "$isbn\n";
    close(F);
}


# finalize specified order

sub finalize_order {
    my ($login, $credit_card_number, $expiry_date) = @_;
    my $order_number = 0;

    if (open ORDER_NUMBER, "$orders_dir/NEXT_ORDER_NUMBER") {
        $order_number = <ORDER_NUMBER>;
        chomp $order_number;
        close(ORDER_NUMBER);
    }
    $order_number++ while -r "$orders_dir/$order_number";
    open F, ">$orders_dir/NEXT_ORDER_NUMBER" or die "Can not open $orders_dir/NEXT_ORDER_NUMBER: $!\n";
    print F ($order_number + 1);
    close(F);

    my @basket_isbns = read_basket($login);
    open ORDER,">$orders_dir/$order_number" or die "Can not open $orders_dir/$order_number:$! \n";
    print ORDER "order_time=".time()."\n";
    print ORDER "credit_card_number=$credit_card_number\n";
    print ORDER "expiry_date=$expiry_date\n";
    print ORDER join("\n",@basket_isbns)."\n";
    close(ORDER);
    unlink "$baskets_dir/$login";
    
    open F, ">>$orders_dir/$login" or die "Can not open $orders_dir/$login:$! \n";
    print F "$order_number\n";
    close(F);
    
}


# return order numbers for specified login

sub login_to_orders {
    my ($login) = @_;
    open F, "$orders_dir/$login" or return ();
    @order_numbers = <F>;
    close(F);
    chomp(@order_numbers);
    return @order_numbers;
}



# return contents of specified order

sub read_order {
    my ($order_number) = @_;
    open F, "$orders_dir/$order_number" or warn "Can not open $orders_dir/$order_number:$! \n";
    @lines = <F>;
    close(F);
    chomp @lines;
    foreach (@lines[0..2]) {s/.*=//};
    return @lines;
}

###
### functions below are only for testing from the command line
### Your do not need to use these funtions
###

sub console_main {
    set_global_variables();
    $debug = 1;
    foreach $dir ($orders_dir,$baskets_dir,$users_dir) {
        if (! -d $dir) {
            print "Creating $dir\n";
            mkdir($dir, 0777) or die("Can not create $dir: $!");
        }
    }
    read_books($books_file);
    my @commands = qw(login new_account search details add drop basket checkout orders quit);
    my @commands_without_arguments = qw(basket checkout orders quit);
    my $login = "";
    
    print "mekong.com.au - ASCII interface\n";
    while (1) {
        $last_error = "";
        print "> ";
        $line = <STDIN> || last;
        $line =~ s/^\s*>\s*//;
        $line =~ /^\s*(\S+)\s*(.*)/ || next;
        ($command, $argument) = ($1, $2);
        $command =~ tr/A-Z/a-z/;
        $argument = "" if !defined $argument;
        $argument =~ s/\s*$//;
        
        if (
            $command !~ /^[a-z_]+$/ ||
            !grep(/^$command$/, @commands) ||
            grep(/^$command$/, @commands_without_arguments) != ($argument eq "") ||
            ($argument =~ /\s/ && $command ne "search")
        ) {
            chomp $line;
            $line =~ s/\s*$//;
            $line =~ s/^\s*//;
            incorrect_command_message("$line");
            next;
        }

        if ($command eq "quit") {
            print "Thanks for shopping at mekong.com.au.\n";
            last;
        }
        if ($command eq "login") {
            $login = login_command($argument);
            next;
        } elsif ($command eq "new_account") {
            $login = new_account_command($argument);
            next;
        } elsif ($command eq "search") {
            search_command($argument);
            next;
        } elsif ($command eq "details") {
            details_command($argument);
            next;
        }
        
        if (!$login) {
            print "Not logged in.\n";
            next;
        }
        
        if ($command eq "basket") {
            basket_command($login);
        } elsif ($command eq "add") {
            add_command($login, $argument);
        } elsif ($command eq "drop") {
            drop_command($login, $argument);
        } elsif ($command eq "checkout") {
            checkout_command($login);
        } elsif ($command eq "orders") {
            orders_command($login);
        } else {
            warn "internal error: unexpected command $command";
        }
    }
}

sub login_command {
    my ($login) = @_;
    if (!legal_login($login)) {
        print "$last_error\n";
        return "";
    }
    if (!-r "$users_dir/$login") {
        print "User '$login' does not exist.\n";
        return "";
    }
    printf "Enter password: ";
    my $pass = <STDIN>;
    chomp $pass;
    if (!authenticate($login, $pass)) {
        print "$last_error\n";
        return "";
    }
    $login = $login;
    print "Welcome to mekong.com.au, $login.\n";
    return $login;
}

sub new_account_command {
    my ($login) = @_;
    if (!legal_login($login)) {
        print "$last_error\n";
        return "";
    }
    if (-r "$users_dir/$login") {
        print "Invalid user name: login already exists.\n";
        return "";
    }
    if (!open(USER, ">$users_dir/$login")) {
        print "Can not create user file $users_dir/$login: $!";
        return "";
    }
    foreach $description (@new_account_rows) {
        my ($name, $label)  = split /\|/, $description;
        next if $name eq "login";
        my $value;
        while (1) {
            print "$label ";
            $value = <STDIN>;
            exit 1 if !$value;
            chomp $value;
            if ($name eq "password" && !legal_password($value)) {
                print "$last_error\n";
                next;
            }
            last if $value =~ /\S+/;
        }
        $user_details{$name} = $value;
        print USER "$name=$value\n";
    }
    close(USER);
    print "Welcome to mekong.com.au, $login.\n";
    return $login;
}

sub search_command {
    my ($search_string) = @_;
    $search_string =~ s/\s*$//;
    $search_string =~ s/^\s*//;
    search_command1(split /\s+/, $search_string);
}

sub search_command1 {
    my (@search_terms) = @_;
    my @matching_isbns = search_books1(@search_terms);
    if ($last_error) {
        print "$last_error\n";
    } elsif (@matching_isbns) {
        print_books(@matching_isbns);
    } else {
        print "No books matched.\n";
    }
}

sub details_command {
    my ($isbn) = @_;
    our %book_details;
    if (!legal_isbn($isbn)) {
        print "$last_error\n";
        return;
    }
    if (!$book_details{$isbn}) {
        print "Unknown isbn: $isbn.\n";
        return;
    }
    print_books($isbn);
    foreach $attribute (sort keys %{$book_details{$isbn}}) {
        next if $attribute =~ /Image|=|^(|price|title|authors|productdescription)$/;
        print "$attribute: $book_details{$isbn}{$attribute}\n";
    }
    my $description = $book_details{$isbn}{productdescription} or return;
    $description =~ s/\s+/ /g;
    $description =~ s/\s*<p>\s*/\n\n/ig;
    while ($description =~ s/<([^">]*)"[^"]*"([^>]*)>/<$1 $2>/g) {}
    $description =~ s/(\s*)<[^>]+>(\s*)/$1 $2/g;
    $description =~ s/^\s*//g;
    $description =~ s/\s*$//g;
    print "$description\n";
}

sub basket_command {
    my ($login) = @_;
    my @basket_isbns = read_basket($login);
    if (!@basket_isbns) {
        print "Your shopping basket is empty.\n";
    } else {
        print_books(@basket_isbns);
        printf "Total: %11s\n", sprintf("\$%.2f", total_books(@basket_isbns));
    }
}

sub add_command {
    my ($login,$isbn) = @_;
    our %book_details;
    if (!legal_isbn($isbn)) {
        print "$last_error\n";
        return;
    }
    if (!$book_details{$isbn}) {
        print "Unknown isbn: $isbn.\n";
        return;
    }
    add_basket($login, $isbn);
}

sub drop_command {
    my ($login,$isbn) = @_;
    my @basket_isbns = read_basket($login);
    if (!legal_isbn($argument)) {
        print "$last_error\n";
        return;
    }
    if (!grep(/^$isbn$/, @basket_isbns)) {
        print "Isbn $isbn not in shopping basket.\n";
        return;
    }
    delete_basket($login, $isbn);
}

sub checkout_command {
    my ($login) = @_;
    my @basket_isbns = read_basket($login);
    if (!@basket_isbns) {
        print "Your shopping basket is empty.\n";
        return;
    }
    print "Shipping Details:\n$user_details{name}\n$user_details{street}\n$user_details{city}\n$user_details{state}, $user_details{postcode}\n\n";
    print_books(@basket_isbns);
    printf "Total: %11s\n", sprintf("\$%.2f", total_books(@basket_isbns));
    print "\n";
    my ($credit_card_number, $expiry_date);
    while (1) {
            print "Credit Card Number: ";
            $credit_card_number = <>;
            exit 1 if !$credit_card_number;
            $credit_card_number =~ s/\s//g;
            next if !$credit_card_number;
            last if $credit_card_number =~ /^\d{16}$/;
            last if legal_credit_card_number($credit_card_number);
            print "$last_error\n";
    }
    while (1) {
            print "Expiry date (mm/yy): ";
            $expiry_date = <>;
            exit 1 if !$expiry_date;
            $expiry_date =~ s/\s//g;
            next if !$expiry_date;
            last if legal_expiry_date($expiry_date);
            print "$last_error\n";
    }
    finalize_order($login, $credit_card_number, $expiry_date);
}

sub orders_command {
    my ($login) = @_;
    print "\n";
    foreach $order (login_to_orders($login)) {
        my ($order_time, $credit_card_number, $expiry_date, @isbns) = read_order($order);
        $order_time = localtime($order_time);
        print "Order #$order - $order_time\n";
        print "Credit Card Number: $credit_card_number (Expiry $expiry_date)\n";
        print_books(@isbns);
        print "\n";
    }
}

# print descriptions of specified books
sub print_books(@) {
    my @isbns = @_;
    print get_book_descriptions(@isbns);
}

# return descriptions of specified books
sub get_book_descriptions {
    my @isbns = @_;
    my $descriptions = "";
    our %book_details;
    my $result_array;
    foreach $isbn (@isbns) {
        die "Internal error: unknown isbn $isbn in print_books\n" if !$book_details{$isbn}; # shouldn't happen
        my $title = $book_details{$isbn}{title} || "";
        my $authors = $book_details{$isbn}{authors} || "";
        $authors =~ s/\n([^\n]*)$/ & $1/g;
        $authors =~ s/\n/, /g;
        $descriptions = sprintf "%s&&%7s&&%s&&%s&&%s\n", 
                                        $isbn, 
                                        $book_details{$isbn}{price},
                                        $title, 
                                        $authors,
                                        $book_details{$isbn}{smallimageurl};
        push (@result_array, $descriptions);
    }
    return @result_array;
}

sub set_global_variables {
    $base_dir = ".";
    $books_file = "$base_dir/books.json";
    $orders_dir = "$base_dir/orders";
    $baskets_dir = "$base_dir/baskets";
    $users_dir = "$base_dir/users";
    $last_error = "";
    %user_details = ();
    %book_details = ();
    %attribute_names = ();
    @new_account_rows = (
          'login|Login:|10',
          'password|Password:|10',
          'name|Full Name:|50',
          'street|Street:|50',
          'city|City/Suburb:|25',
          'state|State:|25',
          'postcode|Postcode:|25',
          'email|Email Address:|35'
          );
}


sub incorrect_command_message {
    my ($command) = @_;
    print "Incorrect command: $command.\n";
    print <<eof;
Possible commands are:
login <login-name>
new_account <login-name>                    
search <words>
details <isbn>
add <isbn>
drop <isbn>
basket
checkout
orders
quit
eof
}


