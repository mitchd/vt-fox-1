#ifndef _I2C_CAMERA_

#define _I2C_CAMERA_

#define CAM_I2C_ADDR	0x42
#define CAM_I2C_ADDRW	0x43

//Camera Registers

#define CAM_GAIN	0x00 //AGC Gain Control
#define CAM_BLUE	0x01 //AWB Blue Channel
#define CAM_RED		0x02 //AWB Red Channel
#define CAM_VREF	0x03 //Vertical Frame Control
#define CAM_COM1	0x04 //Common Control 1
#define CAM_BAVE	0x05 //U/B Average Level
#define CAM_GbAVE	0x06 //Y/Gb Average Level
#define CAM_AECHH	0x07 //Exposure Value MSB5 Bits
#define CAM_RAVE	0x08 //V/R AVerage Level
#define CAM_COM2	0x09 //Common Control 2
#define CAM_PID		0x0A //Product ID MSB
#define CAM_VER		0x0B //Product ID LSB
#define CAM_COM3	0x0C //Common Control 3
#define CAM_COM4	0x0D //Common Control 4
#define CAM_COM5	0x0E //Common Control 5
#define CAM_COM6	0x0F //Common Control 6
#define CAM_AECH	0x10 //Exposure Value
#define CAM_CLKRC	0x11 //Internal Clock
#define CAM_COM7	0x12 //Common Control 7
#define CAM_COM8	0x13 //Common Control 8
#define CAM_COM9	0x14 //Common Control 9
#define CAM_COM10	0x15 //Common Control 10
#define CAM_RSVD	0x16 //Reserved
#define CAM_HSTART	0x17 //Horizontal Frame Start MSB
#define CAM_HSTOP	0x18 //Horizontal Frame Stop MSB
#define CAM_VSTART	0x19 //Vertical Frame Start MSB
#define CAM_VSTOP	0x1A //Vertical Frame Stop MSB
#define CAM_PSHFT	0x1B //Pixel Delay Select
#define CAM_MIDH	0x1C //Manufacturer ID MSB
#define CAM_MIDL	0x1D //Manufacturer ID LSB
#define CAM_MVFP	0x1E //Mirror/Vflip Enable
#define CAM_ADCCTR0	0x20 //ADC Control
#define CAM_AEW		0x24 //AGC/AEC Stable Upper Limit
#define CAM_AEB		0x25 //AGC/AEC Stable Lower Limit
#define CAM_VPT		0x26 //AGC/AEC Fast Mode Region
#define CAM_BBIAS	0x27 //B Channel Signal Output Bias
#define CAM_GbBIAS	0x28 //Gb Channel Signal Output Bias
#define CAM_EXHCH	0x2A //Dummy Pixel Insert MSB
#define CAM_EXHCL	0x2B //Dummy Pixel Insert LSB
#define CAM_RBIAS	0x2C //R Channel Signal Output Bial
#define CAM_ADVFL	0x2D //LSB of Insert Dummy Lines
#define CAM_ADVFH	0x2E //MSB of Insert Dummy Lines
#define CAM_YAVE	0x2F //Y/G Channel Average Value
#define CAM_HSYST	0x30 //HSYNC Rising Edge Delay LSB
#define CAM_HSYEN	0x31 //HSYNC Falling Edge Delay LSB
#define CAM_HREF	0x32 //HREF Control (LSBs for HSTART/HSTOP)
#define CAM_ADC		0x37 //ADC Control
#define CAM_ACOM	0x38 //ADC and Analog Common Mode Control
#define CAM_OFON	0x39 //ADC Offset Control - Reserved
#define CAM_TSLB	0x3A //Line Buffer Test Option
#define CAM_COM11	0x3B //Common Control 11
#define CAM_COM12	0x3C //Common Control 12
#define CAM_COM13	0x3D //Common Control 13
#define CAM_COM14	0x3E //Common Control 14
#define CAM_EDGE	0x3F //Edge Enhancement Adjustment
#define CAM_COM15	0x40 //Common Control 15
#define CAM_COM16	0x41 //Common Control 16
#define CAM_COM17	0x42 //Common Control 17
#define CAM_REG4B	0x4B //Register 4B
#define CAM_DNSTH	0x4C //De-noise Strength
#define CAM_MTX1	0x4F //Matrix Coefficient 1
#define CAM_MTX2	0x50 //Matrix Coefficient 2
#define CAM_MTX3	0x51 //Matrix Coefficient 3
#define CAM_MTX4	0x52 //Matrix Coefficient 4
#define CAM_MTX5	0x53 //Matrix Coefficient 5
#define CAM_MTX6	0x54 //Matrix Coefficient 6
#define CAM_BRIGHT	0x55 //Brightness Control
#define CAM_CONTRAS	0x56 //Contrast Control
#define CAM_CONSTRAS-CENTER	0x57 //Contrast Center
#define CAM_MTXS	0x58 //Matrix Coefficient Sign
#define CAM_LCC1	0x62 //Lens Correction Option 1 - X coordinate
#define CAM_LCC2	0x63 //Lens Correction Option 2 - Y coordinate
#define CAM_LCC3	0x64 //Lens Correction Option 3
#define CAM_LCC4	0x65 //Lens Correction Option 4
#define CAM_LCC5	0x66 //Lens Correction Control
#define CAM_MANU	0x67 //Manual U Value
#define CAM_MANV	0x68 //Manual V Value
#define CAM_GFIX	0x69 //Fix Gain Control
#define CAM_GGAIN	0x6A //G Channel AWB Gain
#define CAM_DBLV	0x6B //PLL Control
#define CAM_AWBCTR3	0x6C //AWB Control 3
#define CAM_AWBCTR2	0x6D //AWB Control 2
#define CAM_AWBCTR1	0x6E //AWB Control 1
#define CAM_AWBCTR0	0x6F //AWB Control 0
#define CAM_SCALING_XSC 0x70 //Test Pattern / Horizontal Scale Factor
#define CAM_SCALING_YSC 0x71 //Test Pattern / Vertical Scale Factor
#define CAM_SCALING_DCWCTR	0x72 //DCW Control
#define CAM_SCALING_PCK_DIV	0x73 //Clock divider for DSP scale control
#define CAM_REG74	0x74 //Digital Gain
#define CAM_REG75	0x75 //Edge Enhancement Lower Limit
#define CAM_REG76	0x76 //B/W Pixel correction, Edge Enhancemente upper lmt
#define CAM_REG77	0x77 //De-noise offset
#define CAM_SLOP	0x7A //Gamma Curve highest segment slop
#define CAM_GAM1	0x7B //Gamma Curve 1st Seg
#define CAM_GAM2	0x7C //Gamma Curve 2nd Seg
#define CAM_GAM3	0x7D //Gamma Curve 3rd Seg
#define CAM_GAM4	0x7E //Gamma Curve 4th Seg
#define CAM_GAM5	0x7F //Gamma Curve 5th Seg
#define CAM_GAM6	0x80 //Gamma Curve 6th Seg
#define CAM_GAM7	0x81 //Gamma Curve 7th Seg
#define CAM_GAM8	0x82 //Gamma Curve 8th Seg
#define CAM_GAM9	0x83 //Gamma Curve 9th Seg
#define CAM_GAM10	0x84 //Gamma Curve 10th Seg
#define CAM_GAM11	0x85 //Gamma Curve 11th Seg
#define CAM_GAM12	0x86 //Gamma Curve 12th Seg
#define CAM_GAM13	0x87 //Gamma Curve 13th Seg
#define CAM_GAM14	0x88 //Gamma Curve 14th Seg
#define CAM_GAM15	0x89 //Gamma Curve 15th Seg
#define CAM_RGB444	0x8C //RGB444 Enable / Format
#define CAM_DM_LNL	0x92 //Dummy Line LSB
#define CAM_DM_LNH	0x93 //Dummy Line MSB
#define	CAM_LCC6	0x94 //Lens Correction Option 6
#define CAM_LCC7	0x95 //Lens Correction Option 7
#define CAM_BD50ST	0x9D //50Hz Banding Filter Value
#define CAM_BD60ST	0x9E //60Hz Banding Filter Value
#define CAM_HAECC1	0x9F //Histogram-based AEC/AGC Control 1
#define CAM_HAECC2	0xA0 //Histogram-based AEC/AGC Control 2
#define CAM_SCALING_PCK_DELAY	0xA2 //Pixel Clock Delay
#define CAM_NT_CTRL	0xA4 //Frame rate adjustment
#define CAM_BD50MAX	0xA5 //50Hz Banding Step Limit
#define CAM_HAECC3	0xA6 //Histogram-based AEC/AGC Control 3
#define CAM_HAECC4	0xA7 //Histogram-based AEC/AGC Control 4
#define CAM_HAECC5	0xA8 //Histogram-based AEC/AGC Control 5
#define CAM_HAECC6	0xA9 //Histogram-based AEC/AGC Control 6
#define CAM_HAECC7	0xAA //AEC Algorithm Select
#define CAM_BD60MAX	0xAB //60Hz Banding Step Limit
#define CAM_STR-OPT	0xAC //Strobe options
#define CAM_STR_R	0xAD //R Gain for LED
#define CAM_STR_G	0xAE //G Gain for LED
#define CAM_STR_B	0xAF //B Gain for LED
#define CAM_ABLC1	0xB1 //ABLC Enable
#define CAM_THL_ST	0xB3 //ABLC Target
#define CAM_THL_DT	0xB5 //ABLC Stable Range
#define CAM_AD-CHB	0xBE //Blue Channel Black level compensation
#define CAM_AD-CHR	0xBF //Red channel black level compensation
#define CAM_AD-CHGb	0xC0 //Gb channel black level compensation
#define CAM_AD-CHGr	0xC1 //Gr channel black level compensation
#define CAM_STATCTR	0xC9 //Saturation Control



//Setup the I2C Camera SCCB
void	setupI2C(void)

//Camera configuration
void	configureCam(void);

//Camera utility functions
void	wakeupCam(void);
void	powerdownCam(void);

//Put n bytes from the FIFO into buf
void	fifoGrabBytes( uint8_t *buf, uint32_t n );

//Camera control thread -- Needs just a little bit of memory
extern WORKING_AREA(waCamera_Thread, 10240);
void	cameraControlThread(void* arg);
#endif