#include "raylib.h"
#include "rcamera.h"
#include "raymath.h"
#include <stdio.h>

#include "poly.h"
#include "cube.h"
#include "boxtree.h"
#include "voxel.h"
#include "bullet.h"
#include "pawn.h"
#include "player.h"
#include "button.h"
#include "textbox.h"
#include "window.h"
#include "database.h"

#define RAYMATH_IMPLEMENTATION

#define MOUSE_MOVE_SENSITIVITY 0.001f

#define WORLD_DEFAULT_LIMIT 100
#define BOXTREE_INITIAL_SIZE 16

#define HUD_LIMIT 10

#define LEVEL_GRID_ROWS 10
#define LEVEL_GRID_COLS 5
#define LEVEL_GRID_DEPTH 10

Vector3 DEFAULT_PLAYER_POSITION = (Vector3){0, 5,-3};
Vector3 CAM_DEFAULT_POS = (Vector3){ 0.0f, 3.0f, 6.0f };
Vector3 CAM_DEFAULT_TARGET = (Vector3){ 0.0f, 2.0f, -2.0f };

float screenFade = 1;
bool screenFading = false;
bool cursorEnabled = true;
bool myDebug = false;
bool editMode = false;
float lookSensitivity = 40.0f;
float camSpeed = 2.0f;
float armX = 0;
float armY = 0;
float dt = 0;
float timePassed = 0;
float playerSpeed = 2.0f;

const int OCT = 8; // octree root size
const float LEVEL_GRID_CELL_SIZE = 1.0f;
struct Voxel* grid3d[LEVEL_GRID_ROWS][LEVEL_GRID_COLS][LEVEL_GRID_DEPTH];

struct Button* editorButtons[HUD_LIMIT];
struct Button* mainmenuButtons[HUD_LIMIT];
struct Textbox* levelTextbox;
struct Window* testWindow;

struct Pawn* worldPawns[WORLD_DEFAULT_LIMIT];
struct Bullet* worldBullets[WORLD_DEFAULT_LIMIT];
int worldBulletCount = 0;
struct Poly* worldPolys[WORLD_DEFAULT_LIMIT];
struct Player* player;

Camera camera = { 0 };

char levelString[LEVEL_GRID_ROWS*LEVEL_GRID_COLS*LEVEL_GRID_DEPTH];

void LoadLevel();
void PlaceVoxelInBoxtree(Voxel* voxel, BoxtreeNode* btnode);
void SpawnWorldBullet(Ray ray);
void SpawnWorldPoly(Vector3 newPos);
Pawn* SpawnWorldPawn(Vector3 newPos, PawnType pt);
bool IsNormalUp(Vector3 vector);
bool ContainsInstance(void *arr[], int size, void *target);
void ExecuteButtonFunction(ButtonFunction btnfunc);
void ResetScene();
void SetSoundPosition(Camera listener, Sound sound, Vector3 position, float maxDist);
void PlaySoundInstance(Sound sound, Vector3 soundPos);
//static bool IsRayHitNormalValid(Vector3 vector);

typedef enum {
    GS_MENU_MAIN,
    GS_EDIT,
    GS_EDIT_PAUSE,
    GS_GAMEPLAY
} GameState;

GameState gamestate = GS_MENU_MAIN;

typedef enum {
    SS_VOXEL,
    SS_TURRET
} SpawnSelection;
SpawnSelection spawnSelection = SS_VOXEL;

