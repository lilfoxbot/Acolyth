#include "raylib.h"
#include "rcamera.h"
#include "raymath.h"
#include <stdio.h>
#include <string.h>

#include "poly.h"
#include "cube.h"
#include "entity.c"

#define RAYMATH_IMPLEMENTATION

#define MOUSE_MOVE_SENSITIVITY 0.001f

#define CUBE_LIMIT 10
#define TRI_LIMIT 50
#define POLY_LIMIT 100
#define ENTITY_LIMIT 50

#define ENTITY_SPAWN_TIMER 1.0f
float entitySpawnTick = 1.0f;

bool myDebug = false;
float lookSensitivity = 40.0f;
float camSpeed = 2.0f;
float axisSize = 0.2f;
float armX = 0;
float armY = 0;
float armSpeed = 0.05f;
int score = 0;
float dt = 0;
float timePassed = 0;

struct _Cube cubes[CUBE_LIMIT];
struct _Poly polys[POLY_LIMIT];
struct Entity* hall_entities[ENTITY_LIMIT];
struct Entity* block_entities[ENTITY_LIMIT];
struct Entity* control_block = NULL;

int FindFreeEntitySlot(struct Entity* entityArr[], int arrSize){
    for (int i = 0; i < ENTITY_LIMIT; i++){
        if (entityArr[i] == NULL){
            return i;
        }
    }
    return -1;
}

void HallwayEntitySpawnTicker(){
    entitySpawnTick -= dt;
    if (entitySpawnTick <= 0){
        int i = FindFreeEntitySlot(hall_entities, ENTITY_LIMIT);
        if (i == -1) return;
        hall_entities[i] = CreateEntity(i, (Vector3){-4, GetRandomValue(0,3), -50});
        entitySpawnTick = ENTITY_SPAWN_TIMER;
    }
}

