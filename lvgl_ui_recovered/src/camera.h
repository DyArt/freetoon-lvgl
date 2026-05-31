/*
 * camera.h — Toon 1 live video tile glue.
 *
 * Always-warm pipeline: vpu_stream is forked once at toonui startup with
 * --warm and keeps the TCP socket + VPU decoder hot, decoding silently
 * in the background. Tile tap (or HA trigger) signals SIGUSR1 to start
 * blitting at the next I-frame; closing signals SIGUSR2 to stop. This
 * gets us sub-second show latency from a cold "Camera" tap.
 *
 * The rect is read from settings (camera_x/y/w/h, all in panel pixels),
 * with sensible defaults so it works out of the box. If those settings
 * change while the warm child is running, the next camera_open() kills
 * and respawns it with the new rect (vpu_stream takes --rect on argv).
 */
#ifndef CAMERA_H
#define CAMERA_H

#include "lvgl/lvgl.h"

/* Add a "Camera" button at LV_ALIGN_BOTTOM_LEFT on the given parent.
 * Tap opens the video; the close placeholder is created on top of the
 * stream on first open. */
void camera_install_button(lv_obj_t * parent);

/* Lifecycle.
 *   camera_init()     -- called once during toonui startup, spawns the
 *                        warm vpu_stream child. No-op if camera disabled.
 *   camera_shutdown() -- called on clean toonui exit, SIGTERMs the child.
 *   camera_open()     -- tile tap / HA trigger, show video.
 *   camera_close()    -- overlay tap / HA hide, stop blitting (child stays warm).
 */
void camera_init(void);
void camera_shutdown(void);
void camera_open(void);
void camera_close(void);

#endif
