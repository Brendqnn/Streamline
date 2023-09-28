#include <stdio.h>
#include <stdlib.h>

#include "../include/util.h"
#define RAYLIB_IMPLEMENTATION
#include "../include/raylib.h"
#define RAYGUI_IMPLEMENTATION
#include "../include/raygui.h"

#include "slio.h"


int main(void) {
    InitWindow(WINDOW_HEIGHT, WINDOW_WIDTH, "Streamline v1.0");

    Rectangle box = {100, 100, 100, 100}; // Initial box position and size
    bool isDragging = false;
    Vector2 offset = {0, 0};

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        
        BeginDrawing();
        ClearBackground(RAYWHITE);

        if (GuiButton((Rectangle){10, 10, 120, 40}, "Start")) {
            
        }

        DrawRectangleRec(box, RED);

        EndDrawing();
    }
    
    CloseWindow();
    return 0;
}

