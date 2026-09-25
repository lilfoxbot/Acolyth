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
#include "linkedlist.h"
#include "console.h"
#include "gridpawn.h"

#define RAYMATH_IMPLEMENTATION

#define MOUSE_MOVE_SENSITIVITY 0.001f
#define WORLD_DEFAULT_LIMIT 100
#define BOXTREE_INITIAL_SIZE 16
#define HUD_LIMIT 10
#define LEVEL_GRID_ROWS 10
#define LEVEL_GRID_COLS 5
#define LEVEL_GRID_DEPTH 10

Vector3 DEFAULT_PLAYER_POSITION = (Vector3){ 0, 5, -3 };
Vector3 CAM_DEFAULT_POS = (Vector3){ 0.0f, 3.0f, 6.0f };
Vector3 CAM_DEFAULT_TARGET = (Vector3){ 0.0f, 2.0f, -2.0f };

Vector2 mousePos;
float screenFade = 1;
bool screenFading = false;
bool myDebug = false;
bool consoleOpen = false;
bool editMode = false;
float lookSensitivity = 40.0f;
float camSpeed = 2.0f;
float armX = 0;
float armY = 0;
float DT = 0;
float timePassed = 0;
float playerSpeed = 2.0f;

Vector3 rayHitNormal = (Vector3){0,0,0};
Vector3 playerColNormal = (Vector3){0,0,0};
struct Ray r1;
Color r1Color = RED;
struct Ray voxelRay;

const int OCTREE_ROOT_SIZE = 8; // octree root size
const float LEVEL_GRID_CELL_SIZE = 1.0f;
struct Voxel* grid3d[LEVEL_GRID_ROWS][LEVEL_GRID_COLS][LEVEL_GRID_DEPTH];
BoxtreeNode* boxtreeRoot;

struct Button* mainButtons[HUD_LIMIT];
struct Window* mainWindow;

struct Button* pauseButtons[HUD_LIMIT];
struct Window* pauseWindow;
struct Window* settingsWindow;

struct List* windowList;
struct Window* testWindowOne;
struct Window* testWindowTwo;
struct Window* testWindowThree;
struct Window* fetchedWindow;
struct Window* focusedWindow;
int windowCount = 3;

struct Console* myConsole;

struct Pawn* worldPawns[WORLD_DEFAULT_LIMIT];
struct Bullet* worldBullets[WORLD_DEFAULT_LIMIT];
int worldBulletCount = 0;
struct Poly* worldPolys[WORLD_DEFAULT_LIMIT];

struct Gridpawn* myGridPawn;

Camera camera = { 0 };

char levelString[LEVEL_GRID_ROWS*LEVEL_GRID_COLS*LEVEL_GRID_DEPTH];

typedef enum {
    GS_MAIN,
    GS_GAMEPLAY,
    GS_PAUSE,
    GS_TEST,
    GS_TEST_PAUSE,
} GameState;
GameState GAME_STATE = GS_MAIN;

typedef enum {
    SS_VOXEL,
    SS_TURRET
} SpawnSelection;
SpawnSelection spawnSelection = SS_VOXEL;

void LoadLevel();
void PlaceVoxelInBoxtree(Voxel* voxel, BoxtreeNode* btnode);
void SpawnWorldBullet(Ray ray);
void SpawnWorldPoly(Vector3 newPos);
Pawn* SpawnWorldPawn(Vector3 newPos, PawnType pt);
bool IsNormalUp(Vector3 vector);
bool ContainsInstance(void *arr[], int size, void *target);
void ExecuteConsoleCommand(ConsoleCommand CC);
void ExecuteButtonFunction(ButtonFunction btnfunc);
void ResetScene();
void SetSoundPosition(Camera listener, Sound sound, Vector3 position, float maxDist);
void PlaySoundInstance(Sound sound, Vector3 soundPos);
void PrintToConsole(const char* out);
const char* GetGameStateAsString(GameState gs);
void GoToMain();
void ChangeGameState(GameState gs);

void MainInit();
void MainReady();
void MainInput();
void MainUpdate();
void MainCollide();
void MainDraw();

int main(void) // @INIT ========================================================================
{
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    //InitWindow(1920, 1080, "Tandem");
    InitWindow(1280, 720, "Tandem");
    MaximizeWindow();

    SetTargetFPS(60);
    InitAudioDevice();
    EnableCursor();
    LoadSounds();

    MainInit();
    MainReady();

    // MAIN GAME LOOP ==========================================================================
    while (!WindowShouldClose())        // Detect window close button or ESC key
    {
        DT = GetFrameTime();
        timePassed += DT;
        
        MainInput();
        MainUpdate();
        MainCollide();
        MainDraw();
    }

    // De-Initialization 
    CloseWindow(); // Close window and OpenGL context
    return 0;
}

