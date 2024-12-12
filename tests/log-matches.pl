#!/usr/bin/env perl
use strict; use warnings;

my @levels = qw(trace debug info warn error);
my %levelmap = map { $levels[$_] => $_ } 0..$#levels;
@ARGV or die "level must be provided";
my $level = $levelmap{$ARGV[0]};
my @unexpected = @levels[0..$level-1];
my $i = $level;
while(my $line = <STDIN>) {
    my @found = grep {$line =~ /$_/ } @unexpected;
    if (@found) {
        die "Invalid level(s) found '" . join(', ', @found) . "'";
    }
    $i < @levels or next;
    if ($line =~ /$levels[$i]/) {
        $i++;
    }
}
$i == @levels or die "Did not find level: $levels[$i]";
