#include <math.h>
#include <raylib.h>
#include <raymath.h>
#include <rlgl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
// #include <time.h>
// #include <stdlib.h>
#define BACKGROUND_DEFAULT_COLOR (Color){ 255,   0,   0, 255 } 

#define  SCREEN_WIDTH 1280 
#define SCREEN_HEIGHT 720  
#define  SCREEN_TITLE "HOLA"

#define MAX_ENTITIES     256  //(1 << (sizeof(unit) * 8)) - 1
#define TOTAL_ENTITIES   8 // PREFABS
#define TOTAL_COMPONENTS 13 // Components

typedef unsigned char   u8;
typedef unsigned short u16;
typedef unsigned int   u32;
typedef unsigned long  u64;
typedef u8 unit;

typedef signed char   i8;
typedef signed short i16;
typedef signed int   i32;
typedef signed long  i64;

#define LAYERS 3

enum Prefabs 
{
  pf_default = 0,
  pf_human_1,
  pf_human_2,
  pf_skeleton_1,
  pf_goblin_1,
  pf_elf_1,
  pf_orc_1,
  pf_pig_1,
};

enum Components
{
  c_t = 0, c_f,
  // Transform ~
  c_p, c_x, c_y, c_rx, c_ry,
  // Kinetic ~
  c_vx, c_vy,
  // Sprite
  c_sprite, c_frame, c_frame_rate,
  // Control
  c_control
};

enum Components2
{
  X
};

enum Flags2
{
  // f_Transform = 1 << 0,
  f_Active  = 1 << 0,
  f_Kinetic = 1 << 1,
  f_Sprite  = 1 << 2,
};

typedef struct Map 
{
  Texture2D tileset;
  Texture2D layers[LAYERS];
  RenderTexture2D render_textures[LAYERS];
  char zoom;
  char pixel_per_tile;
  char current;
} Map;

typedef struct Assets {
  Texture2D sprites[10];
  Texture2D tilesets[10];
  Texture2D layers[10];
} Assets;

Assets assets;

enum Flags
{
  ACTIVE    = 1 << 0,
  KINETIC   = 1 << 1,
  SPRITE    = 1 << 2,
  CONTROL   = 1 << 3,
};

enum Types
{
  DEFAULT  = 0,
  HUMAN    ,
  SKELETON ,
  GOBLIN   ,
  ELF      ,
  ORC      ,
  PIG      ,
};

void input();
void game_loop();
void draw();

// Components
unit _t [MAX_ENTITIES] = { 0 };
unit _f [MAX_ENTITIES] = { 0 };
// Transform ~
u32  _p [MAX_ENTITIES] = { 0 }; 
u32  _x [MAX_ENTITIES] = { 0 }; 
u32  _y [MAX_ENTITIES] = { 0 }; 
u32 _rx [MAX_ENTITIES] = { 0 }; 
u32 _ry [MAX_ENTITIES] = { 0 }; 

// Kinetic ~
i32 _vx [MAX_ENTITIES] = { 0 };
i32 _vy [MAX_ENTITIES] = { 0 };

// Sprite ~
Texture2D _sprite [MAX_ENTITIES] = { 0 };
u8 _frame[MAX_ENTITIES] = { 0 };
u32 _frame_rate[MAX_ENTITIES] = { 0 };

// Control ~
enum Control {
  k_Enter = 1 << 0,
  k_W     = 1 << 1,
  k_A     = 1 << 2,
  k_S     = 1 << 3,
  k_D     = 1 << 4,
};
u32 _control = 0;


// Miscelaneous
unit       LAST_FREE_INDEX = 1; // 0 means full
unit       ACTIVE_ENTITIES = 0;
unit     last_entity_added = 0;
unit last_entity_destroyed = 0;
unit           last_entity = 0;
// unit first_entity = 0;
Camera2D cam;

Texture2D entity_1;

// Entity pre[TOTAL_ENTITIES] = { 0 };
i32 prefabs[TOTAL_ENTITIES][TOTAL_COMPONENTS] = { 0 };

