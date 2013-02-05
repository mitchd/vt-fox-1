
jpegant : jpeg image encoder with modifications
-----------------------------------------------

This is the directory for the modified JPEG encoder.

## Restart intervals ##

Useful documentation on determining how to enable restart intervals can 
be found in Annex B.2 of 
[JPEG ISO/IEC 10918-1 ITU-T Recommendation T.81][http://www.w3.org/Graphics/JPEG/itu-t81.pdf]

## code questions ##

  * main.c/35: the "subsample()" method indicates a 16x16 RGB block; 
    however the input RGB array is 8x16
  * main.c/64: comment claims four 8x8 blocks, but code indicates that 
    there are just two

## Basic output file ##

Output file description, byte-by-byte description. The `huffman_start()` 
method writes exactly 606 bytes broken up into mainly 5 methods.

                    # huffman_start() [2 bytes]
    ffd8            # start of image

                    # write_APP0info() [18 bytes]
    ffe0            # marker
    0010            # length
    4a46 4946       # "JFIF"
    00              # (padding)
    01              # version hi
    01              # version lo
    00              # xyunits
    0001            # xdensity
    0001            # ydensity
    00              # thumbnwidth
    00              # thumbnheight

                    # write_DQTinfo() [134 bytes]
    ffdb            # DQT: define quantization tables
    0084            # dec 132: (length of DQT info?)
    00              # padding
    [64 bytes]      # zig-zag order (luminance)
    01              #
    [64 bytes]      # zig-zag order (chromiance)

                    # write_SOF0info(height, width) [19 bytes]
    ffc0            # start of frame marker SOF0
    0017            # length
    08              # precision
    hhhh            # height
    wwww            # width
    03              # nrofcomponents
    01              # IdY
    21              # HVY, 4:4:4 subsampling (0x22 for 4:2:0)
    00              # QTY
    02              # IdCb
    11              # HVCb
    01              # QTCb
    03              # IdCr
    11              # HVCr
    01              # QTCr

                    # write_DHTinfo() [420 bytes]
    ffc4            # DHT: define Huffman tables
    01a2            # length
    00              # HTYDCinfo
    [16 bytes]      # std_dc_luminance_nrcodes
    [12 bytes]      # std_dc_luminance_values
    10              # HTYACinfo
    [16 bytes]      # std_ac_luminance_nrcodes
    [162 bytes]     # std_ac_luminance_values
    01              # HTCbDCinfo
    [16 bytes]      # std_dc_chromiance_nrcodes
    [12 bytes]      # std_dc_chromiance_values
    11              # HTCbACinfo
    [16 bytes]      # std_ac_chromiance_nrcodes
    [162 bytes]     # std_ac_chromiance_values

                    # write_SOSinfo() [13 bytes]
    ffda            # SOF: start of scan marker
    12              # length
    03              # nrofcomponents
    01              # IdY
    00              # HTY
    02              # IdCb
    11              # HTCb
    03              # IDCr
    11              # HTCr
    00              # Ss
    3f              # Se
    00              # Bf

    ... encoded image data ...

                    # huffman_stop()
    ffd9            # end of image marker

