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

static void test_fb(struct fb_state_s *state); ////
static void test_tcon0(bool show_stats); ////

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
#ifdef NOTUSED
      //// Begin
      if (y % 80 == 1)
        {
          //printf("y=%d\n", y);
          test_tcon0(false);
        }
      //// End
#endif

      dest = ((FAR uint32_t *)row) + area->x;
      for (x = 0; x < area->w; x++)
        {
          *dest++ = g_rgb24[color];
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
          *dest++ = g_rgb16[color];
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
          *dest++ = g_rgb8[color];
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
    }
}

static void draw_rect(FAR struct fb_state_s *state,
                      FAR struct fb_area_s *area, int color)
{
  test_tcon0(false);////

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
  test_tcon0(true); ////

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

#ifdef NOTUSED_CONFIG_FB_OVERLAY ////
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

#ifdef NOTUSED
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

      for (int i = 0; i < 5; i++)
        {
          test_tcon0(false); //// TODO
        }

      usleep(500 * 1000);
      sleep(1); //// TODO

      width  -= (2 * xstep);
      height -= (2 * ystep);
    }

  test_tcon0(true); ////
  printf("Test finished\n");
#else
  // Test Framebuffer
  test_fb(&state);
  UNUSED(x); UNUSED(y); UNUSED(color); UNUSED(height); UNUSED(width); UNUSED(ystep); UNUSED(xstep); UNUSED(nsteps); UNUSED(area);
  UNUSED(draw_rect32); UNUSED(draw_rect);
#endif  // NOTUSED

  munmap(state.fbmem, state.pinfo.fblen);
  close(state.fd);
  return EXIT_SUCCESS;
}

///////////////////////////////////////////////////////////////////////
// Test Framebuffer

static void test_fb(struct fb_state_s *state) {
  // Fill entire framebuffer with grey
  memset(
    state->pinfo.fbmem,
    0x80,
    state->pinfo.fblen
  );

  // Fill entire framebuffer with grey
  // memset(
  //   state->pinfo.fbmem,
  //   0x90,
  //   state->pinfo.fblen
  // );
}

///////////////////////////////////////////////////////////////////////
// Test TCON0

#include "../arch/arm64/src/a64/hardware/a64_memorymap.h"
#include "../arch/arm64/src/common/arm64_arch.h"

/* TCON Global Interrupt Register 0 (A64 Page 509) */

#define TCON_GINT0_REG          (A64_TCON0_ADDR + 0x04)

#define SUN4I_TCON_GINT0_VBLANK_ENABLE(pipe)		BIT(31 - (pipe))
#define SUN4I_TCON_GINT0_TCON0_TRI_FINISH_ENABLE	BIT(27)
#define SUN4I_TCON_GINT0_VBLANK_INT(pipe)		BIT(15 - (pipe))
#define SUN4I_TCON_GINT0_TCON0_TRI_FINISH_INT		BIT(11)

static void test_tcon0(bool show_stats)
{
  static int i = 0;
  static int tcon0_vblank = 0;
  static int tcon1_vblank = 0;
  static int tcon0_tri_finish = 0;

  // Enable TCON0 VBLANK, TCON1 VBLANK, TCON0 CPU Trigger Finish
  if (i == 0)
    {
      uint32_t mask = SUN4I_TCON_GINT0_VBLANK_ENABLE(0) |
        SUN4I_TCON_GINT0_VBLANK_ENABLE(1) |
        SUN4I_TCON_GINT0_TCON0_TRI_FINISH_ENABLE;
      modreg32(mask, mask, TCON_GINT0_REG);
    }

  // Wait for TCON0 CPU Trigger Finish
  for (; i < (1 << 25); i++)
    {
      // Count TCON0 VBLANK, TCON1 VBLANK, TCON0 CPU Trigger Finish
      uint32_t val = getreg32(TCON_GINT0_REG);
      uint32_t mask = SUN4I_TCON_GINT0_VBLANK_INT(0) |
			   SUN4I_TCON_GINT0_VBLANK_INT(1) |
			   SUN4I_TCON_GINT0_TCON0_TRI_FINISH_INT;

      if (val & SUN4I_TCON_GINT0_VBLANK_INT(0)) { tcon0_vblank++; modreg32(0, mask, TCON_GINT0_REG); }
      if (val & SUN4I_TCON_GINT0_VBLANK_INT(1)) { tcon1_vblank++; modreg32(0, mask, TCON_GINT0_REG); }
      if (val & SUN4I_TCON_GINT0_TCON0_TRI_FINISH_INT) { tcon0_tri_finish++; modreg32(0, mask, TCON_GINT0_REG); break; }
    }

    if (show_stats)
      {
        printf("tcon0_vblank=%d\n", tcon0_vblank);
        printf("tcon1_vblank=%d\n", tcon1_vblank);
        printf("tcon0_tri_finish=%d\n", tcon0_tri_finish);
        printf("i=%d\n", i);
      }
}
