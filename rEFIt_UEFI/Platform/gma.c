/*
 *  gma.c
 *
 *
 *  Created by Slice
 *  Fully reworked by Sherlocks, 2017
 *
 *  Original patch by Nawcom
 *  http://forum.voodooprojects.org/index.php/topic,1029.0.html
 */


/*
 Information
 ============== Lists =====================
 https://en.wikipedia.org/wiki/List_of_Intel_chipsets
 https://en.wikipedia.org/wiki/List_of_Intel_graphics_processing_units
 https://github.com/Igalia/intel-osrc-gfx-prm
 https://github.com/anholt/mesa/blob/master/include/pci_ids/i965_pci_ids.h
 https://github.com/RehabMan/OS-X-Clover-Laptop-Config/blob/master/hotpatch/SSDT-IGPU.dsl
 https://github.com/StefanG3D/gpudetect/blob/master/IntelGfx.cfg
 https://fossies.org/linux/mesa/include/pci_ids/i965_pci_ids.h
 https://software.intel.com/en-us/articles/intel-graphics-developers-guides
 https://www.intel.com/content/www/us/en/support/graphics-drivers/000005526.html#core
 http://forge.voodooprojects.org/p/chameleon/source/tree/HEAD/trunk/i386/libsaio/gma.c
 ============== 1st generation ============
 ============== 2nd generation ============
 ============== 3rd generation ============
 https://github.com/Igalia/intel-osrc-gfx-prm/tree/master/000_g35
 ============== 4th generation ============
 https://github.com/Igalia/intel-osrc-gfx-prm/tree/master/001_g45
 ============== 5th generation ============
 https://github.com/Igalia/intel-osrc-gfx-prm/tree/master/002_ilk_-_2010
 http://www.insanelymac.com/forum/topic/286092-guide-1st-generation-intel-hd-graphics-qeci/
 ============== 6th generation ============
 https://github.com/Igalia/intel-osrc-gfx-prm/tree/master/003_snb_-_2011
 https://software.intel.com/sites/default/files/m/d/4/1/d/8/Sandy_Bridge_Intel_HD_Graphics_DirectX_Developer_s_Guide_2dot9dot6.pdf
 ============== 7th generation ============
 https://github.com/Igalia/intel-osrc-gfx-prm/tree/master/004_ivb_-_2012
 https://applelife.ru/threads/intel-hd-2500-ivybridge-zavod-tolko-hdmi.40629/
 http://blog.stuffedcow.net/2012/07/intel-hd4000-qeci-acceleration/
 ============== 7.5th generation ==========
 https://github.com/Igalia/intel-osrc-gfx-prm/blob/master/005_hsw_-_2013/vol04_intel-gfx-prm-osrc-hsw-configurations.pdf
 https://github.com/Igalia/intel-osrc-gfx-prm/tree/master/006_vvw_-_2014
 http://www.insanelymac.com/forum/topic/290783-intel-hd-graphics-4600-haswell-working-displayport/
 http://www.insanelymac.com/forum/topic/328567-intel-hd-graphics-haswell-gt1-qeci-patch/
 ============== 8th generation ============
 https://github.com/Igalia/intel-osrc-gfx-prm/blob/master/007_bdw_-_2014-2015/intel-gfx-prm-osrc-bdw-vol04-configurations_1.pdf
 https://github.com/Igalia/intel-osrc-gfx-prm/tree/master/008_chv-bsw_-_2014-2015
 ============== 9th generation ============
 https://github.com/Igalia/intel-osrc-gfx-prm/blob/master/009_skl_-_2015-2016/intel-gfx-prm-osrc-skl-vol04-configurations.pdf
 ============== 9.5th generation ==========
 https://github.com/Igalia/intel-osrc-gfx-prm/blob/master/010_kbl_-_2016-2017/intel-gfx-prm-osrc-kbl-vol04-configurations.pdf
 ============== 10th generation ===========
 https://github.com/Igalia/release-mesa/blob/ecd8f8580288361f6c4d532ba964a744dd62a9dd/include/pci_ids/i965_pci_ids.h
 */


#include "Platform.h"
#include "gma.h"

#ifndef DEBUG_GMA
#ifndef DEBUG_ALL
#define DEBUG_GMA 1
#else
#define DEBUG_GMA DEBUG_ALL
#endif
#endif

#if DEBUG_GMA == 0
#define DBG(...)
#else
#define DBG(...) DebugLog(DEBUG_GMA, __VA_ARGS__)
#endif


extern CHAR8*    gDeviceProperties;
extern CHAR8     ClassFix[];

UINT32	IntelDisplay = 1;


UINT8 common_vals[3][4] = {
  { 0x00, 0x00, 0x00, 0x00 },   //0 reg_FALSE
  { 0x01, 0x00, 0x00, 0x00 },   //1 reg_TRUE
  { 0x6B, 0x10, 0x00, 0x00 },   //2 "subsystem-vendor-id"
};


UINT8 calistoga_gma_vals[30][4] = {
  { 0x01, 0x00, 0x00, 0x00 },   //0 "AAPL,HasLid"
  { 0x01, 0x00, 0x00, 0x00 },   //1 "AAPL,HasPanel"
  { 0x04, 0x00, 0x00, 0x00 },   //2 "AAPL,NumDisplays"
  { 0x02, 0x00, 0x00, 0x00 },   //3 "AAPL,NumFramebuffers"
  { 0x3f, 0x00, 0x00, 0x08 },   //4 "AAPL01,BacklightIntensity"
  { 0x01, 0x00, 0x00, 0x00 },   //5 "AAPL01,BootDisplay"
  { 0x00, 0x00, 0x00, 0x00 },   //6 "AAPL01,CurrentDisplay"
  { 0x01, 0x00, 0x00, 0x00 },   //7 "AAPL01,DataJustify"
  { 0x00, 0x00, 0x00, 0x00 },   //8 "AAPL01,Depth"
  { 0x00, 0x00, 0x00, 0x00 },   //9 "AAPL01,Dither"
  { 0x20, 0x03, 0x00, 0x00 },   //10 "AAPL01,Height"
  { 0x00, 0x00, 0x00, 0x00 },   //11 "AAPL01,Interlace"
  { 0x00, 0x00, 0x00, 0x00 },   //12 "AAPL01,Inverter"
  { 0x00, 0x00, 0x00, 0x00 },   //13 "AAPL01,InverterCurrent"
  { 0xc8, 0x52, 0x00, 0x00 },   //14 "AAPL01,InverterFrequency"
  { 0x00, 0x10, 0x00, 0x00 },   //15 "AAPL01,IODisplayMode"
  { 0x00, 0x00, 0x00, 0x00 },   //16 "AAPL01,LinkFormat"
  { 0x00, 0x00, 0x00, 0x00 },   //17 "AAPL01,LinkType"
  { 0x01, 0x00, 0x00, 0x00 },   //18 "AAPL01,Pipe"
  { 0x00, 0x00, 0x00, 0x00 },   //19 "AAPL01,PixelFormat"
  { 0x3b, 0x00, 0x00, 0x00 },   //20 "AAPL01,Refresh"
  { 0x00, 0x00, 0x00, 0x00 },   //21 "AAPL01,Stretch"
  { 0x00, 0x00, 0x00, 0x00 },   //22 "AAPL01,T1"
  { 0x01, 0x00, 0x00, 0x00 },   //23 "AAPL01,T2"
  { 0xc8, 0x00, 0x00, 0x00 },   //24 "AAPL01,T3"
  { 0xc8, 0x01, 0x00, 0x00 },   //25 "AAPL01,T4"
  { 0x01, 0x00, 0x00, 0x00 },   //26 "AAPL01,T5"
  { 0x00, 0x00, 0x00, 0x00 },   //27 "AAPL01,T6"
  { 0x90, 0x01, 0x00, 0x00 },   //28 "AAPL01,T7"
  { 0x00, 0x05, 0x00, 0x00 },   //29 "AAPL01,Width"
};


UINT8 crestline_gma_vals[34][4] = {
  { 0x01, 0x00, 0x00, 0x00 },   //0 "AAPL,HasLid"
  { 0x01, 0x00, 0x00, 0x00 },   //1 "AAPL,HasPanel"
  { 0x04, 0x00, 0x00, 0x00 },   //2 "AAPL,NumDisplays"
  { 0x02, 0x00, 0x00, 0x00 },   //3 "AAPL,NumFramebuffers"
  { 0x01, 0x00, 0x00, 0x00 },   //4 "AAPL,SelfRefreshSupported"
  { 0x01, 0x00, 0x00, 0x00 },   //5 "AAPL,aux-power-connected"
  { 0x01, 0x00, 0x00, 0x08 },   //6 "AAPL,backlight-control"
  { 0x00, 0x00, 0x00, 0x08 },   //7 "AAPL00,blackscreen-preferences"
  { 0x01, 0x00, 0x00, 0x00 },   //8 "AAPL01,BootDisplay"
  { 0x38, 0x00, 0x00, 0x08 },   //9 "AAPL01,BacklightIntensity"
  { 0x00, 0x00, 0x00, 0x00 },   //10 "AAPL01,blackscreen-preferences"
  { 0x00, 0x00, 0x00, 0x00 },   //11 "AAPL01,CurrentDisplay"
  { 0x01, 0x00, 0x00, 0x00 },   //12 "AAPL01,DataJustify"
  { 0x20, 0x00, 0x00, 0x00 },   //13 "AAPL01,Depth"
  { 0x00, 0x00, 0x00, 0x00 },   //14 "AAPL01,Dither"
  { 0x20, 0x03, 0x00, 0x00 },   //15 "AAPL01,Height"
  { 0x00, 0x00, 0x00, 0x00 },   //16 "AAPL01,Interlace"
  { 0x00, 0x00, 0x00, 0x00 },   //17 "AAPL01,Inverter"
  { 0x08, 0x52, 0x00, 0x00 },   //18 "AAPL01,InverterCurrent"
  { 0xaa, 0x01, 0x00, 0x00 },   //19 "AAPL01,InverterFrequency"
  { 0x00, 0x00, 0x00, 0x00 },   //20 "AAPL01,LinkFormat"
  { 0x00, 0x00, 0x00, 0x00 },   //21 "AAPL01,LinkType"
  { 0x01, 0x00, 0x00, 0x00 },   //22 "AAPL01,Pipe"
  { 0x00, 0x00, 0x00, 0x00 },   //23 "AAPL01,PixelFormat"
  { 0x3d, 0x00, 0x00, 0x00 },   //24 "AAPL01,Refresh"
  { 0x00, 0x00, 0x00, 0x00 },   //25 "AAPL01,Stretch"
  { 0x00, 0x00, 0x00, 0x00 },   //26 "AAPL01,T1"
  { 0x01, 0x00, 0x00, 0x00 },   //27 "AAPL01,T2"
  { 0xc8, 0x00, 0x00, 0x00 },   //28 "AAPL01,T3"
  { 0xc8, 0x01, 0x00, 0x00 },   //29 "AAPL01,T4"
  { 0x01, 0x00, 0x00, 0x00 },   //30 "AAPL01,T5"
  { 0x00, 0x00, 0x00, 0x00 },   //31 "AAPL01,T6"
  { 0x90, 0x01, 0x00, 0x00 },   //32 "AAPL01,T7"
  { 0x00, 0x05, 0x00, 0x00 },   //33 "AAPL01,Width"
};


UINT8 ironlake_hd_vals[10][4] = {
  { 0x01, 0x00, 0x00, 0x00 },   //0 "AAPL,aux-power-connected"
  { 0x01, 0x00, 0x00, 0x00 },   //1 "AAPL,backlight-control"
  { 0x00, 0x00, 0x00, 0x00 },   //2 "AAPL00,T1"
  { 0x01, 0x00, 0x00, 0x00 },   //3 "AAPL00,T2"
  { 0xc8, 0x00, 0x00, 0x00 },   //4 "AAPL00,T3"
  { 0xc8, 0x01, 0x00, 0x00 },   //5 "AAPL00,T4"
  { 0x01, 0x00, 0x00, 0x00 },   //6 "AAPL00,T5"
  { 0x00, 0x00, 0x00, 0x00 },   //7 "AAPL00,T6"
  { 0x90, 0x01, 0x00, 0x00 },   //8 "AAPL00,T7"
  { 0x00, 0x00, 0x00, 0x12 },   //9 "VRAM,totalsize"
};


UINT8 sandy_bridge_snb_vals[6][4] = {
  { 0x00, 0x00, 0x01, 0x00 },   //0 *MacBookPro8,1 - Intel HD Graphics 3000 - VRAM: 384MB/512MB, Port Index: 01020400, Port Count: 4, Connector: LVDS1/DP3, BL: 0x0710
  { 0x00, 0x00, 0x02, 0x00 },   //1 *MacBookPro8,2/MacBookPro8,3 - Intel HD Graphics 3000 - VRAM: 384MB/512MB, Port Index: 01020100, Port Count: 1, Connector: LVDS1, BL: 0x0710
  { 0x10, 0x00, 0x03, 0x00 },   //2 *Macmini5,1 - Intel HD Graphics 3000 - VRAM: 384MB/512MB, Port Index: 00020300, Port Count: 3, Connector: HDMI1/DP2, BL:
  { 0x20, 0x00, 0x03, 0x00 },   //3 *Macmini5,3 - Intel HD Graphics 3000 - VRAM: 384MB/512MB, Port Index: 01030400, Port Count: 4, Connector: HDMI1/DP3, BL: 0x0710
  { 0x00, 0x00, 0x04, 0x00 },   //4 *MacBookAir4,1/MacBookAir4,2 - Intel HD Graphics 3000 - VRAM: 384MB/512MB, Port Index: 01020300, Port Count: 3, Connector: LVDS1/DP2, BL: 0x0710
  { 0x00, 0x00, 0x05, 0x00 },   //5 Intel HD Graphics 3000 - VRAM: 384MB/512MB, Port Index: 00020100, Port Count: 1, Connector: DP1, BL:
};

UINT8 sandy_bridge_hd_vals[13][4] = {
  { 0x00, 0x00, 0x00, 0x00 },   //0 "AAPL00,DataJustify"
  { 0x00, 0x00, 0x00, 0x00 },   //1 "AAPL00,Dither"
  { 0x00, 0x00, 0x00, 0x00 },   //2 "AAPL00,LinkFormat"
  { 0x00, 0x00, 0x00, 0x00 },   //3 "AAPL00,LinkType"
  { 0x00, 0x00, 0x00, 0x00 },   //4 "AAPL00,PixelFormat"
  { 0x00, 0x00, 0x00, 0x00 },   //5 "AAPL00,T1"
  { 0x14, 0x00, 0x00, 0x00 },   //6 "AAPL00,T2"
  { 0xfa, 0x00, 0x00, 0x00 },   //7 "AAPL00,T3"
  { 0x2c, 0x01, 0x00, 0x00 },   //8 "AAPL00,T4"
  { 0x00, 0x00, 0x00, 0x00 },   //9 "AAPL00,T5"
  { 0x14, 0x00, 0x00, 0x00 },   //10 "AAPL00,T6"
  { 0xf4, 0x01, 0x00, 0x00 },   //11 "AAPL00,T7"
  { 0x04, 0x00, 0x00, 0x00 },   //12 "graphic-options"
};


UINT8 ivy_bridge_ig_vals[12][4] = {
  { 0x00, 0x00, 0x66, 0x01 },   //0 Intel HD Graphics 4000 - Port Count: 4, Pipes: 3, BIOS-allocated memory: 96MB, FBM(cursor): 24MB, VRAM: 1024MB, Connector: LVDS1/DP3, BL: 0x0710
  { 0x01, 0x00, 0x66, 0x01 },   //1 *MacBookPro10,2 - Intel HD Graphics 4000 - Port Count: 4, Pipes: 3, BIOS-allocated memory: 96MB, FBM(cursor): 24MB, VRAM: 1536MB, Connector: LVDS1/DP2/HDMI1, BL: 0x0710
  { 0x02, 0x00, 0x66, 0x01 },   //2 *MacBookPro10,1 - Intel HD Graphics 4000 - Port Count: 1, Pipes: 3, BIOS-allocated memory: 64MB, FBM(cursor): 24MB, VRAM: 1536MB, Connector: LVDS1, BL: 0x0710
  { 0x03, 0x00, 0x66, 0x01 },   //3 *MacBookPro9,2 - Intel HD Graphics 4000 - Port Count: 4, Pipes: 2, BIOS-allocated memory: 64MB, FBM(cursor): 16MB, VRAM: 1536MB, Connector: LVDS1/DP3, BL: 0x0710
  { 0x04, 0x00, 0x66, 0x01 },   //4 *MacBookPro9,1 - Intel HD Graphics 4000 - Port Count: 1, Pipes: 1, BIOS-allocated memory: 32MB, FBM(cursor): 16MB, VRAM: 1536MB, Connector: LVDS1, BL: 0x0710
  { 0x05, 0x00, 0x62, 0x01 },   //5 Intel HD Graphics 4000 - Port Count: 3, Pipes: 2, BIOS-allocated memory: 32MB, FBM(cursor): 16MB, VRAM: 1536MB, Connector: DP3, BL: 0x0710
  { 0x06, 0x00, 0x62, 0x01 },   //6 *iMac13,1 - Intel HD Graphics 4000 - Port Count: 0, Pipes: 0, BIOS-allocated memory: 0MB, FBM(cursor): 0MB, VRAM: 1MB, Connector:, BL: 0x0710
  { 0x07, 0x00, 0x62, 0x01 },   //7 *iMac13,2 - Intel HD Graphics 4000 - Port Count: 0, Pipes: 0, BIOS-allocated memory: 0MB, FBM(cursor): 0MB, VRAM: 1MB, Connector:, BL: 0x0710
  { 0x08, 0x00, 0x66, 0x01 },   //8 *MacBookAir5,1 - Intel HD Graphics 4000 - Port Count: 3, Pipes: 3, BIOS-allocated memory: 64MB, FBM(cursor): 34MB, VRAM: 1536MB, Connector: LVDS1/DP2, BL: 0x0710
  { 0x09, 0x00, 0x66, 0x01 },   //9 *MacBookAir5,2 - Intel HD Graphics 4000 - Port Count: 3, Pipes: 3, BIOS-allocated memory: 64MB, FBM(cursor): 34MB, VRAM: 1536MB, Connector: LVDS1/DP2, BL: 0x0710
  { 0x0a, 0x00, 0x66, 0x01 },   //10 *Macmini6,1 - Intel HD Graphics 4000 - Port Count: 3, Pipes: 2, BIOS-allocated memory: 32MB, FBM(cursor): 32MB, VRAM: 1536MB, Connector: DP2/HDMI1, BL: 0x0710
  { 0x0b, 0x00, 0x66, 0x01 },   //11 *Macmini6,2 - Intel HD Graphics 4000 - Port Count: 3, Pipes: 2, BIOS-allocated memory: 32MB, FBM(cursor): 34MB, VRAM: 1536MB, Connector: DP2/HDMI1, BL: 0x0710
};

UINT8 ivy_bridge_hd_vals[1][4] = {
  { 0x0c, 0x00, 0x00, 0x00 },   //0 "graphics-options"
};


