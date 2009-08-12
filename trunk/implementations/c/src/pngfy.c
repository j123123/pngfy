#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <math.h>
#include <getopt.h>

#include "basename.h"
#include "md5file.h"
#include "pngrw.h"


char *fname;
int verbose = 0;
int bpp = 3;
int depth = 3;
char *tempdir = "/tmp";
char *outname = "out.png";
int doencode = 0;

void ver()
{
    printf("PNGfy ver. 1.0\n");
}

void usage(char *progname)
{
    ver();
    printf("usage: %s [options] <filename> [<out_name>]\n", progname);
    printf("   filename                    File to convert\n\n");
    printf("   ex:\n");
    printf("   decode: %s somefile.png\n", progname);
    printf("   encode: %s -e somefile.ext -o out.png\n\n", progname);
    printf("   Default operation is decode\n\n");
}


void help(char *progname)
{
    ver();
    usage(progname);
    printf("   options: \n\n");
    printf("   --encode, -e                Encode\n");
    printf("   --out=name, -o name         Output png filename (default: out.png)\n");
    printf("   --about, -a                 About\n");
    printf("   --help, -h                  This help\n");
}




void about()
{
    ver();
}



int validate_args(int argc, char *argv[])
{
    int option_index = 0;
    int c;
    static struct option long_options[] =
    {
           {"encode", no_argument, 0, 'e'},
           {"out",  optional_argument, 0, 'o'},
           {"help",  no_argument, 0, 'h'},
           {"about",  no_argument, 0, 'a'},
           {0, 0, 0, 0}
    };
    while ((int)(c = getopt_long (argc, argv, "o:hae",long_options, &option_index)) != -1)
    {
        switch (c)
        {
            case 0:
                break;
            case 'e':
                doencode=1;
                break;
            case 'a':
                about();
                return 0;
                break;
            case 'h':
                help(argv[0]);
                return 0;
                break;
            case 'o':
                outname=optarg;
                break;
            case '?':
                /* getopt_long already printed an error message. */
                usage(argv[0]);
                return 0;
                break;
            default:
                usage(argv[0]);
                return 0;
                abort ();
        }
    }
    if (optind+1 == argc)
    {
        fname = argv[optind];
    }
    if (fname == NULL)
    {
        printf("No file specified\n");
        usage(argv[0]);
        return 0;
    }
    return 1;
}


int decode()
{
    char* png;
    unsigned int width;
    unsigned int height;
    int res = read_png(fname,&png,&width,&height);
    if (res != 0)
        switch (res)
        {
            case 1:
                printf("Cannot open %s for reading\n",fname);
                return 1;
            case 2:
            case 3:
                printf("%s is not a PNG file!\n",fname);
                return 2;
        }
    //stupid but fast
    if (!(png[0] == 'p' && png[1] == 'n' && png[2] == 'g' && png[3] == 'f' && png[4] == 'y'))
    {
        printf("%s is not a PNGfy file!\n",fname);
        return 3;
    }
    char *pos;
    pos = strchr(png,'\0');
    char *header;
    header = malloc((int)(pos-png+1)*sizeof(char));
    strncpy(header,png,pos-png);
    header[(pos-png)]='\0';
   
    char *sig = NULL;
    char *name = NULL;
    unsigned int size = 0;
    int gzip = 0;
    char *md5 = NULL;
    char *gzmd5 = NULL;
    char *pch = NULL;
    pch = strtok (header,"|");
    int i = 0;
    while (pch != NULL && i <= 5)
    {
        switch (i)
        {
            case 0:
                sig = malloc((strlen(pch)+1)*sizeof(char));
                sprintf(sig,pch);
                break;
            case 1:
                name = malloc((strlen(pch)+1)*sizeof(char));
                sprintf(name,pch);
                break;
            case 2:
                size = atoi(pch);
                break;
            case 3:
                gzip = atoi(pch);
                break;
            case 4:
                md5 = malloc((strlen(pch)+1)*sizeof(char));
                sprintf(md5,pch);
                break;
            case 5:
                gzmd5 = malloc((strlen(pch)+1)*sizeof(char));
                sprintf(gzmd5,pch);
                break;
        }
        pch = strtok (NULL, "|");
        i++;
    }
    free(header);

    FILE *fp;
    char *ofname;
    if (gzip)
    {
        ofname = malloc((strlen(name)+6)*sizeof(char));
        sprintf(ofname,"x_%s.gz",name);
        printf("extracting %s | %s |gzmd5: %s\n",ofname,md5,gzmd5);
    }
    else
    {
        ofname = malloc((strlen(name)+3)*sizeof(char));
        sprintf(ofname,"x_%s",name);
        printf("extracting %s | %s\n",ofname,md5);
    }

    fp = fopen(ofname,"wb");

    for (i = 1;i <= size;i++)
    {
        fputc((int)pos[i],fp);
    }

    fclose(fp);
    printf("done!\n");
    //cleanup
    if (sig != NULL) free(sig);
    if (name != NULL) free(name);
    if (ofname != NULL) free(ofname);
    if (md5 != NULL) free(md5);
    if (gzmd5 != NULL) free(gzmd5);    
    return 1;
}

