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
#include "turret.h"
#include "button.h"

#define RAYMATH_IMPLEMENTATION

#define MOUSE_MOVE_SENSITIVITY 0.001f

#define WORLD_PAWN_LIMIT 20
#define WORLD_BULLET_LIMIT 100
#define WORLD_POLY_LIMIT 100

#define BOXTREE_INITIAL_SIZE 16

Vector3 CAM_DEFAULT_POS = (Vector3){ 0.0f, 2.0f, 0.0f };
Vector3 CAM_DEFAULT_TARGET = (Vector3){ 0.0f, 2.0f, -2.0f };

float screenFade = 0.2f;
bool screenFading = false;
bool cursorEnabled = false;
bool myDebug = false;
bool editMode = false;
float lookSensitivity = 40.0f;
float camSpeed = 2.0f;
float axisSize = 0.2f;
float armX = 0;
float armY = 0;
float armSpeed = 0.05f;
int score = 0;
float dt = 0;
float timePassed = 0;
int saveIndex = 0;

const int OCT = 8; // octree root size
const int LEVEL_GRID_ROWS = 10;
const int LEVEL_GRID_COLS = 5;
const int LEVEL_GRID_DEPTH = 10;
const float LEVEL_GRID_CELL_SIZE = 1.0f;

struct Button* menuButtons[4];

struct Pawn* worldPawns[WORLD_PAWN_LIMIT];
struct Turret* worldTurrets[WORLD_PAWN_LIMIT];
struct Bullet* worldBullets[WORLD_BULLET_LIMIT];
struct Poly* worldPolys[WORLD_POLY_LIMIT];
struct Player* player;
int worldBulletCount = 0;

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
    for (int i = 0; i < WORLD_BULLET_LIMIT; i++){
        if (!worldBullets[i]->isActive){
            Spawn_Bullet(worldBullets[i], ray.position, ray.direction);
            break;
        }
    }
}

void SpawnWorldPoly(Vector3 newPos){
    for (int i = 0; i < WORLD_POLY_LIMIT; i++){
        if (!worldPolys[i]->isActive){
            Spawn_Poly(worldPolys[i], newPos);
            break;
        }
    }
}

Turret* SpawnWorldTurret(Vector3 newPos){
    for (int i = 0; i < WORLD_PAWN_LIMIT; i++){
        if (!worldTurrets[i]->isActive){
            Spawn_Turret(worldTurrets[i], newPos);
            return worldTurrets[i];
        }
    }
    return NULL;
}