void clean_entities() 
{
  for (unit *ptr = &_f[0]; ptr < &_f[last_entity]; ptr++) 
    *ptr &= ~ACTIVE; 

  printf("\n✅ Destroyed %d entities\n", ACTIVE_ENTITIES);

  ACTIVE_ENTITIES       = 0;
  LAST_FREE_INDEX       = 1;
  last_entity_added     = 0;
  last_entity_destroyed = 0;
  last_entity           = 0;
}

unit destroy_entity(unit id)
{
  if (_f[id] & ACTIVE)
  {
    _f[id] &= ~ACTIVE;
    // LAST_FREE_INDEX = id;

    ACTIVE_ENTITIES--;
    printf("\n✅ Destroyed {%d} at [%d] ~ %d entities", _t[id], id, ACTIVE_ENTITIES);
    return id;
  }

  printf("\n❌ [%d] empty", id);
  return last_entity_destroyed;
}

unit add_entity(unit type)
{
  if (LAST_FREE_INDEX == 255) LAST_FREE_INDEX = 0;
  // Check if there is room
  if (LAST_FREE_INDEX == 0)
  {
    // If not returns previous entity added
    printf("\n❌ No room");
    return 0;
  }

  // Holds how many entities currently are
  if (last_entity < LAST_FREE_INDEX ) last_entity = LAST_FREE_INDEX;

  // Set components' values
  _f        [LAST_FREE_INDEX] =  prefabs[type][c_f];
  _t        [LAST_FREE_INDEX] =  type;
  _x        [LAST_FREE_INDEX] =  prefabs[type][c_x];
  _y        [LAST_FREE_INDEX] =  prefabs[type][c_y];
  _rx       [LAST_FREE_INDEX] =  prefabs[type][c_rx];
  _ry       [LAST_FREE_INDEX] =  prefabs[type][c_ry];
  _vx       [LAST_FREE_INDEX] =  prefabs[type][c_vx];
  _vy       [LAST_FREE_INDEX] =  prefabs[type][c_vy];
  _p        [LAST_FREE_INDEX] =  0;
  _sprite   [LAST_FREE_INDEX] =  assets.sprites[prefabs[type][c_sprite]];
  _frame    [LAST_FREE_INDEX] =  0;
  _frame_rate[LAST_FREE_INDEX] = prefabs[type][c_frame_rate];

  // entities.transform.rx[LAST_FREE_INDEX] =  pre[type].tran.rx;
  // entities.transform.ry[LAST_FREE_INDEX] =  pre[type].tran.ry;
  // _w        [LAST_FREE_INDEX] =  pre[type];
  // _h        [LAST_FREE_INDEX] =  pre[type][3];
  // _vx       [LAST_FREE_INDEX] =  pre[typ
  // _vy       [LAST_FREE_INDEX] =  pre[type][5];
  // _f    [LAST_FREE_INDEX] =  SPRITE | ACTIVE;
  // _t     [LAST_FREE_INDEX] =  type;
  // _parent   [LAST_FREE_INDEX] =  0;
  // entities.sprite.texture[LAST_FREE_INDEX] =  pre[type].spr.texture;
  // _frame    [LAST_FREE_INDEX] =  0;

  last_entity_added = LAST_FREE_INDEX;

  // Return this entity ID and adds one to it so 
  // next entity will take that ID
  ACTIVE_ENTITIES++;
  printf("\n✅ Added {%d} at [%d] ~ %d entities\n", type, LAST_FREE_INDEX, ACTIVE_ENTITIES);
  return LAST_FREE_INDEX++;
}

Sound    sound;
Music    music;
Font  fonts[3];
Map        map;