UINT8 haswell_ig_vals[16][4] = {
  { 0x00, 0x00, 0x06, 0x04 },   //0 Port Count: 3, Pipes: 3, BIOS-allocated memory: 64MB, FBM(cursor): 16MB, VRAM: 1024MB, Connector: LVDS1/eDP1/HDMI1, BL: 0x1499
  { 0x00, 0x00, 0x06, 0x0c },   //1 Port Count: 3, Pipes: 3, BIOS-allocated memory: 64MB, FBM(cursor): 16MB, VRAM: 1024MB, Connector: LVDS1/eDP1/HDMI1, BL: 0x1499
  { 0x00, 0x00, 0x16, 0x04 },   //2 Intel HD Graphics 4600 - Port Count: 3, Pipes: 3, BIOS-allocated memory: 64MB, FBM(cursor): 16MB, VRAM: 1024MB, Connector: LVDS1/eDP1/HDMI1, BL: 0x1499
  { 0x00, 0x00, 0x16, 0x0a },   //3 Intel HD Graphics 4400 - Port Count: 3, Pipes: 3, BIOS-allocated memory: 64MB, FBM(cursor): 16MB, VRAM: 1024MB, Connector: LVDS1/eDP1/HDMI1, BL: 0x0ad9
  { 0x00, 0x00, 0x16, 0x0c },   //4 Port Count: 3, Pipes: 3, BIOS-allocated memory: 64MB, FBM(cursor): 16MB, VRAM: 1024MB, Connector: LVDS1/eDP1/HDMI1, BL: 0x1499
  { 0x00, 0x00, 0x26, 0x04 },   //5 Intel HD Graphics 5000 - Port Count: 3, Pipes: 3, BIOS-allocated memory: 64MB, FBM(cursor): 16MB, VRAM: 1024MB, Connector: LVDS1/eDP1/HDMI1, BL: 0x1499
  { 0x00, 0x00, 0x26, 0x0a },   //6 Intel HD Graphics 5000 - Port Count: 3, Pipes: 3, BIOS-allocated memory: 64MB, FBM(cursor): 16MB, VRAM: 1024MB, Connector: LVDS1/eDP1/HDMI1, BL: 0x0ad9
  { 0x00, 0x00, 0x26, 0x0c },   //7 Port Count: 3, Pipes: 3, BIOS-allocated memory: 64MB, FBM(cursor): 16MB, VRAM: 1024MB, Connector: LVDS1/eDP1/HDMI1, BL: 0x1499
  { 0x00, 0x00, 0x26, 0x0d },   //8 Intel Iris Pro Graphics 5200 - Port Count: 3, Pipes: 3, BIOS-allocated memory: 64MB, FBM(cursor): 16MB, VRAM: 1024MB, Connector: LVDS1/eDP1/HDMI1, BL: 0x1499
  { 0x03, 0x00, 0x22, 0x0d },   //9 *iMac14,1/iMac14,4 - Intel Iris Pro Graphics 5200 - Port Count: 3, Pipes: 3, BIOS-allocated memory: 32MB, FBM(cursor): 19MB, VRAM: 1536MB, Connector: LVDS1/DP2, BL: 0x1499
  { 0x04, 0x00, 0x12, 0x04 },   //10 Intel HD Graphics 4600 - Port Count: 0, Pipes: 0, BIOS-allocated memory: 32MB, FBM(cursor): 0MB, VRAM: 1536MB, Connector:, BL:
  { 0x05, 0x00, 0x26, 0x0a },   //11 Intel HD Graphics 5000 - Port Count: 3, Pipes: 3, BIOS-allocated memory: 32MB, FBM(cursor): 19MB, VRAM: 1536MB, Connector: LVDS1/DP2, BL: 0x0ad9
  { 0x06, 0x00, 0x26, 0x0a },   //12 *MacBookAir6,1/MacBookAir6,2/Macmini7,1 - Intel HD Graphics 5000 - Port Count: 3, Pipes: 3, BIOS-allocated memory: 32MB, FBM(cursor): 19MB, VRAM: 1536MB, Connector: LVDS1/DP2, BL: 0x0ad9
  { 0x07, 0x00, 0x26, 0x0d },   //13 *MacBookPro11,2/MacBookPro11,3 - Intel Iris Pro Graphics 5200 - Port Count: 4, Pipes: 3, BIOS-allocated memory: 64MB, FBM(cursor): 34MB, VRAM: 1536MB, Connector: LVDS1/DP2/HDMI1, BL: 0x07a1
  { 0x08, 0x00, 0x2e, 0x0a },   //14 *MacBookPro11,1 - Intel Iris Graphics 5100 - Port Count: 3, Pipes: 3, BIOS-allocated memory: 64MB, FBM(cursor): 34MB, VRAM: 1536MB, Connector: LVDS1/DP2, BL: 0x056c
  { 0x0b, 0x00, 0x12, 0x04 },   //15 *iMac15,1 - Intel HD Graphics 4600 - Port Count: 0, Pipes: 0, BIOS-allocated memory: 32MB, FBM(cursor): 0MB, VRAM: 1536MB, Connector:, BL:
};

UINT8 haswell_hd_vals[1][4] = {
  { 0x0c, 0x00, 0x00, 0x00 },   //0 "graphics-options"
};


UINT8 broadwell_ig_vals[22][4] = {
  { 0x00, 0x00, 0x06, 0x16 },   //0 Port Count: 3, Pipes: 3, BIOS-allocated memory: 16MB, FBM(cursor): 15MB, VRAM: 1024MB, Connector: LVDS1/eDP1/HDMI1, BL: 0x1499
  { 0x00, 0x00, 0x0e, 0x16 },   //1 Port Count: 3, Pipes: 3, BIOS-allocated memory: 16MB, FBM(cursor): 15MB, VRAM: 1024MB, Connector: LVDS1/eDP1/HDMI1, BL: 0x1499
  { 0x00, 0x00, 0x16, 0x16 },   //2 Intel HD Graphics 5500 - Port Count: 3, Pipes: 3, BIOS-allocated memory: 16MB, FBM(cursor): 15MB, VRAM: 1024MB, Connector: LVDS1/eDP1/HDMI1, BL: 0x1499
  { 0x00, 0x00, 0x1e, 0x16 },   //3 Intel HD Graphics 5300 - Port Count: 3, Pipes: 3, BIOS-allocated memory: 16MB, FBM(cursor): 15MB, VRAM: 1024MB, Connector: LVDS1/eDP1/HDMI1, BL: 0x1499
  { 0x00, 0x00, 0x22, 0x16 },   //4 Intel Iris Pro Graphics 6200 - Port Count: 3, Pipes: 3, BIOS-allocated memory: 16MB, FBM(cursor): 15MB, VRAM: 1024MB, Connector: LVDS1/eDP1/HDMI1, BL: 0x1499
  { 0x00, 0x00, 0x26, 0x16 },   //5 Intel HD Graphics 6000 - Port Count: 3, Pipes: 3, BIOS-allocated memory: 16MB, FBM(cursor): 15MB, VRAM: 1024MB, Connector: LVDS1/eDP1/HDMI1, BL: 0x1499
  { 0x00, 0x00, 0x2b, 0x16 },   //6 Intel Iris Graphics 6100 - Port Count: 3, Pipes: 3, BIOS-allocated memory: 16MB, FBM(cursor): 15MB, VRAM: 1024MB, Connector: LVDS1/eDP1/HDMI1, BL: 0x1499
  { 0x01, 0x00, 0x0e, 0x16 },   //7 Port Count: 3, Pipes: 3, BIOS-allocated memory: 34MB, FBM(cursor): 21MB, VRAM: 1536MB, Connector: LVDS1/DP2, BL: 0x056c
  { 0x01, 0x00, 0x1e, 0x16 },   //8 *MacBook8,1 - Intel HD Graphics 5300 - Port Count: 3, Pipes: 3, BIOS-allocated memory: 38MB, FBM(cursor): 21MB, VRAM: 1536MB, Connector: LVDS1/DP2, BL: 0x056c
  { 0x02, 0x00, 0x06, 0x16 },   //9 Port Count: 3, Pipes: 3, BIOS-allocated memory: 34MB, FBM(cursor): 21MB, VRAM: 1536MB, Connector: LVDS1/DP2, BL: 0x056c
  { 0x02, 0x00, 0x16, 0x16 },   //10 Intel HD Graphics 5500 - Port Count: 3, Pipes: 3, BIOS-allocated memory: 34MB, FBM(cursor): 21MB, VRAM: 1536MB, Connector: LVDS1/DP2, BL: 0x056c
  { 0x02, 0x00, 0x22, 0x16 },   //11 Intel Iris Pro Graphics 6200 - Port Count: 3, Pipes: 3, BIOS-allocated memory: 34MB, FBM(cursor): 0MB, VRAM: 1536MB, Connector: LVDS1/DP2, BL: 0x056c
  { 0x02, 0x00, 0x26, 0x16 },   //12 Intel HD Graphics 6000 - Port Count: 3, Pipes: 3, BIOS-allocated memory: 34MB, FBM(cursor): 21MB, VRAM: 1536MB, Connector: LVDS1/DP2, BL: 0x056c
  { 0x02, 0x00, 0x2b, 0x16 },   //13 *MacBookPro12,1 - Intel Iris Graphics 6100 - Port Count: 3, Pipes: 3, BIOS-allocated memory: 34MB, FBM(cursor): 21MB, VRAM: 1536MB, Connector: LVDS1/DP2, BL: 0x056c
  { 0x03, 0x00, 0x12, 0x16 },   //14 Intel HD Graphics 5600 - Port Count: 3, Pipes: 3, BIOS-allocated memory: 34MB, FBM(cursor): 0MB, VRAM: 1536MB, Connector: LVDS1/DP2, BL: 0x07a1
  { 0x04, 0x00, 0x26, 0x16 },   //15 Intel HD Graphics 6000 - Port Count: 3, Pipes: 3, BIOS-allocated memory: 34MB, FBM(cursor): 21MB, VRAM: 1536MB, Connector: LVDS1/DP2, BL: 0x1499
  { 0x04, 0x00, 0x2b, 0x16 },   //16 Intel Iris Graphics 6100 - Port Count: 3, Pipes: 3, BIOS-allocated memory: 34MB, FBM(cursor): 21MB, VRAM: 1536MB, Connector: LVDS1/DP2, BL: 0x1499
  { 0x05, 0x00, 0x26, 0x16 },   //17 Intel HD Graphics 6000 - Port Count: 3, Pipes: 3, BIOS-allocated memory: 34MB, FBM(cursor): 21MB, VRAM: 1536MB, Connector: LVDS1/DP2, BL: 0x0ad9
  { 0x06, 0x00, 0x26, 0x16 },   //18 *iMac16,1/MacBookAir7,1/MacBookAir7,2 - Intel HD Graphics 6000 - Port Count: 3, Pipes: 3, BIOS-allocated memory: 34MB, FBM(cursor): 21MB, VRAM: 1536MB, Connector: LVDS1/DP2, BL: 0x0ad9
  { 0x07, 0x00, 0x22, 0x16 },   //19 *iMac16,2 - Intel Iris Pro Graphics 6200 - Port Count: 3, Pipes: 3, BIOS-allocated memory: 38MB, FBM(cursor): 38MB, VRAM: 1536MB, Connector: LVDS1/DP2, BL: 0x1499
  { 0x08, 0x00, 0x26, 0x16 },   //20 Intel HD Graphics 6000 - Port Count: 2, Pipes: 2, BIOS-allocated memory: 34MB, FBM(cursor): 34MB, VRAM: 1536MB, Connector: LVDS1/DP1, BL: 0x1499
  { 0x08, 0x00, 0x2b, 0x16 },   //21 Intel Iris Graphics 6100 - Port Count: 3, Pipes: 3, BIOS-allocated memory: 34MB, FBM(cursor): 0MB, VRAM: 1536MB, Connector: LVDS1/DP2, BL: 0x1499
};

UINT8 broadwell_hd_vals[2][4] = {
  { 0x0c, 0x00, 0x00, 0x00 },   //0 "AAPL,ig-tcon-scaler"
  { 0x0c, 0x00, 0x00, 0x00 },   //1 "graphics-options"
};


UINT8 skylake_ig_vals[15][4] = {
  { 0x00, 0x00, 0x12, 0x19 },   //0 Intel HD Graphics 530 - Port Count: 3, Pipes: 3, BIOS-allocated memory: 34MB, FBM(cursor): 21MB, VRAM: 1536MB, Connector: DP3, BL: 0x056c
  { 0x00, 0x00, 0x16, 0x19 },   //1 Intel HD Graphics 520 - Port Count: 3, Pipes: 3, BIOS-allocated memory: 34MB, FBM(cursor): 21MB, VRAM: 1536MB, Connector: LVDS1/DP2, BL: 0x056c
  { 0x00, 0x00, 0x26, 0x19 },   //2 Intel Iris Graphics 540 - Port Count: 3, Pipes: 3, BIOS-allocated memory: 34MB, FBM(cursor): 21MB, VRAM: 1536MB, Connector: LVDS1/DP2, BL: 0x056c
  { 0x00, 0x00, 0x27, 0x19 },   //3 Intel Iris Graphics 550 - Port Count: 3, Pipes: 3, BIOS-allocated memory: 34MB, FBM(cursor): 21MB, VRAM: 1536MB, Connector: LVDS1/DP2, BL: 0x056c
  { 0x00, 0x00, 0x1b, 0x19 },   //4 *MacBookPro13,3 - Intel HD Graphics 530 - Port Count: 3, Pipes: 3, BIOS-allocated memory: 34MB, FBM(cursor): 21MB, VRAM: 1536MB, Connector: LVDS1/DP2, BL: 0x056c
  { 0x00, 0x00, 0x1e, 0x19 },   //5 Intel HD Graphics 515 - Port Count: 3, Pipes: 3, BIOS-allocated memory: 34MB, FBM(cursor): 21MB, VRAM: 1536MB, Connector: LVDS1/DP2, BL: 0x056c
  { 0x00, 0x00, 0x3b, 0x19 },   //6 Intel Iris Pro Graphics 580 - Port Count: 3, Pipes: 3, BIOS-allocated memory: 34MB, FBM(cursor): 21MB, VRAM: 1536MB, Connector: LVDS1/DP1/HDMI1, BL: 0x056c
  { 0x01, 0x00, 0x12, 0x19 },   //7 *iMac17,1 - Intel HD Graphics 530 - Port Count: 0, Pipes: 0, BIOS-allocated memory: 0MB, FBM(cursor): 0MB, VRAM: 1536MB, Connector:, BL:
  { 0x02, 0x00, 0x16, 0x19 },   //8 Intel HD Graphics 520 - Port Count: 3, Pipes: 3, BIOS-allocated memory: 57MB, FBM(cursor): 0MB, VRAM: 1536MB, Connector: LVDS1/DP2, BL: 0x056c
  { 0x02, 0x00, 0x26, 0x19 },   //9 *MacBookPro13,1 - Intel Iris Graphics 540 - Port Count: 3, Pipes: 3, BIOS-allocated memory: 57MB, FBM(cursor): 0MB, VRAM: 1536MB, Connector: LVDS1/DP2, BL: 0x056c
  { 0x03, 0x00, 0x1e, 0x19 },   //10 *MacBook9,1 - Intel HD Graphics 515 - Port Count: 3, Pipes: 3, BIOS-allocated memory: 34MB, FBM(cursor): 21MB, VRAM: 1536MB, Connector: LVDS1/DP2, BL: 0x056c
  { 0x04, 0x00, 0x26, 0x19 },   //11 Intel Iris Graphics 540 - Port Count: 3, Pipes: 3, BIOS-allocated memory: 34MB, FBM(cursor): 0MB, VRAM: 1536MB, Connector: LVDS1/DP2, BL: 0x056c
  { 0x04, 0x00, 0x27, 0x19 },   //12 *MacBookPro13,2 - Intel Iris Graphics 550 - Port Count: 3, Pipes: 3, BIOS-allocated memory: 34MB, FBM(cursor): 21MB, VRAM: 1536MB, Connector: LVDS1/DP2, BL: 0x056c
  { 0x06, 0x00, 0x1b, 0x19 },   //13 Intel HD Graphics 530 - Port Count: 1, Pipes: 1, BIOS-allocated memory: 38MB, FBM(cursor): 0MB, VRAM: 1536MB, Connector: LVDS1, BL: 0x056c
  { 0x07, 0x00, 0x26, 0x19 },   //14 Intel Iris Graphics 540 - Port Count: 3, Pipes: 3, BIOS-allocated memory: 34MB, FBM(cursor): 0MB, VRAM: 1536MB, Connector: LVDS1/DP2, BL: 0x056c
};

UINT8 skylake_hd_vals[12][4] = {
  { 0x01, 0x00, 0x00, 0x00 },   //0 "AAPL,Gfx324"
  { 0x01, 0x00, 0x00, 0x00 },   //1 "AAPL,GfxYTile"
  { 0xfa, 0x00, 0x00, 0x00 },   //2 "AAPL00,PanelCycleDelay"
  { 0x11, 0x00, 0x00, 0x08 },   //3 "AAPL00,PanelPowerDown"  - MacBook
  { 0x11, 0x00, 0x00, 0x00 },   //4 "AAPL00,PanelPowerOff"   - MacBook
  { 0xe2, 0x00, 0x00, 0x08 },   //5 "AAPL00,PanelPowerOn"    - MacBook
  { 0x48, 0x00, 0x00, 0x00 },   //6 "AAPL00,PanelPowerUp"    - MacBook
  { 0x3c, 0x00, 0x00, 0x08 },   //7 "AAPL00,PanelPowerDown"  - MacBookPro
  { 0x11, 0x00, 0x00, 0x00 },   //8 "AAPL00,PanelPowerOff"   - MacBookPro
  { 0x19, 0x01, 0x00, 0x08 },   //9 "AAPL00,PanelPowerOn"    - MacBookPro
  { 0x30, 0x00, 0x00, 0x00 },   //10 "AAPL00,PanelPowerUp"   - MacBookPro
  { 0x0c, 0x00, 0x00, 0x00 },   //11 "graphic-options"
};


UINT8 kabylake_ig_vals[15][4] = {
  { 0x00, 0x00, 0x12, 0x59 },   //0 Intel HD Graphics 630 - Port Count: 3, Pipes: 3, BIOS-allocated memory: 38MB, FBM(cursor): 0MB, VRAM: 1536MB, Connector: DP3, BL: 0x056c
  { 0x00, 0x00, 0x16, 0x59 },   //1 Intel HD Graphics 620 - Port Count: 3, Pipes: 3, BIOS-allocated memory: 34MB, FBM(cursor): 0MB, VRAM: 1536MB, Connector: LVDS1/DP1/HDMI1, BL: 0x056c
  { 0x00, 0x00, 0x26, 0x59 },   //2 Intel Iris Plus Graphics 640 - Port Count: 3, Pipes: 3, BIOS-allocated memory: 38MB, FBM(cursor): 0MB, VRAM: 1536MB, Connector: LVDS1/DP2, BL: 0x056c
  { 0x00, 0x00, 0x27, 0x59 },   //3 Intel Iris Plus Graphics 650 - Port Count: 3, Pipes: 3, BIOS-allocated memory: 38MB, FBM(cursor): 0MB, VRAM: 1536MB, Connector: LVDS1/DP2, BL: 0x056c
  { 0x00, 0x00, 0x1b, 0x59 },   //4 *MacBookPro14,3 - Intel HD Graphics 630 - Port Count: 3, Pipes: 3, BIOS-allocated memory: 38MB, FBM(cursor): 21MB, VRAM: 1536MB, Connector: LVDS1/DP1/HDMI1, BL: 0x056c
  { 0x00, 0x00, 0x1e, 0x59 },   //5 Intel HD Graphics 615 - Port Count: 3, Pipes: 3, BIOS-allocated memory: 34MB, FBM(cursor): 0MB, VRAM: 1536MB, Connector: LVDS1/DP2, BL: 0x056c
  { 0x00, 0x00, 0x23, 0x59 },   //6 Intel HD Graphics 635 - Port Count: 3, Pipes: 3, BIOS-allocated memory: 38MB, FBM(cursor): 0MB, VRAM: 1536MB, Connector: LVDS1/DP2, BL: 0x056c
  { 0x01, 0x00, 0x1e, 0x59 },   //7 *MacBook10,1 - Intel HD Graphics 615 - Port Count: 3, Pipes: 3, BIOS-allocated memory: 38MB, FBM(cursor): 0MB, VRAM: 1536MB, Connector: LVDS1/DP2, BL: 0x056c
  { 0x02, 0x00, 0x18, 0x59 },   //8 Port Count: 0, Pipes: 0, BIOS-allocated memory: 38MB, FBM(cursor): 0MB, VRAM: 1536MB, Connector:, BL: 0x056c
  { 0x02, 0x00, 0x26, 0x59 },   //9 *MacBookPro14,1/iMac18,1 - Intel Iris Plus Graphics 640 - Port Count: 3, Pipes: 3, BIOS-allocated memory: 57MB, FBM(cursor): 0MB, VRAM: 1536MB, Connector: LVDS1/DP2, BL: 0x056c
  { 0x03, 0x00, 0x12, 0x59 },   //10 *iMac18,2/iMac18,3 - Intel HD Graphics 630 - Port Count: 0, Pipes: 0, BIOS-allocated memory: 0MB, FBM(cursor): 0MB, VRAM: 1536MB, Connector:, BL: 0x056c
  { 0x04, 0x00, 0x27, 0x59 },   //11 *MacBookPro14,2 - Intel Iris Plus Graphics 650 - Port Count: 3, Pipes: 3, BIOS-allocated memory: 57MB, FBM(cursor): 0MB, VRAM: 1536MB, Connector: LVDS1/DP2, BL: 0x056c
  { 0x06, 0x00, 0x1b, 0x59 },   //12 Intel HD Graphics 630 - Port Count: 1, Pipes: 1, BIOS-allocated memory: 38MB, FBM(cursor): 0MB, VRAM: 1536MB, Connector: LVDS1, BL: 0x056c
  { 0x07, 0x00, 0x26, 0x59 },   //13 Intel Iris Plus Graphics 640 - Port Count: 3, Pipes: 3, BIOS-allocated memory: 57MB, FBM(cursor): 21MB, VRAM: 1536MB, Connector: LVDS1/DP2, BL: 0x056c
  { 0x09, 0x00, 0x27, 0x59 },   //14 Intel Iris Plus Graphics 650 - Port Count: 3, Pipes: 3, BIOS-allocated memory: 38MB, FBM(cursor): 0MB, VRAM: 1536MB, Connector: LVDS1/DP2, BL: 0x056c
};

