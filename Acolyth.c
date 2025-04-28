#include "raylib.h"
#include "rcamera.h"
#include <stdio.h>

#define MAX_COLUMNS 20
#define MOUSE_MOVE_SENSITIVITY 0.004f

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

    // Define the camera to look into our 3d world (position, target, up vector)
    Camera camera = { 0 };
    camera.position = (Vector3){ 0.0f, 2.0f, 4.0f };    // Camera position
    camera.target = (Vector3){ 0.0f, 2.0f, 0.0f };      // Camera looking at point
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
    camera.fovy = 60.0f;                                // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE;             // Camera projection type

    int cameraMode = CAMERA_CUSTOM;
    
    bool myDebug = false;
    float lookSensitivity = 10.0f;
    float camSpeed = 0.1f;
    float axisSize = 0.2f;
    
    //-------------------------------------------------------------------------------------

    // Generates some random columns
    // float heights[MAX_COLUMNS] = { 0 };
    // Vector3 positions[MAX_COLUMNS] = { 0 };
    // Color colors[MAX_COLUMNS] = { 0 };

    // for (int i = 0; i < MAX_COLUMNS; i++)
    // {
        // heights[i] = (float)GetRandomValue(1, 12);
        // positions[i] = (Vector3){ (float)GetRandomValue(-15, 15), heights[i]/2.0f, (float)GetRandomValue(-15, 15) };
        // colors[i] = (Color){ GetRandomValue(20, 255), GetRandomValue(10, 55), 30, 255 };
    // }

    DisableCursor();                    // Limit cursor to relative movement inside the window

    SetTargetFPS(60);                   // Set our game to run at 60 frames-per-second
    
    //--------------------------------------------------------------------------------------
    // Main game loop
    //--------------------------------------------------------------------------------------
    while (!WindowShouldClose())        // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        // Switch camera mode
        if (IsKeyPressed(KEY_ONE))
        {
            myDebug = !myDebug;
        }

        // Switch camera projection
        if (IsKeyPressed(KEY_P))
        {
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
        
        float newForward = 0;
        float newRight = 0;
        float newUp = 0;
        // new camera location
        if (IsKeyDown(KEY_W)){
            newForward += camSpeed;
        }
        if (IsKeyDown(KEY_S)){
            newForward -= camSpeed;
        }
        if (IsKeyDown(KEY_D)){
            newRight += camSpeed;
        }
        if (IsKeyDown(KEY_A)){
            newRight -= camSpeed;
        }
        if (IsKeyDown(KEY_SPACE)){
            newUp += camSpeed;
        }
        if (IsKeyDown(KEY_LEFT_CONTROL)){
            newUp -= camSpeed;
        }
        
        // new camera rotation
        Vector2 mousePositionDelta = GetMouseDelta();
        float newYaw = mousePositionDelta.x*MOUSE_MOVE_SENSITIVITY*lookSensitivity;
        float newPitch = mousePositionDelta.y*MOUSE_MOVE_SENSITIVITY*lookSensitivity;

        //UpdateCamera(&camera, cameraMode);
        UpdateCameraPro(&camera, 
            (Vector3){ newForward, newRight, newUp }, // added pos
            (Vector3){ newYaw, newPitch, 0.0f }, // added rot
            0.0f); // zoom

        //----------------------------------------------------------------------------------
        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(SKYBLUE);

            BeginMode3D(camera);
                // Draw Ground
                DrawPlane((Vector3){ 0.0f, -1.0f, 0.0f }, (Vector2){ 32.0f, 32.0f }, LIGHTGRAY);
                // Draw Sun?
                DrawSphere((Vector3){ 0.0f, 5.0f, 0.0f }, 1.0f, YELLOW); 
                DrawSphereWires((Vector3){ 0.0f, 5.0f, 0.0f }, 1.0f, 20, 20, ORANGE);
                
                DrawCylinder((Vector3){camera.target.x, camera.target.y, camera.target.z}, 0.25f, 0.25f, 0.5f, 8, WHITE);
                DrawCylinderWires((Vector3){camera.target.x, camera.target.y, camera.target.z}, 0.25f, 0.25f, 0.5f, 8, BLACK);

                // Draw axis
                if (myDebug)
                {   
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

            EndMode3D();

            // Draw info boxes
            DrawRectangle(5, 5, 330, 100, Fade(SKYBLUE, 0.5f));
            DrawRectangleLines(5, 5, 330, 100, BLUE);

            DrawText("Camera controls:", 15, 15, 10, BLACK);
            DrawText("- Move keys: W, A, S, D, Space, Left-Ctrl", 15, 30, 10, BLACK);
            
            //----------------------------------------------------------------------------------
            DrawRectangle(1080, 5, 195, 100, Fade(SKYBLUE, 0.5f));
            DrawRectangleLines(1080, 5, 195, 100, BLUE);

            DrawText("Camera status:", 1090, 15, 10, BLACK);
            DrawText(TextFormat("- Mode: %s", (cameraMode == CAMERA_FREE) ? "FREE" :
                                              (cameraMode == CAMERA_FIRST_PERSON) ? "FIRST_PERSON" :
                                              (cameraMode == CAMERA_THIRD_PERSON) ? "THIRD_PERSON" :
                                              (cameraMode == CAMERA_ORBITAL) ? "ORBITAL" : "CUSTOM"), 1090, 30, 10, BLACK);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}