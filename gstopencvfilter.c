/* GStreamer
 * Copyright (C) 2019 Peter J. Torelli <peter.j.torelli@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Suite 500,
 * Boston, MA 02110-1335, USA.
 */
/**
 * SECTION:element-gstopencvfilter
 *
 * The opencvfilter element inserts OpenCV into GStreamer.
 *
 * <refsect2>
 * <title>Example launch line</title>
 * |[
 * gst-launch-1.0 -v videotestsrc ! opencvfilter ! videoconvert ! ximagesink
 * ]|
 * The pipeline is a simple transform call.
 * </refsect2>
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gst/gst.h>
#include <gst/video/video.h>
#include <gst/video/gstvideofilter.h>
#include "gstopencvfilter.h"

GST_DEBUG_CATEGORY_STATIC (gst_opencvfilter_debug_category);
#define GST_CAT_DEFAULT gst_opencvfilter_debug_category

/* prototypes */

int openCvInterface(int height, int width, char *iuffer, char *obuffer);

static void gst_opencvfilter_set_property (GObject * object,
    guint property_id, const GValue * value, GParamSpec * pspec);
static void gst_opencvfilter_get_property (GObject * object,
    guint property_id, GValue * value, GParamSpec * pspec);
static void gst_opencvfilter_dispose (GObject * object);
static void gst_opencvfilter_finalize (GObject * object);

static gboolean gst_opencvfilter_start (GstBaseTransform * trans);
static gboolean gst_opencvfilter_stop (GstBaseTransform * trans);
static gboolean gst_opencvfilter_set_info (GstVideoFilter * filter, GstCaps * incaps,
    GstVideoInfo * in_info, GstCaps * outcaps, GstVideoInfo * out_info);
static GstFlowReturn gst_opencvfilter_transform_frame (GstVideoFilter * filter,
    GstVideoFrame * inframe, GstVideoFrame * outframe);
static GstFlowReturn gst_opencvfilter_transform_frame_ip (GstVideoFilter * filter,
    GstVideoFrame * frame);

enum
{
  PROP_0
};

/* pad templates */

/* FIXME: add/remove formats you can handle */
#define VIDEO_SRC_CAPS \
    GST_VIDEO_CAPS_MAKE("{ BGRA }")

/* FIXME: add/remove formats you can handle */
#define VIDEO_SINK_CAPS \
    GST_VIDEO_CAPS_MAKE("{ BGRA }")


/* class initialization */

G_DEFINE_TYPE_WITH_CODE (GstOpencvfilter, gst_opencvfilter, GST_TYPE_VIDEO_FILTER,
  GST_DEBUG_CATEGORY_INIT (gst_opencvfilter_debug_category, "opencvfilter", 0,
  "debug category for opencvfilter element"));

static void
gst_opencvfilter_class_init (GstOpencvfilterClass * klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
  GstBaseTransformClass *base_transform_class = GST_BASE_TRANSFORM_CLASS (klass);
  GstVideoFilterClass *video_filter_class = GST_VIDEO_FILTER_CLASS (klass);

  /* Setting up pads and setting metadata should be moved to
     base_class_init if you intend to subclass this class. */
  gst_element_class_add_pad_template (GST_ELEMENT_CLASS(klass),
      gst_pad_template_new ("src", GST_PAD_SRC, GST_PAD_ALWAYS,
        gst_caps_from_string (VIDEO_SRC_CAPS)));
  gst_element_class_add_pad_template (GST_ELEMENT_CLASS(klass),
      gst_pad_template_new ("sink", GST_PAD_SINK, GST_PAD_ALWAYS,
        gst_caps_from_string (VIDEO_SINK_CAPS)));

  gst_element_class_set_static_metadata (GST_ELEMENT_CLASS(klass),
      "FIXME Long name", "Generic", "FIXME Description",
      "FIXME <fixme@example.com>");

  gobject_class->set_property = gst_opencvfilter_set_property;
  gobject_class->get_property = gst_opencvfilter_get_property;
  gobject_class->dispose = gst_opencvfilter_dispose;
  gobject_class->finalize = gst_opencvfilter_finalize;
  base_transform_class->start = GST_DEBUG_FUNCPTR (gst_opencvfilter_start);
  base_transform_class->stop = GST_DEBUG_FUNCPTR (gst_opencvfilter_stop);
  video_filter_class->set_info = GST_DEBUG_FUNCPTR (gst_opencvfilter_set_info);
  video_filter_class->transform_frame = GST_DEBUG_FUNCPTR (gst_opencvfilter_transform_frame);
  video_filter_class->transform_frame_ip = GST_DEBUG_FUNCPTR (gst_opencvfilter_transform_frame_ip);

}

