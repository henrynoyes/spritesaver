/* spritesaver, Copyright (c) 2025 Henry Noyes <noyes.he@northeastern.edu>
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation.  No representations are made about the suitability of this
 * software for any purpose.  It is provided "as is" without express or 
 * implied warranty.
 */

/* A minimal screensaver to display animated sprites. */

#include "screenhack.h"
#include "ximage-loader.h"

#include "sprites.h"

#define DEFAULT_SPEED 3
#define DEFAULT_ANIMATION_DELAY 2

typedef struct { Pixmap p, m; } PM;

struct state {
  Display *dpy;
  Window window;
  int Width, Height;
  GC fg_gc, bg_gc;
  
  int x, y;
  int dx, dy;
  int speed;
  
  PM left_frames[NUM_FRAMES];
  PM right_frames[NUM_FRAMES];
  int current_frame;
  int animation_delay;
  int animation_counter;
  
  int left_sprite_w, left_sprite_h;
  int right_sprite_w, right_sprite_h;
  
  unsigned long interval;
  Bool going_right;
};

/* inherited from noseguy.c */
static Pixmap
double_pixmap (Display *dpy, Visual *visual, int depth, Pixmap pixmap,
               int pix_w, int pix_h)
{
  int x, y;
  Pixmap p2 = XCreatePixmap(dpy, pixmap, pix_w*2, pix_h*2, depth);
  XImage *i1 = XGetImage (dpy, pixmap, 0, 0, pix_w, pix_h, ~0L, 
                          (depth == 1 ? XYPixmap : ZPixmap));
  XImage *i2 = XCreateImage (dpy, visual, depth, 
                             (depth == 1 ? XYPixmap : ZPixmap), 0, 0,
                             pix_w*2, pix_h*2, 8, 0);
  XGCValues gcv;
  GC gc = XCreateGC (dpy, p2, 0, &gcv);
  i2->data = (char *) calloc(i2->height, i2->bytes_per_line);
  for (y = 0; y < pix_h; y++)
    for (x = 0; x < pix_w; x++)
      {
	unsigned long p = XGetPixel(i1, x, y);
	XPutPixel(i2, x*2,   y*2,   p);
	XPutPixel(i2, x*2+1, y*2,   p);
	XPutPixel(i2, x*2,   y*2+1, p);
	XPutPixel(i2, x*2+1, y*2+1, p);
      }
  free(i1->data); i1->data = 0;
  XDestroyImage(i1);
  XPutImage(dpy, p2, gc, i2, 0, 0, 0, 0, i2->width, i2->height);
  XFreeGC (dpy, gc);
  free(i2->data); i2->data = 0;
  XDestroyImage(i2);
  XFreePixmap(dpy, pixmap);
  return p2;
}

static void
load_sprite_frames(struct state *st)
{
  XWindowAttributes xgwa;
  int i;
  
  XGetWindowAttributes(st->dpy, st->window, &xgwa);
  
  /* load frames */
  for (i = 0; i < NUM_FRAMES; i++) {
    Pixmap mask_left = 0, mask_right = 0;

    Pixmap pixmap_left = image_data_to_pixmap(st->dpy, st->window,
                                              left_frame_data[i].data,
                                              left_frame_data[i].size,
                                              &st->left_sprite_w, &st->left_sprite_h, &mask_left);
    st->left_frames[i].p = pixmap_left;
    st->left_frames[i].m = mask_left;

    Pixmap pixmap_right = image_data_to_pixmap(st->dpy, st->window,
                                               right_frame_data[i].data,
                                               right_frame_data[i].size,
                                               &st->right_sprite_w, &st->right_sprite_h, &mask_right);
    st->right_frames[i].p = pixmap_right;
    st->right_frames[i].m = mask_right;
}

  /* double sprites for high-res displays */
  if (xgwa.width > 2560 || xgwa.height > 2560) {
    for (i = 0; i < NUM_FRAMES; i++) {
      
      st->left_frames[i].p = double_pixmap(st->dpy, xgwa.visual, xgwa.depth,
                                           st->left_frames[i].p, st->left_sprite_w, st->left_sprite_h);
      st->left_frames[i].m = double_pixmap(st->dpy, xgwa.visual, 1,
                                           st->left_frames[i].m, st->left_sprite_w, st->left_sprite_h);
      st->right_frames[i].p = double_pixmap(st->dpy, xgwa.visual, xgwa.depth,
                                            st->right_frames[i].p, st->right_sprite_w, st->right_sprite_h);
      st->right_frames[i].m = double_pixmap(st->dpy, xgwa.visual, 1,
                                            st->right_frames[i].m, st->right_sprite_w, st->right_sprite_h);
    }
    st->left_sprite_w *= 2;
    st->left_sprite_h *= 2;
    st->right_sprite_w *= 2;
    st->right_sprite_h *= 2;
  }
}

