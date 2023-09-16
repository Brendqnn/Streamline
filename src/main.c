#include <stdio.h>
#include <stdlib.h>

#define RAYGUI_IMPLEMENTATION
#include "../include/raygui.h"
#include "../include/raylib.h"
#include "slcodec.h"

#include "slio.h"
#include "slcodec.h"
#include "slcompressor.h"

#define WINDOW_HEIGHT 1200
#define WINDOW_WIDTH 800


int main(void) {
    InitWindow(WINDOW_HEIGHT, WINDOW_WIDTH, "Streamline v1.0");

    Rectangle box = {100, 100, 100, 100}; // Initial box position and size
    bool isDragging = false;
    Vector2 offset = {0, 0};
    
    SetTargetFPS(60);

    SLio *io = init_io("res/feel better.mp4");
    SLcodec *codec = init_codec(io);
    SLcompressor *compressor = init_compressor(io, codec);
            
    while (!WindowShouldClose()) {
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(GetMousePosition(), box)) {
            isDragging = true;
            offset = (Vector2){GetMouseX() - box.x, GetMouseY() - box.y};
        } else if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
            isDragging = false;
        }

        if (isDragging) {
            box.x = GetMouseX() - offset.x;
            box.y = GetMouseY() - offset.y;
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        if (GuiButton((Rectangle){10, 10, 120, 40}, "Start")) {
            compressor_setup(compressor, codec, io);
            compress(compressor, codec, io);
            free_compressor(compressor);
        }
               
        DrawRectangleRec(box, RED);

        EndDrawing();
    }
    //free_compressor(compressor);
    CloseWindow();
    return 0;
}

