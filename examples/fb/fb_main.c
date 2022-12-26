/****************************************************************************
 * apps/examples/fb/fb_main.c
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>

#include <nuttx/video/fb.h>
#include <nuttx/video/rgbcolors.h>

#include "../arch/arm64/src/common/arm64_internal.h" //// TODO
static bool display_frame_done(void); ////

/****************************************************************************
 * Preprocessor Definitions
 ****************************************************************************/

#define NCOLORS 6

/****************************************************************************
 * Private Types
 ****************************************************************************/

struct fb_state_s
{
  int fd;
  struct fb_videoinfo_s vinfo;
  struct fb_planeinfo_s pinfo;
#ifdef CONFIG_FB_OVERLAY
  struct fb_overlayinfo_s oinfo;
#endif
  FAR void *fbmem;
};

/****************************************************************************
 * Private Data
 ****************************************************************************/

static const char g_default_fbdev[] = CONFIG_EXAMPLES_FB_DEFAULTFB;

/* Violet-Blue-Green-Yellow-Orange-Red */

static const uint32_t g_rgb24[NCOLORS] =
{
  RGB24_VIOLET, RGB24_BLUE, RGB24_GREEN,
  RGB24_YELLOW, RGB24_ORANGE, RGB24_RED
};

static const uint16_t g_rgb16[NCOLORS] =
{
  RGB16_VIOLET, RGB16_BLUE, RGB16_GREEN,
  RGB16_YELLOW, RGB16_ORANGE, RGB16_RED
};

static const uint8_t g_rgb8[NCOLORS] =
{
  RGB8_VIOLET, RGB8_BLUE, RGB8_GREEN,
  RGB8_YELLOW, RGB8_ORANGE, RGB8_RED
};

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * draw_rect
 ****************************************************************************/

static void draw_rect32(FAR struct fb_state_s *state,
                        FAR struct fb_area_s *area, int color)
{
  FAR uint32_t *dest;
  FAR uint8_t *row;
  int x;
  int y;

  row = (FAR uint8_t *)state->fbmem + state->pinfo.stride * area->y;
  for (y = 0; y < area->h; y++)
    {
      dest = ((FAR uint32_t *)row) + area->x;
      for (x = 0; x < area->w; x++)
        {
      //// Begin
      if (!display_frame_done()) {}
      //// End

          *dest++ = g_rgb24[color];

          /* Fixes missing rows in the rendered image, not sure why */

          ARM64_DMB(); ////
          ARM64_DSB(); ////
          ARM64_ISB(); ////
        }

      row += state->pinfo.stride;
    }
}

static void draw_rect16(FAR struct fb_state_s *state,
                        FAR struct fb_area_s *area, int color)
{
  FAR uint16_t *dest;
  FAR uint8_t *row;
  int x;
  int y;

  row = (FAR uint8_t *)state->fbmem + state->pinfo.stride * area->y;
  for (y = 0; y < area->h; y++)
    {
      dest = ((FAR uint16_t *)row) + area->x;
      for (x = 0; x < area->w; x++)
        {
      //// Begin
      if (!display_frame_done()) {}
      //// End

          *dest++ = g_rgb16[color];

          /* Fixes missing rows in the rendered image, not sure why */

          ARM64_DMB(); ////
          ARM64_DSB(); ////
          ARM64_ISB(); ////
        }

      row += state->pinfo.stride;
    }
}

static void draw_rect8(FAR struct fb_state_s *state,
                       FAR struct fb_area_s *area, int color)
{
  FAR uint8_t *dest;
  FAR uint8_t *row;
  int x;
  int y;

  row = (FAR uint8_t *)state->fbmem + state->pinfo.stride * area->y;
  for (y = 0; y < area->h; y++)
    {
      dest = row + area->x;
      for (x = 0; x < area->w; x++)
        {
      //// Begin
      if (!display_frame_done()) {}
      //// End

          *dest++ = g_rgb8[color];

          /* Fixes missing rows in the rendered image, not sure why */

          ARM64_DMB(); ////
          ARM64_DSB(); ////
          ARM64_ISB(); ////
        }

      row += state->pinfo.stride;
    }
}

