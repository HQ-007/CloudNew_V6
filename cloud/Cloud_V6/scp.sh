#!/usr/bin/expect
set FILE [lindex $argv 0]
set TO [lindex $argv 1]
set PASSWD [lindex $argv 2]
spawn scp $FILE   $TO
expect {
"yes/no" {
send "yes\n"
expect "password:"
send "$PASSWD\n"
}
"password:" { send "$PASSWD\n" }
}

expect eof
#interact