int main(void) // @INIT ========================================================================
{
    //const int screenWidth = 1920;
    //const int screenHeight = 1080;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    //SetConfigFlags(FLAG_FULLSCREEN_MODE);
    InitWindow(1920, 1080, "Tandem");
    MaximizeWindow();
    
    SetTargetFPS(60);
    InitAudioDevice();
    EnableCursor();
    //DisableCursor();

    LoadSounds();

    camera.position = CAM_DEFAULT_POS;
    camera.target = CAM_DEFAULT_TARGET;
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.fovy = 60.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    BoxtreeNode* boxtreeRoot = BuildBoxtree((Vector3){0,0,0}, BOXTREE_INITIAL_SIZE, 1);

    // @GRID init
    Vector3 gridOrigin = (Vector3){-4.5f, 0.0f, -4.5f};
    int gridIndex = 0;
    
    for (int x = 0; x < LEVEL_GRID_ROWS; x++){
        for (int y = 0; y < LEVEL_GRID_COLS; y++){
            for (int z = 0; z < LEVEL_GRID_DEPTH; z++){
                Voxel* newVoxel = Create_Voxel((Vector3){gridOrigin.x + x, gridOrigin.y + y, gridOrigin.z + z}, (Vector3){x, y, z}, 1);
                grid3d[x][y][z] = newVoxel;
                PlaceVoxelInBoxtree(newVoxel, boxtreeRoot);
                gridIndex++;
            }
        }
    }

    // set all ground level voxels
    for (int x = 0; x < LEVEL_GRID_ROWS; x++){
        for (int y = 0; y < LEVEL_GRID_COLS; y++){
            for (int z = 0; z < LEVEL_GRID_DEPTH; z++){
                if (y == 0){
                    grid3d[x][y][z]->isActive = true;
                } else {
                    grid3d[x][y][z]->isActive = false;
                }
            }
        }
    }

    // OBJECT POOLS
    for (int i = 0; i < WORLD_DEFAULT_LIMIT; i++){
        worldBullets[i] = Create_Bullet();
        worldPawns[i] = Create_Pawn();
        worldPolys[i] = Create_Poly();
    }

    // @Player init
    player = Create_Player();
    
    struct Ray r1;
    r1.position = (Vector3){0,0,0};
    r1.direction = (Vector3){10,10,0};
    Color r1Color = RED;

    struct Ray voxelRay;
    voxelRay.position = (Vector3){0,0,0};
    voxelRay.direction = (Vector3){1,1,0};

    // menus
    for (int i = 0; i < HUD_LIMIT; i++){ editorButtons[i] = Create_Button(); }
    for (int i = 0; i < HUD_LIMIT; i++){ mainmenuButtons[i] = Create_Button(); }

    levelTextbox = Create_Textbox();
    testWindow = Create_Window();
    
    // @READY ==========================================================================

    Spawn_Button(editorButtons[0], btn_edit_origin, (Vector2){60, 30}, "MAIN", 10, BTN_MAIN);
    Spawn_Button(editorButtons[1], (Vector2){btn_edit_origin.x + btn_edit_offset.x, btn_edit_origin.y}, (Vector2){60, 30}, "SAVE", 10, BTN_SAVE);
    Spawn_Button(editorButtons[2], (Vector2){btn_edit_origin.x + btn_edit_offset.x*2, btn_edit_origin.y}, (Vector2){60, 30}, "LOAD", 10, BTN_LOAD);
    Spawn_Button(editorButtons[3], (Vector2){btn_edit_origin.x + btn_edit_offset.x*3, btn_edit_origin.y}, (Vector2){60, 30}, "PREV", 10, BTN_PREV);
    Spawn_Button(editorButtons[4], (Vector2){btn_edit_origin.x + btn_edit_offset.x*4, btn_edit_origin.y}, (Vector2){60, 30}, "NEXT", 10, BTN_NEXT);

    Spawn_Button(editorButtons[5], (Vector2){btn_edit_origin.x, btn_edit_origin.y + btn_edit_offset.y}, (Vector2){60, 30}, "voxel", 10, BTN_VOXEL);
    Spawn_Button(editorButtons[6], (Vector2){btn_edit_origin.x + btn_edit_offset.x, btn_edit_origin.y + btn_edit_offset.y}, (Vector2){60, 30}, "turret", 10, BTN_TURRET);

    Spawn_Button(mainmenuButtons[0], (Vector2){500, 500}, (Vector2){200, 30}, "PLAY", 20, BTN_PLAY);
    Spawn_Button(mainmenuButtons[1], (Vector2){500, 600}, (Vector2){200, 30}, "TEST", 20, BTN_TEST);

    Spawn_Textbox(levelTextbox, (Vector2){600,10}, (Vector2){100,30}, 10);

    Spawn_Window(testWindow, (Vector2){700, 700}, (Vector2){500, 500}, "WINDOW");

    //Spawn_Player(player, (Vector3){0,5,-3});
    
    // printf("\n");
    // printf(TextFormat("%d", sizeof(levelCells) / sizeof(levelCells[0])));
    // printf("\n");

    // MAIN GAME LOOP ==========================================================================
    while (!WindowShouldClose())        // Detect window close button or ESC key
    {
        dt = GetFrameTime();
        timePassed += dt;
        
        // @INPUT ==========================================================================
        Vector2 mousePos = GetMousePosition();
        Vector4 newPlayerVel = (Vector4){0,0,0,0};

        switch (gamestate){
            case GS_EDIT:
                if (IsKeyDown(KEY_LEFT)){ newPlayerVel.x += -playerSpeed; }
                if (IsKeyDown(KEY_RIGHT)){ newPlayerVel.x += playerSpeed; }
                if (IsKeyDown(KEY_UP)){ newPlayerVel.z += -playerSpeed; }
                if (IsKeyDown(KEY_DOWN)){ newPlayerVel.z += playerSpeed; }
                if (IsKeyPressed(KEY_RIGHT_CONTROL)){ newPlayerVel.w = 1; }

                if (IsKeyPressed(KEY_P)){ myDebug = !myDebug; }
                if (IsKeyPressed(KEY_MINUS)){ SetTargetFPS(60); }
                if (IsKeyPressed(KEY_EQUAL)){ SetTargetFPS(120); }
                
                if (IsKeyPressed(KEY_E)){
                    editMode = !editMode;
                    r1Color = (editMode) ? WHITE : RED;
                }

                if (IsMouseButtonPressed(MOUSE_BUTTON_MIDDLE)){
                    if (!cursorEnabled){
                        EnableCursor();
                        gamestate = GS_EDIT_PAUSE;
                    } else {
                        DisableCursor();
                        gamestate = GS_EDIT;
                    }
                        cursorEnabled = !cursorEnabled;
                }

                if (IsKeyPressed(KEY_R)){ ResetScene(); }
                
                // camera movement/input
                camSpeed = (IsKeyDown(KEY_LEFT_SHIFT)) ? 5.0f : 2.0f;
                float newForward = 0;
                float newRight = 0;
                float newUp = 0;
                if (IsKeyDown(KEY_W)) newForward += camSpeed;
                if (IsKeyDown(KEY_S)) newForward -= camSpeed;
                if (IsKeyDown(KEY_D)) newRight += camSpeed;
                if (IsKeyDown(KEY_A)) newRight -= camSpeed;
                if (IsKeyDown(KEY_SPACE)) newUp += camSpeed;
                if (IsKeyDown(KEY_LEFT_CONTROL)) newUp -= camSpeed;
                
                // camera rotation
                Vector2 mousePositionDelta = GetMouseDelta();
                float newYaw = mousePositionDelta.x*MOUSE_MOVE_SENSITIVITY*lookSensitivity;
                float newPitch = mousePositionDelta.y*MOUSE_MOVE_SENSITIVITY*lookSensitivity;

                UpdateCameraPro(&camera, 
                (Vector3){ newForward*dt, newRight*dt, newUp*dt }, // added pos
                (Vector3){ newYaw, newPitch, 0.0f }, // added rot
                0.0f); // zoom

                break;
            case GS_EDIT_PAUSE:
                if (IsMouseButtonPressed(MOUSE_BUTTON_MIDDLE)){
                    if (!cursorEnabled){
                        EnableCursor();
                        gamestate = GS_EDIT_PAUSE;
                    } else {
                        DisableCursor();
                        gamestate = GS_EDIT;
                    }
                    cursorEnabled = !cursorEnabled;
                }
                break;
            default: break;
        }
        
        // @UPDATE ==========================================================================
        
        switch (gamestate){
            case GS_EDIT:
                for (int i = 0; i < WORLD_DEFAULT_LIMIT; i++){
                    Update_Poly(worldPolys[i], dt);
                    Update_Bullet(worldBullets[i], dt);
                }

                for (int i = 0; i < WORLD_DEFAULT_LIMIT; i++){
                    int pawnAction = Update_Pawn(worldPawns[i], dt);
                    switch (pawnAction){
                        case 1:
                            SpawnWorldBullet(worldPawns[i]->aimRay);
                            break;
                        default: break;
                    }
                }

                Update_Player(player, newPlayerVel, dt);
        
                Vector3 camF = GetCameraForward(&camera);
                Vector3 camR = GetCameraRight(&camera);
                Vector3 camU = GetCameraUp(&camera);

                // Cursor Ray
                Vector3 aimRay = Vector3Add(camR, camU);
                aimRay = Vector3Scale(aimRay, 0.2f);
                aimRay = Vector3Add(aimRay, Vector3Add(camera.position, (Vector3){0,-0.2f,0}));
                
                r1.position = aimRay;
                r1.direction = camF;
                break;
            case GS_EDIT_PAUSE:
                for (int i = 0; i < HUD_LIMIT; i++){
                    ButtonFunction btnfunc = Update_Button(editorButtons[i], mousePos);
                    ExecuteButtonFunction(btnfunc);
                }
                Update_Textbox(levelTextbox, mousePos);
                Update_Window(testWindow, mousePos);
                break;
            case GS_GAMEPLAY: break;
            case GS_MENU_MAIN:
                for (int i = 0; i < HUD_LIMIT; i++){
                    ButtonFunction btnfunc = Update_Button(mainmenuButtons[i], mousePos);
                    ExecuteButtonFunction(btnfunc);
                }
                break;
            default: break;
        }
        
        // @COLLISION ==========================================================================

        Vector3 rayHitNormal = (Vector3){0,0,0};
        Vector3 playerColNormal = (Vector3){0,0,0};

        switch (gamestate){
            case GS_EDIT:
                ResetBoxtree(boxtreeRoot);
                Voxel* voxelHits[50] = {NULL};
                GetRayVoxels(r1, boxtreeRoot, voxelHits, 50);
                
                float closestVoxelDist = 100;
                struct Voxel* closestHitVoxel = NULL;

                // player checkin
                Reset_Player(player);
                GetPlayerNodes(player, boxtreeRoot);

                // pawn checkin
                for (int i = 0; i < WORLD_DEFAULT_LIMIT; i++){
                    if (!worldPawns[i]->isActive) continue;
                    Reset_Pawn(worldPawns[i]);
                    GetPawnNodes(worldPawns[i], boxtreeRoot);
                }
                
                // bullet checkin
                for (int i = 0; i < WORLD_DEFAULT_LIMIT; i++){
                    if (!worldBullets[i]->isActive) continue;
                    Reset_Bullet(worldBullets[i]);
                    GetBulletNodes(worldBullets[i], boxtreeRoot);
                }

                // bullet collision
                for (int i = 0; i < WORLD_DEFAULT_LIMIT; i++){
                    // Bullets
                    if (!worldBullets[i]->isActive) continue;
                    if (!worldBullets[i]->isArmed) continue;
                    // Nodes
                    for (int j = 0; j < worldBullets[i]->nodeCount; j++){

                        // Other Bullets
                        for (int l = 0; l < worldBullets[i]->nodes[j]->bulletCount; l++){
                            // check bullet if self
                            if (worldBullets[i] == worldBullets[i]->nodes[j]->bullets[l]) continue;

                            if (CheckCollisionBoxes(worldBullets[i]->bb, worldBullets[i]->nodes[j]->bullets[l]->bb)){
                                if (worldBullets[i]->nodes[j]->bullets[l]->isActive){

                                    worldBullets[i]->color = WHITE;
                                    if (!worldBullets[i]->destroyFlag){
                                        worldBullets[i]->destroyFlag = true;
                                        SpawnWorldPoly(worldBullets[i]->position);
                                        SpawnWorldPoly(worldBullets[i]->position);
                                        SpawnWorldPoly(worldBullets[i]->position);
                                    }
                                }
                            }
                        }
                        // Voxels
                        for (int k = 0; k < worldBullets[i]->nodes[j]->voxelCount; k++){
                            
                            if (CheckCollisionBoxes(worldBullets[i]->bb, worldBullets[i]->nodes[j]->voxels[k]->bb)){
                                if (worldBullets[i]->nodes[j]->voxels[k]->isActive){
                                    worldBullets[i]->nodes[j]->voxels[k]->color = WHITE;
                                    worldBullets[i]->nodes[j]->voxels[k]->fading = true;
                                    
                                    worldBullets[i]->color = WHITE;
                                    if (!worldBullets[i]->destroyFlag){
                                        worldBullets[i]->destroyFlag = true;
                                        SpawnWorldPoly(worldBullets[i]->position);
                                        SpawnWorldPoly(worldBullets[i]->position);
                                        SpawnWorldPoly(worldBullets[i]->position);
                                        SpawnWorldPoly(worldBullets[i]->position);
                                        SpawnWorldPoly(worldBullets[i]->position);
                                    }
                                }
                            }
                        }
                        
                        // Pawns
                        for (int m = 0; m < worldBullets[i]->nodes[j]->pawnCount; m++){
                            if (CheckCollisionBoxes(worldBullets[i]->bb, worldBullets[i]->nodes[j]->pawns[m]->bb)){
                                if (worldBullets[i]->nodes[j]->pawns[m]->isActive){
                                    Pawn* hitPawn = worldBullets[i]->nodes[j]->pawns[m];

                                    // if bullet has not hit this target, add to hitTargets
                                    if (!ContainsInstance(worldBullets[i]->hitTargets, 8, hitPawn)){
                                        worldBullets[i]->hitTargets[worldBullets[i]->hitCount] = hitPawn;
                                        worldBullets[i]->hitCount++;
                                        hitPawn->color = WHITE;
                                        Damage_Pawn(hitPawn);
                                    }

                                    worldBullets[i]->color = WHITE;
                                    if (!worldBullets[i]->destroyFlag){
                                        worldBullets[i]->destroyFlag = true;
                                        SpawnWorldPoly(worldBullets[i]->position);
                                        SpawnWorldPoly(worldBullets[i]->position);
                                        SpawnWorldPoly(worldBullets[i]->position);
                                    }
                                }
                            }
                        }
                    }
                }

                // player collision
                for (int i = 0; i < player->nodeCount; i++){
                    for (int j = 0; j < player->nodes[i]->voxelCount; j++){

                        if(CheckCollisionBoxes(player->bb, player->nodes[i]->voxels[j]->bb)){
                            if (player->nodes[i]->voxels[j]->isActive){
                                Voxel* touchedVoxel = player->nodes[i]->voxels[j];
                                touchedVoxel->bbColor = WHITE;

                                voxelRay.position = (Vector3){player->position.x, touchedVoxel->position.y, player->position.z};
                                voxelRay.direction = Vector3Subtract(touchedVoxel->position, voxelRay.position);
                                RayCollision vrc = GetRayCollisionBox(voxelRay, touchedVoxel->bb);
                                playerColNormal = vrc.normal;

                                if (player->position.y-player->height/2 > touchedVoxel->position.y+0.45f){
                                    if (grid3d[(int)touchedVoxel->coordinates.x][(int)touchedVoxel->coordinates.y+1][(int)touchedVoxel->coordinates.z]->isActive){

                                    } else {
                                        player->position.y = touchedVoxel->position.y + 0.5f + player->height/2;
                                        player->velocity.y = 0;
                                    }
                                    
                                } else if (player->position.y+player->height/2 < touchedVoxel->position.y-0.45f){
                                    if (grid3d[(int)touchedVoxel->coordinates.x][(int)touchedVoxel->coordinates.y-1][(int)touchedVoxel->coordinates.z]->isActive){

                                    } else {
                                        player->position.y = touchedVoxel->position.y - 0.5f - player->height/2;
                                        player->velocity.y = 0;
                                    }
                                    
                                } else if (playerColNormal.x == 1){
                                    player->position.x = touchedVoxel->position.x + 0.5f + player->width/2;
                                    player->velocity.x = 0;
                                } else if (playerColNormal.x == -1){
                                    player->position.x = touchedVoxel->position.x - 0.5f - player->width/2;
                                    player->velocity.x = 0;
                                } else if (playerColNormal.z == 1){
                                    player->position.z = touchedVoxel->position.z + 0.5f + player->width/2;
                                    player->velocity.z = 0;
                                } else if (playerColNormal.z == -1){
                                    player->position.z = touchedVoxel->position.z - 0.5f - player->width/2;
                                    player->velocity.z = 0;
                                }
                            }
                        }
                    }
                }
                
                // edit ray collision
                if (editMode && !cursorEnabled){
                    for (int i = 0; i < 50; i++){
                        if (voxelHits[i] == NULL){
                            break;
                        } else {
                            float dist = Vector3Distance(r1.position, voxelHits[i]->position);
                            if (dist < closestVoxelDist){
                                closestVoxelDist = dist;
                                closestHitVoxel = voxelHits[i];
                            }
                        }
                    }

                    if (closestHitVoxel != NULL) {
                        RayCollision rc = GetRayCollisionBox(r1, closestHitVoxel->bb);
                        rayHitNormal = rc.normal;

                        closestHitVoxel->selected = true;
                        closestHitVoxel->selectedNormal = rayHitNormal;

                        switch(spawnSelection){
                            case SS_VOXEL:
                                if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)){
                                    Vector3 NVC = Vector3Add(closestHitVoxel->coordinates,rayHitNormal);
                                    Voxel* targetVoxel = grid3d[(int)Clamp(NVC.x,0,LEVEL_GRID_ROWS-1)]
                                    [(int)Clamp(NVC.y,0,LEVEL_GRID_COLS-1)]
                                    [(int)Clamp(NVC.z,0,LEVEL_GRID_DEPTH-1)];

                                    if (targetVoxel->isOccupied == false && targetVoxel->isActive == false){
                                        targetVoxel->isActive = true;
                                    }
                                }
                                break;
                            case SS_TURRET:
                                if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) && IsNormalUp(rayHitNormal)){
                                    Vector3 NVC = Vector3Add(closestHitVoxel->coordinates, rayHitNormal);
                                    Voxel* targetVoxel = grid3d[(int)Clamp(NVC.x,0,LEVEL_GRID_ROWS-1)]
                                    [(int)Clamp(NVC.y,0,LEVEL_GRID_COLS-1)]
                                    [(int)Clamp(NVC.z,0,LEVEL_GRID_DEPTH-1)];

                                    if (targetVoxel->isOccupied == true || targetVoxel->isActive == true){ break; }
                                    
                                    Pawn* newPawn = SpawnWorldPawn(Vector3Add(closestHitVoxel->position, Vector3Scale(rayHitNormal,1.5f)), PAWN_TURRET);
                                    targetVoxel->isOccupied = true;
                                    targetVoxel->occupier = OB_TURRET;
                                    newPawn->rootVoxel = targetVoxel;
                                }
                                break;
                            default:
                                break;
                        }

                        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
                            Destroy_Voxel(closestHitVoxel);
                        }
                    }
                } else if (!cursorEnabled) {
                    // shoot a projectile
                    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
                        SpawnWorldBullet(r1);
                    }
                }
                break;
            default: break;
        }
        
        // @DRAW ==========================================================================

        BeginDrawing();
            ClearBackground(GRAY);
            BeginMode3D(camera);
            
            DrawSphere((Vector3){ 0.0f, 10.0f, -50.0f }, 1.0f, WHITE);

            DrawGrid(10, 1.0f);
            DrawCubeWires((Vector3){0,0,0}, 10, 0.2, 10, WHITE);

            if (myDebug) DrawBoxtreeNode(boxtreeRoot);

            for (int i = 0; i < WORLD_DEFAULT_LIMIT; i++){
                Draw_Poly(worldPolys[i]);
                Draw_Pawn(worldPawns[i]);
                Draw_Bullet(worldBullets[i]);
            }
            
            for (int x = 0; x < LEVEL_GRID_ROWS; x++){
                for (int y = 0; y < LEVEL_GRID_COLS; y++){
                    for (int z = 0; z < LEVEL_GRID_DEPTH; z++){
                        Draw_Voxel(grid3d[x][y][z]);
                        Reset_Voxel(grid3d[x][y][z]);
                    }
                }
            }

            Draw_Player(player);

            switch (gamestate){
                case GS_EDIT:
                    DrawRay(r1,r1Color);
                    break;
                case GS_GAMEPLAY: break;
                default: break;
            }
            
            EndMode3D(); // ==========================================================================
            
            int screenWidth = GetScreenWidth();
            int screenHeight = GetScreenHeight();

            switch (gamestate){
                case GS_MENU_MAIN:
                    DrawText(TextFormat("TANDEM"), screenWidth/2, screenHeight/2, 50, BLACK);
                    for (int i = 0; i < HUD_LIMIT; i++){ Draw_Button(mainmenuButtons[i]); }
                    break;
                case GS_EDIT:
                    Vector2 center = { screenWidth / 2.0f, screenHeight / 2.0f };
                    // Draw a simple plus-sign crosshair
                    DrawLine(center.x - 10, center.y, center.x + 10, center.y, WHITE);
                    DrawLine(center.x, center.y - 10, center.x, center.y + 10, WHITE);
                    break;
                case GS_EDIT_PAUSE:
                    for (int i = 0; i < HUD_LIMIT; i++){ Draw_Button(editorButtons[i]); }
                    Draw_Textbox(levelTextbox);
                    Draw_Window(testWindow);
                    break;
                case GS_GAMEPLAY: break;
                default: break;
            }            

            // Draw HUD
            //DrawRectangle(5, 5, 250, 1000, Fade(SKYBLUE, 0.5f));
            //DrawRectangleLines(5, 5, 250, 1000, BLUE);
            
            DrawText(TextFormat("Time Passed: %0.2f", timePassed), 15, 15, 10, BLACK);
            DrawText(TextFormat("Current FPS: %d", GetFPS()), 15, 30, 10, BLACK);
            DrawText(TextFormat("Cam Target: %0.2f _ %0.2f _ %0.2f", camera.target.x, camera.target.y, camera.target.z), 15, 45, 10, BLACK);
            DrawText(TextFormat("Edit Mode: %s", (editMode) ? "ON" : "OFF"), 15, 75, 10, BLACK);
            DrawText(TextFormat("Selected Level: %d", levelSelection+1), 15, 90, 10, BLACK);

            switch(spawnSelection){
                case SS_VOXEL:
                    DrawText(TextFormat("Spawn Selection: Voxel"), 15, 105, 10, BLACK);
                    break;
                case SS_TURRET:
                    DrawText(TextFormat("Spawn Selection: Turret"), 15, 105, 10, BLACK);
                    break;
                default:
                    break;
            }

            
            DrawRectangle(0, 0, screenWidth*2, screenHeight*2, Fade(BLACK, screenFade));
            if (screenFade > 0){ screenFade -= 3*dt; }

        EndDrawing();

        // @CLEANUP =================================================================
        //DestroyOctreeNode(root);
        // ==========================================================================
    }

    // De-Initialization ============================================================
    CloseWindow();        // Close window and OpenGL context
    //===============================================================================

    return 0;
}

