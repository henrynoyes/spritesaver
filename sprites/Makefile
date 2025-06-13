# Minimal Makefile for spritesaver

.SUFFIXES:
.SUFFIXES: .c .o

srcdir		= .
top_srcdir	= ../..
top_builddir	= ../..

prefix		= /usr/local
exec_prefix	= ${prefix}
libexecdir	= ${exec_prefix}/libexec
datarootdir	= ${prefix}/share
datadir		= ${datarootdir}

HACKDIR		= ${libexecdir}/xscreensaver
HACK_CONF_DIR	= ${datadir}/xscreensaver/config

CC		= gcc -std=gnu99 -pedantic -Wall -Wnested-externs -Wstrict-prototypes -Wmissing-prototypes -Wno-overlength-strings
CFLAGS		= -g -O2
LDFLAGS		= -L${exec_prefix}/lib
DEFS		= -DSTANDALONE -DHAVE_CONFIG_H
LIBS		= 

SHELL		= /bin/sh
INSTALL		= /usr/bin/install -c
INSTALL_PROGRAM = ${INSTALL}
INSTALL_DATA	= ${INSTALL} -m 644
INSTALL_DIRS	= ${INSTALL} -d

X_PRE_LIBS	= -lSM -lICE
XFT_LIBS	= -lXft

HACK_PRE	= $(LIBS)
HACK_POST	= $(X_PRE_LIBS) $(XFT_LIBS) -lXt -lX11 -lXext -lm
HACK_LIBS	= $(HACK_PRE) $(HACK_POST)
PNG_LIBS	= $(HACK_PRE) -lgdk_pixbuf-2.0 -lgio-2.0 -lgobject-2.0 -lglib-2.0 $(HACK_POST)

UTILS_SRC	= $(top_srcdir)/utils
UTILS_BIN	= $(top_builddir)/utils
HACKS_SRC	= $(top_srcdir)/hacks

INCLUDES_1	= -I. -I$(srcdir) -I$(UTILS_SRC) -I$(HACKS_SRC) -I$(top_builddir) -I$(sprite_dir)
INCLUDES	= $(INCLUDES_1) -I/usr/include/freetype2 -I/usr/include/libpng16 -I/usr/include/gdk-pixbuf-2.0 -I/usr/include/glib-2.0 -I/usr/lib/x86_64-linux-gnu/glib-2.0/include

HACK_OBJS_1	= $(HACKS_SRC)/fps.o $(UTILS_BIN)/resources.o $(UTILS_BIN)/visual.o \
		  $(UTILS_BIN)/usleep.o $(UTILS_BIN)/yarandom.o \
		  $(UTILS_BIN)/utf8wc.o $(UTILS_BIN)/font-retry.o \
		  $(UTILS_BIN)/xmu.o

HACK_OBJS	= $(HACKS_SRC)/screenhack.o $(HACK_OBJS_1)

EXES		= spritesaver

HACK_CFLAGS_BASE=$(INCLUDES) $(DEFS) $(CPPFLAGS) $(CFLAGS)
.c.o:
	$(CC) -c $(HACK_CFLAGS_BASE) $<

CC_HACK		= $(CC) $(LDFLAGS)

PNG		= $(HACKS_SRC)/ximage-loader.o

default: all
all: check_sprite _gen $(EXES)

check_sprite:
	@if [ -z "$(sprite_dir)" ]; then \
		echo "Error: sprite_dir variable not set."; \
		echo "Usage: make sprite_dir=sprite_name"; \
		echo ""; \
		echo "Available sprites:"; \
		for dir in $(srcdir); do \
			if [ -d "$$dir" ]; then \
				echo "  ${dir##*/}"; \
			fi; \
		done; \
		exit 1; \
	fi
	@if [ ! -d "$(sprite_dir)" ]; then \
		echo "Error: Sprite directory '$(sprite_dir)' not found."; \
		exit 1; \
	fi
headers: _gen

_gen:
	@\
	DIR="$(sprite_dir)/sprite_headers" ;							\
	if ! [ -d "$$DIR" ]; then					\
	        mkdir "$$DIR" ;						\
	fi ;								\
									\
	for png in $$(find $(sprite_dir)/sprite_images -name \*.png); do			\
	  h="$${png%.png}";						\
	  h="$${h##*/}";						\
	  h="$$DIR/$${h}_png.h";					\
	  if [ ! -f "$$h" -o "$$png" -nt "$$h" ] ; then			\
	         $(UTILS_SRC)/bin2c "$$png" "$$h";			\
	  fi ;								\
	done

spritesaver:	spritesaver.o	$(HACK_OBJS) $(PNG)
	$(CC_HACK) -o $@ $@.o	$(HACK_OBJS) $(PNG) $(PNG_LIBS)

$(HACKS_SRC)/screenhack.o:
	$(MAKE) -C $(HACKS_SRC) screenhack.o CC="$(CC)" CFLAGS="$(CFLAGS)" LDFLAGS="$(LDFLAGS)"

$(HACKS_SRC)/fps.o:
	$(MAKE) -C $(HACKS_SRC) fps.o CC="$(CC)" CFLAGS="$(CFLAGS)" LDFLAGS="$(LDFLAGS)"

$(HACKS_SRC)/ximage-loader.o:
	$(MAKE) -C $(HACKS_SRC) ximage-loader.o CC="$(CC)" CFLAGS="$(CFLAGS)" LDFLAGS="$(LDFLAGS)"

$(UTILS_BIN)/resources.o:	$(UTILS_SRC)/resources.c
$(UTILS_BIN)/visual.o:		$(UTILS_SRC)/visual.c
$(UTILS_BIN)/usleep.o:		$(UTILS_SRC)/usleep.c
$(UTILS_BIN)/yarandom.o:	$(UTILS_SRC)/yarandom.c
$(UTILS_BIN)/utf8wc.o:		$(UTILS_SRC)/utf8wc.c
$(UTILS_BIN)/font-retry.o:	$(UTILS_SRC)/font-retry.c
$(UTILS_BIN)/xmu.o:		$(UTILS_SRC)/xmu.c

$(UTILS_BIN)/resources.o $(UTILS_BIN)/visual.o $(UTILS_BIN)/usleep.o $(UTILS_BIN)/yarandom.o $(UTILS_BIN)/utf8wc.o $(UTILS_BIN)/font-retry.o $(UTILS_BIN)/xmu.o:
	$(MAKE) -C $(UTILS_BIN) $(@F) CC="$(CC)" CFLAGS="$(CFLAGS)" LDFLAGS="$(LDFLAGS)"

help:
	@echo "Usage:"
	@echo "  make sprite_dir=sprite_name   - Build with specified sprite"
	@echo "  make clean                  - Remove built files"
	@echo "  make help                   - Show this help"
	@echo ""
	@echo "Available sprites:"
	@for dir in $(srcdir); do \
		if [ -d "$$dir" ]; then \
			echo "  ${dir##*/}"; \
		fi; \
	done

clean::
	-rm -f *.o a.out core spritesaver

spritesaver.o: $(HACKS_SRC)/screenhack.h $(HACKS_SRC)/ximage-loader.h $(sprite_dir)/sprites.h