static void
draw_sprite(struct state *st)
{
  PM *frame;
  int curr_sprite_w, curr_sprite_h;
  
  /* check direction */
  if (st->going_right) {
    frame = &st->right_frames[st->current_frame % NUM_FRAMES];
    curr_sprite_w = st->right_sprite_w;
    curr_sprite_h = st->right_sprite_h;
  } else {
    frame = &st->left_frames[st->current_frame % NUM_FRAMES];
    curr_sprite_w = st->left_sprite_w;
    curr_sprite_h = st->left_sprite_h;
  }
  
  /* draw with mask handling */
  if (frame->m) {
    XSetClipMask(st->dpy, st->fg_gc, frame->m);
    XSetClipOrigin(st->dpy, st->fg_gc, st->x, st->y);
  }
  
  XCopyArea(st->dpy, frame->p, st->window, st->fg_gc,
            0, 0, curr_sprite_w, curr_sprite_h, st->x, st->y);
  
  if (frame->m) {
    XSetClipMask(st->dpy, st->fg_gc, None);
  }
}

static void
update_position(struct state *st)
{
  int curr_sprite_w, curr_sprite_h;

  curr_sprite_w = st->going_right ? st->right_sprite_w : st->left_sprite_w;
  curr_sprite_h = st->going_right ? st->right_sprite_h : st->left_sprite_h;
  
  /* clear previous sprite
   * for unequal left/right sprite dims, there will be a single frame where fill is mismatched 
   * this can be fixed at the cost of efficiency by always clearing max w/h (see reshape)
   */
  XFillRectangle(st->dpy, st->window, st->bg_gc,
                 st->x, st->y, curr_sprite_w, curr_sprite_h);
  
  /* update position */
  st->x += st->dx;
  st->y += st->dy;
  
  /* handle boundaries */
  if (st->x <= 0) {
    st->x = 0;
    st->dx = st->speed;
    st->going_right = True;
  } else if (st->x >= st->Width - st->right_sprite_w) {
    st->x = st->Width - st->right_sprite_w;
    st->dx = -st->speed;
    st->going_right = False;
  }
  
  if (st->y <= 0) {
    st->y = 0;
    st->dy = st->speed;
  } else if (st->y >= st->Height - curr_sprite_h) {
    st->y = st->Height - curr_sprite_h;
    st->dy = -st->speed;
  }
  
}

static void
update_animation(struct state *st)
{
  st->animation_counter++;
  if (st->animation_counter >= st->animation_delay) {
    st->current_frame++;
    st->animation_counter = 0;
  }
}

/* xscreensaver api */

static const char *spritesaver_defaults[] = {
  ".background: black",
  ".foreground: white",
  "*speed: 3",
  "*animationDelay: 2",
  "*fpsSolid: true",
  0
};

static XrmOptionDescRec spritesaver_options[] = {
  { "-speed",          ".speed",         XrmoptionSepArg, 0 },
  { "-animation-delay", ".animationDelay", XrmoptionSepArg, 0 },
  { 0, 0, 0, 0 }
};

