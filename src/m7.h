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

#ifndef M7_H
#define M7_H

#include <external/glad.h>       // For using `glUniformMatrix2fv()`
#include <raylib.h>
#include <stdlib.h>
#include <math.h>

typedef struct {
    float m0, m1;  // First row of the matrix (2 components)
    float m2, m3;  // Second row of the matrix (2 components)
} Matrix2x2;

/*
    Fragment shader for rendering the plane
*/

static const char M7_PlaneFragment[] =
    "#version 330\n"

    "in vec2 fragTexCoord;"
    "out vec4 fragColor;"

    "uniform sampler2D map;"
    "uniform vec2 mapSize;"

    "uniform vec2 camPos;"
    "uniform mat2 camRot;"

    "uniform float offset;"
    "uniform float zoom;"
    "uniform float fov;"
    "uniform int wrap;"

    "void main()"
    "{"
        "vec2 uv = ((vec2(0.5, offset) - fragTexCoord) * vec2(zoom, zoom/fov)) * camRot;"
        "uv = (uv / fragTexCoord.y + camPos) / mapSize;"

        "if (wrap == 0 && (uv.x < 0.0 || uv.x > 1.0 || uv.y < 0.0 || uv.y > 1.0))"
        "{"
            "fragColor = vec4(0.0);"
        "}"
        "else"
        "{"
            "fragColor = texture(map, uv);"
        "}"
    "}";

/*
    Z-Buffer rendering system (structs)
*/

struct M7_ZBuffer_Element_SpaceData {
    Rectangle rectangle;
    Vector2 position;
    Vector2 scale;
};

enum M7_ZBuffer_Element_Type {
    M7_ZBT_TEXTURE,
    M7_ZBT_RECTANGLE,
    M7_ZBT_CIRCLE
};

typedef struct {

    // Represents world coordinates provided by the user
    // (for textures, 'onWorld.rectangle' represents the source rectangle of the texture)
    struct M7_ZBuffer_Element_SpaceData onWorld;

    // Represents screen display coordinates
    struct M7_ZBuffer_Element_SpaceData onScreen;

    Texture2D texture;  // The texture associated with the element
    float distance;     // The distance of the element with camera
    Color tint;         // The tint color of the element

    enum M7_ZBuffer_Element_Type type;  // The type of the ZBuffer element

} M7_ZBuffer_Element;

typedef M7_ZBuffer_Element M7_Element;

typedef struct {
    M7_ZBuffer_Element **ptrElems; // Array of pointers to the original elements (This array will be sorted in the order of rendering based on depth)
    M7_ZBuffer_Element *elems;     // Array of elements
    uint32_t maxCount;             // Maximum capacity of the buffer
    uint32_t count;                // Number of elements currently in the buffer
} M7_ZBuffer;

/*
    Camera struct
*/

typedef struct M7_Camera {

    struct { // An instance per camera of the plane rendering shader

        Shader shader;      // The shader used for rendering

        int locMapTex;      // Location of the map texture uniform
        int locMapSize;     // Location of the map size uniform

        int locCamPos;      // Location of the camera position uniform
        int locCamRot;      // Location of the camera rotation uniform

        int locZoom;        // Location of the zoom uniform
        int locFOV;         // Location of the field of view uniform
        int locOffset;      // Location of the offset uniform
        int locWrap;        // Location of the wrap uniform

    } planeProgram;

    RenderTexture target;   // The render target
    M7_ZBuffer buffer;      // The ZBuffer

    Matrix2x2 rotMat;       // Rotation matrix
    Vector2 position;       // Camera position

    float rotation;         // Camera rotation
    float zoom;             // Camera zoom
    float fov;              // Field of view
    float offset;           // Offset

    float aspect;           // RenderTexture aspect ratio

} M7_Camera;

/*
    Main functions of the Mode 7 rendering module
*/

// Load a Mode 7 camera with specified parameters
M7_Camera M7_Camera_Load(int screenWidth, int screenHeight, Vector2 position, float rotation, float zoom, float fov, float offset, uint32_t maxSprites);

// Unload a Mode 7 camera, freeing associated resources
void M7_Camera_Unload(M7_Camera* camera);