UINT8 kabylake_hd_vals[12][4] = {
  { 0x01, 0x00, 0x00, 0x00 },   //0 "AAPL,Gfx324"
  { 0x01, 0x00, 0x00, 0x00 },   //1 "AAPL,GfxYTile"
  { 0xfa, 0x00, 0x00, 0x00 },   //2 "AAPL00,PanelCycleDelay"
  { 0x11, 0x00, 0x00, 0x08 },   //3 "AAPL00,PanelPowerDown"  - MacBook
  { 0x11, 0x00, 0x00, 0x00 },   //4 "AAPL00,PanelPowerOff"   - MacBook
  { 0xe2, 0x00, 0x00, 0x08 },   //5 "AAPL00,PanelPowerOn"    - MacBook
  { 0x48, 0x00, 0x00, 0x00 },   //6 "AAPL00,PanelPowerUp"    - MacBook
  { 0x3c, 0x00, 0x00, 0x08 },   //7 "AAPL00,PanelPowerDown"  - MacBookPro
  { 0x11, 0x00, 0x00, 0x00 },   //8 "AAPL00,PanelPowerOff"   - MacBookPro
  { 0x19, 0x01, 0x00, 0x08 },   //9 "AAPL00,PanelPowerOn"    - MacBookPro
  { 0x30, 0x00, 0x00, 0x00 },   //10 "AAPL00,PanelPowerUp"   - MacBookPro
  { 0x0c, 0x00, 0x00, 0x00 },   //11 "graphic-options"
};


// The following values came from MacBookPro6,1
UINT8 mbp_HD_os_info[20]  = {
  0x30, 0x49, 0x01, 0x11, 0x01, 0x10, 0x08, 0x00, 0x00, 0x01,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF
};

// The following values came from MacBookAir4,1
UINT8 mba_HD3000_tbl_info[18] = {
  0x30, 0x44, 0x02, 0x02, 0x02, 0x02, 0x00, 0x00, 0x00,
  0x00, 0x02, 0x02, 0x02, 0x02, 0x01, 0x01, 0x01, 0x01
};

// The following values came from MacBookAir4,1
UINT8 mba_HD3000_os_info[20] = {
  0x30, 0x49, 0x01, 0x12, 0x12, 0x12, 0x08, 0x00, 0x00, 0x01,
  0xf0, 0x1f, 0x01, 0x00, 0x00, 0x00, 0x10, 0x07, 0x00, 0x00
};

// The following values came from MacBookPro8,1
UINT8 mbp_HD3000_tbl_info[18] = {
  0x30, 0x44, 0x02, 0x02, 0x02, 0x02, 0x00, 0x00, 0x00,
  0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01
};

// The following values came from MacBookPro8,1
UINT8 mbp_HD3000_os_info[20] = {
  0x30, 0x49, 0x01, 0x11, 0x11, 0x11, 0x08, 0x00, 0x00, 0x01,
  0xf0, 0x1f, 0x01, 0x00, 0x00, 0x00, 0x10, 0x07, 0x00, 0x00
};

// The following values came from Macmini5,1
UINT8 mn_HD3000_tbl_info[18] = {
  0x30, 0x44, 0x02, 0x02, 0x02, 0x02, 0x00, 0x00, 0x00,
  0x00, 0x02, 0x02, 0x02, 0x02, 0x00, 0x00, 0x00, 0x00
};

// The following values came from Macmini5,1
UINT8 mn_HD3000_os_info[20] = {
  0x30, 0x49, 0x01, 0x50, 0x50, 0x50, 0x08, 0x00, 0x00, 0x01,
  0xf0, 0x1f, 0x01, 0x00, 0x00, 0x00, 0x10, 0x07, 0x00, 0x00
};


static struct gma_gpu_t KnownGPUS[] = {

  //============== PowerVR ===================
  //--------Canmore/Sodaville/Groveland-------
  { 0x2E5B, "Intel 500"                      }, //

  //----------------Poulsbo-------------------
  { 0x8108, "Intel 500"                      }, // Menlow
  { 0x8109, "Intel 500"                      }, // Menlow

  //----------------Lincroft------------------
  { 0x4102, "Intel 600"                      }, // Moorestown

  //----------------Cedarview-----------------
  { 0x0BE0, "Intel GMA 3600"                 }, // Cedar Trail
  { 0x0BE1, "Intel GMA 3600"                 }, // Cedar Trail
  { 0x0BE2, "Intel GMA 3650"                 }, // Cedar Trail
  { 0x0BE3, "Intel GMA 3650"                 }, // Cedar Trail

  //----------------Cloverview----------------
  { 0x08C7, "Intel GMA"                      }, // Clover Trail
  { 0x08C8, "Intel GMA"                      }, // Clover Trail
  { 0x08C9, "Intel GMA"                      }, // Clover Trail
  { 0x08CA, "Intel GMA"                      }, // Clover Trail
  { 0x08CB, "Intel GMA"                      }, // Clover Trail
  { 0x08CC, "Intel GMA"                      }, // Clover Trail
  { 0x08CD, "Intel GMA"                      }, // Clover Trail
  { 0x08CE, "Intel GMA"                      }, // Clover Trail
  { 0x08CF, "Intel GMA"                      }, // Clover Trail


  //============== 1st generation ============
  //----------------Auburn--------------------
  { 0x7800, "Intel 740"                      }, // Desktop - Intel 740 GMCH Express Chipset Family

  //----------------Portola-------------------
  { 0x1240, "Intel 752"                      }, // Desktop - Intel 752 GMCH Express Chipset Family

  //----------------Whitney-------------------
  { 0x7121, "Intel 3D graphics 810"          }, // Desktop - Intel 810 GMCH Express Chipset Family
  { 0x7123, "Intel 3D graphics 810"          }, // Desktop - Intel 810-DC100 GMCH Express Chipset Family
  { 0x7125, "Intel 3D graphics 810"          }, // Desktop - Intel 810E GMCH Express Chipset Family

  //----------------Solano--------------------
  { 0x1132, "Intel 3D graphics 815"          }, // Desktop - Intel 815 GMCH Express Chipset Family


  //============== 2nd generation ============
  //----------------Almador-------------------
  { 0x3577, "Intel Extreme Graphics 830"     }, // Mobile - Intel 830M GMCH Express Chipset Family
  { 0x357B, "Intel Extreme Graphics 835"     }, // Desktop - Intel 835G GMCH Express Chipset Family

  //----------------Brookdale-----------------
  { 0x2562, "Intel Extreme Graphics 845"     }, // Desktop - Intel 845G GMCH Express Chipset Family

  //----------------Montara-------------------
  { 0x358E, "Intel Extreme Graphics 2 854"   }, // Mobile - Intel 852GM/855GM GMCH Express Chipset Family
  { 0x3582, "Intel Extreme Graphics 2 855"   }, // Mobile - Intel 852GM/855GM GMCH Express Chipset Family

  //----------------Springdale----------------
  { 0x2572, "Intel Extreme Graphics 2 865"   }, // Desktop - Intel 865G Express Chipset Family


  //============== 3rd generation ============
  //----------------Grantsdale----------------
  { 0x2582, "Intel GMA 900"                  }, // Desktop - Intel 915G Express Chipset Family
  { 0x258A, "Intel GMA 900"                  }, // Desktop - Intel 915GM Express Chipset Family
  { 0x2782, "Intel GMA 900"                  }, // Desktop - Intel 915GV Express Chipset Family

  //----------------Alviso--------------------
  { 0x2592, "Intel GMA 900"                  }, // Mobile - Intel 82915GM/GMS, 910GML Express Chipset Family
  { 0x2792, "Intel GMA 900"                  }, // Mobile - Intel 82915GM/GMS, 910GML Express Chipset Family

  //----------------Lakeport------------------
  { 0x2772, "Intel GMA 950"                  }, // Desktop - Intel 82945G Express Chipset Family
  { 0x2776, "Intel GMA 950"                  }, // Desktop - Intel 82945G Express Chipset Family

  //----------------Calistoga-----------------
  { 0x27A2, "Intel GMA 950"                  }, // Mobile - Intel 945GM Express Chipset Family - MacBook1,1/MacBook2,1
  { 0x27A6, "Intel GMA 950"                  }, // Mobile - Intel 945GM Express Chipset Family
  { 0x27AE, "Intel GMA 950"                  }, // Mobile - Intel 945GM Express Chipset Family

  //----------------Bearlake------------------
  { 0x29B2, "Intel GMA 3100"                 }, // Desktop - Intel Q35 Express Chipset Family
  { 0x29B3, "Intel GMA 3100"                 }, // Desktop - Intel Q35 Express Chipset Family
  { 0x29C2, "Intel GMA 3100"                 }, // Desktop - Intel G33/G31 Express Chipset Family
  { 0x29C3, "Intel GMA 3100"                 }, // Desktop - Intel G33/G31 Express Chipset Family
  { 0x29D2, "Intel GMA 3100"                 }, // Desktop - Intel Q33 Express Chipset Family
  { 0x29D3, "Intel GMA 3100"                 }, // Desktop - Intel Q33 Express Chipset Family

  //----------------Pineview------------------
  { 0xA001, "Intel GMA 3150"                 }, // Nettop - Intel NetTop Atom D410
  { 0xA002, "Intel GMA 3150"                 }, // Nettop - Intel NetTop Atom D510
  { 0xA011, "Intel GMA 3150"                 }, // Netbook - Intel NetBook Atom N4x0
  { 0xA012, "Intel GMA 3150"                 }, // Netbook - Intel NetBook Atom N4x0


  //============== 4th generation ============
  //----------------Lakeport------------------
  { 0x2972, "Intel GMA 3000"                 }, // Desktop - Intel 946GZ Express Chipset Family
  { 0x2973, "Intel GMA 3000"                 }, // Desktop - Intel 946GZ Express Chipset Family

  //----------------Broadwater----------------
  { 0x2992, "Intel GMA 3000"                 }, // Desktop - Intel Q965/Q963 Express Chipset Family
  { 0x2993, "Intel GMA 3000"                 }, // Desktop - Intel Q965/Q963 Express Chipset Family
  { 0x29A2, "Intel GMA X3000"                }, // Desktop - Intel G965 Express Chipset Family
  { 0x29A3, "Intel GMA X3000"                }, // Desktop - Intel G965 Express Chipset Family

  //----------------Crestline-----------------
  { 0x2A02, "Intel GMA X3100"                }, // Mobile - Intel 965 Express Chipset Family - MacBook3,1/MacBook4,1/MacbookAir1,1
  { 0x2A03, "Intel GMA X3100"                }, // Mobile - Intel 965 Express Chipset Family
  { 0x2A12, "Intel GMA X3100"                }, // Mobile - Intel 965 Express Chipset Family
  { 0x2A13, "Intel GMA X3100"                }, // Mobile - Intel 965 Express Chipset Family

  //----------------Bearlake------------------
  { 0x2982, "Intel GMA X3500"                }, // Desktop - Intel G35 Express Chipset Family
  { 0x2983, "Intel GMA X3500"                }, // Desktop - Intel G35 Express Chipset Family

  //----------------Eaglelake-----------------
  { 0x2E02, "Intel GMA 4500"                 }, // Desktop - Intel 4 Series Express Chipset Family
  { 0x2E03, "Intel GMA 4500"                 }, // Desktop - Intel 4 Series Express Chipset Family
  { 0x2E12, "Intel GMA 4500"                 }, // Desktop - Intel G45/G43 Express Chipset Family
  { 0x2E13, "Intel GMA 4500"                 }, // Desktop - Intel G45/G43 Express Chipset Family
  { 0x2E42, "Intel GMA 4500"                 }, // Desktop - Intel B43 Express Chipset Family
  { 0x2E43, "Intel GMA 4500"                 }, // Desktop - Intel B43 Express Chipset Family
  { 0x2E92, "Intel GMA 4500"                 }, // Desktop - Intel B43 Express Chipset Family
  { 0x2E93, "Intel GMA 4500"                 }, // Desktop - Intel B43 Express Chipset Family
  { 0x2E32, "Intel GMA X4500"                }, // Desktop - Intel G45/G43 Express Chipset Family
  { 0x2E33, "Intel GMA X4500"                }, // Desktop - Intel G45/G43 Express Chipset Family
  { 0x2E22, "Intel GMA X4500"                }, // Mobile - Intel G45/G43 Express Chipset Family
  { 0x2E23, "Intel GMA X4500HD"              }, // Mobile - Intel G45/G43 Express Chipset Family

  //----------------Cantiga-------------------
  { 0x2A42, "Intel GMA X4500MHD"             }, // Mobile - Intel 4 Series Express Chipset Family
  { 0x2A43, "Intel GMA X4500MHD"             }, // Mobile - Intel 4 Series Express Chipset Family


  //============== 5th generation ============
  //----------------Ironlake------------------
  { 0x0042, "Intel HD Graphics"              }, // Desktop - Clarkdale
  { 0x0046, "Intel HD Graphics"              }, // Mobile - Arrandale - MacBookPro6,x


  //============== 6th generation ============
  //----------------Sandy Bridge--------------
  //GT1
  { 0x0102, "Intel HD Graphics 2000"         }, // Desktop - iMac12,x
  { 0x0106, "Intel HD Graphics 2000"         }, // Mobile
  { 0x010A, "Intel HD Graphics P3000"        }, // Server
  //GT2
  { 0x0112, "Intel HD Graphics 3000"         }, // Desktop
  { 0x0116, "Intel HD Graphics 3000"         }, // Mobile - MacBookAir4,x/MacBookPro8,2/MacBookPro8,3
  { 0x0122, "Intel HD Graphics 3000"         }, // Desktop
  { 0x0126, "Intel HD Graphics 3000"         }, // Mobile - MacBookPro8,1/Macmini5,x


  //============== 7th generation ============
  //----------------Ivy Bridge----------------
  //GT1
  { 0x0152, "Intel HD Graphics 2500"         }, // Desktop - iMac13,x
  { 0x0156, "Intel HD Graphics 2500"         }, // Mobile
  { 0x015A, "Intel HD Graphics 2500"         }, // Server
  { 0x015E, "Intel Ivy Bridge GT1"           }, // Reserved
  //GT2
  { 0x0162, "Intel HD Graphics 4000"         }, // Desktop
  { 0x0166, "Intel HD Graphics 4000"         }, // Mobile - MacBookPro9,x/MacBookPro10,x/MacBookAir5,x/Macmini6,x
  { 0x016A, "Intel HD Graphics P4000"        }, // Server


  //============== 7.5th generation ==========
  //----------------Haswell-------------------
  //GT1
  { 0x0402, "Intel Haswell GT1"              }, // Desktop
  { 0x0406, "Intel Haswell GT1"              }, // Mobile
  { 0x040A, "Intel Haswell GT1"              }, // Server
  { 0x040B, "Intel Haswell GT1"              }, //
  { 0x040E, "Intel Haswell GT1"              }, //
  //GT2
  { 0x0412, "Intel HD Graphics 4600"         }, // Desktop - iMac15,1
  { 0x0416, "Intel HD Graphics 4600"         }, // Mobile
  { 0x041A, "Intel HD Graphics P4600"        }, // Server
  { 0x041B, "Intel Haswell GT2"              }, //
  { 0x041E, "Intel HD Graphics 4400"         }, //
  //GT3
  { 0x0422, "Intel HD Graphics 5000"         }, // Desktop
  { 0x0426, "Intel HD Graphics 5000"         }, // Mobile
  { 0x042A, "Intel HD Graphics 5000"         }, // Server
  { 0x042B, "Intel Haswell GT3"              }, //
  { 0x042E, "Intel Haswell GT3"              }, //
  //GT1
  { 0x0A02, "Intel Haswell GT1"              }, // Desktop ULT
  { 0x0A06, "Intel HD Graphics"              }, // Mobile ULT
  { 0x0A0A, "Intel Haswell GT1"              }, // Server ULT
  { 0x0A0B, "Intel Haswell GT1"              }, // ULT
  { 0x0A0E, "Intel Haswell GT1"              }, // ULT
  //GT2
  { 0x0A12, "Intel Haswell GT2"              }, // Desktop ULT
  { 0x0A16, "Intel HD Graphics 4400"         }, // Mobile ULT
  { 0x0A1A, "Intel Haswell GT2"              }, // Server ULT
  { 0x0A1B, "Intel Haswell GT2"              }, // ULT
  { 0x0A1E, "Intel HD Graphics 4200"         }, // ULT
  //GT3
  { 0x0A22, "Intel Iris Graphics 5100"       }, // Desktop ULT
  { 0x0A26, "Intel HD Graphics 5000"         }, // Mobile ULT - MacBookAir6,x/Macmini7,1
  { 0x0A2A, "Intel Iris Graphics 5100"       }, // Server ULT
  { 0x0A2B, "Intel Iris Graphics 5100"       }, // ULT
  { 0x0A2E, "Intel Iris Graphics 5100"       }, // ULT - MacBookPro11,1
  //GT1
  { 0x0C02, "Intel Haswell GT1"              }, // Desktop SDV
  { 0x0C06, "Intel Haswell GT1"              }, // Mobile SDV
  { 0x0C0A, "Intel Haswell GT1"              }, // Server SDV
  { 0x0C0B, "Intel Haswell GT1"              }, // SDV
  { 0x0C0E, "Intel Haswell GT1"              }, // SDV
  //GT2
  { 0x0C12, "Intel Haswell GT2"              }, // Desktop SDV
  { 0x0C16, "Intel Haswell GT2"              }, // Mobile SDV
  { 0x0C1A, "Intel Haswell GT2"              }, // Server SDV
  { 0x0C1B, "Intel Haswell GT2"              }, // SDV
  { 0x0C1E, "Intel Haswell GT2"              }, // SDV
  //GT3
  { 0x0C22, "Intel Haswell GT3"              }, // Desktop SDV
  { 0x0C26, "Intel Haswell GT3"              }, // Mobile SDV
  { 0x0C2A, "Intel Haswell GT3"              }, // Server SDV
  { 0x0C2B, "Intel Haswell GT3"              }, // SDV
  { 0x0C2E, "Intel Haswell GT3"              }, // SDV
  //GT1
  { 0x0D02, "Intel Haswell GT1"              }, // Desktop CRW
  { 0x0D06, "Intel Haswell GT1"              }, // Mobile CRW
  { 0x0D0A, "Intel Haswell GT1"              }, // Server CRW
  { 0x0D0B, "Intel Haswell GT1"              }, // CRW
  { 0x0D0E, "Intel Haswell GT1"              }, // CRW
  //GT2
  { 0x0D12, "Intel HD Graphics 4600"         }, // Desktop CRW
  { 0x0D16, "Intel HD Graphics 4600"         }, // Mobile CRW
  { 0x0D1A, "Intel Haswell GT2"              }, // Server CRW
  { 0x0D1B, "Intel Haswell GT2"              }, // CRW
  { 0x0D1E, "Intel Haswell GT2"              }, // CRW
  //GT3
  { 0x0D22, "Intel Iris Pro Graphics 5200"   }, // Desktop CRW - iMac14,1/iMac14,4
  { 0x0D26, "Intel Iris Pro Graphics 5200"   }, // Mobile CRW - MacBookPro11,2/MacBookPro11,3
  { 0x0D2A, "Intel Iris Pro Graphics 5200"   }, // Server CRW
  { 0x0D2B, "Intel Iris Pro Graphics 5200"   }, // CRW
  { 0x0D2E, "Intel Iris Pro Graphics 5200"   }, // CRW

  //----------------ValleyView----------------
  { 0x0F30, "Intel HD Graphics"              }, // Bay Trail
  { 0x0F31, "Intel HD Graphics"              }, // Bay Trail
  { 0x0F32, "Intel HD Graphics"              }, // Bay Trail
  { 0x0F33, "Intel HD Graphics"              }, // Bay Trail
  { 0x0155, "Intel HD Graphics"              }, // Bay Trail
  { 0x0157, "Intel HD Graphics"              }, // Bay Trail