void ExecuteButtonFunction(ButtonFunction btnfunc){
    switch (btnfunc){
        case BTN_SAVE:
            PlaySound(bullet_shot);
            int lsIndex = 0;
            for (int x = 0; x < LEVEL_GRID_ROWS; x++){
                for (int y = 0; y < LEVEL_GRID_COLS; y++){
                    for (int z = 0; z < LEVEL_GRID_DEPTH; z++){
                        if (grid3d[x][y][z]->isActive == true){
                            levelString[lsIndex] = '1';
                        } else if (grid3d[x][y][z]->isOccupied == true){
                            switch (grid3d[x][y][z]->occupier){
                                case OB_TURRET:
                                    levelString[lsIndex] = '2';
                                break;
                                default: break;
                            }
                        } else {
                            levelString[lsIndex] = '0';
                        }
                        lsIndex++;
                    }
                }
            }
            SaveFileText("level_export.txt", (char *)levelString);
            break;
        case BTN_LOAD:
            LoadLevel();
            break;
        case BTN_PREV:
            levelSelection--;
            if (levelSelection < 0) levelSelection = 0;
            LoadLevel();
            break;
        case BTN_NEXT:
            levelSelection++;
            if (levelSelection > 2) levelSelection = 2;
            LoadLevel();
            break;
        case BTN_VOXEL:
            spawnSelection = SS_VOXEL;
            break;
        case BTN_TURRET:
            spawnSelection = SS_TURRET;
            break;
        case BTN_PLAY:
            //gamestate = GS_GAMEPLAY;
            break;
        case BTN_TEST:
            gamestate = GS_EDIT;
            DisableCursor();
            cursorEnabled = false;
            break;
        case BTN_MAIN:
            gamestate = GS_MENU_MAIN;
            ResetScene();
            break;
        case BTN_NONE: break;
        default: break;
    }
}

