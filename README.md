# gba-voxel-engine
A 3D Voxel Engine for the GameBoy Advance

Building requires devkitPro.

Currently, the rendering of a "game-frame" (takes multiple normal frames) is split up into two stages:
 1. Traversing the blocks using a breadth-first search and adding any faces found to a list. This eliminates the need to sort the faces in Z-ordering.
  This is also frustum culled, I also plan on attempting to split this across multiple frames. Currently this takes around 75% - 140% of a single frame.
  I was inspired by this post, but using single blocks instead of chunks: http://tomcc.github.io/2014/08/31/visibility-1.html
 2. Rendering this list in reverse order. Each face is composed of two triangles and filled. 
  I'll try filling whole quads instead, and maybe splitting it in 2 with a single line to create the split color effect?
  There might also be some clever way of assembly unrolling the scanline filling? I'm not sure what the fastest way is, I'm not very experienced with ARM assembly.
  Currently this takes around 200% - 350% of a single frame.

If you have any ideas on further optimizations, I'd love to hear them!
