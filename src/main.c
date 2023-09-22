/*
    MIT License

    Copyright (c) 2023 Le Juez Victor

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
*/

#include <raylib.h>
#include <stdlib.h>
#include <stdint.h>

#define M7_IMPL
#include "m7.h"

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

/* PRE DECLARATION */

Texture2D GenTextureGrid(uint16_t gridSize, uint8_t cellSize);
void DrawRenderInfo(M7_Camera* camera);

/* PROGRAM */

int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "M7-Demo");

    // Data loading

    Texture2D textureGrid = GenTextureGrid(512, 64);
    Texture2D textureGround = LoadTexture("res/ground.png");
    Texture2D textureCharacter = LoadTexture("res/character.png");

    Rectangle srcTexCharac = { 0, 0, textureCharacter.width, textureCharacter.height };

    // Camera setup

    M7_Camera camera = M7_Camera_Load(GetScreenWidth(), GetScreenHeight(), (Vector2) {0}, 0.0f, 80.0f, 0.5f, 0.5f, 48);

    // Placement of elements to be displayed

    M7_Element *firstCharacter = M7_Texture_Add(&camera, textureCharacter, srcTexCharac, (Vector2) { 0, 0 }, (Vector2) { 8, 8 }, WHITE);

    for (int i = 1; i < 10; i++)
    {
        M7_Texture_Add(&camera, textureCharacter, srcTexCharac, (Vector2) { 0, i * -16 }, (Vector2) { 12, 12 }, WHITE);
        M7_Texture_Add(&camera, textureCharacter, srcTexCharac, (Vector2) { -16, i * -16 }, (Vector2) { 8, 8 }, WHITE);
        M7_Texture_Add(&camera, textureCharacter, srcTexCharac, (Vector2) { 16, i * -16 }, (Vector2) { 8, 8 }, WHITE);
        M7_Texture_Add(&camera, textureCharacter, srcTexCharac, (Vector2) { -32, i * -16 }, (Vector2) { 8, 8 }, WHITE);
        M7_Texture_Add(&camera, textureCharacter, srcTexCharac, (Vector2) { 32, i * -16 }, (Vector2) { 8, 8 }, WHITE);
    }

    M7_Rectangle_Add(&camera, (Rectangle) { 64, 64, 16, 16 }, RED);
    M7_Circle_Add(&camera, (Vector2) { -64, 64 }, 8, YELLOW);

    // Main loop

    while (!WindowShouldClose())
    {
        // Automatic camera control

        M7_Camera_Move(&camera, 64);

        // Update movement of the first character using left mouse button click

        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
        {
            Vector2 wPos = M7_ToWorld(&camera, GetMousePosition());
            firstCharacter->onWorld.position = wPos;
        }

        // The commented-out call below is used to render everything automatically in one call,
        // but this method only supports displaying a single ground texture.

        //M7_Camera_Update(
        //    &camera,
        //    textureGround,
        //    (Vector2) { 0, 0 },
        //    (Vector2){ 8.0f, 8.0f },
        //    IsKeyDown(KEY_SPACE), BLUE);

        // Update mode7 rendering (advanced method to render multiple ground textures)

        M7_Camera_Begin(&camera, BLUE);

            if (IsKeyDown(KEY_SPACE))
            {
                M7_Camera_DrawPlane(&camera, textureGrid, (Vector2) {0},
                    (Vector2) { 256, 256 }, (Vector2){ 1.0f, 1.0f }, true);

                goto skipTilesRendering;
            }

            // NOTE: Displaying as many planes for performance testing purposes

            for (int y = -256; y < 256; y += textureGround.height)
            {
                for (int x = -256; x < 256; x += textureGround.width)
                {
                    M7_Camera_DrawPlane(
                        &camera, textureGround, (Vector2) { x, y },
                        (Vector2) { textureGround.width, textureGround.height },
                        (Vector2){ 1.0f, 1.0f }, false);
                }
            }

            for (int y = -1; y <= 1; y++)
            {
                for (int x = -1; x <= 1; x++)
                {
                    if (x || y) M7_Camera_DrawPlane(
                        &camera, textureGrid, (Vector2) { x * 512, y * 512 },
                        (Vector2) { 256, 256 }, (Vector2){ 1.0f, 1.0f }, false);
                }
            }

            skipTilesRendering:

        M7_Camera_End(&camera);

        // Display mode7 rendering

        BeginDrawing();
            ClearBackground(BLACK);
            M7_Camera_Render(&camera);
            DrawRenderInfo(&camera);
        EndDrawing();
    }

    // Program closure

    UnloadTexture(textureCharacter);
    UnloadTexture(textureGround);
    UnloadTexture(textureGrid);

    M7_Camera_Unload(&camera);

    CloseWindow();

    return 0;
}

/*
    Function to generate a grid texture
    Used to generate a ground texture later in the program.
*/
Texture2D GenTextureGrid(uint16_t gridSize, uint8_t cellSize)
{
    uint8_t* data = (uint8_t*)calloc(gridSize * gridSize, 1);

    for (uint16_t y = 0; y < gridSize; y += cellSize)
    {
        for (uint16_t x = 0; x < gridSize; x += cellSize)
        {
            data[y * gridSize + x] = 0xFF;
            data[(y + cellSize - 1) * gridSize + x] = 0xFF;
            data[y * gridSize + x + (cellSize - 1)] = 0xFF;
            data[(y + cellSize - 1) * gridSize + (x + cellSize - 1)] = 0xFF;

            for (uint16_t i = 1; i < cellSize - 1; i++)
            {
                data[y * gridSize + (x + i)] = 0xFF;
                data[(y + i) * gridSize + x] = 0xFF;

                data[(y + cellSize - 1) * gridSize + (x + i)] = 0xFF;
                data[(y + i) * gridSize + (x + cellSize - 1)] = 0xFF;
            }
        }
    }

    Texture2D texture = LoadTextureFromImage((Image){
        .data = data, .width = gridSize, .height = gridSize,
        .mipmaps = 1, .format = PIXELFORMAT_UNCOMPRESSED_GRAYSCALE
    });

    free(data);

    return texture;
}

/*
    Display data related to the camera and rendering in general
*/
void DrawRenderInfo(M7_Camera* camera)
{
    Rectangle rec = { 8, 8, 320, 216 };

    // Info frame

    DrawRectangleRec(rec, ColorAlpha(LIGHTGRAY, 0.65f));
    DrawRectangleLinesEx(rec, 3, DARKGRAY);

    // FPS info

    DrawText(TextFormat("FPS: %i", GetFPS()), 16, 16, 20, BLACK);
    DrawText(TextFormat("MS/Frame: %.2f", 1000 * GetFrameTime()), 162, 16, 20, BLACK);

    // Camera info

    DrawText("Camera:", 16, 56, 20, BLACK);
    DrawText(TextFormat("Position: { %.2f, %.2f }", camera->position.x, camera->position.y), 32, 76, 20, BLACK);
    DrawText(TextFormat("Rotation: %f", camera->rotation), 32, 96, 20, BLACK);
    DrawText(TextFormat("Zoom: %f", camera->zoom), 32, 116, 20, BLACK);
    DrawText(TextFormat("FOV: %f", camera->fov), 32, 136, 20, BLACK);
    DrawText(TextFormat("Offset: %f", camera->offset), 32, 156, 20, BLACK);

    // Sprites info

    DrawText(TextFormat("Sprite count: %i", camera->buffer.count), 16, 196, 20, BLACK);
}