// ==UserScript==
// @name        P2FDecoder
// @version     1.0
// @date        2009-07-17
// @author      kure-ji-neko <anonineko@gmail.com>
// @download    http://os.anime-union.org/pngfy/pngfy.user.js
// @exclude     file://*
// @exclude     ftp://*
// ==/UserScript==

var mimetypes = {
    jpg: 'image/jpeg',
    jpeg: 'image/jpeg',
    html: 'text/html',
    htm: 'text/html',
    xml: 'text/xml',
    gif: 'image/gif',
    js: 'application/x-javascript',
    txt: 'text/plain',
    png: 'image/png',
    ico: 'image/x-icon',
    bmp: 'image/x-ms-bmp',
    svg: 'image/svg+xml',
    doc: 'application/msword',
    docx: 'application/msword',
    pdf: 'application/pdf',
    rar: 'application/x-rar-compressed',
    rpm: 'application/x-redhat-package-manager',
    swf: 'application/x-shockwave-flash',
    xpi: 'application/x-xpinstall',
    zip: 'application/zip',

    mid: 'audio/midi',
    mp3: 'audio/mpeg',
    mpg: 'video/mpeg',
    flv: 'video/x-flv',
    avi: 'video/x-msvideo'
};




/**
*
*  Base64 encode / decode
*  http://www.webtoolkit.info/
*
**/

var Base64 = {

	// private property
	_keyStr : "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=",

	// public method for encoding
	encode : function (input) {
		var output = "";
		var chr1, chr2, chr3, enc1, enc2, enc3, enc4;
		var i = 0;

		// input = Base64._utf8_encode(input);

		while (i < input.length) {

			chr1 = input.charCodeAt(i++);
			chr2 = input.charCodeAt(i++);
			chr3 = input.charCodeAt(i++);

			enc1 = chr1 >> 2;
			enc2 = ((chr1 & 3) << 4) | (chr2 >> 4);
			enc3 = ((chr2 & 15) << 2) | (chr3 >> 6);
			enc4 = chr3 & 63;

			if (isNaN(chr2)) {
				enc3 = enc4 = 64;
			} else if (isNaN(chr3)) {
				enc4 = 64;
			}

			output = output +
			this._keyStr.charAt(enc1) + this._keyStr.charAt(enc2) +
			this._keyStr.charAt(enc3) + this._keyStr.charAt(enc4);

		}

		return output;
	},

	// public method for decoding
	decode : function (input) {
		var output = "";
		var chr1, chr2, chr3;
		var enc1, enc2, enc3, enc4;
		var i = 0;

		input = input.replace(/[^A-Za-z0-9\+\/\=]/g, "");

		while (i < input.length) {

			enc1 = this._keyStr.indexOf(input.charAt(i++));
			enc2 = this._keyStr.indexOf(input.charAt(i++));
			enc3 = this._keyStr.indexOf(input.charAt(i++));
			enc4 = this._keyStr.indexOf(input.charAt(i++));

			chr1 = (enc1 << 2) | (enc2 >> 4);
			chr2 = ((enc2 & 15) << 4) | (enc3 >> 2);
			chr3 = ((enc3 & 3) << 6) | enc4;

			output = output + String.fromCharCode(chr1);

			if (enc3 != 64) {
				output = output + String.fromCharCode(chr2);
			}
			if (enc4 != 64) {
				output = output + String.fromCharCode(chr3);
			}

		}

		output = Base64._utf8_decode(output);

		return output;

	},

	// private method for UTF-8 encoding
	_utf8_encode : function (string) {
		string = string.replace(/\r\n/g,"\n");
		var utftext = "";

		for (var n = 0; n < string.length; n++) {

			var c = string.charCodeAt(n);

			if (c < 128) {
				utftext += String.fromCharCode(c);
			}
			else if((c > 127) && (c < 2048)) {
				utftext += String.fromCharCode((c >> 6) | 192);
				utftext += String.fromCharCode((c & 63) | 128);
			}
			else {
				utftext += String.fromCharCode((c >> 12) | 224);
				utftext += String.fromCharCode(((c >> 6) & 63) | 128);
				utftext += String.fromCharCode((c & 63) | 128);
			}

		}

		return utftext;
	},

	// private method for UTF-8 decoding
	_utf8_decode : function (utftext) {
		var string = "";
		var i = 0;
		var c = c1 = c2 = 0;

		while ( i < utftext.length ) {

			c = utftext.charCodeAt(i);

			if (c < 128) {
				string += String.fromCharCode(c);
				i++;
			}
			else if((c > 191) && (c < 224)) {
				c2 = utftext.charCodeAt(i+1);
				string += String.fromCharCode(((c & 31) << 6) | (c2 & 63));
				i += 2;
			}
			else {
				c2 = utftext.charCodeAt(i+1);
				c3 = utftext.charCodeAt(i+2);
				string += String.fromCharCode(((c & 15) << 12) | ((c2 & 63) << 6) | (c3 & 63));
				i += 3;
			}

		}

		return string;
	}

}

