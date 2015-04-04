/* vi:set et ai sw=2 sts=2 ts=2: */
/*-
 * Copyright (c) 2012 Nick Schermer <nick@xfce.org>
 * Copyright (c) 2019 Peter Maatman <blackwolf12333@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef __COMICBOOK_THUMBNAILER_H__
#define __COMICBOOK_THUMBNAILER_H__

#include <glib-object.h>

G_BEGIN_DECLS

#define TYPE_COMICBOOK_THUMBNAILER            (comicbook_thumbnailer_get_type ())
#define COMICBOOK_THUMBNAILER(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), TYPE_COMICBOOK_THUMBNAILER, ComicbookThumbnailer))
#define COMICBOOK_THUMBNAILER_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), TYPE_COMICBOOK_THUMBNAILER, ComicbookThumbnailerClass))
#define IS_COMICBOOK_THUMBNAILER(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TYPE_COMICBOOK_THUMBNAILER))
#define IS_COMICBOOK_THUMBNAILER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), TYPE_COMICBOOK_THUMBNAILER)
#define COMICBOOK_THUMBNAILER_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), TYPE_COMICBOOK_THUMBNAILER, ComicbookThumbnailerClass))

typedef struct _ComicbookThumbnailerClass   ComicbookThumbnailerClass;
typedef struct _ComicbookThumbnailer        ComicbookThumbnailer;

GType comicbook_thumbnailer_get_type (void) G_GNUC_CONST;
void  comicbook_thumbnailer_register (TumblerProviderPlugin *plugin);

G_END_DECLS

#endif /* !__COMICBOOK_THUMBNAILER_H__ */
