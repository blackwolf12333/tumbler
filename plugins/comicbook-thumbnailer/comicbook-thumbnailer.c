/* vi:set et ai sw=2 sts=2 ts=2: */
/*-
 * Copyright (c) 2009 Jannis Pohlmann <jannis@xfce.org>
 * Copyright (c) 2011 Tam Merlant <tam.ille@free.fr>
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <sys/types.h>
#include <fcntl.h>
#include <memory.h>
#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>

#include <glib.h>
#include <glib/gi18n.h>
#include <glib-object.h>

#include <tumbler/tumbler.h>

#include <gdk-pixbuf/gdk-pixbuf.h>

#include <archive.h>
#include <archive_entry.h>

#include "comicbook-thumbnailer.h"

static void comicbook_thumbnailer_create (TumblerAbstractThumbnailer *thumbnailer,
                                    GCancellable               *cancellable,
                                    TumblerFileInfo            *info);



struct _ComicbookThumbnailerClass
{
  TumblerAbstractThumbnailerClass __parent__;
};

struct _ComicbookThumbnailer
{
  TumblerAbstractThumbnailer __parent__;
};



G_DEFINE_DYNAMIC_TYPE (ComicbookThumbnailer,
                       comicbook_thumbnailer,
                       TUMBLER_TYPE_ABSTRACT_THUMBNAILER);



void
comicbook_thumbnailer_register (TumblerProviderPlugin *plugin)
{
  comicbook_thumbnailer_register_type (G_TYPE_MODULE (plugin));
}



static void
comicbook_thumbnailer_class_init (ComicbookThumbnailerClass *klass)
{
  TumblerAbstractThumbnailerClass *abstractthumbnailer_class;

  abstractthumbnailer_class = TUMBLER_ABSTRACT_THUMBNAILER_CLASS (klass);
  abstractthumbnailer_class->create = comicbook_thumbnailer_create;
}



static void
comicbook_thumbnailer_class_finalize (ComicbookThumbnailerClass *klass)
{
}



static void
comicbook_thumbnailer_init (ComicbookThumbnailer *thumbnailer)
{
}




static GInputStream *
comicbook_thumbnailer_read_archive_first_file_to_stream(GFile *file)
{
  struct archive *a;
  struct archive_entry *entry;
  gint r;
  int size;
  void *buf = NULL;

  a = archive_read_new ();
  archive_read_support_format_rar (a);
  archive_read_support_format_zip (a);
  archive_read_support_format_7zip (a);
  archive_read_support_format_tar (a);

  // FIXME: what should the blocksize be? 10240 was used in the untar example in libarchive
  if ((r = archive_read_open_filename (a, g_file_get_path(file), 10240))) {
    return NULL;
  }

  r = archive_read_next_header (a, &entry);

  if (r == ARCHIVE_EOF || r != ARCHIVE_OK)
    return NULL;

  size = archive_entry_size(entry);
  archive_entry_free (entry);

  archive_read_data(a, buf, size);

  archive_read_close(a);
  archive_read_free (a);

  return g_memory_input_stream_new_from_data(buf, size, NULL);
}


static GdkPixbuf*
comicbook_thumbnailer_get_cover(TumblerAbstractThumbnailer *thumbnailer,
                                GFile *file)
{
  GdkPixbuf* pixbuf;
  GError *error;

  GInputStream *stream = comicbook_thumbnailer_read_archive_first_file_to_stream (file);

  pixbuf = gdk_pixbuf_new_from_stream(stream, NULL, &error);
  if (pixbuf == NULL)
    {
      g_signal_emit_by_name (thumbnailer, "error", "", error->code, error->message);
      g_error_free (error);
    }

  return pixbuf;
}


static void
comicbook_thumbnailer_create (TumblerAbstractThumbnailer *thumbnailer,
                        GCancellable               *cancellable,
                        TumblerFileInfo            *info)
{
  TumblerThumbnailFlavor *flavor;
  TumblerImageData        data;
  TumblerThumbnail       *thumbnail;
  const gchar            *uri;
  gchar                  *path;
  GdkPixbuf              *pixbuf = NULL;
  GError                 *error = NULL;
  GFile                  *file;
  gint                    height;
  gint                    width;
  GdkPixbuf              *scaled;

  g_return_if_fail (IS_COMICBOOK_THUMBNAILER (thumbnailer));
  g_return_if_fail (cancellable == NULL || G_IS_CANCELLABLE (cancellable));
  g_return_if_fail (TUMBLER_IS_FILE_INFO (info));

  /* do nothing if cancelled */
  if (g_cancellable_is_cancelled (cancellable))
    return;

  uri = tumbler_file_info_get_uri (info);
  file = g_file_new_for_uri (uri);

  thumbnail = tumbler_file_info_get_thumbnail (info);
  g_assert (thumbnail != NULL);

  /* get destination size */
  flavor = tumbler_thumbnail_get_flavor (thumbnail);
  g_assert (flavor != NULL);
  tumbler_thumbnail_flavor_get_size (flavor, &width, &height);
  g_object_unref (flavor);

  /* only handle local files */
  path = g_file_get_path (file);
  if (path != NULL && g_path_is_absolute (path))
    {
      pixbuf = comicbook_thumbnailer_get_cover(thumbnailer, file);

      if (pixbuf == NULL)
        {
          g_set_error_literal (&error, TUMBLER_ERROR, TUMBLER_ERROR_NO_CONTENT,
                               _("Thumbnail could not be inferred from file contents"));
        }
    }
  else
    {
      g_set_error_literal (&error, TUMBLER_ERROR, TUMBLER_ERROR_UNSUPPORTED,
                           _("Only local files are supported"));
    }

  g_free (path);
  g_object_unref (file);

  if (pixbuf != NULL)
    {
      scaled = scale_pixbuf (pixbuf, width, height);
      g_object_unref (pixbuf);
      pixbuf = scaled;

      data.data = gdk_pixbuf_get_pixels (pixbuf);
      data.has_alpha = gdk_pixbuf_get_has_alpha (pixbuf);
      data.bits_per_sample = gdk_pixbuf_get_bits_per_sample (pixbuf);
      data.width = gdk_pixbuf_get_width (pixbuf);
      data.height = gdk_pixbuf_get_height (pixbuf);
      data.rowstride = gdk_pixbuf_get_rowstride (pixbuf);
      data.colorspace = (TumblerColorspace) gdk_pixbuf_get_colorspace (pixbuf);

      tumbler_thumbnail_save_image_data (thumbnail, &data,
                                         tumbler_file_info_get_mtime (info),
                                         NULL, &error);
    }

  if (error != NULL)
    {
      g_signal_emit_by_name (thumbnailer, "error", uri, error->code, error->message);
      g_error_free (error);
    }
  else
    {
      g_signal_emit_by_name (thumbnailer, "ready", uri);
      g_object_unref (pixbuf);
    }

  g_object_unref (thumbnail);
}