// Update the camera for rendering:
// - For a single plane (simple update) with the provided texture, position, scale, wrap, and background color
// - For multiple planes (advanced update) with the given background color
void M7_Camera_Update(M7_Camera* camera, Texture2D texture, Vector2 position, Vector2 scale, int wrap, Color backgroundColor);
void M7_Camera_Begin(M7_Camera* camera, Color backgroundColor);
void M7_Camera_End(M7_Camera* camera);

// Render a plane (to be used in advanced mode) with the provided texture, position, origin, scale, and wrap
void M7_Camera_DrawPlane(M7_Camera* camera, Texture2D texture, Vector2 position, Vector2 origin, Vector2 scale, int wrap);

// Display the final rendered view from the camera
void M7_Camera_Render(M7_Camera* camera);

// Set camera properties:
// - Position
// - Rotation
// - Zoom
// - Field of View (FOV)
// - Offset
void M7_Camera_SetPosition(M7_Camera* camera, Vector2 position);
void M7_Camera_SetRotation(M7_Camera* camera, float rotation);
void M7_Camera_SetZoom(M7_Camera* camera, float zoom);
void M7_Camera_SetFOV(M7_Camera* camera, float fov);
void M7_Camera_SetOffset(M7_Camera* camera, float offset);

// Perform camera transformations:
// - Translation (dx, dy)
// - Rotation (delta)
// - Movement (speed)
void M7_Camera_Translate(M7_Camera* camera, float dx, float dy);
void M7_Camera_Rotate(M7_Camera* camera, float delta);
void M7_Camera_Move(M7_Camera* camera, float speed);

// Convert coordinates between world and screen space
Vector3 M7_ToScreen(M7_Camera* camera, Vector2 point);
Vector2 M7_ToWorld(M7_Camera* camera, Vector2 point);

// Functions for adding elements to the world display:
// - Texture
// - Rectangle
// - Circle
M7_Element* M7_Texture_Add(M7_Camera* camera, Texture2D texture, Rectangle source, Vector2 position, Vector2 scale, Color tint);
M7_Element* M7_Rectangle_Add(M7_Camera* camera, Rectangle rectangle, Color tint);
M7_Element* M7_Circle_Add(M7_Camera* camera, Vector2 position, float radius, Color tint);

/*
    IMPLEMENTATION
*/

#ifdef M7_IMPL

/*
    Static function pre-declarations (private)
*/

static M7_ZBuffer M7_ZBuffer_Load(uint32_t maxElements);
static void M7_ZBuffer_Unload(M7_ZBuffer* buffer);

static M7_ZBuffer_Element* M7_ZBuffer_Element_Add(M7_ZBuffer* buffer, M7_ZBuffer_Element* elem);

static void M7_ZBuffer_Element_Update(M7_ZBuffer_Element* elem, M7_Camera* camera);
static void M7_ZBuffer_Element_Draw(M7_ZBuffer_Element* elem);

static int M7_ZBuffer_Compare(const void* a, const void* b);
static void M7_ZBuffer_Sort(M7_ZBuffer* buffer);

static void M7_ZBuffer_Update(M7_Camera* camera);
static void M7_ZBuffer_Draw(M7_ZBuffer* buffer);

/*
    Camera system functions
*/

/**
 * Load a Mode 7 camera with specified parameters.
 *
 * @param screenWidth The screen width.
 * @param screenHeight The screen height.
 * @param position The camera's initial position.
 * @param rotation The camera's initial rotation.
 * @param zoom The camera's initial zoom.
 * @param fov The camera's initial field of view.
 * @param offset The camera's initial offset.
 * @param maxSprites The maximum number of sprites in the ZBuffer.
 *
 * @return The initialized Mode 7 camera.
 */