void LoadLevel(){
    screenFade = 1;

    for (int i = 0; i < WORLD_DEFAULT_LIMIT; i++){
        worldBullets[i]->isActive = false;
        worldPolys[i]->isActive = false;
        worldPawns[i]->isActive = false;
    }

    int lsIndex = 0;
    for (int x = 0; x < LEVEL_GRID_ROWS; x++){
        for (int y = 0; y < LEVEL_GRID_COLS; y++){
            for (int z = 0; z < LEVEL_GRID_DEPTH; z++){
                grid3d[x][y][z]->isActive = false;
                grid3d[x][y][z]->isOccupied = false;

                if (levels[levelSelection][lsIndex] == '1'){
                    grid3d[x][y][z]->isActive = true;
                } else if (levels[levelSelection][lsIndex] == '2'){
                    grid3d[x][y][z]->isOccupied = true;
                    grid3d[x][y][z]->occupier = OB_TURRET;
                    Pawn* newPawn = SpawnWorldPawn(Vector3Add(grid3d[x][y][z]->position, (Vector3){0,0.5f,0}), PAWN_TURRET);
                    newPawn->rootVoxel = grid3d[x][y][z];
                }
                lsIndex++;
            }
        }
    }
}

void ResetScene(){
    screenFade = 1;

    //camera.position = CAM_DEFAULT_POS;
    //camera.target = CAM_DEFAULT_TARGET;

    for (int i = 0; i < WORLD_DEFAULT_LIMIT; i++){ worldBullets[i]->isActive = false; }
    for (int i = 0; i < WORLD_DEFAULT_LIMIT; i++){ worldPolys[i]->isActive = false; }
    for (int i = 0; i < WORLD_DEFAULT_LIMIT; i++){ worldPawns[i]->isActive = false; }

    for (int x = 0; x < LEVEL_GRID_ROWS; x++){
        for (int y = 0; y < LEVEL_GRID_COLS; y++){
            for (int z = 0; z < LEVEL_GRID_DEPTH; z++){
                if (y == 0){
                    grid3d[x][y][z]->isActive = true;
                } else {
                    grid3d[x][y][z]->isActive = false;
                }
                grid3d[x][y][z]->isOccupied = false;
            }
        }
    }

    player->position = DEFAULT_PLAYER_POSITION;
    player->velocity = (Vector3){0,0,0};
}