#define  MUSIC_1 "assets/music/1.mp3"
#define  MUSIC_2 "assets/music/2.mp3"
#define  SOUND_1 "assets/sounds/1.wav"
#define   FONT_1 "assets/fonts/1.ttf"
#define   FONT_2 "assets/fonts/2.ttf"
#define   FONT_3 "assets/fonts/Minecraft.ttf"
#define SHADER_1 "assets/shaders/test.frag"
#define   TILE_1 "assets/textures/tileset_1.png"
#define  LAYER_1 "assets/textures/layer_1.png"  
#define  LAYER_2 "assets/textures/layer_2.png"  
#define  LAYER_3 "assets/textures/layer_3.png"  
#define  LAYER_4 "assets/textures/layer_4.png"  
#define  LAYER_5 "assets/textures/layer_5.png"  
#define  LAYER_6 "assets/textures/layer_6.png"  
#define ENTITY_1 "assets/textures/entity_1.png"  
#define ENTITY_2 "assets/textures/entity_2.png"  
#define ENTITY_3 "assets/textures/entity_3.png"  
#define ENTITY_4 "assets/textures/entity_4.png"  
#define ENTITY_5 "assets/textures/entity_5.png"  
#define ENTITY_6 "assets/textures/entity_6.png"  

void clear_map() 
{
  UnloadTexture(map.tileset);
  for(short i = 0; i < LAYERS; i++) {
    UnloadRenderTexture(map.render_textures[i]);
    UnloadTexture(map.layers[i]);
  }
}

#define scene_2 do {\
  \
  clear_map();\
  map.current = 2;\
  \
  map.tileset   = LoadTexture(TILE_1);\
  map.layers[0] = LoadTexture(LAYER_3);\
  map.layers[1] = LoadTexture(LAYER_4);\
  map.layers[2] = LoadTexture(LAYER_6);\
  \
  for(short i = 0; i < LAYERS; i++)\
  {\
    map.render_textures[i] = LoadRenderTexture(\
      map.layers[i].width * map.zoom * map.pixel_per_tile,\
      map.layers[i].height * map.zoom * map.pixel_per_tile\
    );\
  }\
} while(0)

#define scene_3 do {\
  \
  clear_map();\
  map.current = 3;\
  \
  map.tileset   = LoadTexture(TILE_1);\
  map.layers[0] = LoadTexture(LAYER_3);\
  map.layers[1] = LoadTexture(LAYER_4);\
  map.layers[2] = LoadTexture(LAYER_5);\
  \
  for(short i = 0; i < LAYERS; i++)\
  {\
    map.render_textures[i] = LoadRenderTexture(\
      map.layers[i].width * map.zoom * map.pixel_per_tile,\
      map.layers[i].height * map.zoom * map.pixel_per_tile\
    );\
  }\
} while(0)