M7_Camera M7_Camera_Load(int screenWidth, int screenHeight, Vector2 position, float rotation, float zoom, float fov, float offset, uint32_t maxSprites)
{
    M7_Camera camera = {0};

    camera.aspect = (screenWidth > screenHeight)
        ? ((float)screenWidth / (float)screenHeight)
        : ((float)screenHeight / (float)screenWidth);

    Shader shader = LoadShaderFromMemory(0, M7_PlaneFragment);
    camera.planeProgram.shader = shader;

    camera.planeProgram.locMapTex  = GetShaderLocation(shader, "map");
    camera.planeProgram.locMapSize = GetShaderLocation(shader, "mapSize");
    camera.planeProgram.locCamPos  = GetShaderLocation(shader, "camPos");
    camera.planeProgram.locCamRot  = GetShaderLocation(shader, "camRot");
    camera.planeProgram.locZoom    = GetShaderLocation(shader, "zoom");
    camera.planeProgram.locFOV     = GetShaderLocation(shader, "fov");
    camera.planeProgram.locOffset  = GetShaderLocation(shader, "offset");
    camera.planeProgram.locWrap    = GetShaderLocation(shader, "wrap");

    camera.target = LoadRenderTexture(screenWidth, screenHeight);
    camera.buffer = M7_ZBuffer_Load(maxSprites);

    M7_Camera_SetPosition(&camera, position);
    M7_Camera_SetRotation(&camera, rotation);
    M7_Camera_SetOffset(&camera, offset);
    M7_Camera_SetZoom(&camera, zoom);
    M7_Camera_SetFOV(&camera, fov);

    return camera;
}

/**
 * Unload a Mode 7 camera, freeing associated resources.
 *
 * @param camera The camera to be unloaded.
 */
void M7_Camera_Unload(M7_Camera* camera)
{
    UnloadShader(camera->planeProgram.shader);
    UnloadRenderTexture(camera->target);
    M7_ZBuffer_Unload(&camera->buffer);

    camera->target.id = camera->target.texture.id = 0;
    camera->planeProgram.shader.id = 0;
}

/**
 * Update the camera for rendering a single plane.
 *
 * @param camera The camera to update.
 * @param texture The texture of the plane.
 * @param position The position of the plane.
 * @param scale The scale of the plane.
 * @param wrap The wrap mode for the plane.
 * @param backgroundColor The background color.
 */
void M7_Camera_Update(M7_Camera* camera, Texture2D texture, Vector2 position, Vector2 scale, int wrap, Color backgroundColor)
{
    M7_Camera_Begin(camera, backgroundColor);
        M7_Camera_DrawPlane(camera, texture, position, (Vector2) {
                (texture.width * scale.x) * 0.5f,
                (texture.height * scale.y) * 0.5f
            }, scale, wrap);
    M7_Camera_End(camera);
}

/**
 * Begin rendering with the Mode 7 camera.
 *
 * @param camera The camera to begin rendering with.
 * @param backgroundColor The background color.
 */
void M7_Camera_Begin(M7_Camera* camera, Color backgroundColor)
{
    BeginTextureMode(camera->target);
    ClearBackground(backgroundColor);
}

/**
 * End rendering with the Mode 7 camera.
 *
 * @param camera The camera to end rendering with.
 */
void M7_Camera_End(M7_Camera* camera)
{
    M7_ZBuffer_Update(camera);

    M7_ZBuffer_Sort(&camera->buffer);
    M7_ZBuffer_Draw(&camera->buffer);

    EndTextureMode();
}

/**
 * Draw a plane using the Mode 7 camera.
 *
 * TODO: Add parameter 'Rectangle source'
 *       and make the shader compatible 
 *       with this new feature.
 *
 * @param camera The camera to use for drawing.
 * @param texture The texture of the plane.
 * @param position The position of the plane.
 * @param origin The origin of the plane.
 * @param scale The scale of the plane.
 * @param wrap The wrap mode for the plane.
 */
void M7_Camera_DrawPlane(M7_Camera* camera, Texture2D texture, Vector2 position, Vector2 origin, Vector2 scale, int wrap)
{
    const float mapSize[2] = {
        texture.width * scale.x,
        texture.height * scale.y
    };

    const float camPos[2] = {
        camera->position.x + position.x + origin.x,
        camera->position.y + position.y + origin.y
    };

    SetShaderValue(camera->planeProgram.shader, camera->planeProgram.locMapSize, mapSize, SHADER_UNIFORM_VEC2);
    SetShaderValue(camera->planeProgram.shader, camera->planeProgram.locCamPos, camPos, SHADER_UNIFORM_VEC2);
    SetShaderValue(camera->planeProgram.shader, camera->planeProgram.locWrap, &wrap, SHADER_UNIFORM_INT);

    BeginShaderMode(camera->planeProgram.shader);
        SetShaderValueTexture(camera->planeProgram.shader, camera->planeProgram.locMapTex, texture);
        DrawTexture(camera->target.texture, 0, 0, WHITE);
    EndShaderMode();
}

