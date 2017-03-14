#!/usr/bin/perl
#use File::Slurp;
use Time::HiRes qw(gettimeofday);
use Data::Dumper;
use strict;

my $src=$ARGV[0];
my $dst=$ARGV[1];
my $templates=$ARGV[2];
my $uniq_ids = {};

my $id_storage = "/tmp/preprocessor_hash_store";

sub load_stored_uniq_ids
{
    if(-e $id_storage)
    {
        open FILE, $id_storage;
	undef $/;
        eval <FILE>;
        close FILE;
    }
}

sub save_uniq_ids
{
    $Data::Dumper::Purity = 1;
    open FILE, ">$id_storage";
    print FILE Data::Dumper->Dump([$uniq_ids], ['$uniq_ids']);
    close FILE;
}

sub uniq_id
{
    my ( $s, $ms ) = gettimeofday();
    return sprintf("s_%d_%d",$s, $ms);
}

sub uniq_id_stored
{
    my $key = $_[0];
    if(not exists($uniq_ids->{$key}))
    {
	$uniq_ids->{$key} = uniq_id();
    }
    return $uniq_ids->{$key};
}

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

load_stored_uniq_ids();

printf("Preprocessing %s: ", $src);
my $document = file_read($src);

#$/ = "";
print("linearize, ");
while ( $document =~ /<!\+\+(.+?)\+\+!>/sgm)
{
    my ($line, $rol) = ($1, $1);
    $line =~ s/\s*\n\s*//gs;
    $line =~ s/\"/\\\"/gs;
    #print "$rol \n $line\n\n ";
    $document =~ s/<!\+\+(.+?)\+\+!>/"$line"/s;
}

print("atomic, ");
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

print("functional (iterations: 0");
my $iteration = 1;
while($document =~ m/<\w+?\+\+/)
{
    printf(",%d", $iteration);
    $iteration++;
    foreach my $directory (<$templates/function/*>)
    {
	if (-d $directory)
	{
	    #print "$directory \n";
	    my $regexp      = file_read("$directory/regexp");
	    my $replacement = file_read_without_newline("$directory/replacement.html");
	    $document =~ s/$regexp/$replacement/gee;
	}
    }
    $document =~ s/<dq\+\+(.+?)\+\+dq>/\\\"$1\\\"/sg;
    $document =~ s/<q\+\+(.+?)\+\+q>/'$1'/sg;
}
print("), ");
#foreach my $directory (<$templates/incriment/*>)
#{
#    if (-d $directory)
#    {
#        for(my $i = 0; $i<100; $i++)
#        {
#            #print "$directory \n";
#            my $regexp      = file_read("$directory/regexp");
#            my $replacement = file_read_without_newline("$directory/replacement.html");
#            $regexp         =~ s/!level!/$i/g;
#            $document =~ s/$regexp/$replacement/gee;
#        }
#    }
#}

# quotes

print("uniqid, ");
$document =~ s/<uniqid>/uniq_id()/sge;
$document =~ s/<uniqid_(.+?)>/uniq_id_stored("$1")/sge;
print("trim");
$document =~ s/>\s+</></sg;

my $f;
open($f, ">$dst") or die "Can't read file 'filename' [$!]\n"; 
print {$f} $document;
close($f);
print(". Done.\n");

#uniq_id_stored('aa');
#uniq_id_stored('bb');

save_uniq_ids();