int main()
{
  map.zoom = 3;
  map.pixel_per_tile = 16;

  SetConfigFlags(FLAG_VSYNC_HINT);
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_TITLE);
  InitAudioDevice();

  assets.sprites[HUMAN] = LoadTexture(ENTITY_1);
  assets.sprites[SKELETON] = LoadTexture(ENTITY_2);
  assets.sprites[GOBLIN] = LoadTexture(ENTITY_3);
  assets.sprites[ELF] = LoadTexture(ENTITY_4);
  assets.sprites[ORC] = LoadTexture(ENTITY_5);
  assets.sprites[PIG] = LoadTexture(ENTITY_6);

  assets.tilesets[0] = LoadTexture(TILE_1);
  assets.layers[0] = LoadTexture(LAYER_1);
  assets.layers[1] = LoadTexture(LAYER_2);
  assets.layers[2] = LoadTexture(LAYER_3);

  prefabs[pf_human_1][c_f] = ACTIVE | KINETIC | SPRITE;
  prefabs[pf_human_1][c_sprite] = HUMAN;
  prefabs[pf_human_1][c_rx] = 20;
  prefabs[pf_human_1][c_frame_rate] = 4;

  prefabs[pf_human_2][c_f] = ACTIVE | KINETIC | SPRITE;
  prefabs[pf_human_2][c_rx] = 100;
  prefabs[pf_human_2][c_sprite] = HUMAN;
  prefabs[pf_human_2][c_frame_rate] = 6;

  prefabs[pf_skeleton_1][c_f] = ACTIVE | KINETIC | SPRITE;
  prefabs[pf_skeleton_1][c_ry] = 20;
  prefabs[pf_skeleton_1][c_sprite] = SKELETON;
  prefabs[pf_skeleton_1][c_frame_rate] = 7;

  prefabs[pf_goblin_1][c_f] = ACTIVE | KINETIC | SPRITE;
  prefabs[pf_goblin_1][c_sprite] = GOBLIN;
  prefabs[pf_goblin_1][c_ry] = 40;
  prefabs[pf_goblin_1][c_frame_rate] = 3;

  prefabs[pf_elf_1][c_f] = ACTIVE | KINETIC | SPRITE;
  prefabs[pf_elf_1][c_sprite] = ELF;
  prefabs[pf_elf_1][c_ry] = 60;
  prefabs[pf_elf_1][c_frame_rate] = 5;

  prefabs[pf_orc_1][c_f] = ACTIVE | KINETIC | SPRITE;
  prefabs[pf_orc_1][c_sprite] = ORC;
  prefabs[pf_orc_1][c_ry] = 80;
  prefabs[pf_orc_1][c_frame_rate] = 2;

  prefabs[pf_pig_1][c_f] = ACTIVE | KINETIC | SPRITE | CONTROL;
  prefabs[pf_pig_1][c_sprite] = PIG;
  prefabs[pf_pig_1][c_rx] = 300;
  prefabs[pf_pig_1][c_ry] = 100;
  prefabs[pf_pig_1][c_frame_rate] = 10;


  // unit a = add_entity(pf_default);
  // _rx[a] = 30; _ry[a] = 30;
  // _sprite[a] = assets.sprites[GOBLIN];
  // _f[a] = ACTIVE | KINETIC | SPRITE;
  // unit c = add_entity(pf_elf_1);
  // _p[c] = a; 
  // _rx[c] = 30;
  // _ry[c] = 0;
  // _vx[a] = 100;
  

  sound = LoadSound(SOUND_1);
  music = LoadMusicStream(MUSIC_1);

  SetMusicVolume(music, .1);
  PlayMusicStream(music);

  fonts[0] = LoadFont(FONT_1);
  fonts[1] = LoadFont(FONT_2);
  fonts[2] = LoadFont(FONT_3);

  cam.zoom = 1.;

  printf("\n");

  Shader        shader = LoadShader(0, SHADER_1);

  int      texLocation1 = GetShaderLocation(shader, "texture1");
  int      texLocation2 = GetShaderLocation(shader, "texture2");
  int tileSizesLocation = GetShaderLocation(shader, "t_s");

  add_entity(pf_pig_1);

  while (!WindowShouldClose()) {
    input();
    game_loop();

    BeginDrawing();
      ClearBackground(BACKGROUND_DEFAULT_COLOR);
      BeginMode2D(cam);

      for (int i = 0; i < LAYERS; i++)
      {
        BeginShaderMode(shader);
          SetShaderValueTexture(shader, texLocation1, map.tileset);
          SetShaderValueTexture(shader, texLocation2, map.layers[i]);
          SetShaderValue(shader, tileSizesLocation, 
            &(Vector4){ 
              map.layers[i].width, 
              map.layers[i].height, 
              map.tileset.width, 
              map.tileset.height
            }, 
            SHADER_UNIFORM_VEC4
          );
          DrawTextureRec(
            map.render_textures[i].texture, 
            (Rectangle) { 
              0, 
              0, 
              map.layers[i].width * map.zoom * map.pixel_per_tile, 
              map.layers[i].height * map.zoom * map.pixel_per_tile
            }, 
            (Vector2) { 0, 0 }, WHITE
          );
        EndShaderMode();
      }

      draw();

      rlPushMatrix();
          rlTranslatef(16 * map.zoom * 50, 16 * map.zoom * 50, 0);
          rlRotatef(90, 1, 0, 0);
          DrawGrid(100, 16 * map.zoom);
      rlPopMatrix();

      DrawTextEx(
        fonts[2], 
        "Minecraft!", 
        (Vector2){
          50 * map.zoom, 
          50 * map.zoom
        }, 
        16, 
        0, 
        WHITE
      );

      // DrawTextureEx(entity_1, (Vector2){ 50, 70 }, 0.0, map.zoom, WHITE);
      DrawTexturePro(
        entity_1, 
        (Rectangle){  0,  0, 16, 16 }, 
        (Rectangle){ 
          16 * map.pixel_per_tile * map.zoom, 
          9 * map.pixel_per_tile * map.zoom, 
          16 * map.zoom,  
          16 * map.zoom 
        }, 
        (Vector2) {0, 0}, 
        0, 
        WHITE
      );

      EndMode2D();
      DrawFPS(5, 5);

      // UI

      DrawTextEx(
        fonts[2],
        TextFormat("ACTIVE_ENTITIES=%d", ACTIVE_ENTITIES),
        (Vector2){ 2., GetScreenHeight() - 24.F }, 
        24.F, 
        0,
        WHITE
      );

      DrawTextEx(
        fonts[2],
        TextFormat("LAST_FREE_INDEX=%d", LAST_FREE_INDEX), 
        (Vector2) { 1.0, GetScreenHeight() - (24.F * 2.F) }, 
        24.F, 
        0,
        WHITE
      );

      DrawTextEx(
        fonts[2],
        TextFormat("CURRENT_MAP=%d", map.current), 
        (Vector2) { 1.0, GetScreenHeight() - (24.F * 3.F) }, 
        24.F, 
        0.,
        WHITE
      );
      // UI

    EndDrawing();
  }


  // Unload
  clear_map();
  UnloadSound(sound);
  UnloadMusicStream(music);
  UnloadFont(fonts[0]);
  UnloadFont(fonts[1]);
  UnloadFont(fonts[2]);
  UnloadShader(shader);

  // Close
  CloseAudioDevice();
  CloseWindow();
  return 0;
}

