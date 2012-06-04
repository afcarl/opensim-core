#ifndef OPENSIM_FORCE_ADAPTER_H_
#define OPENSIM_FORCE_ADAPTER_H_
// File:     ForceAdapter.h
// Author:   Peter Eastman
//-----------------------------------------------------------------------------
/*
* Copyright (c) 2008-12, Stanford University. All rights reserved. 
* Use of the OpenSim software in source form is permitted provided that the following
* conditions are met:
* 	1. The software is used only for non-commercial research and education. It may not
*     be used in relation to any commercial activity.
* 	2. The software is not distributed or redistributed.  Software distribution is allowed 
*     only through https://simtk.org/home/opensim.
* 	3. Use of the OpenSim software or derivatives must be acknowledged in all publications,
*      presentations, or documents describing work in which OpenSim or derivatives are used.
* 	4. Credits to developers may not be removed from executables
*     created from modifications of the source.
* 	5. Modifications of source code must retain the above copyright notice, this list of
*     conditions and the following disclaimer. 
* 
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
*  EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
*  OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
*  SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
*  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
*  TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; 
*  HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
*  OR BUSINESS INTERRUPTION) OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY
*  WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

// INCLUDES
#include "OpenSim/Simulation/osimSimulationDLL.h"
#include "Force.h"
#include "Actuator.h"

#include <SimTKsimbody.h>

namespace OpenSim {

//=============================================================================
//=============================================================================
/**
 * This acts as an adapter to allow a Force or Actuator to be used as a SimTK::Force.
 *
 * @authors Peter Eastman
 */
class OSIMSIMULATION_API ForceAdapter : public SimTK::Force::Custom::Implementation
{
//=============================================================================
// DATA
//=============================================================================
private:
    const Force* _force;

//=============================================================================
// METHODS
//=============================================================================
public:
    // CONSTRUCTION AND DESTRUCTION
    ForceAdapter(const Force& force);

    // CALC FORCES (Called by Simbody)
    void calcForce(const SimTK::State& state,
	    SimTK::Vector_<SimTK::SpatialVec>& bodyForces,SimTK::Vector_<SimTK::Vec3>& particleForces,
	    SimTK::Vector& mobilityForces) const;   

    // CALC POTENTIAL ENERGY (Called by Simbody)
    SimTK::Real calcPotentialEnergy(const SimTK::State& state) const;   

    // No need to override realize() methods; we don't provide that service
    // to OpenSim Force elements.
};

} // end of namespace OpenSim

#endif // OPENSIM_FORCE_ADAPTER_H_
