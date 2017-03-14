#!/usr/bin/perl
#use File::Slurp;
use strict;

my $src=$ARGV[0];
my $dst=$ARGV[1];
my $templates=$ARGV[2];

sub file_read
{
    my $from = $_[0];
    local $/;
    my $content;
    open(FILE, $from) or die "Can't read file '$from' [$!]\n";
    $content = <FILE>;
    close (FILE);
    return $content;
}

sub file_read_without_newline
{
    my $from = $_[0];
    my $content = file_read($from);
    $content =~ s/\n//g;
    return $content;
}

my $document = file_read($src);

#$/ = "";
while ( $document =~ /<!\+\+(.+?)\+\+!>/sgm)
{
    my ($line, $rol) = ($1, $1);
    $line =~ s/\s*\n\s*//gs;
    $line =~ s/\"/\\\"/gs;
    #print "$rol \n $line\n\n ";
    $document =~ s/<!\+\+(.+?)\+\+!>/"$line"/s;
}
foreach my $directory (<$templates/atomic/*>)
{
    if (-d $directory)
    {
        #print "$directory \n";
        my $regexp      = file_read("$directory/regexp");
        my $replacement = file_read_without_newline("$directory/replacement.html");
        $document =~ s/$regexp/$replacement/gee;
    }
}

foreach my $directory (<$templates/incriment/*>)
{
    if (-d $directory)
    {
        for(my $i = 0; $i<100; $i++)
        {
            #print "$directory \n";
            my $regexp      = file_read("$directory/regexp");
            my $replacement = file_read_without_newline("$directory/replacement.html");
            $regexp         =~ s/!level!/$i/g;
            $document =~ s/$regexp/$replacement/gee;
        }
    }
}

# quotes

$document =~ s/<dq\+\+(.+?)\+\+dq>/\\\"$1\\\"/sg;
$document =~ s/<q\+\+(.+?)\+\+q>/'$1'/sg;

my $f;
open($f, ">$dst") or die "Can't read file 'filename' [$!]\n"; 
print {$f} $document;
close($f);
