#include <raylib.h>
#include <raymath.h>
#include <rlgl.h>
#include <stdio.h>
#include <stdlib.h>
// #include <time.h>
// #include <stdlib.h>

#define BACKGROUND_DEFAULT_COLOR (Color){71, 102, 143, 255}

#define  SCREEN_WIDTH 1280 
#define SCREEN_HEIGHT 720  
#define  SCREEN_TITLE "HOLA"

#define MAX_ENTITIES     256  //(1 << (sizeof(unit) * 8)) - 1
#define TOTAL_ENTITIES   4
#define TOTAL_COMPONENTS 8

typedef unsigned char   u8;
typedef unsigned short u16;
typedef unsigned int   u32;
typedef unsigned long  u64;
typedef u8 unit;

typedef signed char   i8;
typedef signed short i16;
typedef signed int   i32;
typedef signed long  i64;

enum Flags
{
  ACTIVE    = 1 << 0,
  KINETIC   = 1 << 1,
  SPRITE    = 1 << 2,
};

enum Types
{
  HUMAN    = 0,
  SKELETON = 1,
  GOBLIN   = 2,
  ELF      = 3,
};

enum Colors 
{
  cGreen  = 0,
  cRed    = 1,
  cBlue   = 2,
  cViolet = 3,
};


Color colors[] = { GREEN, RED, BLUE, VIOLET};

void input();
void game_loop();
// Components
u32       _x [MAX_ENTITIES]; // TRANSFORM
u32       _y [MAX_ENTITIES]; // TRANFORM
u32       _w [MAX_ENTITIES];
u32       _h [MAX_ENTITIES];
i32      _vx [MAX_ENTITIES]; // KINETIC
i32      _vy [MAX_ENTITIES]; // KINETIC 
Color _color [MAX_ENTITIES]; //
unit   _type [MAX_ENTITIES];
unit  _flags [MAX_ENTITIES];

// Miscelaneous
unit       LAST_FREE_INDEX = 1; // 0 means full
unit       ACTIVE_ENTITIES = 0;
unit     last_entity_added = 0;
unit last_entity_destroyed = 0;
unit           last_entity = 0;
// unit first_entity = 0;
Camera2D cam;

// Starting stats
i32 starting_stats[TOTAL_ENTITIES][TOTAL_COMPONENTS] = 
{
  {   20,  20, 10, 10,  100,    0,  cGreen, ACTIVE | KINETIC | SPRITE }, // HUMAN
  {   50,  20, 10, 10,    0,  100,   cBlue, ACTIVE | KINETIC | SPRITE }, // SKELETON
  { 1000,  20, 10, 10, -100,  200, cViolet, ACTIVE | KINETIC | SPRITE }, // GOBLIN
  { 1000, 700, 10, 10, -100, -100,    cRed, ACTIVE | KINETIC | SPRITE }, // ELF
};

void clean_entities() 
{
  // for (unit i = 0; i <= last_entity; i++) 
    // _flags[i] &= ~ACTIVE; 

  for (unit *ptr = &_flags[0]; ptr < &_flags[last_entity]; ptr++) 
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
  if (_flags[id] & ACTIVE)
  {
    _flags[id] &= ~ACTIVE;
    // LAST_FREE_INDEX = id;

    ACTIVE_ENTITIES--;
    printf("\n✅ Destroyed {%d} at [%d] ~ %d entities", _type[id], id, ACTIVE_ENTITIES);
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
    return last_entity_added;
  }

  // Holds how many entities currently are
  if (last_entity < LAST_FREE_INDEX ) last_entity = LAST_FREE_INDEX;

  // Set components' values
  _x        [LAST_FREE_INDEX] =  starting_stats[type][0];
  _y        [LAST_FREE_INDEX] =  starting_stats[type][1];
  _w        [LAST_FREE_INDEX] =  starting_stats[type][2];
  _h        [LAST_FREE_INDEX] =  starting_stats[type][3];
  _vx       [LAST_FREE_INDEX] =  starting_stats[type][4];
  _vy       [LAST_FREE_INDEX] =  starting_stats[type][5];
  _color    [LAST_FREE_INDEX] =  colors[starting_stats[type][6]];
  _flags    [LAST_FREE_INDEX] =  starting_stats[type][7];
  _type     [LAST_FREE_INDEX] =  type;

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
Texture2D tiles[3];
Vector4 tileSizes[2];
RenderTexture2D textures[2];

#define  MUSIC_1 "assets/music/1.mp3"
#define  MUSIC_2 "assets/music/2.mp3"
#define  SOUND_1 "assets/sounds/1.wav"
#define   FONT_1 "assets/fonts/1.ttf"
#define SHADER_1 "assets/shaders/test.frag"
#define   TILE_1 "assets/textures/tiles_packed3.png"
#define    MAP_1 "assets/textures/actual_map3.png"  
#define    MAP_2 "assets/textures/actual_map2.png"  

#define scene1 do       \
{                       \
  clean_entities();     \
  add_entity(HUMAN);    \
  add_entity(SKELETON); \
  printf("\nxd!!\n");   \
} while(0)

#define scene2 do {     \
  clean_entities();     \
  add_entity(GOBLIN);   \
  add_entity(ELF);      \
  printf("\nxd!!\n");   \
} while(0)