void MainInit(){
    camera.position = CAM_DEFAULT_POS;
    camera.target = CAM_DEFAULT_TARGET;
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.fovy = 60.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    boxtreeRoot = Boxtree_Build((Vector3){0,0,0}, BOXTREE_INITIAL_SIZE, 1);

    // @GRID init
    Vector3 gridOrigin = (Vector3){-4.5f, 0.0f, -4.5f};
    int gridIndex = 0;
    
    for (int x = 0; x < LEVEL_GRID_ROWS; x++){
        for (int y = 0; y < LEVEL_GRID_COLS; y++){
            for (int z = 0; z < LEVEL_GRID_DEPTH; z++){
                Voxel* newVoxel = Voxel_Construct((Vector3){gridOrigin.x + x, gridOrigin.y + y, gridOrigin.z + z}, (Vector3){x, y, z}, 1);
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
        worldBullets[i] = Bullet_Construct();
        worldPawns[i] = Pawn_Construct();
        worldPolys[i] = Poly_Construct();
    }
    
    r1.position = (Vector3){0,0,0};
    r1.direction = (Vector3){10,10,0};
    voxelRay.position = (Vector3){0,0,0};
    voxelRay.direction = (Vector3){1,1,0};

    testWindowOne = Window_Construct((Vector2){1405, 300}, (Vector2){400, 400}, "WINDOW 1");
    testWindowTwo = Window_Construct((Vector2){1205, 400}, (Vector2){400, 400}, "WINDOW 2");
    testWindowThree = Window_Construct((Vector2){1005, 500}, (Vector2){400, 400}, "WINDOW 3");
    windowList = List_Construct();

    List_Push(windowList, testWindowThree);
    List_Push(windowList, testWindowTwo);
    List_Push(windowList, testWindowOne);
    focusedWindow = testWindowOne;
    testWindowOne->isFocused = true;

    myConsole = Console_Construct();

    mainButtons[0] = Button_Construct((Vector2){500, 500}, (Vector2){200, 30}, "PLAY", BTN_PLAY);
    mainButtons[0]->fontSize = 20;
    mainButtons[1] = Button_Construct((Vector2){500, 600}, (Vector2){200, 30}, "TEST", BTN_TEST);
    mainButtons[1]->fontSize = 20;
    mainButtons[2] = Button_Construct((Vector2){500, 700}, (Vector2){200, 30}, "SETTINGS", BTN_SETTINGS_MAIN);
    mainButtons[2]->fontSize = 20;


    mainWindow = Window_Construct((Vector2){(GetScreenWidth()/2)-400, (GetScreenHeight()/2)-200}, (Vector2){400, 400}, "MAIN");
    mainWindow->buttons[0] = mainButtons[0];
    mainWindow->buttons[1] = mainButtons[1];
    mainWindow->buttons[2] = mainButtons[2];
    mainWindow->buttonCount = 3;
    mainWindow->isFocused = true;

    pauseButtons[0] = Button_Construct((Vector2){500, -500}, (Vector2){200, 30}, "RESUME", BTN_PLAY);
    pauseButtons[0]->fontSize = 20;
    pauseButtons[1] = Button_Construct((Vector2){500, -500}, (Vector2){200, 30}, "SETTINGS", BTN_SETTINGS_PAUSE);
    pauseButtons[1]->fontSize = 20;
    pauseButtons[2] = Button_Construct((Vector2){500, -500}, (Vector2){200, 30}, "MAIN", BTN_MAIN);
    pauseButtons[2]->fontSize = 20;
    pauseWindow = Window_Construct((Vector2){(GetScreenWidth()/2)-200, (GetScreenHeight()/2)-200}, (Vector2){400, 400}, "PAUSE");
    pauseWindow->buttons[0] = pauseButtons[0];
    pauseWindow->buttons[1] = pauseButtons[1];
    pauseWindow->buttons[2] = pauseButtons[2];
    pauseWindow->buttonCount = 3;

    settingsWindow = Window_Construct((Vector2){(GetScreenWidth()/2), (GetScreenHeight()/2)}, (Vector2){400, 400}, "SETTINGS");
    settingsWindow->hasCloseBtn = true;
    settingsWindow->parentWindow = pauseWindow;

    myGridPawn = Gridpawn_Construct();
}

void MainReady(){
    Gridpawn_Spawn(myGridPawn, (Vector3){0,2,0});
}

void MainInput(){
    mousePos = GetMousePosition();

    if (IsKeyPressed(KEY_GRAVE)){ consoleOpen = !consoleOpen; }
    if (consoleOpen){
        if (IsKeyPressed(KEY_ENTER)){
            ExecuteConsoleCommand(Console_Submit(myConsole));
        }
    }

    switch (GAME_STATE){
        case GS_TEST:

            if (IsKeyPressed(KEY_P)){ myDebug = !myDebug; }
            if (IsKeyPressed(KEY_MINUS)){ SetTargetFPS(60); }
            if (IsKeyPressed(KEY_EQUAL)){ SetTargetFPS(120); }
            
            if (IsKeyPressed(KEY_E)){
                editMode = !editMode;
                r1Color = (editMode) ? WHITE : RED;
            }

            if (IsMouseButtonPressed(MOUSE_BUTTON_MIDDLE)){
                EnableCursor();
                ChangeGameState(GS_TEST_PAUSE);
                break;
            }

            if (consoleOpen) break;
            
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
            (Vector3){ newForward*DT, newRight*DT, newUp*DT }, // added pos
            (Vector3){ newYaw, newPitch, 0.0f }, // added rot
            0.0f); // zoom

            break;
        case GS_TEST_PAUSE:
            if (IsMouseButtonPressed(MOUSE_BUTTON_MIDDLE)){
                ChangeGameState(GS_TEST);
            }
            break;
        default: break;
    }
}

void MainUpdate(){
    switch (GAME_STATE){
        case GS_TEST:
            for (int i = 0; i < WORLD_DEFAULT_LIMIT; i++){
                Poly_Update(worldPolys[i], DT);
                Bullet_Update(worldBullets[i], DT);
            }

            for (int i = 0; i < WORLD_DEFAULT_LIMIT; i++){
                int pawnAction = Pawn_Update(worldPawns[i], DT);
                switch (pawnAction){
                    case 1:
                        SpawnWorldBullet(worldPawns[i]->aimRay);
                        break;
                    default: break;
                }
            }
    
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
        case GS_TEST_PAUSE:

            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
                // unfocus all
                for (int i = 0; i < windowList->count; i++){
                    fetchedWindow = (struct Window *)List_GetItem(windowList, i);
                    if (fetchedWindow){
                        fetchedWindow->isFocused = false;
                    }
                }
                // focus
                for (int i = 0; i < windowList->count; i++){
                    fetchedWindow = (struct Window *)List_GetItem(windowList, i);
                    if (fetchedWindow){
                        if(Window_Check(fetchedWindow, mousePos)){
                            // Reorder windows
                            fetchedWindow->isFocused = true;
                            focusedWindow = fetchedWindow;
                            List_MoveToFront(windowList, i);
                            break;
                        } else {
                            fetchedWindow->isFocused = false;
                        }
                    }
                }
            }
            for (int i = 0; i < windowList->count; i++){
                fetchedWindow = (struct Window *)List_GetItem(windowList, i);
                if (i == 0){ Window_Drag(fetchedWindow, mousePos); }
                ExecuteButtonFunction(Window_Update(fetchedWindow, mousePos));
            }
            
            break;
        case GS_GAMEPLAY:
            
            // gridpawn movement, (pokemon gridlock)
            if (!myGridPawn->moving){
                int moveX = 0;
                int moveY = 0;
                if (IsKeyDown(KEY_LEFT)){ 
                    moveX += -1;
                    myGridPawn->moving = true;
                }
                if (IsKeyDown(KEY_RIGHT)){
                    moveX += 1;
                    myGridPawn->moving = true;
                }
                if (IsKeyDown(KEY_UP) && moveX == 0){
                    moveY += -1;
                    myGridPawn->moving = true;
                }
                if (IsKeyDown(KEY_DOWN) && moveX == 0){
                    moveY += 1;
                    myGridPawn->moving = true;
                }

                myGridPawn->targetPos = Vector3Add(myGridPawn->position, (Vector3){moveX, 0, moveY});
                myGridPawn->velocity = (Vector3){moveX*myGridPawn->moveSpeed,0,moveY*myGridPawn->moveSpeed};

                if (myGridPawn->targetPos.x != 0){
                    myGridPawn->targetVector = myGridPawn->targetPos.x;
                } else {
                    myGridPawn->targetVector = myGridPawn->targetPos.y;
                }

            }
            
            // PAUSE
            if (IsKeyPressed(KEY_P)){
                ChangeGameState(GS_PAUSE);
                Window_Open(pauseWindow);
            }
            
            Gridpawn_Update(myGridPawn, DT);
            break;
        case GS_PAUSE:
            
            ExecuteButtonFunction(Window_Update(pauseWindow, mousePos));
            ExecuteButtonFunction(Window_Update(settingsWindow, mousePos));
            
            // UNPAUSE
            if (IsKeyPressed(KEY_P)){
                ChangeGameState(GS_GAMEPLAY);
                Window_Close(pauseWindow);
                Window_Close(settingsWindow);
            }
            break;
        case GS_MAIN:
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
                mainWindow->isActive = true;
            }
            ExecuteButtonFunction(Window_Update(mainWindow, mousePos));
            ExecuteButtonFunction(Window_Update(settingsWindow, mousePos));
            break;
        default: break;
    }
    if (consoleOpen) Console_Update(myConsole);
}

