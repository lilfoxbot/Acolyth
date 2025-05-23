#include "raylib.h"
#include "rcamera.h"
#include "raymath.h"
#include <stdio.h>

#define RAYMATH_IMPLEMENTATION

#define MOUSE_MOVE_SENSITIVITY 0.001f

#define CUBE_LIMIT 10
#define TRI_LIMIT 10

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
    //--------------------------------------------------------------------------------------
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 1280;
    const int screenHeight = 720;

    InitWindow(screenWidth, screenHeight, "Acolyth");
    DisableCursor();                    // Limit cursor to relative movement inside the window
    SetTargetFPS(60);                   // Set our game to run at 60 frames-per-second
    float dt = 0;
    float timePassed = 0;

    // Define the camera to look into our 3d world (position, target, up vector)
    Camera camera = { 0 };
    camera.position = (Vector3){ 0.0f, 2.0f, 0.0f };    // Camera position
    camera.target = (Vector3){ 0.0f, 2.0f, -2.0f };      // Camera looking at point
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
    camera.fovy = 60.0f;                                // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE;             // Camera projection type

    int cameraMode = CAMERA_CUSTOM;
    
    bool myDebug = false;
    float lookSensitivity = 40.0f;
    float camSpeed = 2.0f;
    float axisSize = 0.2f;
    float armX = 0;
    float armY = 0;
    float armSpeed = 0.05f;
    
    int score = 0;
    
    // TRI ----------------------------------------------------------------------------------
    typedef struct {
        Vector3 center;
        Vector3 one;
        Vector3 two;
        Vector3 three;
        Color myTriColor;
        Color myTriOutline;
    } _Tri;
    _Tri tris[10];
    
    // MY TRI
    Vector3 myTriCenter = (Vector3){ 0.0f, 3.0f, 0.0f };
    Vector3 myTriOne = Vector3Add(myTriCenter, (Vector3){ 0.0f, 1.0f, 0.0f });
    Vector3 myTriTwo = Vector3Add(myTriCenter, (Vector3){ 1.0f, -1.0f, 0.0f });
    Vector3 myTriThree = Vector3Add(myTriCenter, (Vector3){ -1.0f, -1.0f, 0.0f });
    Color myTriColor = YELLOW;
    Color myTriOutline = BLACK;
    
    // CUBE ----------------------------------------------------------------------------------
    typedef struct {
        int exist;
        float lifetime;
        float speed;
        Vector3 position;
        Vector3 size;
        struct BoundingBox bb;
        struct RayCollision col;
    } _Cube;
    _Cube cubes[CUBE_LIMIT];
    
    for (int i = 0; i < sizeof(cubes)/sizeof(cubes[0]); i++){
        cubes[i].exist = 0;
    }
    
    Vector2 cubeSpawnX = { -100, 100 };
    Vector2 cubeSpawnY = { 0, 100 };
    Vector2 cubeSpawnZ = { -200, -100 };
    float spawnMod = 0.1f;
    float cubeFallSpeed = 0.005;
    float cubeSize = 1.0f;
    float cubeSpawn = 1.0f;
    
    int SpawnStructCube(){
        // allocate index for new cube
        for (int i = 0; i < sizeof(cubes)/sizeof(cubes[0]); i++){
            if (cubes[i].exist != 1){
                cubes[i].exist = 1;
                cubes[i].lifetime = 0.0f;
                cubes[i].speed = 0.1f;
                cubes[i].position = (Vector3){(float)GetRandomValue(cubeSpawnX.x, cubeSpawnX.y)*spawnMod, 
                        (float)GetRandomValue(cubeSpawnY.x, cubeSpawnY.y)*spawnMod, 
                        (float)GetRandomValue(cubeSpawnZ.x, cubeSpawnZ.y)*spawnMod};
                cubes[i].size = (Vector3){0.5f,0.5f,0.5f};
                cubes[i].bb.min = Vector3Add(cubes[i].position, (Vector3){-(cubes[i].size.x/2),-(cubes[i].size.y/2),-(cubes[i].size.z/2)});
                cubes[i].bb.max = Vector3Add(cubes[i].position, (Vector3){(cubes[i].size.x/2),(cubes[i].size.y/2),(cubes[i].size.z/2)});
                return i;
            }
        }
        
        return -1;
    }
    
    void UpdateStructCube(int i, Vector3 addPos){
        cubes[i].position = (Vector3){cubes[i].position.x + addPos.x,
                                        cubes[i].position.y + addPos.y,
                                        cubes[i].position.z + addPos.z};
        cubes[i].bb.min = Vector3Add(cubes[i].position, (Vector3){-(cubes[i].size.x/2),-(cubes[i].size.y/2),-(cubes[i].size.z/2)});
        cubes[i].bb.max = Vector3Add(cubes[i].position, (Vector3){(cubes[i].size.x/2),(cubes[i].size.y/2),(cubes[i].size.z/2)});
        cubes[i].lifetime += dt;
    }
    
    void CheckStructCollision(int i, Ray cubeRay){
        cubes[i].col = GetRayCollisionBox(cubeRay, cubes[i].bb);
    }
    
    void DrawStructCube(int i){
        if (cubes[i].exist == 1){
            Color cubeColor = (cubes[i].col.hit) ? RED : BLUE;
            DrawCubeV(cubes[i].position, cubes[i].size, cubeColor);
            DrawBoundingBox(cubes[i].bb, WHITE);
        }
    }
    
    void DestroyStructCube(int i){
        cubes[i].exist = 0;
    }

    void CubeSpawnTicker(){
        cubeSpawn -= dt;
        if (cubeSpawn <= 0){
            SpawnStructCube();
            cubeSpawn = 1.0f;
        }
    }
    
    // arrays for test cubes
    Vector3 cubePos[CUBE_LIMIT] = { 0 };
    Vector3 cubeSizes[CUBE_LIMIT] = { 0 };
    float cubeSpeeds[CUBE_LIMIT] = { 0 };
    int cubeExists[CUBE_LIMIT] = { 0 };
    int cubeLifetimes[CUBE_LIMIT] = { 0 };
    struct BoundingBox cubeBBs[CUBE_LIMIT];
    struct RayCollision cubeRayHits[CUBE_LIMIT];
    
    void SpawnNewCube(int i){
        cubePos[i] = (Vector3){(float)GetRandomValue(cubeSpawnX.x, cubeSpawnX.y)*spawnMod, 
                        (float)GetRandomValue(cubeSpawnY.x, cubeSpawnY.y)*spawnMod, 
                        (float)GetRandomValue(cubeSpawnZ.x, cubeSpawnZ.y)*spawnMod};
        cubeSizes[i] = (Vector3){cubeSize, cubeSize, cubeSize};
        cubeSpeeds[i] = (float)GetRandomValue(1,4)*cubeFallSpeed;
        cubeExists[i] = 1;
        cubeLifetimes[i] = 0;
        cubeBBs[i].min = Vector3Add(cubePos[i], (Vector3){-(cubeSize/2),-(cubeSize/2),-(cubeSize/2)});
        cubeBBs[i].max = Vector3Add(cubePos[i], (Vector3){(cubeSize/2),(cubeSize/2),(cubeSize/2)});
    }
    
    // OTHER ----------------------------------------------------------------------------------
    struct Ray r1;
    r1.position = (Vector3){0,0,0};
    r1.direction = (Vector3){10,10,0};
    Color r1Color = WHITE;
    
    // struct BoundingBox b1;
    // b1.min = (Vector3){0,0,0};
    // b1.max = (Vector3){5,5,5};
    
    // Single Pass Cube Update
    void UpdateMyCubes(){
        for (int i = 0; i < CUBE_LIMIT; i++){
            //CheckCubeCollision
            cubeRayHits[i] = GetRayCollisionBox(r1,cubeBBs[i]);
            
            //DrawMyCube(i);
            if (cubeExists[i]){
                if (cubeRayHits[i].hit){
                    DrawCubeV(cubePos[i], cubeSizes[i], RED);
                    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
                        cubeExists[i] = 0;
                        score += 1;
                    }
                } else {
                    if (cubeLifetimes[i] <= 10){
                        DrawCubeV(cubePos[i], cubeSizes[i], WHITE);
                    } else {
                        DrawCubeV(cubePos[i], cubeSizes[i], SKYBLUE);
                    }
                }
                DrawBoundingBox(cubeBBs[i], WHITE);
                cubeLifetimes[i] += 1;
            } else {
                SpawnNewCube(i);
            }
            
            //MoveMyCube(i);
            cubePos[i] = (Vector3){ cubePos[i].x, cubePos[i].y, cubePos[i].z + cubeSpeeds[i]};
            cubeBBs[i].min = Vector3Add(cubePos[i], (Vector3){-(cubeSize/2),-(cubeSize/2),-(cubeSize/2)});
            cubeBBs[i].max = Vector3Add(cubePos[i], (Vector3){(cubeSize/2),(cubeSize/2),(cubeSize/2)});
            
            // Check Position
            if (cubePos[i].z > 10){
                //DespawnCube(i);
                cubeExists[i] = 0;
            }
        }
    }
    
    // READY ----------------------------------------------------------------------------------
    printf("\n");
    printf(TextFormat("%d", sizeof(cubes)/sizeof(cubes[0])));
    printf("\n");
    printf("\n");
    
    // MAIN GAME LOOP ---------------------------------------------------------------------
    while (!WindowShouldClose())        // Detect window close button or ESC key
    {
        dt = GetFrameTime();
        timePassed += dt;
        
        // INPUT --------------------------------------------------------------------------
        // Switch camera mode
        if (IsKeyPressed(KEY_ONE)){ myDebug = !myDebug; }
        if (IsKeyPressed(KEY_TWO)){ SetTargetFPS(60); }
        if (IsKeyPressed(KEY_THREE)){ SetTargetFPS(120); }

        // Switch camera projection
        if (IsKeyPressed(KEY_P)){
            if (camera.projection == CAMERA_PERSPECTIVE)
            {
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
            }
            else if (camera.projection == CAMERA_ORTHOGRAPHIC)
            {
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
        
        // UPDATE -----------------------------------------------------------------------

        //UpdateCamera(&camera, cameraMode);
        UpdateCameraPro(&camera, 
            (Vector3){ newForward*dt, newRight*dt, newUp*dt }, // added pos
            (Vector3){ newYaw, newPitch, 0.0f }, // added rot
            0.0f); // zoom
            
        // Update MY TRI
            myTriOne = Vector3RotateByAxisAngle(myTriOne,myTriCenter, 0.1f);
            myTriTwo = Vector3RotateByAxisAngle(myTriTwo,myTriCenter, 0.1f);
            myTriThree = Vector3RotateByAxisAngle(myTriThree,myTriCenter, 0.1f);
        
        // UpdateStructCube(shakeCube, (Vector3){ (float)GetRandomValue(-1,1)*0.001f, 
                                                            // (float)GetRandomValue(-1,1)*0.001f, 
                                                            // (float)GetRandomValue(-1,1)*0.001f });
        // UpdateStructCube(speedCube, (Vector3){ 1.0f*dt,0,0 });
        
        CubeSpawnTicker();
        
        // COLLISION ---------------------------------------------------------------------
        for (int i = 0; i < CUBE_LIMIT; i++){
            CheckStructCollision(i, r1);
        }
        
        // DRAW --------------------------------------------------------------------------
        BeginDrawing();
            ClearBackground(DARKBLUE);
            BeginMode3D(camera);
            // Grid
            DrawGrid(20,1.0f);
            // Ground
            DrawPlane((Vector3){ 0.0f, -1.0f, 0.0f }, (Vector2){ 100.0f, 100.0f }, LIME);
            // Sun
            DrawSphere((Vector3){ 0.0f, 10.0f, -50.0f }, 1.0f, YELLOW); 
            DrawSphereWires((Vector3){ 0.0f, 10.0f, -50.0f }, 1.0f, 20, 20, WHITE); 
            
            // Draw MY TRI
            DrawTriangle3D(myTriOne,myTriTwo,myTriThree, myTriColor);
            DrawTriangle3D(myTriOne,myTriThree,myTriTwo, myTriColor);
            DrawLine3D(myTriOne,myTriTwo,myTriOutline);
            DrawLine3D(myTriTwo,myTriThree,myTriOutline);
            DrawLine3D(myTriThree,myTriOne,myTriOutline);

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
            Vector3 camR = GetCameraRight(&camera);
            Vector3 camU = GetCameraUp(&camera);
            
            // My ARM?!
            Vector3 myTargetBegin = GetCameraRight(&camera);
            myTargetBegin = Vector3Scale(myTargetBegin, 0.5f);
            myTargetBegin = Vector3Add(myTargetBegin, Vector3Add(camera.position, (Vector3){0,-0.5f,0}));
            
            r1.position = myTargetBegin;
            r1.direction = camF;
            r1Color = (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) ? RED : WHITE;
            DrawRay(r1,r1Color);
            
            // Draw Cubes
            for (int i = 0; i < sizeof(cubes)/sizeof(cubes[0]); i++){
                DrawStructCube(i);
            }
            
            EndMode3D(); // ----------------------------------------------------------------
            
            // Draw info boxes
            // Left side
            DrawRectangle(5, 5, 330, 100, Fade(SKYBLUE, 0.5f));
            DrawRectangleLines(5, 5, 330, 100, BLUE);
            
            DrawText("Move keys: W, A, S, D, Space, Left-Ctrl", 15, 15, 10, BLACK);
            DrawText(TextFormat("Time Passed - %0.2f", timePassed),15, 30, 10, BLACK);
            DrawText(TextFormat("TargetFPS - %d", GetFPS()), 15, 45, 10, BLACK);
            
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