  //============== 8th generation ============
  //----------------Broadwell-----------------
  //GT1
  { 0x1602, "Intel Broadwell GT1"            }, // Desktop
  { 0x1606, "Intel Broadwell GT1"            }, // Mobile
  { 0x160A, "Intel Broadwell GT1"            }, //
  { 0x160B, "Intel Broadwell GT1"            }, //
  { 0x160D, "Intel Broadwell GT1"            }, //
  { 0x160E, "Intel Broadwell GT1"            }, //
  //GT2
  { 0x1612, "Intel HD Graphics 5600"         }, // Mobile
  { 0x1616, "Intel HD Graphics 5500"         }, // Mobile
  { 0x161A, "Intel Broadwell GT2"            }, //
  { 0x161B, "Intel Broadwell GT2"            }, //
  { 0x161D, "Intel Broadwell GT2"            }, //
  { 0x161E, "Intel HD Graphics 5300"         }, // Ultramobile - MacBook8,1
  //GT3
  { 0x1626, "Intel HD Graphics 6000"         }, // Mobile - iMac16,1/MacBookAir7,x
  { 0x162B, "Intel Iris Graphics 6100"       }, // Mobile - MacBookPro12,1
  { 0x162D, "Intel Iris Pro Graphics P6300"  }, // Workstation, Mobile Workstation
  //GT3e
  { 0x1622, "Intel Iris Pro Graphics 6200"   }, // Desktop, Mobile - iMac16,2
  { 0x162A, "Intel Iris Pro Graphics P6300"  }, // Workstation
  //RSVD
  { 0x162E, "Intel Broadwell RSVD"           }, // Reserved
  { 0x1632, "Intel Broadwell RSVD"           }, // Reserved
  { 0x1636, "Intel Broadwell RSVD"           }, // Reserved
  { 0x163A, "Intel Broadwell RSVD"           }, // Reserved
  { 0x163B, "Intel Broadwell RSVD"           }, // Reserved
  { 0x163D, "Intel Broadwell RSVD"           }, // Reserved
  { 0x163E, "Intel Broadwell RSVD"           }, // Reserved

  //------------Cherryview/Braswell-----------
  { 0x22B0, "Intel HD Graphics 400"          }, // Cherry Trail - Atom x5 series - Z83X0/Z8550
  { 0x22B1, "Intel HD Graphics 405"          }, // Cherry Trail - Atom x7 series - Z8750
  { 0x22B2, "Intel HD Graphics 400"          }, // Braswell - Cerelon QC/DC series - X3X60
  { 0x22B3, "Intel HD Graphics 405"          }, // Braswell - Pentium QC series - X3710


  //============== 9th generation ============
  //----------------Skylake-------------------
  //GT1
  { 0x1902, "Intel HD Graphics 510"          }, // Desktop
  { 0x1906, "Intel HD Graphics 510"          }, // Mobile
  { 0x190A, "Intel Skylake GT1"              }, //
  { 0x190B, "Intel HD Graphics 510"          }, //
  { 0x190E, "Intel Skylake GT1"              }, //
  //GT2
  { 0x1912, "Intel HD Graphics 530"          }, // Desktop - iMac17,1
  { 0x1916, "Intel HD Graphics 520"          }, // Mobile
  { 0x191A, "Intel Skylake GT2"              }, //
  { 0x191B, "Intel HD Graphics 530"          }, // Mobile - MacBookPro13,3
  { 0x191D, "Intel HD Graphics P530"         }, // Workstation, Mobile Workstation
  { 0x191E, "Intel HD Graphics 515"          }, // Mobile - MacBook9,1
  { 0x1921, "Intel HD Graphics 520"          }, // 
  //GT2f
  { 0x1913, "Intel Skylake GT2f"             }, //
  { 0x1915, "Intel Skylake GT2f"             }, //
  { 0x1917, "Intel Skylake GT2f"             }, //
  //GT3
  { 0x1923, "Intel HD Graphics 535"          }, //
  //GT3e
  { 0x1926, "Intel Iris Graphics 540"        }, // Mobile - MacBookPro13,1
  { 0x1927, "Intel Iris Graphics 550"        }, // Mobile - MacBookPro13,2
  { 0x192B, "Intel Iris Graphics 555"        }, //
  { 0x192D, "Intel Iris Graphics P555"       }, // Workstation
  //GT4
  { 0x192A, "Intel Skylake GT4"              }, //
  //GT4e
  { 0x1932, "Intel Iris Pro Graphics 580"    }, // Desktop
  { 0x193A, "Intel Iris Pro Graphics P580"   }, // Server
  { 0x193B, "Intel Iris Pro Graphics 580"    }, // Mobile
  { 0x193D, "Intel Iris Pro Graphics P580"   }, // Workstation, Mobile Workstation

  //----------------Goldmont------------------
  { 0x0A84, "Intel HD Graphics"              }, // Broxton(cancelled)
  { 0x1A84, "Intel HD Graphics"              }, // Broxton(cancelled)
  { 0x1A85, "Intel HD Graphics"              }, // Broxton(cancelled)
  { 0x5A84, "Intel HD Graphics 505"          }, // Apollo Lake
  { 0x5A85, "Intel HD Graphics 500"          }, // Apollo Lake


  //============== 9.5th generation ==========
  //----------------Kaby Lake-----------------
  //GT1
  { 0x5902, "Intel HD Graphics 610"          }, // Desktop
  { 0x5906, "Intel HD Graphics 610"          }, // Mobile
  { 0x590A, "Intel Kaby Lake GT1"            }, //
  { 0x5908, "Intel Kaby Lake GT1"            }, //
  { 0x590B, "Intel Kaby Lake GT1"            }, //
  { 0x590E, "Intel Kaby Lake GT1"            }, //
  //GT1.5
  { 0x5913, "Intel Kaby Lake GT1.5"          }, //
  { 0x5915, "Intel Kaby Lake GT1.5"          }, //
  //GT2
  { 0x5912, "Intel HD Graphics 630"          }, // Desktop - iMac18,2/iMac18,3
  { 0x5916, "Intel HD Graphics 620"          }, // Mobile
  { 0x591A, "Intel HD Graphics P630"         }, // Server
  { 0x591B, "Intel HD Graphics 630"          }, // Mobile - MacBookPro14,3
  { 0x591D, "Intel HD Graphics P630"         }, // Workstation, Mobile Workstation
  { 0x591E, "Intel HD Graphics 615"          }, // Mobile - MacBook10,1
  //GT2F
  { 0x5921, "Intel Kaby Lake GT2F"           }, //
  //GT3
  { 0x5923, "Intel HD Graphics 635"          }, //
  { 0x5926, "Intel Iris Plus Graphics 640"   }, // Mobile - MacBookPro14,1/iMac18,1
  { 0x5927, "Intel Iris Plus Graphics 650"   }, // Mobile - MacBookPro14,2
  //GT4
  { 0x593B, "Intel Kaby Lake GT4"            }, //

  //-------------Kaby Lake Refresh------------
  //GT1.5
  { 0x5917, "Intel UHD Graphics 620"         }, // Mobile

  //----------------Coffee Lake---------------
  //GT1
  { 0x3E90, "Intel UHD Graphics 610"         }, // Desktop
  { 0x3E93, "Intel UHD Graphics 610"         }, // Desktop
  //GT2
  { 0x3E91, "Intel UHD Graphics 630"         }, // Desktop
  { 0x3E92, "Intel UHD Graphics 630"         }, // Desktop
  { 0x3E94, "Intel Coffee Lake GT2"          }, //
  { 0x3E96, "Intel Coffee Lake GT2"          }, //
  { 0x3E9B, "Intel Coffee Lake GT2"          }, //
  //GT3
  { 0x3EA5, "Intel Coffee Lake GT3"          }, //
  { 0x3EA6, "Intel Coffee Lake GT3"          }, //
  { 0x3EA7, "Intel Coffee Lake GT3"          }, //
  { 0x3EA8, "Intel Coffee Lake GT3"          }, //

  //----------------Gemini Lake---------------
  { 0x3184, "Intel HD Graphics 605"          }, //
  { 0x3185, "Intel HD Graphics 600"          }, //


  //============== 10th generation ===========
  //----------------Cannonlake----------------
  //GT0.5
  { 0x5A49, "Intel Cannonlake GT0.5"         }, //
  { 0x5A4A, "Intel Cannonlake GT0.5"         }, //
  //GT1
  { 0x5A41, "Intel Cannonlake GT1"           }, //
  { 0x5A42, "Intel Cannonlake GT1"           }, //
  { 0x5A44, "Intel Cannonlake GT1"           }, //
  //GT1.5
  { 0x5A59, "Intel Cannonlake GT1.5"         }, //
  { 0x5A5A, "Intel Cannonlake GT1.5"         }, //
  { 0x5A5C, "Intel Cannonlake GT1.5"         }, //
  //GT2
  { 0x5A50, "Intel Cannonlake GT2"           }, //
  { 0x5A51, "Intel Cannonlake GT2"           }, //
  { 0x5A52, "Intel Cannonlake GT2"           }, //
  { 0x5A54, "Intel Cannonlake GT2"           }, //

};


CHAR8 *get_gma_model(UINT16 id)
{
  INT32 i;

  for (i = 0; i < (sizeof(KnownGPUS) / sizeof(KnownGPUS[0])); i++)
  {
    if (KnownGPUS[i].device == id)
      return KnownGPUS[i].name;
  }
  return KnownGPUS[0].name;
}