static void *
spritesaver_init(Display *dpy, Window window)
{
  struct state *st = (struct state *) calloc(1, sizeof(*st));
  XWindowAttributes xgwa;
  XGCValues gcvalues;
  unsigned long fg, bg;
  Colormap cmap;
  
  st->dpy = dpy;
  st->window = window;
  
  XGetWindowAttributes(st->dpy, st->window, &xgwa);
  st->Width = xgwa.width;
  st->Height = xgwa.height;
  cmap = xgwa.colormap;
  
  /* get resources */
  st->speed = get_integer_resource(st->dpy, "speed", "Integer");
  if (st->speed <= 0) st->speed = DEFAULT_SPEED;
  
  st->animation_delay = get_integer_resource(st->dpy, "animationDelay", "Integer");
  if (st->animation_delay <= 0) st->animation_delay = DEFAULT_ANIMATION_DELAY;
    
  fg = get_pixel_resource(st->dpy, cmap, "foreground", "Foreground");
  bg = get_pixel_resource(st->dpy, cmap, "background", "Background");
  
  /* create graphics contexts */
  gcvalues.foreground = fg;
  gcvalues.background = bg;
  st->fg_gc = XCreateGC(st->dpy, st->window, GCForeground|GCBackground, &gcvalues);
  
  gcvalues.foreground = bg;
  gcvalues.background = fg;
  st->bg_gc = XCreateGC(st->dpy, st->window, GCForeground|GCBackground, &gcvalues);
  
  load_sprite_frames(st);

  int max_sprite_w = (st->left_sprite_w > st->right_sprite_w) ? st->left_sprite_w : st->right_sprite_w;
  int max_sprite_h = (st->left_sprite_h > st->right_sprite_h) ? st->left_sprite_h : st->right_sprite_h;
  
  /* init animation */
  st->x = random() % (st->Width - max_sprite_w);
  st->y = random() % (st->Height - max_sprite_h);
  st->dx = (random() & 1) ? st->speed : -st->speed;
  st->dy = (random() & 1) ? st->speed : -st->speed;
  st->going_right = (st->dx > 0);
  
  st->current_frame = 0;
  st->animation_counter = 0;
  
  st->interval = 33; /* ~30fps */
  
  return st;
}

static unsigned long
spritesaver_draw(Display *dpy, Window window, void *closure)
{
  struct state *st = (struct state *) closure;
  
  update_position(st);
  update_animation(st);
  draw_sprite(st);
  
  return st->interval * 1000; /* convert to microseconds */
}

static void
spritesaver_reshape(Display *dpy, Window window, void *closure,
                     unsigned int w, unsigned int h)
{
  struct state *st = (struct state *) closure;
  st->Width = w;
  st->Height = h;

  int max_sprite_w = (st->left_sprite_w > st->right_sprite_w) ? st->left_sprite_w : st->right_sprite_w;
  int max_sprite_h = (st->left_sprite_h > st->right_sprite_h) ? st->left_sprite_h : st->right_sprite_h;

  if (st->x >= st->Width - max_sprite_w) {
    st->x = st->Width - max_sprite_w - 1;
  }
  if (st->y >= st->Height - max_sprite_h) {
    st->y = st->Height - max_sprite_h - 1;
  }
}

static Bool
spritesaver_event(Display *dpy, Window window, void *closure, XEvent *event)
{
  return False;
}

static void
spritesaver_free(Display *dpy, Window window, void *closure)
{
  struct state *st = (struct state *) closure;
  int i;
  
  for (i = 0; i < NUM_FRAMES; i++) {
    if (st->left_frames[i].p) XFreePixmap(dpy, st->left_frames[i].p);
    if (st->left_frames[i].m) XFreePixmap(dpy, st->left_frames[i].m);
    if (st->right_frames[i].p) XFreePixmap(dpy, st->right_frames[i].p);
    if (st->right_frames[i].m) XFreePixmap(dpy, st->right_frames[i].m);
  }
  
  XFreeGC(dpy, st->fg_gc);
  XFreeGC(dpy, st->bg_gc);
  
  free(st);
}

XSCREENSAVER_MODULE("SpriteSaver", spritesaver)