/**
 * Render the final view of the camera to the screen.
 *
 * @param camera The camera to render.
 */
void M7_Camera_Render(M7_Camera* camera)
{
    DrawTexturePro(camera->target.texture,
        (Rectangle){ 0, 0, camera->target.texture.width, -camera->target.texture.height },
        (Rectangle){ 0, 0, camera->target.texture.width, camera->target.texture.height },
        (Vector2){0}, 0, WHITE);
}

/**
 * Set the position of the Mode 7 camera.
 *
 * @param camera The camera to set the position for.
 * @param position The new position.
 */
void M7_Camera_SetPosition(M7_Camera* camera, Vector2 position)
{
    camera->position = position; // Position sent in addition to the origin of the plane in DrawPlane()
}

/**
 * Set the rotation of the Mode 7 camera.
 *
 * @param camera The camera to set the rotation for.
 * @param rotation The new rotation.
 */
void M7_Camera_SetRotation(M7_Camera* camera, float rotation)
{
    camera->rotation = rotation;

    float cosR = cosf(rotation);
    float sinR = sinf(rotation);

    camera->rotMat.m0 = cosR;
    camera->rotMat.m1 = -sinR;
    camera->rotMat.m2 = sinR;
    camera->rotMat.m3 = cosR;

    glUseProgram(camera->planeProgram.shader.id);
    glUniformMatrix2fv(camera->planeProgram.locCamRot, 1, GL_FALSE, (float*)(&camera->rotMat));
}

/**
 * Set the zoom of the Mode 7 camera.
 *
 * @param camera The camera to set the zoom for.
 * @param zoom The new zoom value.
 */
void M7_Camera_SetZoom(M7_Camera* camera, float zoom)
{
    camera->zoom = zoom;

    SetShaderValue(
        camera->planeProgram.shader,
        camera->planeProgram.locZoom,
        &zoom, SHADER_UNIFORM_FLOAT);
}

/**
 * Set the field of view (FOV) of the Mode 7 camera.
 *
 * @param camera The camera to set the FOV for.
 * @param fov The new FOV value.
 */
void M7_Camera_SetFOV(M7_Camera* camera, float fov)
{
    camera->fov = fov;

    SetShaderValue(
        camera->planeProgram.shader,
        camera->planeProgram.locFOV,
        &fov, SHADER_UNIFORM_FLOAT);
}

/**
 * Set the offset of the Mode 7 camera.
 *
 * @param camera The camera to set the offset for.
 * @param offset The new offset value.
 */
void M7_Camera_SetOffset(M7_Camera* camera, float offset)
{
    camera->offset = offset;

    SetShaderValue(
        camera->planeProgram.shader,
        camera->planeProgram.locOffset,
        &offset, SHADER_UNIFORM_FLOAT);
}

/**
 * Translate the Mode 7 camera by a given amount.
 *
 * @param camera The camera to translate.
 * @param dx The change in X position.
 * @param dy The change in Y position.
 */
void M7_Camera_Translate(M7_Camera* camera, float dx, float dy)
{
    M7_Camera_SetPosition(camera, (Vector2) {
        camera->position.x + (dx * camera->rotMat.m0 + dy * camera->rotMat.m1),
        camera->position.y + (dx * camera->rotMat.m2 + dy * camera->rotMat.m3)
    });
}

/**
 * Rotate the Mode 7 camera by a specified delta angle.
 *
 * @param camera The camera to rotate.
 * @param delta The rotation angle change.
 */
void M7_Camera_Rotate(M7_Camera* camera, float delta)
{
    M7_Camera_SetRotation(camera, camera->rotation + delta);
}

/**
 * Move the Mode 7 camera based on user input and speed.
 *
 * @param camera The camera to move.
 * @param speed The movement speed.
 */