int main(void)
{
    const int screenWidth = 1280;
    const int screenHeight = 720;

    InitWindow(screenWidth, screenHeight, "Acolyth");
    DisableCursor();                    // Limit cursor to relative movement inside the window
    SetTargetFPS(60);                   // Set our game to run at 60 frames-per-second

    // Define the camera to look into our 3d world (position, target, up vector)
    Camera camera = { 0 };
    camera.position = (Vector3){ 0.0f, 2.0f, 0.0f };    // Camera position
    camera.target = (Vector3){ 0.0f, 2.0f, -2.0f };      // Camera looking at point
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
    camera.fovy = 60.0f;                                // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE;             // Camera projection type

    int cameraMode = CAMERA_CUSTOM;

    for (int i = 0; i < CUBE_LIMIT; i++){
        cubes[i].exist = 0;
    }
    
    struct Ray r1;
    r1.position = (Vector3){0,0,0};
    r1.direction = (Vector3){10,10,0};
    Color r1Color = WHITE;
    
    // struct BoundingBox b1;
    // b1.min = (Vector3){0,0,0};
    // b1.max = (Vector3){5,5,5};
    
    // READY ----------------------------------------------------------------------------------
    // printf("\n");
    // printf(TextFormat("%d", CUBE_LIMIT)));
    // printf("\n");

    // MAIN GAME LOOP ---------------------------------------------------------------------
    while (!WindowShouldClose())        // Detect window close button or ESC key
    {
        dt = GetFrameTime();
        timePassed += dt;
        
        // @INPUT --------------------------------------------------------------------------
        // Switch camera mode
        if (IsKeyPressed(KEY_ONE)){ myDebug = !myDebug; }
        if (IsKeyPressed(KEY_TWO)){ SetTargetFPS(60); }
        if (IsKeyPressed(KEY_THREE)){ SetTargetFPS(120); }

        // Switch camera projection
        if (IsKeyPressed(KEY_P)){
            if (camera.projection == CAMERA_PERSPECTIVE){
                // Create isometric view
                cameraMode = CAMERA_THIRD_PERSON;
                // Note: The target distance is related to the render distance in the orthographic projection
                camera.position = (Vector3){ 0.0f, 2.0f, -100.0f };
                camera.target = (Vector3){ 0.0f, 2.0f, 0.0f };
                camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
                camera.projection = CAMERA_ORTHOGRAPHIC;
                camera.fovy = 20.0f; // near plane width in CAMERA_ORTHOGRAPHIC
                CameraYaw(&camera, -135 * DEG2RAD, true);
                CameraPitch(&camera, -45 * DEG2RAD, true, true, false);
            } else if (camera.projection == CAMERA_ORTHOGRAPHIC) {
                // Reset to default view
                cameraMode = CAMERA_THIRD_PERSON;
                camera.position = (Vector3){ 0.0f, 2.0f, 10.0f };
                camera.target = (Vector3){ 0.0f, 2.0f, 0.0f };
                camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
                camera.projection = CAMERA_PERSPECTIVE;
                camera.fovy = 60.0f;
            }
        }
        
        // camera movement/input
        float newForward = 0;
        float newRight = 0;
        float newUp = 0;
        if (IsKeyDown(KEY_W)) newForward += camSpeed;
        if (IsKeyDown(KEY_S)) newForward -= camSpeed;
        if (IsKeyDown(KEY_D)) newRight += camSpeed;
        if (IsKeyDown(KEY_A)) newRight -= camSpeed;
        if (IsKeyDown(KEY_SPACE)) newUp += camSpeed;
        if (IsKeyDown(KEY_LEFT_CONTROL)) newUp -= camSpeed;
        
        if (IsKeyDown(KEY_LEFT)) armX -= armSpeed;
        if (IsKeyDown(KEY_RIGHT)) armX += armSpeed;
        if (IsKeyDown(KEY_UP)) armY += armSpeed;
        if (IsKeyDown(KEY_DOWN)) armY -= armSpeed;
        
        // new camera rotation
        Vector2 mousePositionDelta = GetMouseDelta();
        float newYaw = mousePositionDelta.x*MOUSE_MOVE_SENSITIVITY*lookSensitivity;
        float newPitch = mousePositionDelta.y*MOUSE_MOVE_SENSITIVITY*lookSensitivity;
        
        // Save block entity positions to text file
        if (IsKeyPressed(KEY_KP_DECIMAL)){
            char outString[1000] = "";
            // compile string of entity positions
            for (int i = 0; i < ENTITY_LIMIT; i++){
                if (block_entities[i] == NULL) break;
                strcat(outString, TextFormat("%0.2f,%0.2f,%0.2f\n", block_entities[i]->position.x, block_entities[i]->position.y, block_entities[i]->position.z));
            }
            strcat(outString, "\0");
            SaveFileText("entity_pos.txt", outString);
        }
        
        // Load block entity positions from text file
        if (IsKeyPressed(KEY_KP_MULTIPLY)){
            char* inString = LoadFileText("entity_pos.txt");
            char* line = strtok(inString, "\n");
            int i = 0;
            while (line != NULL && i < ENTITY_LIMIT){
                float x, y, z;
                sscanf(line, "%f,%f,%f", &x, &y, &z);
                if (block_entities[i] == NULL){
                    block_entities[i] = CreateEntity(i, (Vector3){x,y,z});
                } else {
                    block_entities[i]->position = (Vector3){x,y,z};
                }
                line = strtok(NULL, "\n");
                i++;
            }
            UnloadFileText(inString);
        }

        // create placeable block
        if (IsKeyPressed(KEY_KP_ADD)){
            int i = FindFreeEntitySlot(block_entities, ENTITY_LIMIT);
            if (i == -1) return -1;
            block_entities[i] = CreateEntity(i, (Vector3){0,0,0});
            control_block = block_entities[i];
            
        }

        // Undo
        if (IsKeyPressed(KEY_KP_SUBTRACT)){
            for (int i = ENTITY_LIMIT-1; i >= 0; i--){
                if (block_entities[i] != NULL){
                    DestroyEntity(block_entities[i]);
                    block_entities[i] = NULL;
                    break;
                }
            }
        }

        // Clear all blocks 
        if (IsKeyPressed(KEY_KP_DIVIDE)){
            for (int i = 0; i < ENTITY_LIMIT; i++){
                if (block_entities[i] != NULL){
                    DestroyEntity(block_entities[i]);
                    block_entities[i] = NULL;
                }
            }
            control_block = NULL;
        }

        if (IsKeyPressed(KEY_KP_8)){ if (control_block != NULL) control_block->position.z -= 1; }
        if (IsKeyPressed(KEY_KP_5)){ if (control_block != NULL) control_block->position.z += 1; }
        if (IsKeyPressed(KEY_KP_4)){ if (control_block != NULL) control_block->position.x -= 1; }
        if (IsKeyPressed(KEY_KP_6)){ if (control_block != NULL) control_block->position.x += 1; }
        if (IsKeyPressed(KEY_KP_7)){ if (control_block != NULL) control_block->position.y += 1; }
        if (IsKeyPressed(KEY_KP_1)){ if (control_block != NULL) control_block->position.y -= 1; }
        
        // @UPDATE -----------------------------------------------------------------------

        //HallwayEntitySpawnTicker();
        
        for (int i = 0; i < ENTITY_LIMIT; i++){
            UpdateEntity(hall_entities[i]);
        }

        UpdateCameraPro(&camera, 
            (Vector3){ newForward*dt, newRight*dt, newUp*dt }, // added pos
            (Vector3){ newYaw, newPitch, 0.0f }, // added rot
            0.0f); // zoom
        
        // @COLLISION ---------------------------------------------------------------------

            for (int i = 0; i < ENTITY_LIMIT; i++){
                if (hall_entities[i] == NULL) continue;
                if (hall_entities[i]->position.z > 5){
                    DestroyEntity(hall_entities[i]);
                    hall_entities[i] = NULL;
                }
            }
        
        // @DRAW --------------------------------------------------------------------------

        BeginDrawing();
            ClearBackground(DARKBLUE);
            BeginMode3D(camera);
            // Grid
            DrawGrid(20,1.0f);
            // Ground
            DrawPlane((Vector3){ 0.0f, -1.0f, 0.0f }, (Vector2){ 20.0f, 20.0f }, DARKGRAY);
            // Sun
            DrawSphere((Vector3){ 0.0f, 10.0f, -50.0f }, 1.0f, YELLOW); 
            DrawSphereWires((Vector3){ 0.0f, 10.0f, -50.0f }, 1.0f, 20, 20, WHITE); 

            for (int i = 0; i < ENTITY_LIMIT; i++){
                DrawEntity(hall_entities[i]);
                DrawEntity(block_entities[i]);
            }  
            
            // Debug axis
            if (myDebug){   
                // draw an axis with cubes
                // CENTER
                DrawCube(camera.target, axisSize, axisSize, axisSize, WHITE);
                DrawCubeWires(camera.target, axisSize, axisSize, axisSize, BLACK);
                // RIGHT
                DrawCube((Vector3){camera.target.x + 0.5f, camera.target.y, camera.target.z}, axisSize, axisSize, axisSize, RED);
                DrawCubeWires((Vector3){camera.target.x + 0.5f, camera.target.y, camera.target.z}, axisSize, axisSize, axisSize, BLACK);
                // UP
                DrawCube((Vector3){camera.target.x, camera.target.y + 0.5f, camera.target.z}, axisSize, axisSize, axisSize, GREEN);
                DrawCubeWires((Vector3){camera.target.x, camera.target.y + 0.5f, camera.target.z}, axisSize, axisSize, axisSize, BLACK);
                // BACK
                DrawCube((Vector3){camera.target.x, camera.target.y, camera.target.z + 0.5f}, axisSize, axisSize, axisSize, BLUE);
                DrawCubeWires((Vector3){camera.target.x, camera.target.y, camera.target.z + 0.5f}, axisSize, axisSize, axisSize, BLACK);
            }
            
            Vector3 camF = GetCameraForward(&camera);
            //Vector3 camR = GetCameraRight(&camera);
            //Vector3 camU = GetCameraUp(&camera);
            
            // My ARM?!
            Vector3 myTargetBegin = GetCameraRight(&camera);
            myTargetBegin = Vector3Scale(myTargetBegin, 0.5f);
            myTargetBegin = Vector3Add(myTargetBegin, Vector3Add(camera.position, (Vector3){0,-0.5f,0}));
            
            r1.position = myTargetBegin;
            r1.direction = camF;
            r1Color = (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) ? RED : WHITE;
            DrawRay(r1,r1Color);
            
            EndMode3D(); // ----------------------------------------------------------------
            
            // Draw HUD
            // Left side
            DrawRectangle(5, 5, 330, 100, Fade(SKYBLUE, 0.5f));
            DrawRectangleLines(5, 5, 330, 100, BLUE);
            
            DrawText("Move keys: W, A, S, D, Space, Left-Ctrl", 15, 15, 10, BLACK);
            DrawText(TextFormat("Time Passed - %0.2f", timePassed),15, 30, 10, BLACK);
            DrawText(TextFormat("Current FPS - %d", GetFPS()), 15, 45, 10, BLACK);
            
            //Right side
            DrawRectangle(1080, 5, 195, 100, Fade(SKYBLUE, 0.5f));
            DrawRectangleLines(1080, 5, 195, 100, BLUE);

            DrawText("Camera status:", 1090, 15, 10, BLACK);
            DrawText(TextFormat("- Mode: %s", (cameraMode == CAMERA_FREE) ? "FREE" :
                                              (cameraMode == CAMERA_FIRST_PERSON) ? "FIRST_PERSON" :
                                              (cameraMode == CAMERA_THIRD_PERSON) ? "THIRD_PERSON" :
                                              (cameraMode == CAMERA_ORBITAL) ? "ORBITAL" : "CUSTOM"), 1090, 30, 10, BLACK);
            DrawText(TextFormat("camTarget - %0.2f - %0.2f - %0.2f",camera.target.x,camera.target.y,camera.target.z), 1090, 45, 10, BLACK);
            DrawText(TextFormat("Score = %d", score), 1090, 60, 10, BLACK);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}
