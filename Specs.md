
```
Version 1 rev. 4
```



# Idea #
```
head -c 1440000 /dev/urandom > file
convert -size 800x600 -depth 8 rgb:file png:out.png
convert -depth 8 png:out.png rgb:file2
md5sum file*
```

# Abstract #
## Encoding ##
```
                    File.ext                    // stage 0
                       |                      
                       V                       
          Gziping File.ext if needed
                       |                       
                       V                       
        Adding header to the File.ext           // stage 1
                       |                      
                       V                      
  Making PNG Image from File.ext with header    // stage 2

```
## Decoding ##
```
Extracting File.ext with header from PNG image  // stage 2
                       |
                       V
        Stripping header from File.ext          // stage 1  
                       |                       
                       V              
         Ungziping File.ext if needed
                       |                       
                       V                       
                    File.ext                    // stage 0

```

# File data format #
If not gziped - just as is.

If gziped - without gzip timestamp in order to have same md5's for images maked in different time from same files.

# File with header format #
pngfy|_<file name>_|_<file data length>_|_<gzip flag>_|_<data md5>_[|_gzip md5_]_`<null>`__<file data>_`[`_trailing zeros_`]`

  * pngfy: header signature, 70 6e 67 66 79 in hex.
  * _<file name>_: basename. No path.
  * _<file data length>_: encoding file length
  * _<gzip flag>_: '1' or '0'
  * _<data md5>_: md5 hash of file
  * [|_gzip md5_]: writing only if _<gzip flag>_ == 1. if _<gzip flag>_ == 0.
  * _`<`null`>`_: null-string terminator; symbol 00 in hex, determines end of the header.
  * _<file data>_: writing right after header.
  * `[`_trailing zeros_`]`: writing only if file length is not dividable by the number of bytes per one image pixel or for adjusting to a specific image size.

# Image file format #
  * Color depth: 8 bit
  * Bytes per pixel: 3 byte

Depth 16 and bpp 3 is works too; produce less pixels with more size and better gzipable, but not all languages can handle this.

# Examples #
  * http://code.google.com/p/pngfy/source/browse/trunk/examples/example_1
  * http://code.google.com/p/pngfy/source/browse/trunk/examples/example_2
  * http://code.google.com/p/pngfy/source/browse/trunk/examples/example_3

# Algorythm #
This is not an any real language, so i can't prove it's working and there might be a bunch of mistakes.So, instead, you'd better to see the real perl implementation: http://os.anime-union.org/pngfy/

## Encode ##
```
$input_file_name=basename("./file.txt");
$input_file=read($input_file_name);
$output_file="./image.png";
$tempfile="./tmp";
$gzip=FALSE;
$gzip_flag=0;
const $depth=8; // there is possible to use also 16, but it is
                    // *NOT* compatible with standart since not all 
                    // languages can handle that much and javascript is example.
const $bpp=3; // bytes (not bits) per pixel; 3 for depth 8; 6 for depth 16.

input_file_md5=md5($input_file);
if ($gzip) {
  $gzip_flag=1;
  $input_file=gzip($input_file); // no gzip timestamp!
  $gzip_md5=md5($input_file);
}

/*
  leading pipe is for adding header's length
*/
$header="pngfy|".$input_file_name."|".$input_file->length."|".$gzip_flag."|".$input_file_md5;
if ($gzip) {
  $header.="|".$gzip_md5;
}
$header.="\x0";

/*
  write(<output_file>,<input string>[,number of bytes to write])
*/
write($tempfile,$header);
write($tempfile,$input_file);

/*
  since integrating file size be divisible by $bpp to successfuly
  convert rgb data into png, we have to fill missing bytes with zeros.
  this will not affect original file since we specified it's length.
*/
if (( $tempfile->length % $bpp ) != 0) {
  $missing_bytes=($bpp - ( $tempfile->length % $bpp ));
  write($tempfile,"\x0" x $missing_bytes);
}

/*
  now we finding that x and y, product of which as close to 
  ($tempfile->size / $bpp) as possible. best if equal, but
  finding divisors with least sum, product of which is equal 
  to number is not rational due to slowness and prime numbers
*/
$x=$y=sqrt(($tempfile->length / $bpp)); 
while (( $x * $y ) < ($tempfile->length / $bpp)) { 
  $x++; 
}

if ($x > $y) {
  $missing_bytes=((($x * $y) - ($tempfile->length / $bpp)) * $bpp);
  write($tempfile,"\x0" x $missing_bytes);
}
  
/*
  using utility from ImageMagick package
*/
exec("convert -depth ".$depth." -size ".$x."x".$y." rgb:".$tempfile." png:".$output_file);
delete($tempfile);
```

## Decode ##
```
$input_file="image.png";
$temp="";

$depth=get_image_depth(input_file);
$header_length=0;

// fscanf and read moving the pointer, so we don't need to mind offsets: we continue right where we stopped
if (read($input_file,5) != "pngfy") {
  alert("not a pngfy file\n");
  return 1;
}
read($input_file,1); //pipe
fscanf($input_file,"%s",$header); // reads to header's string null-terminator
($output_file, $data_length, $gzip_flag, $file_md5, $gzip_md5)=split("|",$header);
$temp=read($input_file,$data_length);

if (1 == $gzip_flag)  {
  if ($gzip_md5 != md5($temp)) {
    alert("gzip md5 does not match\n");
  }
  $temp=ungzip($temp);
}

if ($file_md5 != md5($temp)) {
  alert("file md5 does not match\n");
}

write($output_file,$temp);
```

# Implementations #
  * [C](http://code.google.com/p/pngfy/source/browse/trunk/implementations/c)
  * [perl](http://code.google.com/p/pngfy/source/browse/trunk/implementations/perl)
  * [javascript](http://code.google.com/p/pngfy/source/browse/trunk/implementations/javascript)
  * [posix v7 sh](http://code.google.com/p/pngfy/source/browse/trunk/implementations/shell/posix-v7)

# Links #
  * http://os.anime-union.org/pngfy/