static void draw_rect1(FAR struct fb_state_s *state,
                       FAR struct fb_area_s *area, int color)
{
  FAR uint8_t *pixel;
  FAR uint8_t *row;
  uint8_t color8 = (color & 1) == 0 ? 0 : 0xff;

  int start_full_x;
  int end_full_x;
  int start_bit_shift;
  int last_bits;
  uint8_t lmask;
  uint8_t rmask;
  int y;

  /* Calculate the framebuffer address of the first row to draw on */

  row    = (FAR uint8_t *)state->fbmem + state->pinfo.stride * area->y;

  /* Calculate the position of the first complete (with all bits) byte.
   * Then calculate the last byte with all the bits.
   */

  start_full_x = ((area->x + 7) >> 3);
  end_full_x   = ((area->x + area->w) >> 3);

  /* Calculate the number of bits in byte before start that need to remain
   * unchanged. Later calculate the mask.
   */

  start_bit_shift = 8 + area->x - (start_full_x << 3);
  lmask = 0xff >> start_bit_shift;

  /* Calculate the number of bits that needs to be changed after last byte
   * with all the bits. Later calculate the mask.
   */

  last_bits = area->x + area->w - (end_full_x << 3);
  rmask = 0xff << (8 - last_bits);

  /* Calculate a mask on the first and last bytes of the sequence that may
   * not be completely filled with pixel.
   */

  /* Now draw each row, one-at-a-time */

  for (y = 0; y < area->h; y++)
    {
      //// Begin
      if (!display_frame_done()) {}
      //// End

      /* 'pixel' points to the 1st pixel the next row */

      /* Special case: The row starts and ends within the same byte */

      if (start_full_x > end_full_x)
        {
          pixel = row + start_full_x - 1;
          *pixel = (*pixel & (~lmask | ~rmask)) | (lmask & rmask & color8);
          continue;
        }

      if (start_bit_shift != 0)
        {
          pixel = row + start_full_x - 1;
          *pixel = (*pixel & ~lmask) | (lmask & color8);
        }

      if (end_full_x > start_full_x)
        {
          pixel = row + start_full_x;
          memset(pixel, color8, end_full_x - start_full_x);
        }

      if (last_bits != 0)
        {
          pixel = row + end_full_x;
          *pixel = (*pixel & ~rmask) | (rmask & color8);
        }

      row += state->pinfo.stride;

      /* Fixes missing rows in the rendered image, not sure why */

      ARM64_DMB(); ////
      ARM64_DSB(); ////
      ARM64_ISB(); ////
    }
}