static void
gst_opencvfilter_init (GstOpencvfilter *opencvfilter)
{
}

void
gst_opencvfilter_set_property (GObject * object, guint property_id,
    const GValue * value, GParamSpec * pspec)
{
  GstOpencvfilter *opencvfilter = GST_OPENCVFILTER (object);

  GST_DEBUG_OBJECT (opencvfilter, "set_property");

  switch (property_id) {
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
  }
}

void
gst_opencvfilter_get_property (GObject * object, guint property_id,
    GValue * value, GParamSpec * pspec)
{
  GstOpencvfilter *opencvfilter = GST_OPENCVFILTER (object);

  GST_DEBUG_OBJECT (opencvfilter, "get_property");

  switch (property_id) {
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
  }
}

void
gst_opencvfilter_dispose (GObject * object)
{
  GstOpencvfilter *opencvfilter = GST_OPENCVFILTER (object);

  GST_DEBUG_OBJECT (opencvfilter, "dispose");

  /* clean up as possible.  may be called multiple times */

  G_OBJECT_CLASS (gst_opencvfilter_parent_class)->dispose (object);
}

void
gst_opencvfilter_finalize (GObject * object)
{
  GstOpencvfilter *opencvfilter = GST_OPENCVFILTER (object);

  GST_DEBUG_OBJECT (opencvfilter, "finalize");

  /* clean up object here */

  G_OBJECT_CLASS (gst_opencvfilter_parent_class)->finalize (object);
}

static gboolean
gst_opencvfilter_start (GstBaseTransform * trans)
{
  GstOpencvfilter *opencvfilter = GST_OPENCVFILTER (trans);

  GST_DEBUG_OBJECT (opencvfilter, "start");

  return TRUE;
}

static gboolean
gst_opencvfilter_stop (GstBaseTransform * trans)
{
  GstOpencvfilter *opencvfilter = GST_OPENCVFILTER (trans);

  GST_DEBUG_OBJECT (opencvfilter, "stop");

  return TRUE;
}

static gboolean
gst_opencvfilter_set_info (GstVideoFilter * filter, GstCaps * incaps,
    GstVideoInfo * in_info, GstCaps * outcaps, GstVideoInfo * out_info)
{
  GstOpencvfilter *opencvfilter = GST_OPENCVFILTER (filter);

  GST_DEBUG_OBJECT (opencvfilter, "set_info");

  return TRUE;
}

/* transform */
static GstFlowReturn
gst_opencvfilter_transform_frame (GstVideoFilter * filter, GstVideoFrame * inframe,
    GstVideoFrame * outframe)
{
  GstOpencvfilter *opencvfilter = GST_OPENCVFILTER (filter);

  gint height;
  gint width;
  guint8 *idata;
  guint8 *odata;

  idata = GST_VIDEO_FRAME_PLANE_DATA (inframe, 0);
  odata = GST_VIDEO_FRAME_PLANE_DATA (outframe, 0);
  width = GST_VIDEO_FRAME_COMP_WIDTH (inframe, 0);
  height = GST_VIDEO_FRAME_COMP_HEIGHT (inframe, 0);

  openCvInterface(height, width, idata, odata);

  return GST_FLOW_OK;
}

static GstFlowReturn
gst_opencvfilter_transform_frame_ip (GstVideoFilter * filter, GstVideoFrame * frame)
{
  GstOpencvfilter *opencvfilter = GST_OPENCVFILTER (filter);

  GST_DEBUG_OBJECT (opencvfilter, "transform_frame_ip");

  return GST_FLOW_OK;
}

static gboolean
plugin_init (GstPlugin * plugin)
{

  /* FIXME Remember to set the rank if it's an element that is meant
     to be autoplugged by decodebin. */
  return gst_element_register (plugin, "opencvfilter", GST_RANK_NONE,
      GST_TYPE_OPENCVFILTER);
}

/* FIXME: these are normally defined by the GStreamer build system.
   If you are creating an element to be included in gst-plugins-*,
   remove these, as they're always defined.  Otherwise, edit as
   appropriate for your external plugin package. */
#ifndef VERSION
#define VERSION "0.0.1"
#endif
#ifndef PACKAGE
#define PACKAGE "opencv"
#endif
#ifndef PACKAGE_NAME
#define PACKAGE_NAME "OpenCV Tools"
#endif
#ifndef GST_PACKAGE_ORIGIN
#define GST_PACKAGE_ORIGIN "no origin"
#endif

GST_PLUGIN_DEFINE (GST_VERSION_MAJOR,
    GST_VERSION_MINOR,
    opencvfilter,
    "OpenCV Video Filter",
    plugin_init, VERSION, "LGPL", PACKAGE_NAME, GST_PACKAGE_ORIGIN)