void MainCollide(){
    rayHitNormal = (Vector3){0,0,0};
    playerColNormal = (Vector3){0,0,0};

    switch (GAME_STATE){
        case GS_TEST:
            Boxtree_Reset(boxtreeRoot);
            Voxel* voxelHits[50] = {NULL};
            Boxtree_GetRayVoxels(r1, boxtreeRoot, voxelHits, 50);
            
            float closestVoxelDist = 100;
            struct Voxel* closestHitVoxel = NULL;

            // pawn checkin
            for (int i = 0; i < WORLD_DEFAULT_LIMIT; i++){
                if (!worldPawns[i]->isActive) continue;
                Pawn_Reset(worldPawns[i]);
                Boxtree_GetPawnNodes(worldPawns[i], boxtreeRoot);
            }
            
            // bullet checkin
            for (int i = 0; i < WORLD_DEFAULT_LIMIT; i++){
                if (!worldBullets[i]->isActive) continue;
                Bullet_Reset(worldBullets[i]);
                Boxtree_GetBulletNodes(worldBullets[i], boxtreeRoot);
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
                                    Pawn_Damage(hitPawn);
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
            
            // edit ray collision
            if (editMode){
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

                if (closestHitVoxel != NULL){
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
                        default: break;
                    }

                    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
                        Voxel_Destroy(closestHitVoxel);
                    }
                }
            } else {
                // shoot a projectile
                if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
                    SpawnWorldBullet(r1);
                }
            }
            break;
        default: break;
    }
}