void M7_Camera_Move(M7_Camera* camera, float speed)
{
    float dt = GetFrameTime(); speed *= dt;

    float dx = (IsKeyDown(KEY_A) - IsKeyDown(KEY_D));
    float dy = (IsKeyDown(KEY_W) - IsKeyDown(KEY_S));

    if (dx || dy)
    {
        float mag = sqrtf(dx*dx + dy*dy);
        dx /= mag, dy /= mag;
    }

    M7_Camera_Translate(camera, dx*speed, dy*speed);
    M7_Camera_Rotate(camera, (IsKeyDown(KEY_RIGHT) - IsKeyDown(KEY_LEFT)) * dt);

    M7_Camera_SetFOV(camera, camera->fov + -GetMouseWheelMove() * 0.1f);
    M7_Camera_SetZoom(camera, camera->zoom + (IsKeyDown(KEY_DOWN) - IsKeyDown(KEY_UP)) * speed);
    M7_Camera_SetOffset(camera, camera->offset + (IsKeyDown(KEY_PAGE_DOWN) - IsKeyDown(KEY_PAGE_UP)) * dt);
}

/**
 * Convert world coordinates to screen coordinates using the Mode 7 camera.
 *
 * @param camera The Mode 7 camera.
 * @param point The world coordinates to convert.
 *
 * @return The screen coordinates in Vector3 format with (x, y) and distance (z).
 */
Vector3 M7_ToScreen(M7_Camera* camera, Vector2 point)
{
    // Determine the coordinates in camera space
    float objX = -(camera->position.x - point.x) / camera->zoom;
    float objY = (camera->position.y - point.y) / camera->zoom;

    // Apply the camera rotation
    float spaceX = -objX * camera->rotMat.m0 - objY * camera->rotMat.m1;
    float spaceY = (objX * camera->rotMat.m2 + objY * camera->rotMat.m3) * camera->fov;

    // Calculate the distance relative to the camera
    float distance = 1 - spaceY;

    // Calculate the coordinates on the screen
    float screenPosX = (spaceX / distance) * camera->offset * camera->target.texture.width + camera->target.texture.width / 2;
    float screenPosY = ((spaceY + camera->offset - 1) / distance) * camera->target.texture.height + camera->target.texture.height;

    // Calculate the approximate size in pixels on the plane
    float size = (camera->offset * camera->target.texture.width) / (camera->zoom * distance);

    return (Vector3) { screenPosX, screenPosY, size };
}

/**
 * Convert screen coordinates to world coordinates using the Mode 7 camera.
 *
 * @param camera The Mode 7 camera.
 * @param point The screen coordinates to convert.
 *
 * @return The world coordinates in Vector2 format.
 */
Vector2 M7_ToWorld(M7_Camera* camera, Vector2 point)
{
    float sx = (camera->target.texture.width / 2 - point.x) * (camera->zoom / camera->aspect);
    float sy = (camera->offset * camera->target.texture.height - point.y) * (camera->zoom / camera->fov);

    float rotX = sx * camera->rotMat.m0 + sy * camera->rotMat.m1;
    float rotY = sx * camera->rotMat.m2 + sy * camera->rotMat.m3;

    return (Vector2) {
        rotX / point.y + camera->position.x,
        rotY / point.y + camera->position.y
    };
}

/*
    Element management functions rendering of world space in perspective
*/

/**
 * Add a textured element to the Mode 7 camera's world space in perspective.
 *
 * @param camera The Mode 7 camera.
 * @param texture The texture to add.
 * @param source The source rectangle of the texture.
 * @param position The position of the element.
 * @param scale The scale of the element.
 * @param tint The tint color of the element.
 *
 * @return A pointer to the added Mode 7 element.
 */
M7_Element* M7_Texture_Add(M7_Camera* camera, Texture2D texture, Rectangle source, Vector2 position, Vector2 scale, Color tint)
{
    M7_ZBuffer_Element tex = {
        
        .onWorld = (struct M7_ZBuffer_Element_SpaceData) {
            .rectangle = source,
            .position = position,
            .scale = scale
        },

        .type = M7_ZBT_TEXTURE,
        .texture = texture,
        .tint = tint
    };

    return M7_ZBuffer_Element_Add(&camera->buffer, &tex);
}