bool IsRayHitNormalValid(Vector3 vector){
    if (vector.x != 0 && vector.x != 1 && vector.x != -1){ return false; }
    if (vector.y != 0 && vector.y != 1 && vector.y != -1){ return false; }
    if (vector.z != 0 && vector.z != 1 && vector.z != -1){ return false; }

    return true;
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

// void pointer (Thinking Emoji)
int GetEmptyArraySlot(void *arr[], int limit){
    for (int i = 0; i < limit; i++){
        if (arr[i] == NULL) return i;
        // this wouldn't work, because the arrays are not null (object pool)
    }
    return -1;
}

typedef enum {
    SS_VOXEL,
    SS_TURRET
} SpawnSelection;

SpawnSelection ss = SS_VOXEL;

int main(void) // @INIT ========================================================================
{
    const int screenWidth = 1280;
    const int screenHeight = 720;

    InitWindow(screenWidth, screenHeight, "Tandem");
    SetTargetFPS(60);
    InitAudioDevice();
    DisableCursor();

    Sound testSFX = LoadSound("resources/sound/blip.wav");

    // Define the camera to look into our 3d world (position, target, up vector)
    Camera camera = { 0 };
    camera.position = CAM_DEFAULT_POS;    // Camera position
    camera.target = CAM_DEFAULT_TARGET;      // Camera looking at point
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
    camera.fovy = 60.0f;                                // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE;             // Camera projection type

    BoxtreeNode* boxtreeRoot = BuildBoxtree((Vector3){0,0,0}, BOXTREE_INITIAL_SIZE, 1);

    // @GRID init
    Vector3 gridOrigin = (Vector3){-4.5f, 0.0f, -4.5f};
    int gridIndex = 0;

    struct Voxel* grid3d[LEVEL_GRID_ROWS][LEVEL_GRID_COLS][LEVEL_GRID_DEPTH];
    char levelString[LEVEL_GRID_ROWS*LEVEL_GRID_COLS*LEVEL_GRID_DEPTH];
    char levelStringSave[] = "11111111110000000010000000001000000000100000000010111111111100000000000000000000000000000000000000101111111111000000000000000000000000000000000000001011111111110000000010000000001000000000100000000010111111111100000000000000000000000000000000000000001111111111000000000000000000000000000000000000000011111111110000000000000000000000000000000000000000111111111100000000000000000000000000000000000000001111111111000000000000000000000000000000000000000011111111110000000000000000000000000000000000000000";
    
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

    // @BULLET init
    for (int i = 0; i < WORLD_BULLET_LIMIT; i++){
        worldBullets[i] = Create_Bullet();
    }

    // @PAWN init
    for (int i = 0; i < WORLD_PAWN_LIMIT; i++){
        worldPawns[i] = Create_Pawn();
        worldTurrets[i] = Create_Turret();
    }

    // @POLY init
    for (int i = 0; i < WORLD_POLY_LIMIT; i++){
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

    // @Menu init
    for (int i = 0; i < 4; i++){
        menuButtons[i] = Create_Button();
    }

    //Mesh myMesh = GenMeshCube(1, 1, 1);
    //Model placeHolderModel = LoadModelFromMesh(myMesh);
    //Model myModel = LoadModel("resources/models/myCube.obj");
    
    // READY ==========================================================================

    //Spawn_Pawn(worldPawns[0], SEEKER, (Vector3){3,6,0});
    //Spawn_Pawn(worldPawns[1], SHOOTER, (Vector3){3,2,-3});

    Spawn_Button(menuButtons[0], (Vector2){1200,120}, "save", BTN_SAVE);
    Spawn_Button(menuButtons[1], (Vector2){1200,160}, "load", BTN_LOAD);

    Spawn_Player(player, (Vector3){0,5,-3});
    
    // printf("\n");
    // printf(TextFormat("%d", sizeof(levelCells) / sizeof(levelCells[0])));
    // printf("\n");

    // MAIN GAME LOOP ==========================================================================
    while (!WindowShouldClose())        // Detect window close button or ESC key
    {
        dt = GetFrameTime();
        timePassed += dt;
        
        // @INPUT ==========================================================================

        Vector4 newPlayerVel = (Vector4){0,0,0,0};
        float ps = 2.0f;
        
        if (IsKeyDown(KEY_LEFT)){ newPlayerVel.x += -ps; }
        if (IsKeyDown(KEY_RIGHT)){ newPlayerVel.x += ps; }
        if (IsKeyDown(KEY_UP)){ newPlayerVel.z += -ps; }
        if (IsKeyDown(KEY_DOWN)){ newPlayerVel.z += ps; }
        if (IsKeyPressed(KEY_RIGHT_CONTROL)){ newPlayerVel.w = 1; }

        if (IsMouseButtonPressed(MOUSE_BUTTON_MIDDLE)){
            if (!cursorEnabled){
                EnableCursor();
            } else {
                DisableCursor();
            }
            cursorEnabled = !cursorEnabled;
        }

        if (IsKeyPressed(KEY_P)){ myDebug = !myDebug; }
        if (IsKeyPressed(KEY_MINUS)){ SetTargetFPS(60); }
        if (IsKeyPressed(KEY_EQUAL)){ SetTargetFPS(120); }

        if (IsKeyPressed(KEY_E)){
            editMode = !editMode;
            r1Color = (editMode) ? WHITE : RED;
        }
        if (IsKeyPressed(KEY_R)){ screenFading = true; }

        if (IsKeyPressed(KEY_ONE)){ ss = SS_VOXEL; }
        if (IsKeyPressed(KEY_TWO)){ ss = SS_TURRET; }
        
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
        if (!cursorEnabled){
            Vector2 mousePositionDelta = GetMouseDelta();
            float newYaw = mousePositionDelta.x*MOUSE_MOVE_SENSITIVITY*lookSensitivity;
            float newPitch = mousePositionDelta.y*MOUSE_MOVE_SENSITIVITY*lookSensitivity;

            UpdateCameraPro(&camera, 
            (Vector3){ newForward*dt, newRight*dt, newUp*dt }, // added pos
            (Vector3){ newYaw, newPitch, 0.0f }, // added rot
            0.0f); // zoom
        }
        
        // @UPDATE ==========================================================================
        Vector2 mousePos = GetMousePosition();

        if (cursorEnabled) for (int i = 0; i < 4; i++){
            int lsIndex = 0;
            ButtonFunction btnfunc = Update_Button(menuButtons[i], mousePos);
            switch (btnfunc){
                case BTN_SAVE:
                    PlaySound(testSFX);
                    
                    for (int x = 0; x < LEVEL_GRID_ROWS; x++){
                        for (int y = 0; y < LEVEL_GRID_COLS; y++){
                            for (int z = 0; z < LEVEL_GRID_DEPTH; z++){
                                if (grid3d[x][y][z]->isActive == true){
                                    levelString[lsIndex] = '1';
                                } else {
                                    levelString[lsIndex] = '0';
                                }
                                lsIndex++;
                            }
                        }
                    }
                    SaveFileText("level.txt", (char *)levelString);

                    break;
                case BTN_LOAD:
                    PlaySound(testSFX);
                    for (int x = 0; x < LEVEL_GRID_ROWS; x++){
                        for (int y = 0; y < LEVEL_GRID_COLS; y++){
                            for (int z = 0; z < LEVEL_GRID_DEPTH; z++){
                                grid3d[x][y][z]->isActive = false;
                                if (levelStringSave[lsIndex] == '1'){
                                    grid3d[x][y][z]->isActive = true;
                                }
                                lsIndex++;
                            }
                        }
                    }

                    break;
                case BTN_NONE:
                    break;
                default:
                    break;
            }
        }

        for (int i = 0; i < WORLD_POLY_LIMIT; i++){ Update_Poly(worldPolys[i], dt); }
        
        for (int i = 0; i < WORLD_BULLET_LIMIT; i++){ Update_Bullet(worldBullets[i], dt); }

        for (int i = 0; i < WORLD_PAWN_LIMIT; i++){
            int pawnAction = Update_Pawn(worldPawns[i], dt);
            switch (pawnAction){
                case 1:
                    SpawnWorldBullet(worldPawns[i]->aimRay);
                    break;
                default:
                    break;
            }
        }

        for (int i = 0; i < WORLD_PAWN_LIMIT; i++){
            int turretAction = Update_Turret(worldTurrets[i], dt);
            switch (turretAction){
                case 1:
                    SpawnWorldBullet(worldTurrets[i]->aimRay);
                    break;
                default:
                    break;
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
        
        // @COLLISION ==========================================================================
        
        ResetBoxtree(boxtreeRoot);
        Voxel* voxelHits[50] = {NULL};
        GetRayVoxels(r1, boxtreeRoot, voxelHits, 50);

        Vector3 rayHitNormal = (Vector3){0,0,0};
        Vector3 playerColNormal = (Vector3){0,0,0};
        float closestVoxelDist = 100;
        struct Voxel* closestHitVoxel = NULL;

        // player checkin
        Reset_Player(player);
        GetPlayerNodes(player, boxtreeRoot);

        // pawn checkin
        for (int i = 0; i < WORLD_PAWN_LIMIT; i++){
            if (!worldPawns[i]->isActive) continue;
            Reset_Pawn(worldPawns[i]);
            GetPawnNodes(worldPawns[i], boxtreeRoot);
        }
        
        // bullet checkin
        for (int i = 0; i < WORLD_BULLET_LIMIT; i++){
            if (!worldBullets[i]->isActive) continue;
            Reset_Bullet(worldBullets[i]);
            GetBulletNodes(worldBullets[i], boxtreeRoot);
        }

        // turret checkin
        for (int i = 0; i < WORLD_PAWN_LIMIT; i++){
            if (!worldTurrets[i]->isActive) continue;
            Reset_Turret(worldTurrets[i]);
            GetTurretNodes(worldTurrets[i], boxtreeRoot);
        }

        // =====================

        // bullet collision
        for (int i = 0; i < WORLD_BULLET_LIMIT; i++){
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
                
                // Turrets
                for (int m = 0; m < worldBullets[i]->nodes[j]->turretCount; m++){
                    if (CheckCollisionBoxes(worldBullets[i]->bb, worldBullets[i]->nodes[j]->turrets[m]->bb)){
                        if (worldBullets[i]->nodes[j]->turrets[m]->isActive){
                            Turret* hitTurret = worldBullets[i]->nodes[j]->turrets[m];

                            // if bullet has not hit this target, add to hitTargets
                            if (!ContainsInstance(worldBullets[i]->hitTargets, 8, hitTurret)){
                                worldBullets[i]->hitTargets[worldBullets[i]->hitCount] = hitTurret;
                                worldBullets[i]->hitCount++;
                                hitTurret->color = WHITE;
                                Damage_Turret(hitTurret);
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

                switch(ss){
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

                            Vector3 NVC2 = Vector3Add(closestHitVoxel->coordinates, Vector3Scale(rayHitNormal, 2));
                            Voxel* targetVoxel2 = grid3d[(int)Clamp(NVC2.x,0,LEVEL_GRID_ROWS-1)]
                            [(int)Clamp(NVC2.y,0,LEVEL_GRID_COLS-1)]
                            [(int)Clamp(NVC2.z,0,LEVEL_GRID_DEPTH-1)];

                            // check if (2) voxels are occupied or active
                            if (targetVoxel->isOccupied == true || targetVoxel->isActive == true){ break; }
                            if (targetVoxel2->isOccupied == true || targetVoxel2->isActive == true){ break; }
                            
                            Turret* newTurret = SpawnWorldTurret(Vector3Add(closestHitVoxel->position, Vector3Scale(rayHitNormal,1.5f)));
                            targetVoxel->isOccupied = true;
                            newTurret->occupiedVoxels[0] = targetVoxel;
                            targetVoxel2->isOccupied = true;
                            newTurret->occupiedVoxels[1] = targetVoxel2;

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
        
        // @DRAW ==========================================================================

        BeginDrawing();
            ClearBackground(GRAY);
            BeginMode3D(camera);
            
            // North Star
            DrawSphere((Vector3){ 0.0f, 10.0f, -50.0f }, 1.0f, YELLOW);
            DrawSphereWires((Vector3){ 0.0f, 10.0f, -50.0f }, 1.0f, 20, 20, WHITE);

            DrawGrid(10, 1.0f);
            DrawCubeWires((Vector3){0,0,0}, 10, 0.2, 10, WHITE);

            if (myDebug) DrawBoxtreeNode(boxtreeRoot);

            for (int i = 0; i < WORLD_POLY_LIMIT; i++){
                Draw_Poly(worldPolys[i]);
            }

            for (int i = 0; i < WORLD_PAWN_LIMIT; i++){
                Draw_Pawn(worldPawns[i]);
                Draw_Turret(worldTurrets[i]);
            }

            for (int i = 0; i < WORLD_BULLET_LIMIT; i++){
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
            
            DrawRay(r1,r1Color);
            DrawRay(voxelRay, WHITE);
            
            EndMode3D(); // ==========================================================================

            if (cursorEnabled) for (int i = 0; i < 4; i++){ Draw_Button(menuButtons[i]); }

            // Draw HUD
            // Left side
            DrawRectangle(5, 5, 250, 100, Fade(SKYBLUE, 0.5f));
            DrawRectangleLines(5, 5, 250, 100, BLUE);
            
            DrawText(TextFormat("Time Passed: %0.2f", timePassed), 15, 15, 10, BLACK);
            DrawText(TextFormat("Current FPS: %d", GetFPS()), 15, 30, 10, BLACK);
            DrawText(TextFormat("Cam Target: %0.2f _ %0.2f _ %0.2f", camera.target.x, camera.target.y, camera.target.z), 15, 45, 10, BLACK);
            DrawText(TextFormat("Edit Mode: %s", (editMode) ? "ON" : "OFF"), 15, 75, 10, BLACK);

            switch(ss){
                case SS_VOXEL:
                    DrawText(TextFormat("Spawn Selection: Voxel"), 15, 90, 10, BLACK);
                    break;
                case SS_TURRET:
                    DrawText(TextFormat("Spawn Selection: Turret"), 15, 90, 10, BLACK);
                    break;
                default:
                    break;
            }
            
            // Right side
            DrawRectangle(1080, 5, 195, 100, Fade(SKYBLUE, 0.5f));
            DrawRectangleLines(1080, 5, 195, 100, BLUE);
            
            DrawText(TextFormat("Player Col Normal: %0.1f _ %0.1f _ %0.1f",
                playerColNormal.x,
                playerColNormal.y,
                playerColNormal.z), 1090, 15, 10, BLACK);
            
            // Screen Fade
            if (screenFading){
                screenFade += 2*dt;
                DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, screenFade));
                if (screenFade >= 1){ 
                    screenFading = false;
                    screenFade = 0;

                    // Do Scene Reset
                    
                    camera.position = CAM_DEFAULT_POS;
                    camera.target = CAM_DEFAULT_TARGET;

                    for (int i = 0; i < WORLD_BULLET_LIMIT; i++){
                        worldBullets[i]->isActive = false;
                    }

                    for (int i = 0; i < WORLD_POLY_LIMIT; i++){
                        worldPolys[i]->isActive = false;
                    }

                    for (int i = 0; i < WORLD_PAWN_LIMIT; i++){
                        worldTurrets[i]->isActive = false;
                    }

                    // reset grid
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

                    player->position = (Vector3){0, 5,-3};
                    player->velocity = (Vector3){0,0,0};
                }
            }

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