int intlen(int value)
{   
    int i = 1;
    while((value / 10)>0)
    {
        i++;
        value=value / 10;
    }
    return i;
}

int encode()
{
    FILE *fp;
    char md5digest[MD5_DIGEST_LENGTH];        
    int r = md5_file(fname,md5digest);
    if (r!=0)
    {
        printf("can't open %s\n",fname);
        return 3;
    }
    fp = fopen(fname,"rb");
    long len = 0;
    char *buf = NULL;
    if (fp == NULL)
    {
        printf("can't open %s\n",fname);
        return 3;
    }
    fseek(fp,0,SEEK_END); 
    len = ftell(fp); 
    fseek(fp,0,SEEK_SET); 
    buf = (char*) malloc (sizeof(char)*len);
    int result = fread (buf,1,len,fp);
    if (result != len)
    {
        printf("error reading %s\n",fname);
        return 3;
    }
    fclose(fp);

    char *bname = basename(fname);
    char *header = NULL;
    header = malloc((5+1+strlen(bname)+1+intlen(len)+1+1+1+strlen(md5digest)+1)*sizeof(char));
    sprintf(header,"pngfy|%s|%ld|0|%s",bname,len,md5digest);
    int rem = 0;
    int hlen = strlen(header);
    rem = (len+hlen) % 3;
    
    int padding;
    if (rem == 0)
        padding = 0;
    else
        padding = 3-rem;
    
    int wh = 1;
    int div = 2;
    while ((len+hlen+padding)/3 >= div*div)
    {
        div++;
    }
    wh = div;
    padding += (wh*wh*3)-(len+hlen+padding);
    int rgbsize = (len+hlen+padding);
    char *rgbdata;
    rgbdata = malloc(rgbsize*sizeof(char));
    strcpy(rgbdata,header);
    memcpy(rgbdata+hlen+1,buf,len);
    if (buf!=NULL) free(buf);
    if (header!=NULL) free(header);
    int i;
    int datasize = len+hlen+1;
    for (i = datasize;i < datasize+padding-1;i++)
    {
        rgbdata[i] = '\0';
    }
    int ret = write_png(outname,(unsigned char*)rgbdata,wh,wh);
    if (rgbdata!=NULL) free(rgbdata);
    if (ret != 0)
    {
        printf("error writing %s\n",outname);
        return 3;
    }
    printf("done!\nwrote %s\n",outname);
    return 0;
}


int main(int argc, char *argv[])
{
    if(validate_args(argc,argv) == 0) exit(1);
    ver();
    if (doencode)
        return(encode());
    else
        return(decode());
}