/**
 * Add a rectangle element to the Mode 7 camera's world space in perspective.
 *
 * @param camera The Mode 7 camera.
 * @param rectangle The rectangle to add.
 * @param tint The tint color of the element.
 *
 * @return A pointer to the added Mode 7 element.
 */
M7_Element* M7_Rectangle_Add(M7_Camera* camera, Rectangle rectangle, Color tint)
{
    M7_ZBuffer_Element rect = {
        
        .onWorld = (struct M7_ZBuffer_Element_SpaceData) {
            .rectangle = (Rectangle) { rectangle.x, rectangle.y, 1.0f, 1.0f },
            .position = (Vector2) { rectangle.x, rectangle.y },
            .scale = (Vector2) { rectangle.width, rectangle.height }
        },

        .type = M7_ZBT_RECTANGLE,
        .tint = tint
    };

    return M7_ZBuffer_Element_Add(&camera->buffer, &rect);
}

/**
 * Add a circle element to the Mode 7 camera's world space in perspective.
 *
 * @param camera The Mode 7 camera.
 * @param position The position of the circle.
 * @param radius The radius of the circle.
 * @param tint The tint color of the element.
 *
 * @return A pointer to the added Mode 7 element.
 */
M7_Element* M7_Circle_Add(M7_Camera* camera, Vector2 position, float radius, Color tint)
{
    M7_ZBuffer_Element circle = {
        
        .onWorld = (struct M7_ZBuffer_Element_SpaceData) {

            .rectangle = (Rectangle) {
                position.x - radius,
                position.y - radius,
                1.0f, 1.0f
            },

            .position = position,
            .scale = (Vector2) { radius, radius }

        },

        .type = M7_ZBT_CIRCLE,
        .tint = tint
    };

    return M7_ZBuffer_Element_Add(&camera->buffer, &circle);
}

/*
    Z-Buffer functions management (functions automatically called by the module)
*/

/**
 * Load a new Mode 7 Z-Buffer with a specified maximum number of elements.
 *
 * @param maxElements The maximum number of elements the Z-Buffer can hold.
 *
 * @return The initialized Mode 7 Z-Buffer.
 */
static M7_ZBuffer M7_ZBuffer_Load(uint32_t maxElements)
{
    M7_ZBuffer buffer = {0};

    if (maxElements > 0)
    {
        buffer.elems = (M7_ZBuffer_Element*)malloc(maxElements * sizeof(M7_ZBuffer_Element));
        buffer.ptrElems = (M7_ZBuffer_Element**)malloc(maxElements * sizeof(M7_ZBuffer_Element*));
    }
    else
    {
        buffer.elems = NULL;
        buffer.ptrElems = NULL;
    }

    buffer.maxCount = maxElements;
    buffer.count = 0;

    return buffer;
}

/**
 * Unload a Mode 7 Z-Buffer, freeing associated resources.
 *
 * @param buffer The Mode 7 Z-Buffer to unload.
 */
static void M7_ZBuffer_Unload(M7_ZBuffer* buffer)
{
    if (buffer->ptrElems)
    {
        free(buffer->ptrElems);
        buffer->ptrElems = NULL;
    }

    if (buffer->elems)
    {
        free(buffer->elems);
        buffer->elems = NULL;
    }

    buffer->maxCount = 0;
    buffer->count = 0;
}

/**
 * Add a Mode 7 Z-Buffer element to the buffer.
 *
 * @param buffer The Mode 7 Z-Buffer.
 * @param elem The Mode 7 Z-Buffer element to add.
 *
 * @return A pointer to the added Mode 7 Z-Buffer element.
 */
static M7_ZBuffer_Element* M7_ZBuffer_Element_Add(M7_ZBuffer* buffer, M7_ZBuffer_Element* elem)
{
    if (buffer->count == buffer->maxCount) return NULL;

    M7_ZBuffer_Element *ptr = buffer->elems + buffer->count;
    *ptr = *elem, buffer->ptrElems[buffer->count] = ptr;

    return &(buffer->elems[buffer->count++]);
}

/**
 * Update a Mode 7 Z-Buffer element based on the camera's state.
 *
 * @param elem The Mode 7 Z-Buffer element to update.
 * @param camera The Mode 7 camera.
 */