BOOLEAN setup_gma_devprop(pci_dt_t *gma_dev)
{
  UINTN           j;
  UINTN           i;
  CHAR8           *devicepath;
  CHAR8           *model;
  DevPropDevice   *device = NULL;
  UINT8           BuiltIn = 0x00;
  UINT32          FakeID;
  UINT32          DualLink = 1;
  BOOLEAN         SetUGAWidth = FALSE;
  BOOLEAN         SetUGAHeight = FALSE;
  BOOLEAN         Injected = FALSE;
  BOOLEAN         SetFake = FALSE;
  BOOLEAN         SetSnb = FALSE;
  BOOLEAN         SetIg = FALSE;

  MACHINE_TYPES   MacModel = GetModelFromString(gSettings.ProductName);

  devicepath = get_pci_dev_path(gma_dev);

  model = get_gma_model(gma_dev->device_id);

  for (j = 0; j < NGFX; j++) {
    if ((gGraphics[j].Vendor == Intel) &&
        (gGraphics[j].DeviceID == gma_dev->device_id)) {
      model = gGraphics[j].Model;
      break;
    }
  }
  //DBG("Finally model=%a\n", model);

  DBG("%a [%04x:%04x] :: %a\n",
      model, gma_dev->vendor_id, gma_dev->device_id, devicepath);

  // Resolution
  switch (UGAWidth) {
    case 160:
      SetUGAWidth = TRUE;
      if(UGAHeight == 120) {
        SetUGAHeight = TRUE;
        DBG ("  Found quarter quarter VGA Display - 4:3 :: Width=%d Height=%d\n", UGAWidth, UGAHeight);
      } else {
        DBG ("  Found Unknown Resolution Display - ?:? :: Width=%d Height=%d\n", UGAWidth, UGAHeight);
      }
      break;
    case 240:
      SetUGAWidth = TRUE;
      if(UGAHeight == 160) {
        SetUGAHeight = TRUE;
        DBG ("  Found Half quarter VGA Display - 3:2 :: Width=%d Height=%d\n", UGAWidth, UGAHeight);
      } else {
        DBG ("  Found Unknown Resolution Display - ?:? :: Width=%d Height=%d\n", UGAWidth, UGAHeight);
      }
      break;
    case 320:
      SetUGAWidth = TRUE;
      if(UGAHeight == 240) {
        SetUGAHeight = TRUE;
        DBG ("  Found quarter VGA Display - 4:3 :: Width=%d Height=%d\n", UGAWidth, UGAHeight);
      } else {
        DBG ("  Found Unknown Resolution Display - ?:? :: Width=%d Height=%d\n", UGAWidth, UGAHeight);
      }
      break;
    case 400:
      SetUGAWidth = TRUE;
      if(UGAHeight == 240) {
        SetUGAHeight = TRUE;
        DBG ("  Found Wide quarter VGA Display - 5:3 :: Width=%d Height=%d\n", UGAWidth, UGAHeight);
      } else {
        DBG ("  Found Unknown Resolution Display - ?:? :: Width=%d Height=%d\n", UGAWidth, UGAHeight);
      }
      break;
    case 480:
      SetUGAWidth = TRUE;
      if(UGAHeight == 320) {
        SetUGAHeight = TRUE;
        DBG ("  Found Half-size VGA Display - 3:2 :: Width=%d Height=%d\n", UGAWidth, UGAHeight);
      } else {
        DBG ("  Found Unknown Resolution Display - ?:? :: Width=%d Height=%d\n", UGAWidth, UGAHeight);
      }
      break;
    case 640:
      SetUGAWidth = TRUE;
      switch (UGAHeight) {
        case 360:
          SetUGAHeight = TRUE;
          DBG ("  Found one ninth of a Full HD Display - 16:9 :: Width=%d Height=%d\n", UGAWidth, UGAHeight);
          break;
        case 480:
          SetUGAHeight = TRUE;
          DBG ("  Found VGA Display - 4:3 :: Width=%d Height=%d\n", UGAWidth, UGAHeight);
          break;
        default:
          DBG ("  Found Unknown Resolution Display - ?:? :: Width=%d Height=%d\n", UGAWidth, UGAHeight);
          break;
      }
      break;
    case 800:
      SetUGAWidth = TRUE;
      switch (UGAHeight) {
        case 480:
          SetUGAHeight = TRUE;
          DBG ("  Found Wide VGA Display - 5:3 :: Width=%d Height=%d\n", UGAWidth, UGAHeight);
          break;
        case 600:
          SetUGAHeight = TRUE;
          DBG ("  Found Super VGA Display - 4:3 :: Width=%d Height=%d\n", UGAWidth, UGAHeight);
          break;
        default:
          DBG ("  Found Unknown Resolution Display - ?:? :: Width=%d Height=%d\n", UGAWidth, UGAHeight);
          break;
      }
      break;
    case 854:
      SetUGAWidth = TRUE;
      if(UGAHeight == 480) {
        SetUGAHeight = TRUE;
        DBG ("  Found Full Wide VGA Display - 16:9 :: Width=%d Height=%d\n", UGAWidth, UGAHeight);
      } else {
        DBG ("  Found Unknown Resolution Display - ?:? :: Width=%d Height=%d\n", UGAWidth, UGAHeight);
      }
      break;
    case 960:
      SetUGAWidth = TRUE;
      switch (UGAHeight) {
        case 540:
          SetUGAHeight = TRUE;
          DBG ("  Found one quarter of Full HD Display - 16:9 :: Width=%d Height=%d\n", UGAWidth, UGAHeight);
          break;
        case 640:
          SetUGAHeight = TRUE;
          DBG ("  Found Double-size VGA Display - 3:2 :: Width=%d Height=%d\n", UGAWidth, UGAHeight);
          break;
        default:
          DBG ("  Found Unknown Resolution Display - ?:? :: Width=%d Height=%d\n", UGAWidth, UGAHeight);
          break;
      }
      break;
    case 1024:
      SetUGAWidth = TRUE;
      switch (UGAHeight) {
        case 576:
          SetUGAHeight = TRUE;
          DBG ("  Found Wide Super VGA Display - 16:9 :: Width=%d Height=%d\n", UGAWidth, UGAHeight);
          break;
        case 600:
          SetUGAHeight = TRUE;
          DBG ("  Found Wide Super VGA Display - 17:10 :: Width=%d Height=%d\n", UGAWidth, UGAHeight);
          break;
        case 768:
          SetUGAHeight = TRUE;
          DBG ("  Found XGA Display - 4:3 :: Width=%d Height=%d\n", UGAWidth, UGAHeight);
          break;
        default:
          DBG ("  Found Unknown Resolution Display - ?:? :: Width=%d Height=%d\n", UGAWidth, UGAHeight);
          break;
      }
      break;
    case 1152:
      SetUGAWidth = TRUE;
      if(UGAHeight == 864) {
        SetUGAHeight = TRUE;
        DBG ("  Found XGA Plus Display - 4:3 :: Width=%d Height=%d\n", UGAWidth, UGAHeight);
      } else {
        DBG ("  Found Unknown Resolution Display - ?:? :: Width=%d Height=%d\n", UGAWidth, UGAHeight);
      }
      break;
    case 1280:
      SetUGAWidth = TRUE;
      switch (UGAHeight) {
        case 720:
          SetUGAHeight = TRUE;
          DBG ("  Found HD Display - 16:9 :: Width=%d Height=%d\n", UGAWidth, UGAHeight);
          break;
        case 768:
          SetUGAHeight = TRUE;
          DBG ("  Found Wide XGA Display - 5:3 :: Width=%d Height=%d\n", UGAWidth, UGAHeight);
          break;
        case 800:
          SetUGAHeight = TRUE;
          DBG ("  Found Wide XGA Display - 16:10 :: Width=%d Height=%d\n", UGAWidth, UGAHeight);
          break;
        case 1024:
          SetUGAHeight = TRUE;
          DBG ("  Found Super XGA Display - 5:4 :: Width=%d Height=%d\n", UGAWidth, UGAHeight);
          break;
        default:
          DBG ("  Found Unknown Resolution Display - ?:? :: Width=%d Height=%d\n", UGAWidth, UGAHeight);
          break;
      }
      break;
    case 1366:
      SetUGAWidth = TRUE;
      if(UGAHeight == 768) {
        SetUGAHeight = TRUE;
        DBG ("  Found Full Wide XGA Display - 16:9 :: Width=%d Height=%d\n", UGAWidth, UGAHeight);
      } else {
        DBG ("  Found Unknown Resolution Display - ?:? :: Width=%d Height=%d\n", UGAWidth, UGAHeight);
      }
      break;
    case 1400:
      SetUGAWidth = TRUE;
      if(UGAHeight == 1050) {
        SetUGAHeight = TRUE;
        DBG ("  Found Super XGA Plus Display - 4:3 :: Width=%d Height=%d\n", UGAWidth, UGAHeight);
      } else {
        DBG ("  Found Unknown Resolution Display - ?:? :: Width=%d Height=%d\n", UGAWidth, UGAHeight);
      }
      break;
    case 1440:
      SetUGAWidth = TRUE;
      if(UGAHeight == 900) {
        SetUGAHeight = TRUE;
        DBG ("  Found Wide XGA Plus Display - 16:10 :: Width=%d Height=%d\n", UGAWidth, UGAHeight);
      } else {
        DBG ("  Found Unknown Resolution Display - ?:? :: Width=%d Height=%d\n", UGAWidth, UGAHeight);
      }
      break;
    case 1600:
      SetUGAWidth = TRUE;
      switch (UGAHeight) {
        case 900:
          SetUGAHeight = TRUE;
          DBG ("  Found HD Plus Display - 16:9 :: Width=%d Height=%d\n", UGAWidth, UGAHeight);
          break;
        case 1200:
          SetUGAHeight = TRUE;
          DBG ("  Found Ultra XGA Display - 4:3 :: Width=%d Height=%d\n", UGAWidth, UGAHeight);
          break;
        default:
          DBG ("  Found Unknown Resolution Display - ?:? :: Width=%d Height=%d\n", UGAWidth, UGAHeight);
          break;
      }
      break;
    case 1680:
      SetUGAWidth = TRUE;
      switch (UGAHeight) {
        case 1050:
          SetUGAHeight = TRUE;
          DBG ("  Found Widescreen Super XGA Plus Display - 16:10 :: Width=%d Height=%d\n", UGAWidth, UGAHeight);
          break;
        default:
          DBG ("  Found Unknown Resolution Display - ?:? :: Width=%d Height=%d\n", UGAWidth, UGAHeight);
          break;
      }
      break;
    case 1920:
      SetUGAWidth = TRUE;
      switch (UGAHeight) {
        case 1080:
          SetUGAHeight = TRUE;
          DBG ("  Found Full HD Display - 16:9 :: Width=%d Height=%d\n", UGAWidth, UGAHeight);
          break;
        case 1200:
          SetUGAHeight = TRUE;
          DBG ("  Found Wide Ultra XGA Display - 16:10 :: Width=%d Height=%d\n", UGAWidth, UGAHeight);
          break;
        default:
          DBG ("  Found Unknown Resolution Display - ?:? :: Width=%d Height=%d\n", UGAWidth, UGAHeight);
          break;
      }
      break;
    case 2048:
      SetUGAWidth = TRUE;
      switch (UGAHeight) {
        case 1152:
          SetUGAHeight = TRUE;
          DBG ("  Found Quad Wide XGA Display - 16:9 :: Width=%d Height=%d\n", UGAWidth, UGAHeight);
          break;
        case 1536:
          SetUGAHeight = TRUE;
          DBG ("  Found Quad XGA Display - 4:3 :: Width=%d Height=%d\n", UGAWidth, UGAHeight);
          break;
        default:
          DBG ("  Found Unknown Resolution Display - ?:? :: Width=%d Height=%d\n", UGAWidth, UGAHeight);
          break;
      }
      break;
    case 2560:
      SetUGAWidth = TRUE;
      switch (UGAHeight) {
        case 1440:
          SetUGAHeight = TRUE;
          DBG ("  Found Quad HD Display - 16:9 :: Width=%d Height=%d\n", UGAWidth, UGAHeight);
          break;
        case 1600:
          SetUGAHeight = TRUE;
          DBG ("  Found Wide Quad XGA Display - 16:10 :: Width=%d Height=%d\n", UGAWidth, UGAHeight);
          break;
        case 2048:
          SetUGAHeight = TRUE;
          DBG ("  Found Quad Wide XGA Display - 5:4 :: Width=%d Height=%d\n", UGAWidth, UGAHeight);
          break;
        default:
          DBG ("  Found Unknown Resolution Display - ?:? :: Width=%d Height=%d\n", UGAWidth, UGAHeight);
          break;
      }
      break;
    case 2880:
      SetUGAWidth = TRUE;
      if(UGAHeight == 1800) {
        SetUGAHeight = TRUE;
        DBG ("  Found Wide Quad XGA Display - 16:10 :: Width=%d Height=%d\n", UGAWidth, UGAHeight);
      } else {
        DBG ("  Found Unknown Resolution Display - ?:? :: Width=%d Height=%d\n", UGAWidth, UGAHeight);
      }
      break;
    case 3200:
      SetUGAWidth = TRUE;
      switch (UGAHeight) {
        case 1800:
          SetUGAHeight = TRUE;
          DBG ("  Found Quad HD Plus Display - 16:9 :: Width=%d Height=%d\n", UGAWidth, UGAHeight);
          break;
        case 2048:
          SetUGAHeight = TRUE;
          DBG ("  Found Wide Quad Super XGA Display - 25:16 :: Width=%d Height=%d\n", UGAWidth, UGAHeight);
          break;
        case 2400:
          SetUGAHeight = TRUE;
          DBG ("  Found Quad Ultra XGA Display - 4:3 :: Width=%d Height=%d\n", UGAWidth, UGAHeight);
          break;
        default:
          DBG ("  Found Unknown Resolution Display - ?:? :: Width=%d Height=%d\n", UGAWidth, UGAHeight);
          break;
      }
      break;
    case 3840:
      SetUGAWidth = TRUE;
      switch (UGAHeight) {
        case 2160:
          SetUGAHeight = TRUE;
          DBG ("  Found Ultra HD, 4K Display - 16:9 :: Width=%d Height=%d\n", UGAWidth, UGAHeight);
          break;
        case 2400:
          SetUGAHeight = TRUE;
          DBG ("  Found Wide Quad Ultra XGA Display - 16:10 :: Width=%d Height=%d\n", UGAWidth, UGAHeight);
          break;
        default:
          DBG ("  Found Unknown Resolution Display - ?:? :: Width=%d Height=%d\n", UGAWidth, UGAHeight);
          break;
      }
      break;
    case 4096:
      SetUGAWidth = TRUE;
      if(UGAHeight == 3072) {
        SetUGAHeight = TRUE;
        DBG ("  Found Hex XGA Display - 4:3 :: Width=%d Height=%d\n", UGAWidth, UGAHeight);
      } else {
        DBG ("  Found Unknown Resolution Display - ?:? :: Width=%d Height=%d\n", UGAWidth, UGAHeight);
      }
      break;
    case 5120:
      SetUGAWidth = TRUE;
      switch (UGAHeight) {
        case 2880:
          SetUGAHeight = TRUE;
          DBG ("  Found Ultra HD Plus Display - 16:9 :: Width=%d Height=%d\n", UGAWidth, UGAHeight);
          break;
        case 3200:
          SetUGAHeight = TRUE;
          DBG ("  Found Wide Hex XGA Display - 16:10 :: Width=%d Height=%d\n", UGAWidth, UGAHeight);
          break;
        case 4096:
          SetUGAHeight = TRUE;
          DBG ("  Found Hex Super XGA Display - 5:4 :: Width=%d Height=%d\n", UGAWidth, UGAHeight);
          break;
        default:
          DBG ("  Found Unknown Resolution Display - ?:? :: Width=%d Height=%d\n", UGAWidth, UGAHeight);
          break;
      }
      break;
    case 6400:
      SetUGAWidth = TRUE;
      switch (UGAHeight) {
        case 4096:
          SetUGAHeight = TRUE;
          DBG ("  Found Wide Hex Super XGA Display - 25:16 :: Width=%d Height=%d\n", UGAWidth, UGAHeight);
          break;
        case 4800:
          SetUGAHeight = TRUE;
          DBG ("  Found Hex Ultra XGA Display - 4:3 :: Width=%d Height=%d\n", UGAWidth, UGAHeight);
          break;
        default:
          DBG ("  Found Unknown Resolution Display - ?:? :: Width=%d Height=%d\n", UGAWidth, UGAHeight);
          break;
      }
      break;
    case 7680:
      SetUGAWidth = TRUE;
      switch (UGAHeight) {
        case 4320:
          SetUGAHeight = TRUE;
          DBG ("  Found Full Ultra HD Display - 16:9 :: Width=%d Height=%d\n", UGAWidth, UGAHeight);
          break;
        case 4800:
          SetUGAHeight = TRUE;
          DBG ("  Found Wide Hex Ultra XGA Display - 16:10 :: Width=%d Height=%d\n", UGAWidth, UGAHeight);
          break;
        default:
          DBG ("  Found Unknown Resolution Display - ?:? :: Width=%d Height=%d\n", UGAWidth, UGAHeight);
          break;
      }
      break;
    default:
      DBG ("  Found Unknown Resolution Display - ?:? :: Width=%d Height=%d\n", UGAWidth, UGAHeight);
      break;
  }

  if (!string) {
    string = devprop_create_string();
  }

  if (gma_dev && !gma_dev->used) {
    device = devprop_add_device_pci(string, gma_dev);
    gma_dev->used = TRUE;
  }

  if (!device) {
    return FALSE;
  }

  if (gSettings.NrAddProperties != 0xFFFE) {
    for (i = 0; i < gSettings.NrAddProperties; i++) {
      if (gSettings.AddProperties[i].Device != DEV_INTEL) {
        continue;
      }
      Injected = TRUE;
      devprop_add_value(device, gSettings.AddProperties[i].Key,
                        (UINT8*)gSettings.AddProperties[i].Value,
                        gSettings.AddProperties[i].ValueLen);
    }
  }

  if (Injected) {
    DBG("  Additional Intel GFX properties injected, continue\n");
  }

  if (gSettings.UseIntelHDMI) {
    devprop_add_value(device, "hda-gfx", (UINT8*)"onboard-1", 10);
    DBG("  IntelHDMI: used\n");
  }

  if (gSettings.InjectEDID && gSettings.CustomEDID) {
    switch (gma_dev->device_id) {
      case 0x2772: // "Intel GMA 950"                   // Desktop - Intel 82945G Express Chipset Family
      case 0x2776: // "Intel GMA 950"                   // Desktop - Intel 82945G Express Chipset Family
      case 0x27A2: // "Intel GMA 950"                   // Mobile - Intel 945GM Express Chipset Family - MacBook1,1/MacBook2,1
      case 0x27A6: // "Intel GMA 950"                   // Mobile - Intel 945GM Express Chipset Family
      case 0x27AE: // "Intel GMA 950"                   // Mobile - Intel 945GM Express Chipset Family
      case 0xA001: // "Intel GMA 3150"                  // Nettop - Intel NetTop Atom D410
      case 0xA002: // "Intel GMA 3150"                  // Nettop - Intel NetTop Atom D510
      case 0xA011: // "Intel GMA 3150"                  // Netbook - Intel NetBook Atom N4x0
      case 0xA012: // "Intel GMA 3150"                  // Netbook - Intel NetBook Atom N4x0
      case 0x2A02: // "Intel GMA X3100"                 // Mobile - Intel 965 Express Chipset Family - MacBook3,1/MacBook4,1/MacbookAir1,1
      case 0x2A03: // "Intel GMA X3100"                 // Mobile - Intel 965 Express Chipset Family
      case 0x2A12: // "Intel GMA X3100"                 // Mobile - Intel 965 Express Chipset Family
      case 0x2A13: // "Intel GMA X3100"                 // Mobile - Intel 965 Express Chipset Family
        devprop_add_value(device, "AAPL01,override-no-connect", gSettings.CustomEDID, 128);
        DBG("  AAPL01,override-no-connect: added\n");
        break;
      default:
        devprop_add_value(device, "AAPL00,override-no-connect", gSettings.CustomEDID, 128);
        DBG("  AAPL00,override-no-connect: added\n");
        break;
    }
  }

  // DualLink
  // Low resolution(1366x768-) - DualLink = 0, but no need it
  // High resolution(1400x1050+) - DualLink = 1
  // Default DualLink is auto-detection
  switch (gma_dev->device_id) {
    case 0x2772: // "Intel GMA 950"                   // Desktop - Intel 82945G Express Chipset Family
    case 0x2776: // "Intel GMA 950"                   // Desktop - Intel 82945G Express Chipset Family
    case 0x27A2: // "Intel GMA 950"                   // Mobile - Intel 945GM Express Chipset Family - MacBook1,1/MacBook2,1
    case 0x27A6: // "Intel GMA 950"                   // Mobile - Intel 945GM Express Chipset Family
    case 0x27AE: // "Intel GMA 950"                   // Mobile - Intel 945GM Express Chipset Family
    case 0xA001: // "Intel GMA 3150"                  // Nettop - Intel NetTop Atom D410
    case 0xA002: // "Intel GMA 3150"                  // Nettop - Intel NetTop Atom D510
    case 0xA011: // "Intel GMA 3150"                  // Netbook - Intel NetBook Atom N4x0
    case 0xA012: // "Intel GMA 3150"                  // Netbook - Intel NetBook Atom N4x0
    case 0x2A02: // "Intel GMA X3100"                 // Mobile - Intel 965 Express Chipset Family - MacBook3,1/MacBook4,1/MacbookAir1,1
    case 0x2A03: // "Intel GMA X3100"                 // Mobile - Intel 965 Express Chipset Family
    case 0x2A12: // "Intel GMA X3100"                 // Mobile - Intel 965 Express Chipset Family
    case 0x2A13: // "Intel GMA X3100"                 // Mobile - Intel 965 Express Chipset Family
      if ((gSettings.DualLink == 0) || (gSettings.DualLink == 1)) {
        if (gSettings.DualLink == 1) {
          DBG("  DualLink: set to 1\n");
          devprop_add_value(device, "AAPL01,DualLink", (UINT8*)&gSettings.DualLink, 1);
          DBG("  AAPL01,DualLink = 1\n");
        } else {
          DBG("  DualLink: set to 0\n");
          DBG("  AAPL01,DualLink: not used\n");
        }
      } else {
        DBG("  Beginning DualLink auto-detection\n");
        if (SetUGAWidth && SetUGAHeight) {
          if (UGAWidth < 1400) {
            DBG("  Low Resolution Display\n");
            DBG("  AAPL01,DualLink: not used\n");
          } else {
            DBG("  High Resolution Display\n");
            devprop_add_value(device, "AAPL01,DualLink", (UINT8*)&DualLink, 1);
            DBG("  AAPL01,DualLink = 1\n");
          }
        } else {
          DBG("  Unknown Resolution Display\n");
          devprop_add_value(device, "AAPL01,DualLink", (UINT8*)&DualLink, 1);
          DBG("  AAPL01,DualLink = 1\n");
        }
      }
      break;
    default:
      if ((gSettings.DualLink == 0) || (gSettings.DualLink == 1)) {
        if (gSettings.DualLink == 1) {
          DBG("  DualLink: set to 1\n");
          devprop_add_value(device, "AAPL00,DualLink", (UINT8*)&gSettings.DualLink, 1);
          DBG("  AAPL00,DualLink = 1\n");
        } else {
          DBG("  DualLink: set to 0\n");
          DBG("  AAPL00,DualLink: not used\n");
        }
      } else {
        DBG("  Beginning DualLink auto-detection\n");
        if (SetUGAWidth && SetUGAHeight) {
          if (UGAWidth < 1400) {
            DBG("  Low Resolution Display\n");
            DBG("  AAPL00,DualLink: not used\n");
          } else {
            DBG("  High Resolution Display\n");
            devprop_add_value(device, "AAPL00,DualLink", (UINT8*)&DualLink, 1);
            DBG("  AAPL00,DualLink = 1\n");
          }
        } else {
          DBG("  Unknown Resolution Display\n");
          devprop_add_value(device, "AAPL00,DualLink", (UINT8*)&DualLink, 1);
          DBG("  AAPL00,DualLink = 1\n");
        }
      }
      break;
  }

  if (gSettings.FakeIntel) {
    FakeID = gSettings.FakeIntel >> 16;
    devprop_add_value(device, "device-id", (UINT8*)&FakeID, 4);
    FakeID = gSettings.FakeIntel & 0xFFFF;
    devprop_add_value(device, "vendor-id", (UINT8*)&FakeID, 4);
    SetFake = TRUE;
    DBG("  FakeID Intel GFX = 0x%08lx\n", gSettings.FakeIntel);
  } else {
    DBG("  FakeID Intel GFX: not set\n");
  }

  // platform-id
  switch (gma_dev->device_id) {
    case 0x0102: // "Intel HD Graphics 2000"          // Desktop - iMac12,x
    case 0x0106: // "Intel HD Graphics 2000"          // Mobile
    case 0x010A: // "Intel HD Graphics P3000"         // Server
    case 0x0112: // "Intel HD Graphics 3000"          // Desktop
    case 0x0116: // "Intel HD Graphics 3000"          // Mobile - MacBookAir4,x/MacBookPro8,2/MacBookPro8,3
    case 0x0122: // "Intel HD Graphics 3000"          // Desktop
    case 0x0126: // "Intel HD Graphics 3000"          // Mobile - MacBookPro8,1/Macmini5,x
      if (gSettings.IgPlatform != 0) {
        devprop_add_value(device, "AAPL,snb-platform-id", (UINT8*)&gSettings.IgPlatform, 4);
        DBG("  snb-platform-id = 0x%08lx\n", gSettings.IgPlatform);
        SetSnb = TRUE;
      } else {
        DBG("  snb-platform-id: not set\n");
      }
      break;
    default:
      if (gSettings.IgPlatform != 0) {
        devprop_add_value(device, "AAPL,ig-platform-id", (UINT8*)&gSettings.IgPlatform, 4);
        DBG("  ig-platform-id = 0x%08lx\n", gSettings.IgPlatform);
        SetIg = TRUE;
      } else {
        DBG("  ig-platform-id: not set\n");
      }
      break;
  }

  if (gSettings.NoDefaultProperties) {
    DBG("  Intel: no default properties\n");
    return TRUE;
  }

  devprop_add_value(device, "model", (UINT8*)model, (UINT32)AsciiStrLen(model));
  devprop_add_value(device, "device_type", (UINT8*)"display", 7);
  devprop_add_value(device, "subsystem-vendor-id", common_vals[2], 4);
  devprop_add_value(device, "class-code", (UINT8*)ClassFix, 4);
  devprop_add_value(device, "AAPL,backlight-control", (UINT8*)&IntelDisplay, 4);
  devprop_add_value(device, "AAPL,HasLid", (UINT8*)&IntelDisplay, 4);
  devprop_add_value(device, "AAPL,HasPanel", (UINT8*)&IntelDisplay, 4);
  devprop_add_value(device, "@0,backlight-control", (UINT8*)&IntelDisplay, 4);


  // Clover will automatically detect these values if there is no ig-platform-id or FakeID Intel GFX value.
  // If there are Intel GFX values in ACPI injection, their values will be overwritten on the values of Intel GFX auto-detection.

  if ((SetSnb && SetFake) || (SetIg && SetFake)) {
    //DBG("  Beginning ACPI injection\n");
  } else {
    DBG("  Beginning Intel GFX auto-detection with ACPI injection\n");
  }

  switch (gma_dev->device_id) {

      //============== PowerVR ===================
      //--------Canmore/Sodaville/Groveland-------
    case 0x2E5B: // "Intel 500"                       //
      break;

      //----------------Poulsbo-------------------
    case 0x8108: // "Intel 500"                       // Menlow
    case 0x8109: // "Intel 500"                       // Menlow
      break;

      //----------------Lincroft------------------
    case 0x4102: // "Intel 600"                       // Moorestown
      break;

      //----------------Cedarview-----------------
    case 0x0BE0: // "Intel GMA 3600"                  // Cedar Trail
    case 0x0BE1: // "Intel GMA 3600"                  // Cedar Trail
    case 0x0BE2: // "Intel GMA 3650"                  // Cedar Trail
    case 0x0BE3: // "Intel GMA 3650"                  // Cedar Trail
      break;

      //----------------Cloverview----------------
    case 0x08C7: // "Intel GMA"                       // Clover Trail
    case 0x08C8: // "Intel GMA"                       // Clover Trail
    case 0x08C9: // "Intel GMA"                       // Clover Trail
    case 0x08CA: // "Intel GMA"                       // Clover Trail
    case 0x08CB: // "Intel GMA"                       // Clover Trail
    case 0x08CC: // "Intel GMA"                       // Clover Trail
    case 0x08CD: // "Intel GMA"                       // Clover Trail
    case 0x08CE: // "Intel GMA"                       // Clover Trail
    case 0x08CF: // "Intel GMA"                       // Clover Trail
      break;


      //============== 1st generation ============
      //----------------Auburn--------------------
    case 0x7800: // "Intel 740"                       // Desktop - Intel 740 GMCH Express Chipset Family
      break;

      //----------------Portola-------------------
    case 0x1240: // "Intel 752"                       // Desktop - Intel 752 GMCH Express Chipset Family
      break;

      //----------------Whitney-------------------
    case 0x7121: // "Intel 3D graphics 810"           // Desktop - Intel 810 GMCH Express Chipset Family
    case 0x7123: // "Intel 3D graphics 810"           // Desktop - Intel 810-DC100 GMCH Express Chipset Family
    case 0x7125: // "Intel 3D graphics 810"           // Desktop - Intel 810E GMCH Express Chipset Family
      break;

      //----------------Solano--------------------
    case 0x1132: // "Intel 3D graphics 815"           // Desktop - Intel 815 GMCH Express Chipset Family
      break;


      //============== 2nd generation ============
      //----------------Almador-------------------
    case 0x3577: // "Intel Extreme Graphics 830"      // Mobile - Intel 830M GMCH Express Chipset Family
    case 0x357B: // "Intel Extreme Graphics 835"      // Desktop - Intel 835G GMCH Express Chipset Family
      break;

      //----------------Brookdale-----------------
    case 0x2562: // "Intel Extreme Graphics 845"      // Desktop - Intel 845G GMCH Express Chipset Family
      break;

      //----------------Montara-------------------
    case 0x358E: // "Intel Extreme Graphics 2 854"    // Mobile - Intel 852GM/855GM GMCH Express Chipset Family
    case 0x3582: // "Intel Extreme Graphics 2 855"    // Mobile - Intel 852GM/855GM GMCH Express Chipset Family
      break;

      //----------------Springdale----------------
    case 0x2572: // "Intel Extreme Graphics 2 865"    // Desktop - Intel 865G Express Chipset Family
      break;


      //============== 3rd generation ============
      //----------------Grantsdale----------------
    case 0x2582: // "Intel GMA 900"                   // Desktop - Intel 915G Express Chipset Family
    case 0x258A: // "Intel GMA 900"                   // Desktop - Intel 915GM Express Chipset Family
    case 0x2782: // "Intel GMA 900"                   // Desktop - Intel 915GV Express Chipset Family
      break;

      //----------------Alviso--------------------
    case 0x2592: // "Intel GMA 900"                   // Mobile - Intel 82915GM/GMS, 910GML Express Chipset Family
    case 0x2792: // "Intel GMA 900"                   // Mobile - Intel 82915GM/GMS, 910GML Express Chipset Family
      break;

      //----------------Lakeport------------------
    case 0x2772: // "Intel GMA 950"                   // Desktop - Intel 82945G Express Chipset Family
    case 0x2776: // "Intel GMA 950"                   // Desktop - Intel 82945G Express Chipset Family
      //break;

      //----------------Calistoga-----------------
    case 0x27A2: // "Intel GMA 950"                   // Mobile - Intel 945GM Express Chipset Family - MacBook1,1/MacBook2,1
    case 0x27A6: // "Intel GMA 950"                   // Mobile - Intel 945GM Express Chipset Family
    case 0x27AE: // "Intel GMA 950"                   // Mobile - Intel 945GM Express Chipset Family
      if (!SetFake) {
        FakeID = 0x27A28086 >> 16;
        DBG("  Found FakeID Intel GFX = 0x%04lx8086\n", FakeID);
        devprop_add_value(device, "device-id", (UINT8*)&FakeID, 4);
        FakeID = 0x27A28086 & 0xFFFF;
        devprop_add_value(device, "vendor-id", (UINT8*)&FakeID, 4);
      }
      devprop_add_value(device, "built-in", &BuiltIn, 1);
      //devprop_add_value(device, "AAPL,HasLid", calistoga_gma_vals[0], 4);
      devprop_add_value(device, "AAPL,HasPanel", calistoga_gma_vals[1], 4);
      //devprop_add_value(device, "AAPL,NumDisplays", calistoga_gma_vals[2], 4);
      //devprop_add_value(device, "AAPL,NumFramebuffers", calistoga_gma_vals[3], 4);
      devprop_add_value(device, "AAPL01,BacklightIntensity", calistoga_gma_vals[4], 4);
      devprop_add_value(device, "AAPL01,BootDisplay", calistoga_gma_vals[5], 4);
      //devprop_add_value(device, "AAPL01,CurrentDisplay", calistoga_gma_vals[6], 4);
      devprop_add_value(device, "AAPL01,DataJustify", calistoga_gma_vals[7], 4);
      //devprop_add_value(device, "AAPL01,Depth", calistoga_gma_vals[8], 4);
      devprop_add_value(device, "AAPL01,Dither", calistoga_gma_vals[9], 4);
      //devprop_add_value(device, "AAPL01,Height", calistoga_gma_vals[10], 4);
      devprop_add_value(device, "AAPL01,Interlace", calistoga_gma_vals[11], 4);
      devprop_add_value(device, "AAPL01,Inverter", calistoga_gma_vals[12], 4);
      devprop_add_value(device, "AAPL01,InverterCurrent", calistoga_gma_vals[13], 4);
      //devprop_add_value(device, "AAPL01,InverterFrequency", calistoga_gma_vals[14], 4);
      //devprop_add_value(device, "AAPL01,IODisplayMode", calistoga_gma_vals[15], 4);
      devprop_add_value(device, "AAPL01,LinkFormat", calistoga_gma_vals[16], 4);
      devprop_add_value(device, "AAPL01,LinkType", calistoga_gma_vals[17], 4);
      devprop_add_value(device, "AAPL01,Pipe", calistoga_gma_vals[18], 4);
      //devprop_add_value(device, "AAPL01,PixelFormat", calistoga_gma_vals[19], 4);
      devprop_add_value(device, "AAPL01,Refresh", calistoga_gma_vals[20], 4);
      devprop_add_value(device, "AAPL01,Stretch", calistoga_gma_vals[21], 4);
      devprop_add_value(device, "AAPL01,T1", calistoga_gma_vals[22], 4);
      devprop_add_value(device, "AAPL01,T2", calistoga_gma_vals[23], 4);
      devprop_add_value(device, "AAPL01,T3", calistoga_gma_vals[24], 4);
      devprop_add_value(device, "AAPL01,T4", calistoga_gma_vals[25], 4);
      devprop_add_value(device, "AAPL01,T5", calistoga_gma_vals[26], 4);
      devprop_add_value(device, "AAPL01,T6", calistoga_gma_vals[27], 4);
      devprop_add_value(device, "AAPL01,T7", calistoga_gma_vals[28], 4);
      //devprop_add_value(device, "AAPL01,Width", calistoga_gma_vals[29], 4);
      break;

      //----------------Bearlake------------------
    case 0x29B2: // "Intel GMA 3100"                  // Desktop - Intel Q35 Express Chipset Family
    case 0x29B3: // "Intel GMA 3100"                  // Desktop - Intel Q35 Express Chipset Family
    case 0x29C2: // "Intel GMA 3100"                  // Desktop - Intel G33/G31 Express Chipset Family
    case 0x29C3: // "Intel GMA 3100"                  // Desktop - Intel G33/G31 Express Chipset Family
    case 0x29D2: // "Intel GMA 3100"                  // Desktop - Intel Q33 Express Chipset Family
    case 0x29D3: // "Intel GMA 3100"                  // Desktop - Intel Q33 Express Chipset Family
      break;

      //----------------Pineview------------------
    case 0xA001: // "Intel GMA 3150"                  // Nettop - Intel NetTop Atom D410
    case 0xA002: // "Intel GMA 3150"                  // Nettop - Intel NetTop Atom D510
    case 0xA011: // "Intel GMA 3150"                  // Netbook - Intel NetBook Atom N4x0
    case 0xA012: // "Intel GMA 3150"                  // Netbook - Intel NetBook Atom N4x0
      if (!SetFake) {
        FakeID = 0x27A28086 >> 16;
        DBG("  Found FakeID Intel GFX = 0x%04lx8086\n", FakeID);
        devprop_add_value(device, "device-id", (UINT8*)&FakeID, 4);
        FakeID = 0x27A28086 & 0xFFFF;
        devprop_add_value(device, "vendor-id", (UINT8*)&FakeID, 4);
        // (c) Chameleon team, meklort - GMA3150 patch for AppleIntelIntegratedFramebuffer.kext of the GMA950, no QE yet.
        // Cursor corruption fix. getPhysicalAddress() and more. jump past getPhysicalAddress binding. NOTE: last six bytes are unusable
        // Bundle name: com.apple.driver.AppleIntelIntegratedFramebuffer
        // Find: 8b550883bab0000000017e36890424e832bbffff
        // Replace: b800000002909090909090909090eb0400000000
      }
      devprop_add_value(device, "built-in", &BuiltIn, 1);
      //devprop_add_value(device, "AAPL,HasLid", calistoga_gma_vals[0], 4);
      devprop_add_value(device, "AAPL,HasPanel", calistoga_gma_vals[1], 4);
      //devprop_add_value(device, "AAPL,NumDisplays", calistoga_gma_vals[2], 4);
      //devprop_add_value(device, "AAPL,NumFramebuffers", calistoga_gma_vals[3], 4);
      devprop_add_value(device, "AAPL01,BacklightIntensity", calistoga_gma_vals[4], 4);
      devprop_add_value(device, "AAPL01,BootDisplay", calistoga_gma_vals[5], 4);
      //devprop_add_value(device, "AAPL01,CurrentDisplay", calistoga_gma_vals[6], 4);
      devprop_add_value(device, "AAPL01,DataJustify", calistoga_gma_vals[7], 4);
      //devprop_add_value(device, "AAPL01,Depth", calistoga_gma_vals[8], 4);
      devprop_add_value(device, "AAPL01,Dither", calistoga_gma_vals[9], 4);
      //devprop_add_value(device, "AAPL01,Height", calistoga_gma_vals[10], 4);
      devprop_add_value(device, "AAPL01,Interlace", calistoga_gma_vals[11], 4);
      devprop_add_value(device, "AAPL01,Inverter", calistoga_gma_vals[12], 4);
      devprop_add_value(device, "AAPL01,InverterCurrent", calistoga_gma_vals[13], 4);
      //devprop_add_value(device, "AAPL01,InverterFrequency", calistoga_gma_vals[14], 4);
      //devprop_add_value(device, "AAPL01,IODisplayMode", calistoga_gma_vals[15], 4);
      devprop_add_value(device, "AAPL01,LinkFormat", calistoga_gma_vals[16], 4);
      devprop_add_value(device, "AAPL01,LinkType", calistoga_gma_vals[17], 4);
      devprop_add_value(device, "AAPL01,Pipe", calistoga_gma_vals[18], 4);
      //devprop_add_value(device, "AAPL01,PixelFormat", calistoga_gma_vals[19], 4);
      devprop_add_value(device, "AAPL01,Refresh", calistoga_gma_vals[20], 4);
      devprop_add_value(device, "AAPL01,Stretch", calistoga_gma_vals[21], 4);
      devprop_add_value(device, "AAPL01,T1", calistoga_gma_vals[22], 4);
      devprop_add_value(device, "AAPL01,T2", calistoga_gma_vals[23], 4);
      devprop_add_value(device, "AAPL01,T3", calistoga_gma_vals[24], 4);
      devprop_add_value(device, "AAPL01,T4", calistoga_gma_vals[25], 4);
      devprop_add_value(device, "AAPL01,T5", calistoga_gma_vals[26], 4);
      devprop_add_value(device, "AAPL01,T6", calistoga_gma_vals[27], 4);
      devprop_add_value(device, "AAPL01,T7", calistoga_gma_vals[28], 4);
      //devprop_add_value(device, "AAPL01,Width", calistoga_gma_vals[29], 4);
      break;


      //============== 4th generation ============
      //----------------Lakeport------------------
    case 0x2972: // "Intel GMA 3000"                  // Desktop - Intel 946GZ Express Chipset Family
    case 0x2973: // "Intel GMA 3000"                  // Desktop - Intel 946GZ Express Chipset Family
      break;

      //----------------Broadwater----------------
    case 0x2992: // "Intel GMA 3000"                  // Desktop - Intel Q965/Q963 Express Chipset Family
    case 0x2993: // "Intel GMA 3000"                  // Desktop - Intel Q965/Q963 Express Chipset Family
    case 0x29A2: // "Intel GMA X3000"                 // Desktop - Intel G965 Express Chipset Family
    case 0x29A3: // "Intel GMA X3000"                 // Desktop - Intel G965 Express Chipset Family
      break;

      //----------------Crestline-----------------
    case 0x2A02: // "Intel GMA X3100"                 // Mobile - Intel 965 Express Chipset Family - MacBook3,1/MacBook4,1/MacbookAir1,1
    case 0x2A03: // "Intel GMA X3100"                 // Mobile - Intel 965 Express Chipset Family
    case 0x2A12: // "Intel GMA X3100"                 // Mobile - Intel 965 Express Chipset Family
    case 0x2A13: // "Intel GMA X3100"                 // Mobile - Intel 965 Express Chipset Family
      if (!SetFake) {
        FakeID = 0x2A028086 >> 16;
        DBG("  Found FakeID Intel GFX = 0x%04lx8086\n", FakeID);
        devprop_add_value(device, "device-id", (UINT8*)&FakeID, 4);
        FakeID = 0x2A028086 & 0xFFFF;
        devprop_add_value(device, "vendor-id", (UINT8*)&FakeID, 4);
      }
      devprop_add_value(device, "built-in", &BuiltIn, 1);
      //devprop_add_value(device, "AAPL,HasLid", crestline_gma_vals[0], 4);
      devprop_add_value(device, "AAPL,HasPanel", crestline_gma_vals[1], 4);
      //devprop_add_value(device, "AAPL,NumDisplays", crestline_gma_vals[2], 4);
      //devprop_add_value(device, "AAPL,NumFramebuffers", crestline_gma_vals[3], 4);
      devprop_add_value(device, "AAPL,SelfRefreshSupported", crestline_gma_vals[4], 4);
      devprop_add_value(device, "AAPL,aux-power-connected", crestline_gma_vals[5], 4);
      devprop_add_value(device, "AAPL,backlight-control", crestline_gma_vals[6], 4);
      devprop_add_value(device, "AAPL00,blackscreen-preferences", crestline_gma_vals[7], 4);
      devprop_add_value(device, "AAPL01,BootDisplay", crestline_gma_vals[8], 4);
      devprop_add_value(device, "AAPL01,BacklightIntensity", crestline_gma_vals[9], 4);
      devprop_add_value(device, "AAPL01,blackscreen-preferences", crestline_gma_vals[10], 4);
      //devprop_add_value(device, "AAPL01,CurrentDisplay", crestline_gma_vals[11], 4);
      devprop_add_value(device, "AAPL01,DataJustify", crestline_gma_vals[12], 4);
      //devprop_add_value(device, "AAPL01,Depth", crestline_gma_vals[13], 4);
      devprop_add_value(device, "AAPL01,Dither", crestline_gma_vals[14], 4);
      //devprop_add_value(device, "AAPL01,Height", crestline_gma_vals[15], 4);
      devprop_add_value(device, "AAPL01,Interlace", crestline_gma_vals[16], 4);
      devprop_add_value(device, "AAPL01,Inverter", crestline_gma_vals[17], 4);
      devprop_add_value(device, "AAPL01,InverterCurrent", crestline_gma_vals[18], 4);
      //devprop_add_value(device, "AAPL01,InverterFrequency", crestline_gma_vals[19], 4);
      devprop_add_value(device, "AAPL01,LinkFormat", crestline_gma_vals[20], 4);
      devprop_add_value(device, "AAPL01,LinkType", crestline_gma_vals[21], 4);
      devprop_add_value(device, "AAPL01,Pipe", crestline_gma_vals[22], 4);
      //devprop_add_value(device, "AAPL01,PixelFormat", crestline_gma_vals[23], 4);
      devprop_add_value(device, "AAPL01,Refresh", crestline_gma_vals[24], 4);
      devprop_add_value(device, "AAPL01,Stretch", crestline_gma_vals[25], 4);
      devprop_add_value(device, "AAPL01,T1", crestline_gma_vals[26], 4);
      devprop_add_value(device, "AAPL01,T2", crestline_gma_vals[27], 4);
      devprop_add_value(device, "AAPL01,T3", crestline_gma_vals[28], 4);
      devprop_add_value(device, "AAPL01,T4", crestline_gma_vals[29], 4);
      devprop_add_value(device, "AAPL01,T5", crestline_gma_vals[30], 4);
      devprop_add_value(device, "AAPL01,T6", crestline_gma_vals[31], 4);
      devprop_add_value(device, "AAPL01,T7", crestline_gma_vals[32], 4);
      //devprop_add_value(device, "AAPL01,Width", crestline_gma_vals[33], 4);
      break;

      //----------------Bearlake------------------
    case 0x2982: // "Intel GMA X3500"                 // Desktop - Intel G35 Express Chipset Family
    case 0x2983: // "Intel GMA X3500"                 // Desktop - Intel G35 Express Chipset Family
      break;

      //----------------Eaglelake-----------------
    case 0x2E02: // "Intel GMA 4500"                  // Desktop - Intel 4 Series Express Chipset Family
    case 0x2E03: // "Intel GMA 4500"                  // Desktop - Intel 4 Series Express Chipset Family
    case 0x2E12: // "Intel GMA 4500"                  // Desktop - Intel G45/G43 Express Chipset Family
    case 0x2E13: // "Intel GMA 4500"                  // Desktop - Intel G45/G43 Express Chipset Family
    case 0x2E42: // "Intel GMA 4500"                  // Desktop - Intel B43 Express Chipset Family
    case 0x2E43: // "Intel GMA 4500"                  // Desktop - Intel B43 Express Chipset Family
    case 0x2E92: // "Intel GMA 4500"                  // Desktop - Intel B43 Express Chipset Family
    case 0x2E93: // "Intel GMA 4500"                  // Desktop - Intel B43 Express Chipset Family
    case 0x2E32: // "Intel GMA X4500"                 // Desktop - Intel G45/G43 Express Chipset Family
    case 0x2E33: // "Intel GMA X4500"                 // Desktop - Intel G45/G43 Express Chipset Family
    case 0x2E22: // "Intel GMA X4500"                 // Mobile - Intel G45/G43 Express Chipset Family
    case 0x2E23: // "Intel GMA X4500HD"               // Mobile - Intel G45/G43 Express Chipset Family
      break;

      //----------------Cantiga-------------------
    case 0x2A42: // "Intel GMA X4500MHD"              // Mobile - Intel 4 Series Express Chipset Family
    case 0x2A43: // "Intel GMA X4500MHD"              // Mobile - Intel 4 Series Express Chipset Family
      break;


      //============== 5th generation ============
      //----------------Ironlake------------------
    case 0x0042: // "Intel HD Graphics"               // Desktop - Clarkdale
    case 0x0046: // "Intel HD Graphics"               // Mobile - Arrandale - MacBookPro6,x
      if (!SetFake) {
        switch (gma_dev->device_id) {
          case 0x0042:
            FakeID = 0x00428086 >> 16;
            DBG("  Found FakeID Intel GFX = 0x%04lx8086\n", FakeID);
            devprop_add_value(device, "device-id", (UINT8*)&FakeID, 4);
            FakeID = 0x00428086 & 0xFFFF;
            devprop_add_value(device, "vendor-id", (UINT8*)&FakeID, 4);
            break;
          case 0x0046:
            FakeID = 0x00468086 >> 16;
            DBG("  Found FakeID Intel GFX = 0x%04lx8086\n", FakeID);
            devprop_add_value(device, "device-id", (UINT8*)&FakeID, 4);
            FakeID = 0x00468086 & 0xFFFF;
            devprop_add_value(device, "vendor-id", (UINT8*)&FakeID, 4);
            break;
          default:
            break;
        }
      }
      switch (MacModel) {
        case MacBookPro61:
        case MacBookPro62:
          devprop_add_value(device, "AAPL,os-info", mbp_HD_os_info, 20);
          devprop_add_value(device, "AAPL,aux-power-connected", ironlake_hd_vals[0], 4);
          devprop_add_value(device, "AAPL,backlight-control", ironlake_hd_vals[1], 4);
          devprop_add_value(device, "AAPL00,T1", ironlake_hd_vals[2], 4);
          devprop_add_value(device, "AAPL00,T2", ironlake_hd_vals[3], 4);
          devprop_add_value(device, "AAPL00,T3", ironlake_hd_vals[4], 4);
          devprop_add_value(device, "AAPL00,T4", ironlake_hd_vals[5], 4);
          devprop_add_value(device, "AAPL00,T5", ironlake_hd_vals[6], 4);
          devprop_add_value(device, "AAPL00,T6", ironlake_hd_vals[7], 4);
          devprop_add_value(device, "AAPL00,T7", ironlake_hd_vals[8], 4);
          devprop_add_value(device, "VRAM,totalsize", ironlake_hd_vals[9], 4);
          break;
        default:
          break;
      }
      break;


      //============== 6th generation ============
      //----------------Sandy Bridge--------------
      //GT1
    case 0x0102: // "Intel HD Graphics 2000"          // Desktop - iMac12,x
    case 0x0106: // "Intel HD Graphics 2000"          // Mobile
    case 0x010A: // "Intel HD Graphics P3000"         // Server
      //GT2
    case 0x0112: // "Intel HD Graphics 3000"          // Desktop
    case 0x0116: // "Intel HD Graphics 3000"          // Mobile - MacBookAir4,x/MacBookPro8,2/MacBookPro8,3
    case 0x0122: // "Intel HD Graphics 3000"          // Desktop
    case 0x0126: // "Intel HD Graphics 3000"          // Mobile - MacBookPro8,1/Macmini5,x
      if (!SetFake) {
        switch (gma_dev->device_id) {
          case 0x0116:
            FakeID = 0x01168086 >> 16;
            DBG("  Found FakeID Intel GFX = 0x%04lx8086\n", FakeID);
            devprop_add_value(device, "device-id", (UINT8*)&FakeID, 4);
            FakeID = 0x01168086 & 0xFFFF;
            devprop_add_value(device, "vendor-id", (UINT8*)&FakeID, 4);
            break;
          case 0x0102:
          case 0x0106:
          case 0x010A:
          case 0x0112:
          case 0x0122:
          case 0x0126:
            FakeID = 0x01268086 >> 16;
            DBG("  Found FakeID Intel GFX = 0x%04lx8086\n", FakeID);
            devprop_add_value(device, "device-id", (UINT8*)&FakeID, 4);
            FakeID = 0x01268086 & 0xFFFF;
            devprop_add_value(device, "vendor-id", (UINT8*)&FakeID, 4);
            break;
          default:
            break;
        }
      }
      if (!SetSnb) {
        switch (MacModel) {
          case MacBookAir41:
          case MacBookAir42:
          case MacBookPro81:
          case MacBookPro82:
          case MacBookPro83:
            devprop_add_value(device, "AAPL,snb-platform-id", sandy_bridge_snb_vals[0], 4);
            DBG("  Found snb-platform-id = 0x00010000\n");
            break;
          default:
            devprop_add_value(device, "AAPL,snb-platform-id", sandy_bridge_snb_vals[2], 4);
            DBG("  Found snb-platform-id = 0x00030010\n");
            break;
        }
      }
      switch (MacModel) {
        case MacBookAir41:
        case MacBookAir42:
          devprop_add_value(device, "AAPL,tbl-info", mba_HD3000_tbl_info, 18);
          devprop_add_value(device, "AAPL,os-info", mba_HD3000_os_info, 20);
          break;
        case MacBookPro81:
        case MacBookPro82:
        case MacBookPro83:
          devprop_add_value(device, "AAPL,tbl-info", mbp_HD3000_tbl_info, 18);
          devprop_add_value(device, "AAPL,os-info", mbp_HD3000_os_info, 20);
          devprop_add_value(device, "AAPL00,DataJustify", sandy_bridge_hd_vals[0], 4);
          devprop_add_value(device, "AAPL00,Dither", sandy_bridge_hd_vals[1], 4);
          devprop_add_value(device, "AAPL00,LinkFormat", sandy_bridge_hd_vals[2], 4);
          devprop_add_value(device, "AAPL00,LinkType", sandy_bridge_hd_vals[3], 4);
          devprop_add_value(device, "AAPL00,PixelFormat", sandy_bridge_hd_vals[4], 4);
          devprop_add_value(device, "AAPL00,T1", sandy_bridge_hd_vals[5], 4);
          devprop_add_value(device, "AAPL00,T2", sandy_bridge_hd_vals[6], 4);
          devprop_add_value(device, "AAPL00,T3", sandy_bridge_hd_vals[7], 4);
          devprop_add_value(device, "AAPL00,T4", sandy_bridge_hd_vals[8], 4);
          devprop_add_value(device, "AAPL00,T5", sandy_bridge_hd_vals[9], 4);
          devprop_add_value(device, "AAPL00,T6", sandy_bridge_hd_vals[10], 4);
          devprop_add_value(device, "AAPL00,T7", sandy_bridge_hd_vals[11], 4);
          break;
        default:
          devprop_add_value(device, "AAPL,tbl-info", mn_HD3000_tbl_info, 18);
          devprop_add_value(device, "AAPL,os-info", mn_HD3000_os_info, 20);
          break;
      }
      devprop_add_value(device, "graphic-options", sandy_bridge_hd_vals[12], 4);
      break;


      //============== 7th generation ============
      //----------------Ivy Bridge----------------
      //GT1
    case 0x0152: // "Intel HD Graphics 2500"          // Desktop - iMac13,x
    case 0x0156: // "Intel HD Graphics 2500"          // Mobile
    case 0x015A: // "Intel HD Graphics 2500"          // Server
    case 0x015E: // "Intel Ivy Bridge GT1"            // Reserved
      //GT2
    case 0x0162: // "Intel HD Graphics 4000"          // Desktop
    case 0x0166: // "Intel HD Graphics 4000"          // Mobile - MacBookPro9,x/MacBookPro10,x/MacBookAir5,x/Macmini6,x
    case 0x016A: // "Intel HD Graphics P4000"         // Server
      switch (gma_dev->device_id) {
        case 0x0152:
        case 0x015A:
          if (!SetFake) {
            FakeID = 0x01528086 >> 16;
            DBG("  Found FakeID Intel GFX = 0x%04lx8086\n", FakeID);
            devprop_add_value(device, "device-id", (UINT8*)&FakeID, 4);
            FakeID = 0x01528086 & 0xFFFF;
            devprop_add_value(device, "vendor-id", (UINT8*)&FakeID, 4);
          }
          if (!SetIg) {
            devprop_add_value(device, "AAPL,ig-platform-id", ivy_bridge_ig_vals[5], 4);
            DBG("  Found ig-platform-id = 0x01620005\n");
          }
          break;
        case 0x0156:
          if (!SetFake) {
            FakeID = 0x01568086 >> 16;
            DBG("  Found FakeID Intel GFX = 0x%04lx8086\n", FakeID);
            devprop_add_value(device, "device-id", (UINT8*)&FakeID, 4);
            FakeID = 0x01568086 & 0xFFFF;
            devprop_add_value(device, "vendor-id", (UINT8*)&FakeID, 4);
          }
          if (!SetIg) {
            devprop_add_value(device, "AAPL,ig-platform-id", ivy_bridge_ig_vals[9], 4);
            DBG("  Found ig-platform-id = 0x01660009\n");
          }
          break;
        case 0x0162:
        case 0x016A:
          if (!SetFake) {
            FakeID = 0x01628086 >> 16;
            DBG("  Found FakeID Intel GFX = 0x%04lx8086\n", FakeID);
            devprop_add_value(device, "device-id", (UINT8*)&FakeID, 4);
            FakeID = 0x01628086 & 0xFFFF;
            devprop_add_value(device, "vendor-id", (UINT8*)&FakeID, 4);
          }
          if (!SetIg) {
            devprop_add_value(device, "AAPL,ig-platform-id", ivy_bridge_ig_vals[5], 4);
            DBG("  Found ig-platform-id = 0x01620005\n");
          }
          break;
        case 0x0166:
          if (!SetFake) {
            FakeID = 0x01668086 >> 16;
            DBG("  Found FakeID Intel GFX = 0x%04lx8086\n", FakeID);
            devprop_add_value(device, "device-id", (UINT8*)&FakeID, 4);
            FakeID = 0x01668086 & 0xFFFF;
            devprop_add_value(device, "vendor-id", (UINT8*)&FakeID, 4);
          }
          if (!SetIg) {
            if (UGAWidth < 1600) {
              devprop_add_value(device, "AAPL,ig-platform-id", ivy_bridge_ig_vals[3], 4);
              DBG("  Found ig-platform-id = 0x01660003\n");
            } else {
              // HD+(1600x900+)
              devprop_add_value(device, "AAPL,ig-platform-id", ivy_bridge_ig_vals[4], 4);
              DBG("  Found ig-platform-id = 0x01660004\n");
            }
          }
          break;
        default:
          break;
      }
      devprop_add_value(device, "graphic-options", ivy_bridge_hd_vals[0], 4);
      break;


      //============== 7.5th generation ==========
      //----------------Haswell-------------------
      //GT1
    case 0x0402: // "Intel Haswell GT1"               // Desktop
    case 0x0406: // "Intel Haswell GT1"               // Mobile
    case 0x040A: // "Intel Haswell GT1"               // Server
    case 0x040B: // "Intel Haswell GT1"               //
    case 0x040E: // "Intel Haswell GT1"               //
      //GT2
    case 0x0412: // "Intel HD Graphics 4600"          // Desktop - iMac15,1
    case 0x0416: // "Intel HD Graphics 4600"          // Mobile
    case 0x041A: // "Intel HD Graphics P4600"         // Server
    case 0x041B: // "Intel Haswell GT2"               //
    case 0x041E: // "Intel HD Graphics 4400"          //
      //GT3
    case 0x0422: // "Intel HD Graphics 5000"          // Desktop
    case 0x0426: // "Intel HD Graphics 5000"          // Mobile
    case 0x042A: // "Intel HD Graphics 5000"          // Server
    case 0x042B: // "Intel Haswell GT3"               //
    case 0x042E: // "Intel Haswell GT3"               //
      //GT1
    case 0x0A02: // "Intel Haswell GT1"               // Desktop ULT
    case 0x0A06: // "Intel HD Graphics"               // Mobile ULT
    case 0x0A0A: // "Intel Haswell GT1"               // Server ULT
    case 0x0A0B: // "Intel Haswell GT1"               // ULT
    case 0x0A0E: // "Intel Haswell GT1"               // ULT
      //GT2
    case 0x0A12: // "Intel Haswell GT2"               // Desktop ULT
    case 0x0A16: // "Intel HD Graphics 4400"          // Mobile ULT
    case 0x0A1A: // "Intel Haswell GT2"               // Server ULT
    case 0x0A1B: // "Intel Haswell GT2"               // ULT
    case 0x0A1E: // "Intel HD Graphics 4200"          // ULT
      //GT3
    case 0x0A22: // "Intel Iris Graphics 5100"        // Desktop ULT
    case 0x0A26: // "Intel HD Graphics 5000"          // Mobile ULT - MacBookAir6,x/Macmini7,1
    case 0x0A2A: // "Intel Iris Graphics 5100"        // Server ULT
    case 0x0A2B: // "Intel Iris Graphics 5100"        // ULT
    case 0x0A2E: // "Intel Iris Graphics 5100"        // ULT - MacBookPro11,1
      //GT1
    case 0x0C02: // "Intel Haswell GT1"               // Desktop SDV
    case 0x0C06: // "Intel Haswell GT1"               // Mobile SDV
    case 0x0C0A: // "Intel Haswell GT1"               // Server SDV
    case 0x0C0B: // "Intel Haswell GT1"               // SDV
    case 0x0C0E: // "Intel Haswell GT1"               // SDV
      //GT2
    case 0x0C12: // "Intel Haswell GT2"               // Desktop SDV
    case 0x0C16: // "Intel Haswell GT2"               // Mobile SDV
    case 0x0C1A: // "Intel Haswell GT2"               // Server SDV
    case 0x0C1B: // "Intel Haswell GT2"               // SDV
    case 0x0C1E: // "Intel Haswell GT2"               // SDV
      //GT3
    case 0x0C22: // "Intel Haswell GT3"               // Desktop SDV
    case 0x0C26: // "Intel Haswell GT3"               // Mobile SDV
    case 0x0C2A: // "Intel Haswell GT3"               // Server SDV
    case 0x0C2B: // "Intel Haswell GT3"               // SDV
    case 0x0C2E: // "Intel Haswell GT3"               // SDV
      //GT1
    case 0x0D02: // "Intel Haswell GT1"               // Desktop CRW
    case 0x0D06: // "Intel Haswell GT1"               // Mobile CRW
    case 0x0D0A: // "Intel Haswell GT1"               // Server CRW
    case 0x0D0B: // "Intel Haswell GT1"               // CRW
    case 0x0D0E: // "Intel Haswell GT1"               // CRW
      //GT2
    case 0x0D12: // "Intel HD Graphics 4600"          // Desktop CRW
    case 0x0D16: // "Intel HD Graphics 4600"          // Mobile CRW
    case 0x0D1A: // "Intel Haswell GT2"               // Server CRW
    case 0x0D1B: // "Intel Haswell GT2"               // CRW
    case 0x0D1E: // "Intel Haswell GT2"               // CRW
      //GT3
    case 0x0D22: // "Intel Iris Pro Graphics 5200"    // Desktop CRW - iMac14,1/iMac14,4
    case 0x0D26: // "Intel Iris Pro Graphics 5200"    // Mobile CRW - MacBookPro11,2/MacBookPro11,3
    case 0x0D2A: // "Intel Iris Pro Graphics 5200"    // Server CRW
    case 0x0D2B: // "Intel Iris Pro Graphics 5200"    // CRW
    case 0x0D2E: // "Intel Iris Pro Graphics 5200"    // CRW
      if (!SetFake) {
        switch (gma_dev->device_id) {
          case 0x0412:
          case 0x0416:
          case 0x041A:
          case 0x041E:
          case 0x0A16:
          case 0x0A1E:
          case 0x0D12:
          case 0x0D16:
            FakeID = 0x04128086 >> 16;
            DBG("  Found FakeID Intel GFX = 0x%04lx8086\n", FakeID);
            devprop_add_value(device, "device-id", (UINT8*)&FakeID, 4);
            FakeID = 0x04128086 & 0xFFFF;
            devprop_add_value(device, "vendor-id", (UINT8*)&FakeID, 4);
            break;
          case 0x0A26:
            FakeID = 0x0A268086 >> 16;
            DBG("  Found FakeID Intel GFX = 0x%04lx8086\n", FakeID);
            devprop_add_value(device, "device-id", (UINT8*)&FakeID, 4);
            FakeID = 0x0A268086 & 0xFFFF;
            devprop_add_value(device, "vendor-id", (UINT8*)&FakeID, 4);
            break;
          case 0x0A22:
          case 0x0A2A:
          case 0x0A2B:
          case 0x0A2E:
            FakeID = 0x0A2E8086 >> 16;
            DBG("  Found FakeID Intel GFX = 0x%04lx8086\n", FakeID);
            devprop_add_value(device, "device-id", (UINT8*)&FakeID, 4);
            FakeID = 0x0A2E8086 & 0xFFFF;
            devprop_add_value(device, "vendor-id", (UINT8*)&FakeID, 4);
            break;
          case 0x0D22:
          case 0x0D2A:
          case 0x0D2B:
          case 0x0D2E:
            FakeID = 0x0D228086 >> 16;
            DBG("  Found FakeID Intel GFX = 0x%04lx8086\n", FakeID);
            devprop_add_value(device, "device-id", (UINT8*)&FakeID, 4);
            FakeID = 0x0D228086 & 0xFFFF;
            devprop_add_value(device, "vendor-id", (UINT8*)&FakeID, 4);
            break;
          case 0x0D26:
            FakeID = 0x0D268086 >> 16;
            DBG("  Found FakeID Intel GFX = 0x%04lx8086\n", FakeID);
            devprop_add_value(device, "device-id", (UINT8*)&FakeID, 4);
            FakeID = 0x0D268086 & 0xFFFF;
            devprop_add_value(device, "vendor-id", (UINT8*)&FakeID, 4);
            break;
          default:
            break;
        }
      }
      if (!SetIg) {
        switch (MacModel) {
          case MacBookAir61:
          case MacBookAir62:
          case MacBookPro111:
          case MacBookPro112:
          case MacBookPro113:
          case MacBookPro114:
          case MacBookPro115:
            if (UGAWidth < 2560) {
              devprop_add_value(device, "AAPL,ig-platform-id", haswell_ig_vals[12], 4);
              DBG("  Found ig-platform-id = 0x0A260006\n");
            } else {
              // QHD+(2560x1440+)
              devprop_add_value(device, "AAPL,ig-platform-id", haswell_ig_vals[14], 4);
              DBG("  Found ig-platform-id = 0x0A2E0008\n");
            }
            break;
          default:
            devprop_add_value(device, "AAPL,ig-platform-id", haswell_ig_vals[9], 4);
            DBG("  Found ig-platform-id = 0x0D220003\n");
            break;
        }
      }
      devprop_add_value(device, "graphic-options", haswell_hd_vals[0], 4);
      break;
	  
      //----------------ValleyView----------------
    case 0x0F30: // "Intel HD Graphics"               // Bay Trail
    case 0x0F31: // "Intel HD Graphics"               // Bay Trail
    case 0x0F32: // "Intel HD Graphics"               // Bay Trail
    case 0x0F33: // "Intel HD Graphics"               // Bay Trail
    case 0x0155: // "Intel HD Graphics"               // Bay Trail
    case 0x0157: // "Intel HD Graphics"               // Bay Trail
      break;


      //============== 8th generation ============
      //----------------Broadwell-----------------
      //GT1
    case 0x1602: // "Intel Broadwell GT1"             // Desktop
    case 0x1606: // "Intel Broadwell GT1"             // Mobile
    case 0x160A: // "Intel Broadwell GT1"             //
    case 0x160B: // "Intel Broadwell GT1"             //
    case 0x160D: // "Intel Broadwell GT1"             //
    case 0x160E: // "Intel Broadwell GT1"             //
      //GT2
    case 0x1612: // "Intel HD Graphics 5600"          // Mobile
    case 0x1616: // "Intel HD Graphics 5500"          // Mobile
    case 0x161A: // "Intel Broadwell GT2"             //
    case 0x161B: // "Intel Broadwell GT2"             //
    case 0x161D: // "Intel Broadwell GT2"             //
    case 0x161E: // "Intel HD Graphics 5300"          // Ultramobile - MacBook8,1
      //GT3
    case 0x1626: // "Intel HD Graphics 6000"          // Mobile - iMac16,1/MacBookAir7,x
    case 0x162B: // "Intel Iris Graphics 6100"        // Mobile - MacBookPro12,1
    case 0x162D: // "Intel Iris Pro Graphics P6300"   // Workstation, Mobile Workstation
      //GT3e
    case 0x1622: // "Intel Iris Pro Graphics 6200"    // Desktop, Mobile - iMac16,2
    case 0x162A: // "Intel Iris Pro Graphics P6300"   // Workstation
      //RSVD
    case 0x162E: // "Intel Broadwell RSVD"            // Reserved
    case 0x1632: // "Intel Broadwell RSVD"            // Reserved
    case 0x1636: // "Intel Broadwell RSVD"            // Reserved
    case 0x163A: // "Intel Broadwell RSVD"            // Reserved
    case 0x163B: // "Intel Broadwell RSVD"            // Reserved
    case 0x163D: // "Intel Broadwell RSVD"            // Reserved
    case 0x163E: // "Intel Broadwell RSVD"            // Reserved
      switch (gma_dev->device_id) {
        case 0x1612:
          if (!SetFake) {
            FakeID = 0x16128086 >> 16;
            DBG("  Found FakeID Intel GFX = 0x%04lx8086\n", FakeID);
            devprop_add_value(device, "device-id", (UINT8*)&FakeID, 4);
            FakeID = 0x16128086 & 0xFFFF;
            devprop_add_value(device, "vendor-id", (UINT8*)&FakeID, 4);
          }
          if (!SetIg) {
            devprop_add_value(device, "AAPL,ig-platform-id", broadwell_ig_vals[18], 4);
            DBG("  Found ig-platform-id = 0x16260006\n");
          }
          break;
        case 0x1616:
          if (!SetFake) {
            FakeID = 0x16168086 >> 16;
            DBG("  Found FakeID Intel GFX = 0x%04lx8086\n", FakeID);
            devprop_add_value(device, "device-id", (UINT8*)&FakeID, 4);
            FakeID = 0x16168086 & 0xFFFF;
            devprop_add_value(device, "vendor-id", (UINT8*)&FakeID, 4);
          }
          if (!SetIg) {
            devprop_add_value(device, "AAPL,ig-platform-id", broadwell_ig_vals[10], 4);
            DBG("  Found ig-platform-id = 0x16160002\n");
          }
          break;
        case 0x161E:
          if (!SetFake) {
            FakeID = 0x161E8086 >> 16;
            DBG("  Found FakeID Intel GFX = 0x%04lx8086\n", FakeID);
            devprop_add_value(device, "device-id", (UINT8*)&FakeID, 4);
            FakeID = 0x161E8086 & 0xFFFF;
            devprop_add_value(device, "vendor-id", (UINT8*)&FakeID, 4);
          }
          if (!SetIg) {
            devprop_add_value(device, "AAPL,ig-platform-id", broadwell_ig_vals[18], 4);
            DBG("  Found ig-platform-id = 0x16260006\n");
          }
          break;
        case 0x1626:
          if (!SetFake) {
            FakeID = 0x16268086 >> 16;
            DBG("  Found FakeID Intel GFX = 0x%04lx8086\n", FakeID);
            devprop_add_value(device, "device-id", (UINT8*)&FakeID, 4);
            FakeID = 0x16268086 & 0xFFFF;
            devprop_add_value(device, "vendor-id", (UINT8*)&FakeID, 4);
          }
          if (!SetIg) {
            devprop_add_value(device, "AAPL,ig-platform-id", broadwell_ig_vals[18], 4);
            DBG("  Found ig-platform-id = 0x16260006\n");
          }
          break;
        case 0x162B:
          if (!SetFake) {
            FakeID = 0x162B8086 >> 16;
            DBG("  Found FakeID Intel GFX = 0x%04lx8086\n", FakeID);
            devprop_add_value(device, "device-id", (UINT8*)&FakeID, 4);
            FakeID = 0x162B8086 & 0xFFFF;
            devprop_add_value(device, "vendor-id", (UINT8*)&FakeID, 4);
          }
          if (!SetIg) {
            devprop_add_value(device, "AAPL,ig-platform-id", broadwell_ig_vals[18], 4);
            DBG("  Found ig-platform-id = 0x16260006\n");
          }
          break;
        case 0x1622:
        case 0x162A:
        case 0x162D:
          if (!SetFake) {
            FakeID = 0x16228086 >> 16;
            DBG("  Found FakeID Intel GFX = 0x%04lx8086\n", FakeID);
            devprop_add_value(device, "device-id", (UINT8*)&FakeID, 4);
            FakeID = 0x16228086 & 0xFFFF;
            devprop_add_value(device, "vendor-id", (UINT8*)&FakeID, 4);
          }
          if (!SetIg) {
            devprop_add_value(device, "AAPL,ig-platform-id", broadwell_ig_vals[18], 4);
            DBG("  Found ig-platform-id = 0x16260006\n");
          }
          break;
        default:
          break;
      }
      switch (MacModel) {
        case MacBook81:
          devprop_add_value(device, "AAPL,ig-tcon-scaler", broadwell_hd_vals[0], 4);
          break;
        default:
          break;
      }
      devprop_add_value(device, "graphic-options", broadwell_hd_vals[1], 4);
      break;

      //------------Cherryview/Braswell-----------
    case 0x22B0: // "Intel HD Graphics 400"           // Cherry Trail - Atom x5 series - Z83X0/Z8550
    case 0x22B1: // "Intel HD Graphics 405"           // Cherry Trail - Atom x7 series - Z8750
    case 0x22B2: // "Intel HD Graphics 400"           // Braswell - Cerelon QC/DC series - X3X60
    case 0x22B3: // "Intel HD Graphics 405"           // Braswell - Pentium QC series - X3710
      break;


      //============== 9th generation ============
      //----------------Skylake-------------------
      //GT1
    case 0x1902: // "Intel HD Graphics 510"           // Desktop
    case 0x1906: // "Intel HD Graphics 510"           // Mobile
    case 0x190A: // "Intel Skylake GT1"               //
    case 0x190B: // "Intel HD Graphics 510"           //
    case 0x190E: // "Intel Skylake GT1"               //
      //GT2
    case 0x1912: // "Intel HD Graphics 530"           // Desktop - iMac17,1
    case 0x1916: // "Intel HD Graphics 520"           // Mobile
    case 0x191A: // "Intel Skylake GT2"               //
    case 0x191B: // "Intel HD Graphics 530"           // Mobile - MacBookPro13,3
    case 0x191D: // "Intel HD Graphics P530"          // Workstation, Mobile Workstation
    case 0x191E: // "Intel HD Graphics 515"           // Mobile - MacBook9,1
    case 0x1921: // "Intel HD Graphics 520"           //
      //GT2f
    case 0x1913: // "Intel Skylake GT2f"              //
    case 0x1915: // "Intel Skylake GT2f"              //
    case 0x1917: // "Intel Skylake GT2f"              //
      //GT3
    case 0x1923: // "Intel HD Graphics 535"           //
      //GT3e
    case 0x1926: // "Intel Iris Graphics 540"         // Mobile - MacBookPro13,1
    case 0x1927: // "Intel Iris Graphics 550"         // Mobile - MacBookPro13,2
    case 0x192B: // "Intel Iris Graphics 555"         //
    case 0x192D: // "Intel Iris Graphics P555"        // Workstation
      //GT4
    case 0x192A: // "Intel Skylake GT4"               //
      //GT4e
    case 0x1932: // "Intel Iris Pro Graphics 580"     // Desktop
    case 0x193A: // "Intel Iris Pro Graphics P580"    // Server
    case 0x193B: // "Intel Iris Pro Graphics 580"     // Mobile
    case 0x193D: // "Intel Iris Pro Graphics P580"    // Workstation, Mobile Workstation
      switch (gma_dev->device_id) {
        case 0x1902:
        case 0x1906:
          if (!SetFake) {
            FakeID = 0x19028086 >> 16;
            DBG("  Found FakeID Intel GFX = 0x%04lx8086\n", FakeID);
            devprop_add_value(device, "device-id", (UINT8*)&FakeID, 4);
            FakeID = 0x19028086 & 0xFFFF;
            devprop_add_value(device, "vendor-id", (UINT8*)&FakeID, 4);
          }
          if (!SetIg) {
            devprop_add_value(device, "AAPL,ig-platform-id", skylake_ig_vals[1], 4);
            DBG("  Found ig-platform-id = 0x19160000\n");
          }
          break;
        case 0x1912:
          if (!SetFake) {
            FakeID = 0x19128086 >> 16;
            DBG("  Found FakeID Intel GFX = 0x%04lx8086\n", FakeID);
            devprop_add_value(device, "device-id", (UINT8*)&FakeID, 4);
            FakeID = 0x19128086 & 0xFFFF;
            devprop_add_value(device, "vendor-id", (UINT8*)&FakeID, 4);
          }
          if (!SetIg) {
            devprop_add_value(device, "AAPL,ig-platform-id", skylake_ig_vals[0], 4);
            DBG("  Found ig-platform-id = 0x19120000\n");
          }
          break;
        case 0x1916:
        case 0x1921:
          if (!SetFake) {
            FakeID = 0x19168086 >> 16;
            DBG("  Found FakeID Intel GFX = 0x%04lx8086\n", FakeID);
            devprop_add_value(device, "device-id", (UINT8*)&FakeID, 4);
            FakeID = 0x19168086 & 0xFFFF;
            devprop_add_value(device, "vendor-id", (UINT8*)&FakeID, 4);
          }
          if (!SetIg) {
            devprop_add_value(device, "AAPL,ig-platform-id", skylake_ig_vals[1], 4);
            DBG("  Found ig-platform-id = 0x19160000\n");
          }
          break;
        case 0x191B:
        case 0x191D:
        case 0x1923:
          if (!SetFake) {
            FakeID = 0x191B8086 >> 16;
            DBG("  Found FakeID Intel GFX = 0x%04lx8086\n", FakeID);
            devprop_add_value(device, "device-id", (UINT8*)&FakeID, 4);
            FakeID = 0x191B8086 & 0xFFFF;
            devprop_add_value(device, "vendor-id", (UINT8*)&FakeID, 4);
          }
          if (!SetIg) {
            devprop_add_value(device, "AAPL,ig-platform-id", skylake_ig_vals[4], 4);
            DBG("  Found ig-platform-id = 0x191B0000\n");
          }
          break;
        case 0x191E:
          if (!SetFake) {
            FakeID = 0x191E8086 >> 16;
            DBG("  Found FakeID Intel GFX = 0x%04lx8086\n", FakeID);
            devprop_add_value(device, "device-id", (UINT8*)&FakeID, 4);
            FakeID = 0x191E8086 & 0xFFFF;
            devprop_add_value(device, "vendor-id", (UINT8*)&FakeID, 4);
          }
          if (!SetIg) {
            devprop_add_value(device, "AAPL,ig-platform-id", skylake_ig_vals[5], 4);
            DBG("  Found ig-platform-id = 0x191E0000\n");
          }
          break;
        case 0x1926:
          if (!SetFake) {
            FakeID = 0x19268086 >> 16;
            DBG("  Found FakeID Intel GFX = 0x%04lx8086\n", FakeID);
            devprop_add_value(device, "device-id", (UINT8*)&FakeID, 4);
            FakeID = 0x19268086 & 0xFFFF;
            devprop_add_value(device, "vendor-id", (UINT8*)&FakeID, 4);
          }
          if (!SetIg) {
            devprop_add_value(device, "AAPL,ig-platform-id", skylake_ig_vals[2], 4);
            DBG("  Found ig-platform-id = 0x19260000\n");
          }
          break;
        case 0x1927:
        case 0x192B:
        case 0x192D:
          if (!SetFake) {
            FakeID = 0x19278086 >> 16;
            DBG("  Found FakeID Intel GFX = 0x%04lx8086\n", FakeID);
            devprop_add_value(device, "device-id", (UINT8*)&FakeID, 4);
            FakeID = 0x19278086 & 0xFFFF;
            devprop_add_value(device, "vendor-id", (UINT8*)&FakeID, 4);
          }
          if (!SetIg) {
            devprop_add_value(device, "AAPL,ig-platform-id", skylake_ig_vals[3], 4);
            DBG("  Found ig-platform-id = 0x19270000\n");
          }
          break;
        case 0x1932:
        case 0x193A:
        case 0x193B:
        case 0x193D:
          if (!SetFake) {
            FakeID = 0x193B8086 >> 16;
            DBG("  Found FakeID Intel GFX = 0x%04lx8086\n", FakeID);
            devprop_add_value(device, "device-id", (UINT8*)&FakeID, 4);
            FakeID = 0x193B8086 & 0xFFFF;
            devprop_add_value(device, "vendor-id", (UINT8*)&FakeID, 4);
          }
          if (!SetIg) {
            devprop_add_value(device, "AAPL,ig-platform-id", skylake_ig_vals[6], 4);
            DBG("  Found ig-platform-id = 0x193B0000\n");
          }
          break;
        default:
          break;
      }
      switch (MacModel) {
        case MacBook91:
          devprop_add_value(device, "AAPL00,PanelCycleDelay", skylake_hd_vals[2], 4);
          devprop_add_value(device, "AAPL00,PanelPowerDown", skylake_hd_vals[3], 4);
          devprop_add_value(device, "AAPL00,PanelPowerOff", skylake_hd_vals[4], 4);
          devprop_add_value(device, "AAPL00,PanelPowerOn", skylake_hd_vals[5], 4);
          devprop_add_value(device, "AAPL00,PanelPowerUp", skylake_hd_vals[6], 4);
          devprop_add_value(device, "graphic-options", skylake_hd_vals[11], 4);
          break;
        case MacBookPro131:
        case MacBookPro132:
        case MacBookPro133:  // it has only the "graphic-options" value. However, we use built-in graphics.
          devprop_add_value(device, "AAPL,Gfx324", skylake_hd_vals[0], 4);
          devprop_add_value(device, "AAPL00,PanelCycleDelay", skylake_hd_vals[2], 4);
          devprop_add_value(device, "AAPL00,PanelPowerDown", skylake_hd_vals[7], 4);
          devprop_add_value(device, "AAPL00,PanelPowerOff", skylake_hd_vals[8], 4);
          devprop_add_value(device, "AAPL00,PanelPowerOn", skylake_hd_vals[9], 4);
          devprop_add_value(device, "AAPL00,PanelPowerUp", skylake_hd_vals[10], 4);
          devprop_add_value(device, "graphic-options", skylake_hd_vals[11], 4);
          break;
        default:
          break;
      }
      devprop_add_value(device, "AAPL,GfxYTile", skylake_hd_vals[1], 4);
      break;

      //----------------Goldmont------------------
    case 0x0A84: // "Intel HD Graphics"               // Broxton(cancelled)
    case 0x1A84: // "Intel HD Graphics"               // Broxton(cancelled)
    case 0x1A85: // "Intel HD Graphics"               // Broxton(cancelled)
    case 0x5A84: // "Intel HD Graphics 505"           // Apollo Lake
    case 0x5A85: // "Intel HD Graphics 500"           // Apollo Lake
      break;


      //============== 9.5th generation ==========
      //----------------Kaby Lake-----------------
      //GT1
    case 0x5902: // "Intel HD Graphics 610"           // Desktop
    case 0x5906: // "Intel HD Graphics 610"           // Mobile
    case 0x590A: // "Intel Kaby Lake GT1"             //
    case 0x5908: // "Intel Kaby Lake GT1"             //
    case 0x590B: // "Intel Kaby Lake GT1"             //
    case 0x590E: // "Intel Kaby Lake GT1"             //
      //GT1.5
    case 0x5913: // "Intel Kaby Lake GT1.5"           //
    case 0x5915: // "Intel Kaby Lake GT1.5"           //
      //GT2
    case 0x5912: // "Intel HD Graphics 630"           // Desktop - iMac18,2/iMac18,3
    case 0x5916: // "Intel HD Graphics 620"           // Mobile
    case 0x591A: // "Intel HD Graphics P630"          // Server
    case 0x591B: // "Intel HD Graphics 630"           // Mobile - MacBookPro14,3
    case 0x591D: // "Intel HD Graphics P630"          // Workstation, Mobile Workstation
    case 0x591E: // "Intel HD Graphics 615"           // Mobile - MacBook10,1
      //GT2F
    case 0x5921: // "Intel Kaby Lake GT2F"            //
      //GT3
    case 0x5923: // "Intel HD Graphics 635"           //
    case 0x5926: // "Intel Iris Plus Graphics 640"    // Mobile - MacBookPro14,1/iMac18,1
    case 0x5927: // "Intel Iris Plus Graphics 650"    // Mobile - MacBookPro14,2
      //GT4
    case 0x593B: // "Intel Kaby Lake GT4"             //

      //-------------Kaby Lake Refresh------------
      //GT1.5
    case 0x5917: // "Intel UHD Graphics 620"          // Mobile

      //----------------Coffee Lake---------------
      //GT1
    case 0x3E90: // "Intel UHD Graphics 610"          // Desktop
    case 0x3E93: // "Intel UHD Graphics 610"          // Desktop
      //GT2
    case 0x3E91: // "Intel UHD Graphics 630"          // Desktop
    case 0x3E92: // "Intel UHD Graphics 630"          // Desktop
    case 0x3E94: // "Intel Coffee Lake GT2"           //
    case 0x3E96: // "Intel Coffee Lake GT2"           //
    case 0x3E9B: // "Intel Coffee Lake GT2"           //
      //GT3
    case 0x3EA5: // "Intel Coffee Lake GT3"           //
    case 0x3EA6: // "Intel Coffee Lake GT3"           //
    case 0x3EA7: // "Intel Coffee Lake GT3"           //
    case 0x3EA8: // "Intel Coffee Lake GT3"           //
      switch (gma_dev->device_id) {
        case 0x5902:
        case 0x5906:
        case 0x3E90:
        case 0x3E93:
          if (!SetFake) {
            FakeID = 0x59028086 >> 16;
            DBG("  Found FakeID Intel GFX = 0x%04lx8086\n", FakeID);
            devprop_add_value(device, "device-id", (UINT8*)&FakeID, 4);
            FakeID = 0x59028086 & 0xFFFF;
            devprop_add_value(device, "vendor-id", (UINT8*)&FakeID, 4);
          }
          if (!SetIg) {
            devprop_add_value(device, "AAPL,ig-platform-id", kabylake_ig_vals[1], 4);
            DBG("  Found ig-platform-id = 0x59160000\n");
          }
          break;
        case 0x5912:
        case 0x3E91:
        case 0x3E92:
          if (!SetFake) {
            FakeID = 0x59128086 >> 16;
            DBG("  Found FakeID Intel GFX = 0x%04lx8086\n", FakeID);
            devprop_add_value(device, "device-id", (UINT8*)&FakeID, 4);
            FakeID = 0x59128086 & 0xFFFF;
            devprop_add_value(device, "vendor-id", (UINT8*)&FakeID, 4);
          }
          if (!SetIg) {
            devprop_add_value(device, "AAPL,ig-platform-id", kabylake_ig_vals[0], 4);
            DBG("  Found ig-platform-id = 0x59120000\n");
          }
          break;
        case 0x5916:
        case 0x5917:
          if (!SetFake) {
            FakeID = 0x59168086 >> 16;
            DBG("  Found FakeID Intel GFX = 0x%04lx8086\n", FakeID);
            devprop_add_value(device, "device-id", (UINT8*)&FakeID, 4);
            FakeID = 0x59168086 & 0xFFFF;
            devprop_add_value(device, "vendor-id", (UINT8*)&FakeID, 4);
          }
          if (!SetIg) {
            devprop_add_value(device, "AAPL,ig-platform-id", kabylake_ig_vals[1], 4);
            DBG("  Found ig-platform-id = 0x59160000\n");
          }
          break;
        case 0x591A:
        case 0x591B:
        case 0x591D:
          if (!SetFake) {
            FakeID = 0x591B8086 >> 16;
            DBG("  Found FakeID Intel GFX = 0x%04lx8086\n", FakeID);
            devprop_add_value(device, "device-id", (UINT8*)&FakeID, 4);
            FakeID = 0x591B8086 & 0xFFFF;
            devprop_add_value(device, "vendor-id", (UINT8*)&FakeID, 4);
          }
          if (!SetIg) {
            devprop_add_value(device, "AAPL,ig-platform-id", kabylake_ig_vals[4], 4);
            DBG("  Found ig-platform-id = 0x591B0000\n");
          }
          break;
        case 0x591E:
          if (!SetFake) {
            FakeID = 0x591E8086 >> 16;
            DBG("  Found FakeID Intel GFX = 0x%04lx8086\n", FakeID);
            devprop_add_value(device, "device-id", (UINT8*)&FakeID, 4);
            FakeID = 0x591E8086 & 0xFFFF;
            devprop_add_value(device, "vendor-id", (UINT8*)&FakeID, 4);
          }
          if (!SetIg) {
            devprop_add_value(device, "AAPL,ig-platform-id", kabylake_ig_vals[5], 4);
            DBG("  Found ig-platform-id = 0x591E0000\n");
          }
          break;
        case 0x5923:
          if (!SetFake) {
            FakeID = 0x59238086 >> 16;
            DBG("  Found FakeID Intel GFX = 0x%04lx8086\n", FakeID);
            devprop_add_value(device, "device-id", (UINT8*)&FakeID, 4);
            FakeID = 0x59238086 & 0xFFFF;
            devprop_add_value(device, "vendor-id", (UINT8*)&FakeID, 4);
          }
          if (!SetIg) {
            devprop_add_value(device, "AAPL,ig-platform-id", kabylake_ig_vals[6], 4);
            DBG("  Found ig-platform-id = 0x59230000\n");
          }
          break;
        case 0x5926:
          if (!SetFake) {
            FakeID = 0x59268086 >> 16;
            DBG("  Found FakeID Intel GFX = 0x%04lx8086\n", FakeID);
            devprop_add_value(device, "device-id", (UINT8*)&FakeID, 4);
            FakeID = 0x59268086 & 0xFFFF;
            devprop_add_value(device, "vendor-id", (UINT8*)&FakeID, 4);
          }
          if (!SetIg) {
            devprop_add_value(device, "AAPL,ig-platform-id", kabylake_ig_vals[2], 4);
            DBG("  Found ig-platform-id = 0x59260000\n");
          }
          break;
        case 0x5927:
          if (!SetFake) {
            FakeID = 0x59278086 >> 16;
            DBG("  Found FakeID Intel GFX = 0x%04lx8086\n", FakeID);
            devprop_add_value(device, "device-id", (UINT8*)&FakeID, 4);
            FakeID = 0x59278086 & 0xFFFF;
            devprop_add_value(device, "vendor-id", (UINT8*)&FakeID, 4);
          }
          if (!SetIg) {
            devprop_add_value(device, "AAPL,ig-platform-id", kabylake_ig_vals[3], 4);
            DBG("  Found ig-platform-id = 0x59270000\n");
          }
          break;
        default:
          break;
      }
      switch (MacModel) {
        case MacBook101:
          devprop_add_value(device, "AAPL00,PanelCycleDelay", kabylake_hd_vals[2], 4);
          devprop_add_value(device, "AAPL00,PanelPowerDown", kabylake_hd_vals[3], 4);
          devprop_add_value(device, "AAPL00,PanelPowerOff", kabylake_hd_vals[4], 4);
          devprop_add_value(device, "AAPL00,PanelPowerOn", kabylake_hd_vals[5], 4);
          devprop_add_value(device, "AAPL00,PanelPowerUp", kabylake_hd_vals[6], 4);
          devprop_add_value(device, "graphic-options", kabylake_hd_vals[11], 4);
          break;
        case MacBookPro141:
        case MacBookPro142:
        case MacBookPro143:  // it has only the "graphic-options" value. However, we use built-in graphics.
          devprop_add_value(device, "AAPL,Gfx324", kabylake_hd_vals[0], 4);
          devprop_add_value(device, "AAPL00,PanelCycleDelay", kabylake_hd_vals[2], 4);
          devprop_add_value(device, "AAPL00,PanelPowerDown", kabylake_hd_vals[7], 4);
          devprop_add_value(device, "AAPL00,PanelPowerOff", kabylake_hd_vals[8], 4);
          devprop_add_value(device, "AAPL00,PanelPowerOn", kabylake_hd_vals[9], 4);
          devprop_add_value(device, "AAPL00,PanelPowerUp", kabylake_hd_vals[10], 4);
          devprop_add_value(device, "graphic-options", kabylake_hd_vals[11], 4);
          break;
        default:
          break;
      }
      devprop_add_value(device, "AAPL,GfxYTile", kabylake_hd_vals[1], 4);
      break;

      //----------------Gemini Lake---------------
    case 0x3184: // "Intel HD Graphics 605"           //
    case 0x3185: // "Intel HD Graphics 600"           //
      break;


      //============== 10th generation ===========
      //----------------Cannonlake----------------
      //GT0.5
    case 0x5A49: // "Intel Cannonlake GT0.5"          //
    case 0x5A4A: // "Intel Cannonlake GT0.5"          //
      //GT1
    case 0x5A41: // "Intel Cannonlake GT1"            //
    case 0x5A42: // "Intel Cannonlake GT1"            //
    case 0x5A44: // "Intel Cannonlake GT1"            //
      //GT1.5
    case 0x5A59: // "Intel Cannonlake GT1.5"          //
    case 0x5A5A: // "Intel Cannonlake GT1.5"          //
    case 0x5A5C: // "Intel Cannonlake GT1.5"          //
      //GT2
    case 0x5A50: // "Intel Cannonlake GT2"            //
    case 0x5A51: // "Intel Cannonlake GT2"            //
    case 0x5A52: // "Intel Cannonlake GT2"            //
    case 0x5A54: // "Intel Cannonlake GT2"            //
      break;


    default:
      DBG("  Intel card id=%x unsupported, please report to the clover thread\n", gma_dev->device_id);
      return FALSE;
  }

#if DEBUG_GMA == 2
  gBS->Stall(5000000);
#endif

  return TRUE;
}
