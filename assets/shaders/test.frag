// #version 130 // enables bitwise operations
// precision lowp float; // highp -> vertex ~ mediump -> coords ~ lowp -> colors

varying vec2 fragTexCoord;

uniform sampler2D texture1;
uniform sampler2D texture2;

uniform vec4 t_s;

#define TILE_SIZE 16.
#define RGB 255.

void main()
{
  float tiles_x = (t_s.b / TILE_SIZE);
  float tiles_y = (t_s.a / TILE_SIZE);
  float uv_map_size_x = 1. / tiles_x;
  float uv_map_size_y = 1. / tiles_y;

  vec2 tile_uv = fragTexCoord.xy / vec2(tiles_y / t_s.r, tiles_y / t_s.g); 
  tile_uv.x /=  t_s.b / t_s.a;
  vec4 black_color = texture2D(texture2, fragTexCoord);

  vec4 tile_color;

  float x = black_color.r * RGB;
  float y = black_color.g * RGB;

  tile_color  = texture2D(texture1, tile_uv + vec2(
    uv_map_size_x * (x - floor(fragTexCoord.x * t_s.r)) ,//x
    uv_map_size_y * (y - floor(fragTexCoord.y * t_s.g)) //y
  ));

  tile_color *= black_color.a;

  gl_FragColor = tile_color;
}