# siboimg
This is the SIBO Image Extractor, designed to extract files from Psion Solid State Disk images. In future it might also build images for copying/burning to SSDs.

**THIS IS ALPHA-QUALITY SOFTWARE!** It's worked for me, but it might not work for you.

## Build
For Linux, BSD, macOS and other POSIX-compliant systems.
```make```

For Windows you will need MinGW and either gcc or clang. Pre-made binaries can be found in _releases_.

## Usage

```siboimg _image file_```

At the moment the switches don't work so all it does it extract the files to a directory. The directory will be named after the image's embedded name.

## Credits
Thanks to Clive DW Feather for his description of the SIBO Flash filesystem. Thanks to Karl for helping me out.