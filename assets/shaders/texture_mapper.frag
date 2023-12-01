#ifdef GL_ES
precision mediump float;
#endif

uniform vec2 u_resolution;

uniform sampler2D u_texture_0; // default.png?filtering=nearest
uniform sampler2D u_texture_1; // BLACKMAP_8x8.png?filtering=nearest
uniform sampler2D u_texture_2; // TILEMAP_16x16_160x80.png?filtering=nearest
uniform sampler2D u_texture_3; // TILEMAP_10x10_100x50.png?filtering=nearest
uniform sampler2D u_texture_4; // BLACKMAP_12x12.png?filtering=nearest
uniform sampler2D u_texture_5; // BLACKMAP_10x10.png?filtering=nearest
uniform sampler2D u_texture_6; // actual_map.png?filtering=nearest

#define TILE_SIZE 16.
#define TILESET u_texture_0
#define T_W 368.
#define T_H 128.
#define BLACKMAP u_texture_6
#define B_W 30. // blackmap width
#define B_H 30. // blackmap height
#define ZOOM 3.

#define RGB 255.

void main()
{
    float         x;
    float         y;
    float      perc;
    float      unit;
    float      zoom;
    float tile_size;
    vec2       t_uv;
    vec2     ref_uv;
    vec2    ref_pxs;
    vec2    tile_uv;
    vec2   tile_pxs;    
    vec2  ref_tiles;
    vec4      color;
    vec4  ref_color;

    ref_pxs      = vec2(B_W, B_H);
    tile_pxs     = vec2(T_W, T_H);
    tile_size    = TILE_SIZE;
    ref_tiles    = vec2(tile_pxs.x / tile_size , tile_pxs.y / tile_size );
    tile_uv      = vec2(tile_size  / tile_pxs.x, tile_size  / tile_pxs.y);

    zoom     = ZOOM;
    ref_uv   = gl_FragCoord.xy / (ref_pxs  * zoom * tile_size);
    t_uv     = gl_FragCoord.xy / (tile_pxs * zoom);
    
    ref_color = texture2D(BLACKMAP, ref_uv);
    x = ref_color.r * RGB;
    y = (ref_tiles.y - 1.) - ref_color.g * RGB;

    perc = tile_size / tile_pxs.x;
    unit = tile_size / ref_pxs.x / tile_size;

    color = texture2D(TILESET, t_uv + vec2(
        (x * tile_uv.x) - (floor(ref_uv.x / unit) * tile_uv.x), 
        (y * tile_uv.y) - (floor(ref_uv.y / unit) * tile_uv.y)
    ));

    color *= ref_color.a;

    gl_FragColor = color;
}