void MainDraw(){
    BeginDrawing(); // =====================================================
    ClearBackground(GRAY);
    BeginMode3D(camera); // =====================================================
    
    DrawSphere((Vector3){ 0.0f, 10.0f, -50.0f }, 1.0f, WHITE);
    DrawGrid(10, 1.0f);
    DrawCubeWires((Vector3){0,0,0}, 10, 0.2, 10, WHITE);

    if (myDebug) Boxtree_DrawNode(boxtreeRoot);

    for (int i = 0; i < WORLD_DEFAULT_LIMIT; i++){
        Poly_Draw(worldPolys[i]);
        Pawn_Draw(worldPawns[i]);
        Bullet_Draw(worldBullets[i]);
    }
    
    for (int x = 0; x < LEVEL_GRID_ROWS; x++){
        for (int y = 0; y < LEVEL_GRID_COLS; y++){
            for (int z = 0; z < LEVEL_GRID_DEPTH; z++){
                Voxel_Draw(grid3d[x][y][z]);
                Voxel_Reset(grid3d[x][y][z]);
            }
        }
    }

    switch (GAME_STATE){
        case GS_TEST:
            DrawRay(r1,r1Color);
            break;
        case GS_GAMEPLAY:
            Gridpawn_Draw(myGridPawn);
            break;
        case GS_PAUSE:
            Gridpawn_Draw(myGridPawn);
            break;
        default: break;
    }
    
    EndMode3D(); // =====================================================
    
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();

    switch (GAME_STATE){
        case GS_MAIN:
            DrawText(TextFormat("TANDEM"), screenWidth/2, screenHeight/2, 50, BLACK);
            Window_Draw(mainWindow);
            Window_Draw(settingsWindow);
            break;
        case GS_TEST:
            Vector2 center = { screenWidth / 2.0f, screenHeight / 2.0f };
            // Draw a simple plus-sign crosshair
            DrawLine(center.x - 10, center.y, center.x + 10, center.y, WHITE);
            DrawLine(center.x, center.y - 10, center.x, center.y + 10, WHITE);
            break;
        case GS_TEST_PAUSE:
            for (int i = HUD_LIMIT; i >= 0; i--){
                fetchedWindow = (struct Window *)List_GetItem(windowList, i);
                Window_Draw(fetchedWindow);
            }
            break;
        case GS_GAMEPLAY: break;
        case GS_PAUSE:
            Window_Draw(pauseWindow);
            Window_Draw(settingsWindow);
            break;
        default: break;
    }
    if (consoleOpen) Console_Draw(myConsole);

    // Draw HUD
    //DrawRectangle(5, 5, 250, 1000, Fade(SKYBLUE, 0.5f));
    //DrawRectangleLines(5, 5, 250, 1000, BLUE);
    
    DrawText(TextFormat("Game State: %s", GetGameStateAsString(GAME_STATE)), 15, 15, 10, BLACK);
    DrawText(TextFormat("Time Passed: %0.2f", timePassed), 15, 30, 10, BLACK);
    DrawText(TextFormat("Current FPS: %d", GetFPS()), 15, 45, 10, BLACK);
    DrawText(TextFormat("Cam Target: %0.2f _ %0.2f _ %0.2f", camera.target.x, camera.target.y, camera.target.z), 15, 60, 10, BLACK);
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
    if (screenFade > 0){ screenFade -= 3*DT; }

    EndDrawing();
}