int main()
{
  SetConfigFlags(FLAG_VSYNC_HINT);
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_TITLE);
  InitAudioDevice();

  sound = LoadSound(SOUND_1);
  music = LoadMusicStream(MUSIC_1);

  SetMusicVolume(music, .1);
  PlayMusicStream(music);

  fonts[0] = LoadFont(FONT_1);
  // fonts[1] = LoadFont("assets/fonts/2.ttf");

  cam.zoom = 1.;

  printf("\n");
  
  tiles[0] = LoadTexture(TILE_1);
  tiles[1] = LoadTexture(MAP_1);
  tiles[2] = LoadTexture(MAP_2);

  tileSizes[0] = (Vector4){ 20,  8, 368, 128 };
  tileSizes[1] = (Vector4){ 30, 30, 368, 128 };

  int zoom = 5 * 16;

  textures[0] = LoadRenderTexture(
    tileSizes[0].x * zoom, 
    tileSizes[0].y * zoom
  );

  textures[1] = LoadRenderTexture(
    tileSizes[1].x * zoom, 
    tileSizes[1].y * zoom
  );

  Shader        shader = LoadShader(0, SHADER_1);

  int      texLocation1 = GetShaderLocation(shader, "texture1");
  int      texLocation2 = GetShaderLocation(shader, "texture2");
  int tileSizesLocation = GetShaderLocation(shader, "t_s");


  while (!WindowShouldClose()) {
    input();

    BeginDrawing();
      ClearBackground(BACKGROUND_DEFAULT_COLOR);
      BeginMode2D(cam);

      BeginShaderMode(shader);
        SetShaderValueTexture(shader, texLocation1, tiles[0]);
        SetShaderValueTexture(shader, texLocation2, tiles[1]);
        SetShaderValue(shader, tileSizesLocation, &tileSizes[0], SHADER_UNIFORM_VEC4);
        DrawTextureRec(
          textures[0].texture, 
          (Rectangle) { 
            0, 
            0, 
            tileSizes[0].x * zoom, 
            tileSizes[0].y * zoom
          }, 
          (Vector2) { 0, 0 }, WHITE
        );
      EndShaderMode();

      BeginShaderMode(shader);
        SetShaderValueTexture(shader, texLocation1, tiles[0]);
        SetShaderValueTexture(shader, texLocation2, tiles[2]);
        SetShaderValue(shader, tileSizesLocation, &tileSizes[1], SHADER_UNIFORM_VEC4);
        DrawTextureRec(
          textures[1].texture, 
          (Rectangle) { 
            0, 
            0, 
            tileSizes[1].x * zoom, 
            tileSizes[1].y * zoom
          }, 
          (Vector2) { 0, 0 }, WHITE
        );
      EndShaderMode();

      game_loop();

      EndMode2D();
      DrawFPS(5, 5);

      // UI
      DrawText(TextFormat("ACTIVE_ENTITIES=%d", ACTIVE_ENTITIES), 1., GetScreenHeight() - 24.F, 24.F, WHITE);
      DrawText(TextFormat("LAST_FREE_INDEX=%d", LAST_FREE_INDEX), 1., GetScreenHeight() - (24.F * 2.F), 24.F, WHITE);
      // UI

    EndDrawing();
  }


  // Unload
  UnloadRenderTexture(textures[0]);
  UnloadRenderTexture(textures[1]);
  UnloadTexture(tiles[0]);
  UnloadTexture(tiles[1]);
  UnloadTexture(tiles[2]);
  UnloadSound(sound);
  UnloadMusicStream(music);
  UnloadFont(fonts[0]);
  UnloadFont(fonts[1]);

  // Close
  CloseAudioDevice();
  CloseWindow();
  return 0;
}

void game_loop()
{
  UpdateMusicStream(music);

  for (unit i = 1; i <= last_entity; i++) 
  {
    if (!(_flags[i] & ACTIVE)) continue;

    if (_flags[i] & KINETIC) 
    {
      _x[i] += _vx[i] * GetFrameTime();
      _y[i] += _vy[i] * GetFrameTime();
    }

    if (_flags[i] & SPRITE) 
    {
      DrawRectangle(_x[i], _y[i], _w[i], _h[i], _color[i]);
    }
  }
}

void input()
{

  if (IsKeyPressed(KEY_T))
  {
    scene1;
  }

  if (IsKeyPressed(KEY_R))
  {
    scene2;
  }

  if (IsKeyDown(KEY_C))
  {
    add_entity(rand() % TOTAL_ENTITIES);
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