/*
/ PNG2FILE reader
/ Based on work by hid.im
*/
var PNG_Reader = {
  removeTransparency: function(array) {
    for(var i = 0; i < array.length; i++) {
      array[i].splice(3, 1);
    }
    return array;
  },

  inGroupsOf: function(arr, number) {
    if(typeof(arr.slice) == "undefined") {
      arr.slice = Array.prototype.slice;
    }

    var slices = [];
    var index = -number;
    while((index += number) < arr.length) {
      slices.push(arr.slice(index, index+number));
    }
    return slices;
  },

  transpose: function(array) {
    var height = array.length;
    var width = array[0].length;
    var transposed = [];

    for(var i = 0; i < width; i++) {
      for(var j = 0; j < height; j++) {
	if(transposed[i] == null) { transposed[i] = []; }
	transposed[i][j] = array[j][i];
      }
    }
    return transposed;
  },

  flatten: function(array) {
    var r = [];
    for(var i = 0; i < array.length; i++) {
      r.push.apply(r,array[i]);
    }
    return r;
  },

  toChars: function(array) {
    charArray = [];
    for(var i in array) {
      charArray.push(String.fromCharCode(array[i]));
    }
    return charArray;
  },


  extractFromImg: function(img) {
    var canvas = document.getElementById('tmpCanvas');
    if(!canvas) {
      canvas = document.createElement('canvas');
    }
    var context = canvas.getContext('2d');

    canvas.width = img.width;
    canvas.height = img.height;
    context.drawImage(img, 0, 0);

    return context.getImageData(0, 0, img.width, img.height).data;
  },

  readPng: function(img) {
    var data = this.extractFromImg(img);

    // Split the raw data into pixels (r, g, b, a)
    var pixels = this.inGroupsOf(data, 4);

    // Alpha is always set to fully opaque; we're not using it to store data so remove the alpha byte
    pixels = this.removeTransparency(pixels);

    // Group the pixel data into a matrix of rows, and reverse them since we are going to be reading upwards
    var rows = this.inGroupsOf(pixels, img.width);//.reverse();

    // Since we're scanning vertically instead of horizontally we need to transpose
    var transposed = rows;

    // Convert the matrix to a 1-dimensional array
    var dataf = this.flatten(this.flatten(transposed));
    //get all data
    var tfile=this.toChars(dataf).join('');
    if (tfile.slice(0,5)!="pngfy")
    {
        var result = 'fial';
        return result;
    }
    //get header
    var header=tfile.slice(0,tfile.indexOf("\0"));
    //get header fields
    var header_ar=header.split('|');
    var sig = header_ar[0];
    var fname = header_ar[1];
    var flen = header_ar[2];
    var fgz = header_ar[3];
    var fmd5 = header_ar[4];

    //finally, get file
    var file=tfile.slice(parseInt(tfile.indexOf("\0")+1),tfile.indexOf("\0")+1+parseInt(flen));

    var result = {
        file: {
	        data: file,
            md5: fmd5,
            length: flen,
            name: fname,
            gz: fgz
        }
    };

    return result;
  }
};


