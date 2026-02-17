#pragma once

#include "raylib.h"
#include "raymath.h"
#include <vector>

#define LAYER_DIST 30.0
#define K 900000.0f

struct Bond {
    int electron_id;
    int nucleus_id;
    float distance;
};

enum class ParticleType {
    Nucleus,
    Electron
};

struct Particle {
    Vector2 position;
    Vector2 old_position;
    Vector2 force;

    bool dirty;
    bool bonded;

    float mass;
    float charge;
    float radius;
    ParticleType type;

    std::vector<Bond> bonds;

    bool has_bond(int nucleus_id){
        for (const auto& b : bonds)
            if (b.nucleus_id == nucleus_id)
                return true;
        return false;
    }

    inline void add_force(Vector2 external_force){
        force += external_force;
        dirty = true;
    }

    inline void draw(){
        Color color = RED;

        if (type == ParticleType::Nucleus){
            color = BLUE;
        } else{ // Electron
            color = YELLOW;
        }

        DrawCircleV(position, radius, color);
    }
};

struct Nucleus{
    Particle particle;

    inline int get_empty_layer(){ // 2 * n**2
        int electrons = particle.bonds.size();
        int layer = 1;

        while (true){
            int capacity = 2 * layer * layer;

            if (electrons < capacity){
                return layer;
            }

            electrons -= capacity;
            layer++;
        }
    }

    inline int max_layer(){
        int layer = 1;
        int remaining = particle.charge;

        while (remaining > 0){
            remaining -= 2 * layer * layer;
            layer++;
        }

        return layer - 1;
    }

    inline void draw(){
        particle.draw();

        for (int l = 1; l <= max_layer(); l ++)
            DrawCircleLinesV(particle.position, LAYER_DIST * l, GRAY);
    }
};

inline Nucleus create_nucleus(Vector2 pos, int charge){
    Nucleus nucleus;

    nucleus.particle.position = nucleus.particle.old_position = pos;
    nucleus.particle.mass = 3.0f;// 1836.0f; // TODO: fix this
    nucleus.particle.charge = charge;
    nucleus.particle.radius = 8.0f;
    nucleus.particle.type = ParticleType::Nucleus;
    nucleus.particle.dirty = false;
    nucleus.particle.bonded = false;

    return nucleus;
}

inline Particle create_electron(Vector2 pos){
    Particle electron;
    electron.position = electron.old_position = pos;
    electron.mass = 1.0f;
    electron.charge = -1.0f;
    electron.radius = 3.0f;
    electron.type = ParticleType::Electron;
    electron.dirty = false;
    electron.bonded = false;

    return electron;
}

// apply coulomb force automatically into Particle a
void coulomb_force(Particle& a, const Particle& b)
{
    Vector2 delta = Vector2Subtract(a.position, b.position);
    
    float r2 = Vector2LengthSqr(delta) + 1.0f;
    
    float invR = 1.0f / sqrtf(r2);
    
    float coulomb = K * a.charge * b.charge * invR * invR;
    
    float strength = coulomb;

    a.add_force(Vector2Scale(delta, strength * invR));
}

void coulomb_force_electron_nucleus(Particle& a, const Nucleus& b)
{
    Vector2 delta = Vector2Subtract(a.position, b.particle.position);
    
    float r2 = Vector2LengthSqr(delta) + 1.0f;
    
    float invR = 1.0f / sqrtf(r2);
    
    int charge = b.particle.charge - b.particle.bonds.size();

    float coulomb = K * a.charge * charge * invR * invR;
    
    float strength = coulomb;

    a.add_force(Vector2Scale(delta, strength * invR));
}

void solve_distance_constraint(
    Particle& a,
    Particle& b,
    float rest,
    float sitffness
){
    Vector2 delta = Vector2Subtract(b.position, a.position);
    float dist = Vector2Length(delta);
    if (dist < 0.0001f) return;

    float diff = (dist - rest) / dist;
    Vector2 correction = Vector2Scale(delta, 0.5f * diff * sitffness);

    a.position = Vector2Add(a.position, correction);
    b.position = Vector2Subtract(b.position, correction);
}

// MUST BE CALLED AFTER Nucleus::update_bonds()
inline void integrate(Particle& p, float dt) {
    if (p.dirty){ // check if particle hasnt already been updated
        Vector2 velocity = Vector2Subtract(p.position, p.old_position);
        Vector2 acc = Vector2Scale(p.force, 1.0f / p.mass);

        p.old_position = p.position;
        p.position = Vector2Add(
            Vector2Add(p.position, velocity),
            Vector2Scale(acc, dt * dt)
        );

        p.force = {0, 0};

        p.dirty = false;
    }
}

inline void keep_inside_screen(Particle& p, float w, float h){
    Vector2 vel = Vector2Subtract(p.position, p.old_position);

    // left
    if (p.position.x < p.radius){
        p.position.x = p.radius;
        p.old_position.x = p.position.x + vel.x * -0.5f;
    }

    // right
    if (p.position.x > w - p.radius){
        p.position.x = w - p.radius;
        p.old_position.x = p.position.x + vel.x * -0.5f;
    }

    // top
    if (p.position.y < p.radius){
        p.position.y = p.radius;
        p.old_position.y = p.position.y + vel.y * -0.5f;
    }

    // bottom
    if (p.position.y > h - p.radius){
        p.position.y = h - p.radius;
        p.old_position.y = p.position.y + vel.y * -0.5f;
    }
}
