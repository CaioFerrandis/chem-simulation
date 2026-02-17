#pragma once

#include <stdio.h>
#include <vector>
#include "particle.h"

#define SPRING_CONST 50000

struct World {
    std::vector<Nucleus> nucleus;
    std::vector<Particle> electrons;

    inline void update(float dt){
        // =========================
        // ELECTRON <-> ELECTRON
        // =========================
        for (int i = 0; i < electrons.size(); i++){
            for (int j = i + 1; j < electrons.size(); j++){
                // only if both are free or bonded
                if (electrons[i].bonds.size() == electrons[j].bonds.size()){
                    coulomb_force(electrons[i], electrons[j]);
                    coulomb_force(electrons[j], electrons[i]);
                }
            }
        }

        // =========================
        // NUCLEUS <-> NUCLEUS
        // =========================
        for (int i = 0; i < nucleus.size(); i++){
            for (int j = i + 1; j < nucleus.size(); j++){
                coulomb_force(nucleus[i].particle, nucleus[j].particle);
                coulomb_force(nucleus[j].particle, nucleus[i].particle);
            }
        }

        // =========================
        // ELECTRON <-> NUCLEUS
        // =========================
        for (int n_id = 0; n_id < nucleus.size(); n_id++){
            Nucleus& n = nucleus[n_id];

            for (int e_id = 0; e_id < electrons.size(); e_id++){
                Particle& e = electrons[e_id];

                float bond_dist = LAYER_DIST * n.get_empty_layer();

                bool near =
                    Vector2DistanceSqr(e.position, n.particle.position)
                    < bond_dist * bond_dist;

                bool nucleus_has_charge =
                    n.particle.charge - n.particle.bonds.size() > 0;

                bool already_bonded_here = e.has_bond(n_id);

                bool can_bond =
                    near &&
                    nucleus_has_charge &&
                    !already_bonded_here &&
                    e.bonds.size() < 2; // simple covalent bond

                if (can_bond){
                    e.bonds.push_back(Bond{ e_id, n_id, bond_dist });
                    n.particle.bonds.push_back(Bond{ e_id, n_id, bond_dist });

                    // snap verlet
                    e.old_position = e.position;
                    n.particle.old_position = n.particle.position;
                }

                if (!already_bonded_here){
                    coulomb_force_electron_nucleus(e, n);
                    coulomb_force(n.particle, e);
                }
            }
        }

        // =========================
        // CONSTRAINTS (BONDS)
        // =========================
        for (int iter = 0; iter < 5; iter++){
            for (auto& e : electrons){
                for (auto& b : e.bonds){
                    Particle& ep = electrons[b.electron_id];
                    Particle& np = nucleus[b.nucleus_id].particle;

                    solve_distance_constraint(
                        ep,
                        np,
                        b.distance,
                        0.3f
                    );
                }
            }
        }

        // =========================
        // INTEGRA
        // =========================
        for (auto& e : electrons){
            integrate(e, dt);
            keep_inside_screen(e, WIDTH, HEIGHT);
        }

        for (auto& n : nucleus){
            integrate(n.particle, dt);
            keep_inside_screen(n.particle, WIDTH, HEIGHT);
        }
    }

    inline void draw(){
        for (auto& n : nucleus){
            n.draw();
        }

        for (auto& e : electrons){
            e.draw();
        }
    }
};
