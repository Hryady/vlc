--- qcom.h
+++ qcom.h
@@ -1,3 +1,14 @@
-#include <vlc_picture.h>
+#ifndef _QCOM_H_
+#define _QCOM_H_
 
-void qcom_convert(const uint8_t *src, picture_t *pic);
+#ifdef __cplusplus
+extern "C" {
+#endif
+
+void qcom_convert(const uint8_t *src, uint8_t* dst, size_t w, size_t h);
+
+#ifdef __cplusplus
+}
+#endif
+
+#endif
--- qcom.c
+++ qcom.c
@@ -20,18 +20,10 @@
  * Inc., 51 Franklin Street, Fifth Floor, Boston MA 02110-1301, USA.
  *****************************************************************************/
 
-#ifdef HAVE_CONFIG_H
-# include "config.h"
-#endif
-
-#include <vlc_picture.h>
-
-#include <string.h>
 #include <stdint.h>
-
+#include <string.h>
 #include "qcom.h"
 
-
 /*
  * The format is called QOMX_COLOR_FormatYUV420PackedSemiPlanar64x32Tile2m8ka.
  * First wtf: why call it YUV420? It is NV12 (interleaved U&V).
@@ -42,8 +34,7 @@
 #define TILE_SIZE (TILE_WIDTH * TILE_HEIGHT)
 
 /* get frame tile coordinate. XXX: nothing to be understood here, don't try. */
-static size_t tile_pos(size_t x, size_t y, size_t w, size_t h)
-{
+static size_t tile_pos(size_t x, size_t y, size_t w, size_t h) {
     size_t flim = x + (y & ~1) * w;
 
     if (y & 1) {
@@ -55,11 +46,10 @@
     return flim;
 }
 
-void qcom_convert(const uint8_t *src, picture_t *pic)
-{
-    size_t width = pic->format.i_width;
-    size_t pitch = pic->p[0].i_pitch;
-    size_t height = pic->format.i_height;
+void qcom_convert(const uint8_t *src, uint8_t* dst, size_t w, size_t h) {
+    size_t width = w; //pic->format.i_width;
+    size_t pitch = w; //pic->p[0].i_pitch;
+    size_t height = h; //pic->format.i_height;
 
     const size_t tile_w = (width - 1) / TILE_WIDTH + 1;
     const size_t tile_w_align = (tile_w + 1) & ~1;
@@ -70,21 +60,23 @@
     size_t luma_size = tile_w_align * tile_h_luma * TILE_SIZE;
 
 #define TILE_GROUP_SIZE (4 * TILE_SIZE)
-    if((luma_size % TILE_GROUP_SIZE) != 0)
+    if ((luma_size % TILE_GROUP_SIZE) != 0)
         luma_size = (((luma_size - 1) / TILE_GROUP_SIZE) + 1) * TILE_GROUP_SIZE;
 
-    for(size_t y = 0; y < tile_h_luma; y++) {
+    for (size_t y = 0; y < tile_h_luma; y++) {
         size_t row_width = width;
-        for(size_t x = 0; x < tile_w; x++) {
+        for (size_t x = 0; x < tile_w; x++) {
             /* luma source pointer for this tile */
-            const uint8_t *src_luma  = src
-                + tile_pos(x, y,tile_w_align, tile_h_luma) * TILE_SIZE;
+            const uint8_t *src_luma = src
+                    + tile_pos(x, y, tile_w_align, tile_h_luma) * TILE_SIZE;
 
             /* chroma source pointer for this tile */
             const uint8_t *src_chroma = src + luma_size
-                + tile_pos(x, y/2, tile_w_align, tile_h_chroma) * TILE_SIZE;
+                    + tile_pos(x, y / 2, tile_w_align, tile_h_chroma)
+                            * TILE_SIZE;
+
             if (y & 1)
-                src_chroma += TILE_SIZE/2;
+                src_chroma += TILE_SIZE / 2;
 
             /* account for right columns */
             size_t tile_width = row_width;
@@ -101,19 +93,21 @@
 
             /* dest chroma memory index for this tile */
             /* XXX: remove divisions */
-            size_t chroma_idx = (luma_idx / pitch) * pitch/2 + (luma_idx % pitch);
+            size_t chroma_idx = (luma_idx / pitch) * pitch / 2
+                    + (luma_idx % pitch) + w*h;
 
             tile_height /= 2; // we copy 2 luma lines at once
             while (tile_height--) {
-                memcpy(&pic->p[0].p_pixels[luma_idx], src_luma, tile_width);
+
+                memcpy(&dst[luma_idx], src_luma, tile_width);
                 src_luma += TILE_WIDTH;
                 luma_idx += pitch;
 
-                memcpy(&pic->p[0].p_pixels[luma_idx], src_luma, tile_width);
+                memcpy(&dst[luma_idx], src_luma, tile_width);
                 src_luma += TILE_WIDTH;
                 luma_idx += pitch;
 
-                memcpy(&pic->p[1].p_pixels[chroma_idx], src_chroma, tile_width);
+                memcpy(&dst[chroma_idx], src_chroma, tile_width);
                 src_chroma += TILE_WIDTH;
                 chroma_idx += pitch;
}