void PlaceVoxelInBoxtree(Voxel* voxel, BoxtreeNode* btnode){
    if (voxel == NULL || btnode == NULL) return;

    if (voxel->coordinates.y > 0){
        voxel->isActive = false;
    }

    if (CheckCollisionBoxes(voxel->bb, btnode->bb)){
        if (btnode->depth == MAX_BOXTREE_DEPTH){
            btnode->voxels[btnode->voxelCount] = voxel;
            btnode->voxelCount++;
        }
    }

    if (btnode->depth == MAX_BOXTREE_DEPTH) return;

    for (int i = 0; i < 8; i++) {
        PlaceVoxelInBoxtree(voxel, btnode->children[i]);
    }
}

void SpawnWorldBullet(Ray ray){
    // find empty slot in bullet object pool
    for (int i = 0; i < WORLD_DEFAULT_LIMIT; i++){
        if (!worldBullets[i]->isActive){
            Spawn_Bullet(worldBullets[i], ray.position, ray.direction);
            break;
        }
    }
    PlaySoundInstance(bullet_shot, ray.position);
}

void SpawnWorldPoly(Vector3 newPos){
    for (int i = 0; i < WORLD_DEFAULT_LIMIT; i++){
        if (!worldPolys[i]->isActive){
            Spawn_Poly(worldPolys[i], newPos);
            break;
        }
    }
}

