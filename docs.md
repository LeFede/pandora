# Pandora
## A game framework

The struct Map has all information related to current
map.

```c
typedef struct Map {
  Texture2D tileset; // The current tileset
  Texture2D layers[LAYERS]; // Each layer the current map needs
  RenderTexture2D render_textures[LAYERS]; // Canvas to draw
  char zoom; // Normalized so everything is bigger
  char pixel_per_tile; // Pixel per tile
  char current; // Current map
} Map;
```

All entities' components are stored in individual arrays. 
Index must be taken as the component ID.

Transform:
> _parent 
> _x
> _y
_w
_h

Kinetic:
> _vx
> _vy