static void draw_rect(FAR struct fb_state_s *state,
                      FAR struct fb_area_s *area, int color)
{
#ifdef CONFIG_FB_UPDATE
  int ret;
#endif

  switch (state->pinfo.bpp)
    {
      case 32:
        draw_rect32(state, area, color);
        break;

      case 16:
        draw_rect16(state, area, color);
        break;

      case 8:
      default:
        draw_rect8(state, area, color);
        break;

      case 1:
        draw_rect1(state, area, color);
        break;
    }

#ifdef CONFIG_FB_UPDATE
  ret = ioctl(state->fd, FBIO_UPDATE,
              (unsigned long)((uintptr_t)area));
  if (ret < 0)
    {
      int errcode = errno;
      fprintf(stderr, "ERROR: ioctl(FBIO_UPDATE) failed: %d\n",
              errcode);
    }
#endif
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * fb_main
 ****************************************************************************/

int main(int argc, FAR char *argv[])
{
  FAR const char *fbdev = g_default_fbdev;
  struct fb_state_s state;
  struct fb_area_s area;
  int nsteps;
  int xstep;
  int ystep;
  int width;
  int height;
  int color;
  int x;
  int y;
  int ret;

  /* There is a single required argument:  The path to the framebuffer
   * driver.
   */

  if (argc == 2)
    {
      fbdev = argv[1];
    }
  else if (argc != 1)
    {
      fprintf(stderr, "ERROR: Single argument required\n");
      fprintf(stderr, "USAGE: %s [<fb-driver-path>]\n", argv[0]);
      return EXIT_FAILURE;
    }

  /* Open the framebuffer driver */

  state.fd = open(fbdev, O_RDWR);
  if (state.fd < 0)
    {
      int errcode = errno;
      fprintf(stderr, "ERROR: Failed to open %s: %d\n", fbdev, errcode);
      return EXIT_FAILURE;
    }

  /* Get the characteristics of the framebuffer */

  ret = ioctl(state.fd, FBIOGET_VIDEOINFO,
              (unsigned long)((uintptr_t)&state.vinfo));
  if (ret < 0)
    {
      int errcode = errno;
      fprintf(stderr, "ERROR: ioctl(FBIOGET_VIDEOINFO) failed: %d\n",
              errcode);
      close(state.fd);
      return EXIT_FAILURE;
    }

  printf("VideoInfo:\n");
  printf("      fmt: %u\n", state.vinfo.fmt);
  printf("     xres: %u\n", state.vinfo.xres);
  printf("     yres: %u\n", state.vinfo.yres);
  printf("  nplanes: %u\n", state.vinfo.nplanes);

#ifdef TODO_CONFIG_FB_OVERLAY ////
  printf("noverlays: %u\n", state.vinfo.noverlays);

  /* Select the first overlay, which should be the composed framebuffer */

  ret = ioctl(state.fd, FBIO_SELECT_OVERLAY, 0);
  if (ret < 0)
    {
      int errcode = errno;
      fprintf(stderr, "ERROR: ioctl(FBIO_SELECT_OVERLAY) failed: %d\n",
              errcode);
      close(state.fd);
      return EXIT_FAILURE;
    }

  /* Get the first overlay information */

  state.oinfo.overlay = 0;
  ret = ioctl(state.fd, FBIOGET_OVERLAYINFO,
                        (unsigned long)((uintptr_t)&state.oinfo));
  if (ret < 0)
    {
      int errcode = errno;
      fprintf(stderr, "ERROR: ioctl(FBIOGET_OVERLAYINFO) failed: %d\n",
              errcode);
      close(state.fd);
      return EXIT_FAILURE;
    }

  printf("OverlayInfo (overlay 0):\n");
  printf("    fbmem: %p\n", state.oinfo.fbmem);
  printf("    fblen: %lu\n", (unsigned long)state.oinfo.fblen);
  printf("   stride: %u\n", state.oinfo.stride);
  printf("  overlay: %u\n", state.oinfo.overlay);
  printf("      bpp: %u\n", state.oinfo.bpp);
  printf("    blank: %u\n", state.oinfo.blank);
  printf("chromakey: 0x%08" PRIx32 "\n", state.oinfo.chromakey);
  printf("    color: 0x%08" PRIx32 "\n", state.oinfo.color);
  printf("   transp: 0x%02x\n", state.oinfo.transp.transp);
  printf("     mode: %u\n", state.oinfo.transp.transp_mode);
  printf("     area: (%u,%u) => (%u,%u)\n",
                      state.oinfo.sarea.x, state.oinfo.sarea.y,
                      state.oinfo.sarea.w, state.oinfo.sarea.h);
  printf("     accl: %" PRIu32 "\n", state.oinfo.accl);

#endif

  ret = ioctl(state.fd, FBIOGET_PLANEINFO,
              (unsigned long)((uintptr_t)&state.pinfo));
  if (ret < 0)
    {
      int errcode = errno;
      fprintf(stderr, "ERROR: ioctl(FBIOGET_PLANEINFO) failed: %d\n",
              errcode);
      close(state.fd);
      return EXIT_FAILURE;
    }

  printf("PlaneInfo (plane 0):\n");
  printf("    fbmem: %p\n", state.pinfo.fbmem);
  printf("    fblen: %lu\n", (unsigned long)state.pinfo.fblen);
  printf("   stride: %u\n", state.pinfo.stride);
  printf("  display: %u\n", state.pinfo.display);
  printf("      bpp: %u\n", state.pinfo.bpp);

  /* Only these pixel depths are supported.  viinfo.fmt is ignored, only
   * certain color formats are supported.
   */

  if (state.pinfo.bpp != 32 && state.pinfo.bpp != 16 &&
      state.pinfo.bpp != 8  && state.pinfo.bpp != 1)
    {
      fprintf(stderr, "ERROR: bpp=%u not supported\n", state.pinfo.bpp);
      close(state.fd);
      return EXIT_FAILURE;
    }

  /* mmap() the framebuffer.
   *
   * NOTE: In the FLAT build the frame buffer address returned by the
   * FBIOGET_PLANEINFO IOCTL command will be the same as the framebuffer
   * address.  mmap(), however, is the preferred way to get the framebuffer
   * address because in the KERNEL build, it will perform the necessary
   * address mapping to make the memory accessible to the application.
   */

  state.fbmem = mmap(NULL, state.pinfo.fblen, PROT_READ | PROT_WRITE,
                     MAP_SHARED | MAP_FILE, state.fd, 0);
  if (state.fbmem == MAP_FAILED)
    {
      int errcode = errno;
      fprintf(stderr, "ERROR: ioctl(FBIOGET_PLANEINFO) failed: %d\n",
              errcode);
      close(state.fd);
      return EXIT_FAILURE;
    }

  printf("Mapped FB: %p\n", state.fbmem);

  /* Draw some rectangles */

  nsteps = 2 * (NCOLORS - 1) + 1;
  xstep  = state.vinfo.xres / nsteps;
  ystep  = state.vinfo.yres / nsteps;
  width  = state.vinfo.xres;
  height = state.vinfo.yres;

  for (x = 0, y = 0, color = 0;
       color < NCOLORS;
       x += xstep, y += ystep, color++)
    {
      area.x = x;
      area.y = y;
      area.w = width;
      area.h = height;

      printf("%2d: (%3d,%3d) (%3d,%3d)\n",
             color, area.x, area.y, area.w, area.h);

      draw_rect(&state, &area, color);
      usleep(500 * 1000);

      width  -= (2 * xstep);
      height -= (2 * ystep);
      sleep(1); ////
    }

  printf("Test finished\n");
  munmap(state.fbmem, state.pinfo.fblen);
  close(state.fd);
  return EXIT_SUCCESS;
}

#include "../arch/arm64/src/common/arm64_arch.h"

#define SUNXI_LCD0_BASE			0x01c0C000
#define u32 uint32_t
#define u8 uint8_t
#define readl(addr) getreg32((unsigned long) (addr))
#define writel(data, addr) putreg32(data, (unsigned long) (addr))

struct sunxi_lcdc_reg {
	u32 ctrl;			/* 0x00 */
	u32 int0;			/* 0x04 */
	u32 int1;			/* 0x08 */
	u8 res0[0x04];			/* 0x0c */
	u32 tcon0_frm_ctrl;		/* 0x10 */
	u32 tcon0_frm_seed[6];		/* 0x14 */
	u32 tcon0_frm_table[4];		/* 0x2c */
	u8 res1[4];			/* 0x3c */
	u32 tcon0_ctrl;			/* 0x40 */
	u32 tcon0_dclk;			/* 0x44 */
	u32 tcon0_timing_active;	/* 0x48 */
	u32 tcon0_timing_h;		/* 0x4c */
	u32 tcon0_timing_v;		/* 0x50 */
	u32 tcon0_timing_sync;		/* 0x54 */
	u32 tcon0_hv_intf;		/* 0x58 */
	u8 res2[0x04];			/* 0x5c */
	u32 tcon0_cpu_intf;		/* 0x60 */
	u32 tcon0_cpu_wr_dat;		/* 0x64 */
	u32 tcon0_cpu_rd_dat0;		/* 0x68 */
	u32 tcon0_cpu_rd_dat1;		/* 0x6c */
	u32 tcon0_ttl_timing0;		/* 0x70 */
	u32 tcon0_ttl_timing1;		/* 0x74 */
	u32 tcon0_ttl_timing2;		/* 0x78 */
	u32 tcon0_ttl_timing3;		/* 0x7c */
	u32 tcon0_ttl_timing4;		/* 0x80 */
	u32 tcon0_lvds_intf;		/* 0x84 */
	u32 tcon0_io_polarity;		/* 0x88 */
	u32 tcon0_io_tristate;		/* 0x8c */
	u32 tcon1_ctrl;			/* 0x90 */
	u32 tcon1_timing_source;	/* 0x94 */
	u32 tcon1_timing_scale;		/* 0x98 */
	u32 tcon1_timing_out;		/* 0x9c */
	u32 tcon1_timing_h;		/* 0xa0 */
	u32 tcon1_timing_v;		/* 0xa4 */
	u32 tcon1_timing_sync;		/* 0xa8 */
	u8 res3[0x44];			/* 0xac */
	u32 tcon1_io_polarity;		/* 0xf0 */
	u32 tcon1_io_tristate;		/* 0xf4 */
	u32 ecc_fifo;			/* 0xf8 */
	u8 res4[0x104];			/* 0xfc */
	u32 mux_ctrl;			/* 0x200 */
	u8 res5[0x1c];			/* 0x204 */
	u32 lvds_ana0;			/* 0x220 */
	u32 lvds_ana1;			/* 0x224 */
};

// Bit 11 is TCON0_Tri_Finish_Int_Flag
// Trigger when cpu trigger mode finish
static bool display_frame_done(void)
{
	struct sunxi_lcdc_reg * const lcdc = (struct sunxi_lcdc_reg *)SUNXI_LCD0_BASE;

	uint32_t status = readl(&lcdc->int0);
	if (status & BIT(11)) {
		writel(0, &lcdc->int0); /* Disable all interrupts */
	}

	return status & BIT(11);
}