Pawn* SpawnWorldPawn(Vector3 newPos, PawnType pt){
    for (int i = 0; i < WORLD_DEFAULT_LIMIT; i++){
        if (!worldPawns[i]->isActive){
            Spawn_Pawn(worldPawns[i], newPos, pt);
            return worldPawns[i];
        }
    }
    return NULL;
}

bool IsNormalUp(Vector3 vector){
    if (vector.x != 0){ return false; }
    if (vector.y != 1){ return false; }
    if (vector.z != 0){ return false; }

    return true;
}

bool ContainsInstance(void *arr[], int size, void *target){
    for (int i = 0; i < size; i++) {
        // Direct address comparison
        if (arr[i] == target) return true;
    }
    return false;
}

void PlaySoundInstance(Sound sound, Vector3 soundPos){
    Sound soundInstance = LoadSoundAlias(sound);
    
    SetSoundPosition(camera, soundInstance, soundPos, 1.0f);
    PlaySound(soundInstance);
}

void SetSoundPosition(Camera listener, Sound sound, Vector3 position, float maxDist){
    // Calculate direction vector and distance between listener and sound source
    Vector3 direction = Vector3Subtract(position, listener.position);
    float distance = Vector3Length(direction);

    // Apply logarithmic distance attenuation and clamp between 0-1
    float attenuation = 1.0f/(1.0f + (distance/maxDist));
    attenuation = Clamp(attenuation, 0.0f, 1.0f);

    // Calculate normalized vectors for spatial positioning
    Vector3 normalizedDirection = Vector3Normalize(direction);
    Vector3 forward = Vector3Normalize(Vector3Subtract(listener.target, listener.position));
    Vector3 right = Vector3Normalize(Vector3CrossProduct(listener.up, forward));

    // Reduce volume for sounds behind the listener
    float dotProduct = Vector3DotProduct(forward, normalizedDirection);
    if (dotProduct < 0.0f) attenuation *= (1.0f + dotProduct*0.5f);

    // Set stereo panning based on sound position relative to listener
    float pan = 0.5f + 0.5f*Vector3DotProduct(normalizedDirection, right);

    // Apply final sound properties
    SetSoundVolume(sound, attenuation);
    SetSoundPan(sound, pan);
}

// bool IsRayHitNormalValid(Vector3 vector){
//     if (vector.x != 0 && vector.x != 1 && vector.x != -1){ return false; }
//     if (vector.y != 0 && vector.y != 1 && vector.y != -1){ return false; }
//     if (vector.z != 0 && vector.z != 1 && vector.z != -1){ return false; }

//     return true;
// }