int norm(int n)
{
  return n > 0 ? 1 : n < 0 ? -1 : 0;
}

void game_loop()
{
  UpdateMusicStream(music);

  for (unit i = 1; i <= last_entity; i++) 
  {
    if (!(_f[i] & ACTIVE)) continue;

    if (_f[i] & KINETIC) 
    {
      _rx[i] += _vx[i];
      _ry[i] += _vy[i];
    }

    _x[i] = _rx[i] + _x[_p[i]];
    _y[i] = _ry[i] + _y[_p[i]];

    if (_f[i] & CONTROL) 
    {
      _vx[i] = 5 * (norm(_control & k_D) - norm(_control & k_A));
      _vy[i] = 5 * (norm(_control & k_S) - norm(_control & k_W));
      if (_vx[i] != 0 && _vy[i] != 0){
        _vx[i] *= .8;
        _vy[i] *= .8;
      }
    }




    // if (_f[i] & SPRITE) 
    // {
    //   DrawRectangle(_x[i], _y[i], _w[i], _h[i], _color[i]);
    // }
  }
}

void draw()
{
  for (unit i = 1; i <= last_entity; i++) 
  {
    if (!(_f[i] & ACTIVE)) continue;

    if (_f[i] & SPRITE) 
    {
      // u32 x = _x[i] + _x[_parent[i]];
      // u32 y = _y[i] + _y[_parent[i]];
      // DrawRectangle(x, y, _w[i], _h[i], _color[i]);
    }

    u32 x = _x[i];
    u32 y = _y[i];

    u32 a = ((int)(GetTime() * _frame_rate[i]) % 3) + 1;

    DrawTexturePro(
      _sprite[i], 
      (Rectangle){  a * 16,  0, 16, 16 }, 
      (Rectangle){ 
        x, 
        y, 
        16 * map.zoom,  
        16 * map.zoom 
      }, 
      (Vector2) {0, 0}, 
      0, 
      WHITE
    );

    DrawCircle(x, y, 2, RED);
  }
}