/*
/ P2F Helper box
/
*/
var P2FHelper = {
  helpbox: '',
  init: function() {
    var helpBox = document.createElement('div');
    helpBox.style.backgroundColor = "rgba(0, 0, 0, 0.9)";
    helpBox.style.color = "#FFFFFF";
    helpBox.style.fontSize = "12px";
    helpBox.style.fontFamily = '"Droid Sans", Helvetica, sans-serif;';
    helpBox.style.padding = "8px";
    helpBox.style.position = "fixed";
    helpBox.style.display = "block";
    helpBox.style.visibility = "normal";
    helpBox.style.top = "30px";
    helpBox.style.textAlign = "center";
    helpBox.style.MozBorderRadius = "5px";
    helpBox.style.WebkitBorderRadius = "5px";
    helpBox.style.left = ((document.body.clientWidth / 2) - 250) + "px";
    helpBox.style.width = "500px";

    var titleLabel = document.createElement('div');
    titleLabel.style.fontSize = "24px";
    titleLabel.innerHTML = "P2F Decoder";

    helpBox.appendChild(titleLabel);
    helpBox.innerHTML += "Click on a image on this page to decode.  ";
    helpBox.style.display="none";
    document.body.appendChild(helpBox);
    this.helpbox=helpBox;
  },

  enable: function() {
    var images = document.getElementsByTagName('img');

    for(var i = 0; i < images.length; i++) {
      var image = images[i];
      image.addEventListener('click', this.addInfo, false);
    }
    this.helpbox.style.display="";
  },

  disable: function() {
    var images = document.getElementsByTagName('img');

    for(var i = 0; i < images.length; i++) {
      var image = images[i];
      image.removeEventListener('click', this.addInfo, false);
    }
    this.helpbox.style.display="none";
  },

  toggle: function() {
    if (this.helpbox=='') this.init();
    if (this.helpbox.style.display=="none")
    {
        this.enable();
    }
    else
    {
        this.disable();
    }
  },

  addInfo: function() {
    var a = PNG_Reader.readPng(this);
    if (a=='fial') { alert('Not a pngfy file!'); return false;}
    // Remove the info box if it's already here
    if(document.getElementById(a.file.md5)) {
      document.body.removeChild(document.getElementById(a.file.md5));
      return false;
    }
    if (a.file.gz==1) a.file.name+='.gz';
    var ext=a.file.name.substr(a.file.name.lastIndexOf('.')+1);
    
    var mime=eval('mimetypes.' + ext);
    if (typeof mime=='undefined')
    {
        mime='application/octet-stream';
    }
    var data = "data:"+mime+";base64,"+Base64.encode(a.file.data);

    var infoButton = document.createElement('div');
    infoButton.style.backgroundColor = "#000000";
    infoButton.style.color = "#FFFFFF";
    infoButton.style.fontSize = "10px";
    infoButton.style.fontFamily = '"Droid Sans", Helvetica, sans-serif;';
    infoButton.style.padding = "4px";
    infoButton.style.borderTop = "2px solid #222";
    infoButton.style.position = "absolute";
    infoButton.style.display = "block";
    infoButton.style.visibility = "normal";
    infoButton.style.top = (this.offsetTop + this.height) + "px";
    infoButton.style.left = (this.offsetLeft) + "px";
    infoButton.style.width = (Math.max(300, this.width) - 8) + "px";

    infoButton.id = a.file.md5;
    document.body.appendChild(infoButton);

    infoButton.innerHTML = " <b>" + a.file.name.replace(/</g, '&lt;') + "</b>";
    infoButton.innerHTML += " <br> File Length: " + parseInt(a.file.length) + "Bytes";
    var downloadLink = document.createElement('a');
    downloadLink.href = data;
    downloadLink.style.color = "#FFFFFF";
    downloadLink.style.backgroundColor = "#000000";
    downloadLink.innerHTML = "download";
    infoButton.appendChild(downloadLink);

    return false;
  }
};

