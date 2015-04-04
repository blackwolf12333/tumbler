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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <glib.h>
#include <glib-object.h>
#include <glib/gi18n.h>

#include <gdk-pixbuf/gdk-pixbuf.h>
#include <tumbler/tumbler.h>

#include <comicbook-thumbnailer/comicbook-thumbnailer-provider.h>
#include <comicbook-thumbnailer/comicbook-thumbnailer.h>



static void   comicbook_thumbnailer_provider_thumbnailer_provider_init (TumblerThumbnailerProviderIface *iface);
static GList *comicbook_thumbnailer_provider_get_thumbnailers          (TumblerThumbnailerProvider      *provider);
static void   comicbook_thumbnailer_provider_finalize                  (GObject                         *object);



struct _ComicbookThumbnailerProviderClass
{
  GObjectClass __parent__;
};

struct _ComicbookThumbnailerProvider
{
  GObject __parent__;
};



G_DEFINE_DYNAMIC_TYPE_EXTENDED (ComicbookThumbnailerProvider,
                                comicbook_thumbnailer_provider,
                                G_TYPE_OBJECT,
                                0,
                                TUMBLER_ADD_INTERFACE (TUMBLER_TYPE_THUMBNAILER_PROVIDER,
                                                       comicbook_thumbnailer_provider_thumbnailer_provider_init));



void
comicbook_thumbnailer_provider_register (TumblerProviderPlugin *plugin)
{
  comicbook_thumbnailer_provider_register_type (G_TYPE_MODULE (plugin));
}



static void
comicbook_thumbnailer_provider_class_init (ComicbookThumbnailerProviderClass *klass)
{
  GObjectClass *gobject_class;

  gobject_class = G_OBJECT_CLASS (klass);
  gobject_class->finalize = comicbook_thumbnailer_provider_finalize;
}



static void
comicbook_thumbnailer_provider_class_finalize (ComicbookThumbnailerProviderClass *klass)
{
}



static void
comicbook_thumbnailer_provider_thumbnailer_provider_init (TumblerThumbnailerProviderIface *iface)
{
  iface->get_thumbnailers = comicbook_thumbnailer_provider_get_thumbnailers;
}



static void
comicbook_thumbnailer_provider_init (ComicbookThumbnailerProvider *provider)
{
}



static void
comicbook_thumbnailer_provider_finalize (GObject *object)
{
  (*G_OBJECT_CLASS (comicbook_thumbnailer_provider_parent_class)->finalize) (object);
}



static GList *
comicbook_thumbnailer_provider_get_thumbnailers (TumblerThumbnailerProvider *provider)
{
  ComicbookThumbnailer   *thumbnailer;
  GList              *thumbnailers = NULL;
  GStrv               uri_schemes;
  static const gchar *mime_types[] =
  {
    "application/x-cbr",
    NULL
  };

  /* determine the URI schemes supported by GIO */
  uri_schemes = tumbler_util_get_supported_uri_schemes ();

  /* create the pixbuf thumbnailer */
  thumbnailer = g_object_new (TYPE_COMICBOOK_THUMBNAILER,
                              "uri-schemes", uri_schemes,
                              "mime-types", mime_types,
                              NULL);

  /* add the thumbnailer to the list */
  thumbnailers = g_list_append (thumbnailers, thumbnailer);

  /* free URI schemes */
  g_strfreev (uri_schemes);

  return thumbnailers;
}