static void M7_ZBuffer_Element_Update(M7_ZBuffer_Element* elem, M7_Camera* camera)
{
    Vector3 posAndSize = M7_ToScreen(camera, elem->onWorld.position);

    elem->onScreen.scale.x = (posAndSize.z * elem->onWorld.scale.x) / elem->onWorld.rectangle.width;
    elem->onScreen.scale.y = (posAndSize.z * elem->onWorld.scale.y) / elem->onWorld.rectangle.height;

    elem->onScreen.rectangle = (Rectangle) {

        posAndSize.x - (elem->onWorld.rectangle.width * elem->onScreen.scale.x) * 0.5f,
        posAndSize.y - elem->onWorld.rectangle.height * elem->onScreen.scale.y,

        elem->onWorld.rectangle.width * elem->onScreen.scale.x,
        elem->onWorld.rectangle.width * elem->onScreen.scale.y

    };

    elem->onScreen.position = (Vector2) { posAndSize.x, posAndSize.y };
    elem->distance = posAndSize.z;
}

/**
 * Draw a Mode 7 Z-Buffer element.
 *
 * @param elem The Mode 7 Z-Buffer element to draw.
 */
static void M7_ZBuffer_Element_Draw(M7_ZBuffer_Element* elem)
{
    switch (elem->type)
    {
        case M7_ZBT_TEXTURE: {

            // Checks whether the scale is unintentionally flipped.
            // (Occurs when the object is behind the camera)

            if ( (elem->onScreen.scale.x > 0) == (elem->onWorld.scale.x > 0)
              && (elem->onScreen.scale.y > 0) == (elem->onWorld.scale.y > 0) )
            {
                DrawTexturePro(
                    elem->texture,
                    elem->onWorld.rectangle,
                    elem->onScreen.rectangle,
                    (Vector2) {0}, 0, elem->tint);
            }

        } break;

        case M7_ZBT_RECTANGLE: {
            DrawRectangleRec(elem->onScreen.rectangle, elem->tint);
        } break;

        case M7_ZBT_CIRCLE: {
            Vector2 pos = elem->onScreen.position;
            pos.y -= elem->onScreen.rectangle.width;
            DrawCircleV(pos, elem->onScreen.rectangle.width, elem->tint);
        } break;
    }
}

/**
 * Compare the distance of two Mode 7 Z-Buffer elements.
 *
 * @param a Pointer to the first Mode 7 Z-Buffer element.
 * @param b Pointer to the second Mode 7 Z-Buffer element.
 *
 * @return Negative if the first element is closer, positive if the second element is closer, or zero if they are at the same distance.
 */
static int M7_ZBuffer_Compare(const void* a, const void* b)
{
    const M7_ZBuffer_Element *e1 = *(const M7_ZBuffer_Element**)a;
    const M7_ZBuffer_Element *e2 = *(const M7_ZBuffer_Element**)b;
    return (e1->distance > e2->distance) - (e1->distance < e2->distance);
}

/**
 * Sort the Mode 7 Z-Buffer elements based on their distance from the camera.
 *
 * @param buffer The Mode 7 Z-Buffer to sort.
 */
static void M7_ZBuffer_Sort(M7_ZBuffer* buffer)
{
    qsort(buffer->ptrElems, buffer->count, sizeof(M7_ZBuffer_Element*), M7_ZBuffer_Compare);
}

/**
 * Update all elements in the Mode 7 Z-Buffer based on the camera's state.
 *
 * @param camera The Mode 7 camera.
 */
static void M7_ZBuffer_Update(M7_Camera* camera)
{
    M7_ZBuffer* buffer = &camera->buffer;
    for (uint32_t i = 0; i < buffer->count; i++)
    {
        M7_ZBuffer_Element_Update(buffer->ptrElems[i], camera);
    }
}

/**
 * Render all elements in the Mode 7 Z-Buffer.
 *
 * @param buffer The Mode 7 Z-Buffer to render.
 */
static void M7_ZBuffer_Draw(M7_ZBuffer* buffer)
{
    for (uint32_t i = 0; i < buffer->count; i++)
    {
        M7_ZBuffer_Element_Draw(buffer->ptrElems[i]);
    }
}

#endif // M7_IMPL
#endif // M7_H
