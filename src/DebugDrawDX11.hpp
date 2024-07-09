#pragma once
#define DEBUG_DRAW_EXPLICIT_CONTEXT
#include "debug_draw.hpp"

struct SimpleVec3 {
    float x,y,z;
};

int debug_draw_init(void* device, void* deviceCtx, float width, float height);
void debug_draw_update(float dt);
void debug_draw_shutdown();
void debug_draw_projected_text(const char* text, const float pos[], const float color[], float size);
SimpleVec3 debug_draw_world_to_screen(const float worldPos[], const float size);

dd::ContextHandle dd_ctx();