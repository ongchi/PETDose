//////////////////////////////////////////////////////////////////////////
// License & Copyright
// ===================
// 
// Copyright 2012 Christopher M Poole <mail@christopherpoole.net>
// 
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//////////////////////////////////////////////////////////////////////////


// USER //
#include "PrimaryGeneratorAction.hh"

// GEANT4 //
#include "globals.hh"
#include "Randomize.hh"

#include "G4Event.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"


PrimaryGeneratorAction::PrimaryGeneratorAction()
{
    particle_gun = new G4ParticleGun();
    particle_gun->SetParticleEnergy(0); // At rest
    G4ParticleTable* particle_table = G4ParticleTable::GetParticleTable();
  
    dicom_reader = new DicomDataIO();
}

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
    delete particle_gun;
}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* event)
{
    // Randomly pick a point in the activity dataset
    unsigned int voxel_x = G4UniformRand() * activity->GetShape()[0];
    unsigned int voxel_y = G4UniformRand() * activity->GetShape()[1];
    unsigned int voxel_z = G4UniformRand() * activity->GetShape()[2];
    G4ThreeVector voxel = G4ThreeVector(voxel_x, voxel_y, voxel_z); 
 
    double position_x = voxel_x * activity->GetSpacing()[0] - 
            (activity->GetShape()[0] * activity->GetSpacing()[0])/2.;
    double position_y = voxel_y * activity->GetSpacing()[1] -
            (activity->GetShape()[1] * activity->GetSpacing()[1])/2.;
    double position_z = voxel_z * activity->GetSpacing()[2] -
            (activity->GetShape()[2] * activity->GetSpacing()[2])/2.;
    G4ThreeVector position = G4ThreeVector(position_x, position_y, position_z);

    // Test if the activity is high enough to sample. Higher
    // activities will be sampled uniformly more often.
    double setpoint = activity->GetValue(voxel) / max_activity;
    double trigger = G4UniformRand();

    if (setpoint < trigger) {
        return;
    }

    // Fluorine-18
    G4int Z = 9;
    G4int A = 18;
    G4double excitation_energy = 0*keV;
       
    G4ParticleDefinition* ion = 
        G4ParticleTable::GetParticleTable()->GetIon(Z, A, excitation_energy);

    particle_gun->SetParticleDefinition(ion);
    particle_gun->SetParticleCharge(0*eplus);
    particle_gun->SetParticlePosition(position - pet_origin);
    particle_gun->GeneratePrimaryVertex(event);
}