void input()
{

  // switch (GetKeyPressed()) 
  // {
  //   case KEY_W: _control |= k_W; break;
  //   case KEY_A: _control |= k_A; break;
  //   case KEY_S: _control |= k_S; break;
  //   case KEY_D: _control |= k_D; break;
  //   default: break;
  // }
  if (IsKeyPressed(KEY_W)) _control |= k_W; 
  if (IsKeyPressed(KEY_A)) _control |= k_A; 
  if (IsKeyPressed(KEY_S)) _control |= k_S; 
  if (IsKeyPressed(KEY_D)) _control |= k_D; 

  
  if (IsKeyReleased(KEY_W)) _control &= ~k_W;
  if (IsKeyReleased(KEY_A)) _control &= ~k_A;
  if (IsKeyReleased(KEY_S)) _control &= ~k_S;
  if (IsKeyReleased(KEY_D)) _control &= ~k_D;
  
  if (IsKeyPressed(KEY_Z)) printf("\n%d %d %d %d\n",
                                  _control & k_W,
                                  _control & k_A,
                                  _control & k_S,
                                  _control & k_D);

  
  if (IsKeyPressed(KEY_THREE))
  {
    scene_3;
  }

  if (IsKeyPressed(KEY_TWO))
  {
    scene_2;
  }


  if (IsKeyDown(KEY_C))
  {
    unit a = add_entity(1 + (rand() % TOTAL_ENTITIES));
  }

  if (IsKeyPressed(KEY_G))
  {
    unit a = add_entity(SKELETON);
    unit b = add_entity(GOBLIN);
    
    if (a > 0 && b > 0)
      _p[b] = a;
  }

  if (IsKeyPressed(KEY_FIVE))
  {
    destroy_entity(5);
  }

  if (IsKeyPressed(KEY_P))
  {
    clean_entities();
  }

  // if (IsKeyPressed(KEY_SPACE))
  // {
  //   cam.target = (Vector2){ _x[0] - (GetScreenWidth() / cam.zoom / 2), _y[0] - (GetScreenHeight() / cam.zoom / 2) };
  //   startTimer(&fadeTimer, 2.0f);
  // }

  if (IsKeyPressed(KEY_EQUAL) && cam.zoom >= 1.) 
  {
    cam.zoom += .125;
    if (cam.zoom < 1.)
      cam.zoom = 1.;
  }

  if (IsKeyPressed(KEY_MINUS) && cam.zoom >= 1.) 
  {
    add_entity(pf_elf_1);
    cam.zoom -= .125;
    if (cam.zoom < 1.)
      cam.zoom = 1.;
  }

  // if (IsKeyPressed(KEY_A))
  // {
    // invoke(&keyA);
  // }

  if (IsKeyPressed(KEY_SPACE))
  {
    StopMusicStream(music);
    music = LoadMusicStream("assets/music/2.mp3");
    SetMusicVolume(music, .1);
    PlayMusicStream(music);
  }

  if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
  {
    PlaySound(sound);
  }

  if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
  {
    PlayMusicStream(music);
  }


  if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
  {
    Vector2 delta = GetMouseDelta();
    delta = Vector2Scale(delta, -1.0f / cam.zoom);

    cam.target = Vector2Add(cam.target, delta);
  }

  // zoom based on wheel
  float wheel = GetMouseWheelMove();
  if (wheel != 0)
  {
    // get the world point that is under the mouse
    Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), cam);

    // set the offset to where the mouse is
    cam.offset = GetMousePosition();

    // set the target to match, so that the camera maps the world space point under the cursor to the screen space point under the cursor at any zoom
    cam.target = mouseWorldPos;

    // zoom
    cam.zoom += wheel * 0.125f;
    // printf("zoom: %f\n", cam.zoom);
    if (cam.zoom < 1.0f)
      cam.zoom = 1.0f;
  }

  // printf("%d\n", sizeof(RenderTexture2D));

  // check for alt + enter
  if (IsKeyPressed(KEY_ENTER) && (IsKeyDown(KEY_LEFT_ALT) || IsKeyDown(KEY_RIGHT_ALT)))
  {
    // see what display we are on right now
    int display = GetCurrentMonitor();
    if (IsWindowFullscreen())
    {
      ToggleFullscreen();
      SetWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
      // SetWindowMonitor(0);
    }
    else
    {
      ToggleFullscreen();
      SetWindowSize(GetMonitorWidth(display), GetMonitorHeight(display));
      // SetWindowMonitor(0);
    }
  }
}
