#!/usr/bin/perl

use strict;

use File::Basename;
use Digest::MD5 qw(md5_hex);
use constant BYTES_PER_PIXEL	=> 3;
use constant DEPTH				=> 8;

unless ((scalar @ARGV)==2) {
	die("Usage: $0 <in_file> <out_png>\n");
}
my $file=$ARGV[0];
my $png=$ARGV[1];

my $basename=basename($file);
my @stat = stat($file);
my $size=$stat[7];

open(FILE_IN, '<'.$file) || die("Can't open $file for reading: $!");
binmode(FILE_IN);
my $md5=Digest::MD5->new->addfile(*FILE_IN)->hexdigest;
close FILE_IN;

my $header="pngfy|$basename|$size|0|$md5\x0";
my $headsize=length($header);
my $rem = ($size+$headsize) % BYTES_PER_PIXEL;
my $padding = $rem != 0 ? (BYTES_PER_PIXEL - $rem) : 0;

open(FILE_OUT, '>temp.tmp') || die("Can't open tempfile for writing: $!");
open(FILE_IN, '<'.$file) || die("Can't open $file for reading: $!");
binmode(FILE_OUT);
binmode(FILE_IN);

print FILE_OUT $header;
my $buf;
while (!eof(FILE_IN))
{
    my $readed=read(FILE_IN,$buf,$size);
    print FILE_OUT $buf;
}
close FILE_IN;
print FILE_OUT "\x0" x $padding;

my ($width, $height)=(1,1);
my $div=2;
while (($size+$headsize+$padding)/BYTES_PER_PIXEL >= $div*$div) {
                $div ++;
}
$width=$height=$div;
$padding=($width*$height*BYTES_PER_PIXEL)-($size+$headsize+$padding);

print FILE_OUT "\x0" x $padding;
close FILE_OUT;


die("Can't convert to png: $!") if (system('convert -size '.$width.'x'.$height.' -depth '.DEPTH.' rgb:temp.tmp -depth '.DEPTH.' '.$png) < 0);
unlink('temp.tmp') || die("Couldn't unlink tempfile: $!");
