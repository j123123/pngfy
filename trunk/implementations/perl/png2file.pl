#!/usr/bin/perl
 
my $file = $ARGV[0];
die("Usage: $0 <png file>\n") unless (defined($file));
die("No such file $file\n") unless (-e $file);
print "converting...\n";
system('convert '.$file.' -depth 8 rgb:temp.tmp');
print "extracting...\n";
open(FILE_IN,"<temp.tmp");
binmode(FILE_IN);
my $c=0;
my $hs="";
while ($c ne "\x0")
{
    read(FILE_IN,$c,1);
    $hs.=$c;
}
if ($hs=~m/^pngfy/)
{
    my ($sig,$filename,$fsize,$gzip,$md5)=split(/\|/,$hs);
    if ($gzip)
    {
     ($sig,$filename,$fsize,$gzip,$md5,$gmd5)=split(/\|/,$hs);
     open(FILE_OUT,">x_$filename.gz") or die "Error opening file for writing: $!\n";
    }
    else
    {
     open(FILE_OUT,">x_$filename") or die "Error opening file for writing: $!\n";
    }
    binmode(FILE_OUT);
    my $readed=read(FILE_IN,$buf,$fsize);
    print FILE_OUT $buf;
    close FILE_OUT;
    close FILE_IN;
    unlink("temp.tmp");
    ($gzip) ? print "extracted x_$filename.gz | $gmd5 | x_$filename: $md5\n" : print "extracted x_$filename | $md5\n";
    print "done!\n";
}
else
{
    close FILE_IN;
    die("Not an pngfy file!\n");
}