void ExecuteConsoleCommand(ConsoleCommand CC){
    switch(CC){
        case CC_END:
            //CloseWindow();
            break;
        case CC_RESET:
            ResetScene();
            break;
        case CC_MAIN:
            GoToMain();
            break;
        case CC_LOAD:
            break;
        case CC_NONE: break;
        default: break;
    }
}

void ExecuteButtonFunction(ButtonFunction btnfunc){
    switch (btnfunc){
        case BTN_MAIN:
            GoToMain();
            break;
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
            ChangeGameState(GS_GAMEPLAY);
            Window_Close(pauseWindow);
            Window_Close(settingsWindow);
            Window_Close(mainWindow);
            break;
        case BTN_TEST:
            ChangeGameState(GS_TEST);
            break;
        case BTN_SETTINGS_PAUSE:
            pauseWindow->isFocused = false;
            settingsWindow->parentWindow = pauseWindow;
            Window_Open(settingsWindow);
            break;
        case BTN_SETTINGS_MAIN:
            mainWindow->isFocused = false;
            settingsWindow->parentWindow = mainWindow;
            Window_Open(settingsWindow);
            break;
        case BTN_NONE: break;
        default: break;
    }
}

void GoToMain(){
    ResetScene();
    EnableCursor();
    ChangeGameState(GS_MAIN);
    camera.position = CAM_DEFAULT_POS;
    camera.target = CAM_DEFAULT_TARGET;
    consoleOpen = false;
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
            Bullet_Spawn(worldBullets[i], ray.position, ray.direction);
            break;
        }
    }
    PlaySoundInstance(bullet_shot, ray.position);
}

void SpawnWorldPoly(Vector3 newPos){
    for (int i = 0; i < WORLD_DEFAULT_LIMIT; i++){
        if (!worldPolys[i]->isActive){
            Poly_Spawn(worldPolys[i], newPos);
            break;
        }
    }
}

Pawn* SpawnWorldPawn(Vector3 newPos, PawnType pt){
    for (int i = 0; i < WORLD_DEFAULT_LIMIT; i++){
        if (!worldPawns[i]->isActive){
            Pawn_Spawn(worldPawns[i], newPos, pt);
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

void PrintToConsole(const char* out){
    Console_Print(myConsole, out);
}

const char* GetGameStateAsString(GameState gs){
   switch (gs) {
      case GS_MAIN:         return "MAIN";
      case GS_GAMEPLAY:     return "GAMEPLAY";
      case GS_PAUSE:        return "PAUSE";
      case GS_TEST:         return "TEST";
      case GS_TEST_PAUSE:   return "TEST_PAUSE";
      default:              return "???";
   }
}

void ChangeGameState(GameState gs){
    // Exit State
    switch (GAME_STATE){
        default: break;
    }
    GAME_STATE = gs;
    // Enter State
    switch (gs){
        case GS_MAIN:
            EnableCursor();
            break;
        case GS_GAMEPLAY:
            DisableCursor();
            break;
        case GS_TEST:
            DisableCursor();
            break;
        case GS_TEST_PAUSE:
            EnableCursor();
            break;
        default: break;
    }
}