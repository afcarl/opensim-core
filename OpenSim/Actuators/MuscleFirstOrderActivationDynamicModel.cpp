// Author: Matthew Millard
//MuscleFirstOrderActivationDynamicModel.cpp
/*
 * Permission is hereby granted, free of charge, to any person obtaining a    *
 * copy of this software and associated documentation files (the "Software"), *
 * to deal in the Software without restriction, including without limitation  *
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,   *
 * and/or sell copies of the Software, and to permit persons to whom the      *
 * Software is furnished to do so, subject to the following conditions:       *
 *                                                                            *
 * The above copyright notice and this permission notice shall be included in *
 * all copies or substantial portions of the Software.                        *
 *                                                                            *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR *
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,   *
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL    *
 * THE AUTHORS, CONTRIBUTORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,    *
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR      *
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE  *
 * USE OR OTHER DEALINGS IN THE SOFTWARE.                                     *
 * -------------------------------------------------------------------------- */
//=============================================================================
// INCLUDES
//=============================================================================

//#include <SimTKcommon\Testing.h>
#include "MuscleFirstOrderActivationDynamicModel.h"

using namespace OpenSim;
using namespace SimTK;
//=============================================================================
// Code
//=============================================================================
MuscleFirstOrderActivationDynamicModel::
    MuscleFirstOrderActivationDynamicModel():m_minAS(SimTK::NaN)
{
    setName("default_MuscleFirstOrderActivationDynamicModel");
    setNull();
    constructProperties();
}

/*
Detailed Computational Cost
        Comparison  Div.    Mult.   Add.    Assign
        3           1               1       1

*/
MuscleFirstOrderActivationDynamicModel::
        MuscleFirstOrderActivationDynamicModel(double tauActivation, 
        double tauDeactivation, double minActivation, 
        const std::string& muscleName)
{
    setNull();
    constructProperties();

    std::string name = muscleName;
    name.append("_activation");
    setName(name);

    double smallTol = sqrt(SimTK::Eps);
    SimTK_ERRCHK1_ALWAYS( tauActivation>smallTol && tauDeactivation>smallTol,
        "MuscleFirstOrderActivationDynamicModel::"
        "MuscleFirstOrderActivationDynamicModel",
        "%s: Activation/Deactivation time constants", name.c_str());

    SimTK_ERRCHK1_ALWAYS( minActivation >= 0 && minActivation < 1-smallTol,
        "MuscleFirstOrderActivationDynamicModel::"
        "MuscleFirstOrderActivationDynamicModel",
        "%s: Minimum activation must be greater than 0 and less than 1",
        name.c_str());

    set_activation_time_constant(tauActivation);
    set_deactivation_time_constant(tauDeactivation);
    set_minimum_activation(minActivation);

    //This simple quantity is precomputed to save a division, a 
    //subtraction and an assignment ...
    m_minAS = minActivation/(1-minActivation); 

}
        
void MuscleFirstOrderActivationDynamicModel::setNull()
{
    m_minAS = SimTK::NaN;
}

void MuscleFirstOrderActivationDynamicModel::constructProperties()
{
    constructProperty_activation_time_constant(0.010);
    constructProperty_deactivation_time_constant(0.040);
    constructProperty_minimum_activation(0.01);

    m_minAS = get_minimum_activation()/(1-get_minimum_activation());
}

double MuscleFirstOrderActivationDynamicModel::
    calcDerivative(double activation, double excitation) const
{
    SimTK::Array_<int> dx(1);
    dx[0] = 0;

    SimTK::Vector x(2);
    x(0) = activation;
    x(1) = excitation;

    return calcDerivative(dx,x);
}



double MuscleFirstOrderActivationDynamicModel::
    getActivationTimeConstant() const
{
    return get_activation_time_constant();
}
        

double MuscleFirstOrderActivationDynamicModel::
    getDeactivationTimeConstant() const
{
    return get_deactivation_time_constant();
}
        

double MuscleFirstOrderActivationDynamicModel::getMinActivation() const
{
    return get_minimum_activation();
}


void MuscleFirstOrderActivationDynamicModel::
    setActivationTimeConstant(double activationTimeConstant) 
{
    set_activation_time_constant(activationTimeConstant);
}
        

void MuscleFirstOrderActivationDynamicModel::
    setDeactivationTimeConstant(double deactivationTimeConstant) 
{
    set_deactivation_time_constant(deactivationTimeConstant);
}
        

void MuscleFirstOrderActivationDynamicModel::
    setMinActivation(double minimumActivation)
{
    set_minimum_activation(minimumActivation);
}

double MuscleFirstOrderActivationDynamicModel::
    calcValue(const SimTK::Vector& x) const
{
    SimTK_ERRCHK1_ALWAYS(x.size() == 2,
        "MuscleFirstOrderActivationDynamicModel::calcDerivative",
        "%s: Two arguments are required: excitation and activation", 
        getName().c_str());

    double activation = x(0);
    double excitation = x(1);

    return activation;
}

/*
Detailed Computational Cost     
        Comparison  Div.    Mult.   Add.    Assign
        9           2       2       4       5
*/
double MuscleFirstOrderActivationDynamicModel::
    calcDerivative(const SimTK::Array_<int>& derivComponents, 
                    const SimTK::Vector& x) const
{
    SimTK_ERRCHK1_ALWAYS(x.size() == 2,
        "MuscleFirstOrderActivationDynamicModel::calcDerivative",
        "%s: Two arguments are required: excitation and activation", 
        getName().c_str());

    double da = 0;

    switch (derivComponents.size()){
        case 0:
            {
                da = calcValue(x);
            }
            break;
        case 1:
            {
                if(derivComponents[0] == 0){
                    double activation = x(0);
                    double excitation = x(1);      
                   
                    
                    SimTK_ERRCHK1_ALWAYS(excitation >= 0 && excitation <= 1,
                    "MuscleFirstOrderActivationDynamicModel::calcDerivative",
                    "%s: Excitation must be bounded by 0 and 1",getName().c_str());


                    double minAct = get_minimum_activation();

                    SimTK_ERRCHK2_ALWAYS(activation >= minAct && activation<=1,
                    "MuscleFirstOrderActivationDynamicModel::calcDerivative",
                    "%s: Activation must be between minA and 1 (%f and 1)",
                        getName().c_str(), minAct);
                    

                    double aS = activation/(1-minAct);
                    double tau = 0;

                    double ta = get_activation_time_constant();
                    double td = get_deactivation_time_constant();

                    if(excitation > (aS-m_minAS)){
                        tau = ta*(0.5 + 1.5*(aS-m_minAS));
                    }else{
                        tau = td*(0.5+1.5*(aS-m_minAS));
                    }
                    da = (excitation - (aS-m_minAS))/tau;

                }else{
                   SimTK_ERRCHK1_ALWAYS(false,
                    "MuscleFirstOrderActivationDynamicModel::calcDerivative",
                    "%s: calcDerivative is only valid for the 0th partial", 
                    getName().c_str());
                }
            }
            break;
        default:
            SimTK_ERRCHK1_ALWAYS(false,
            "MuscleFirstOrderActivationDynamicModel::calcDerivative",
            "%s: calcDerivative is only valid for the 0th and 1st derivative", 
            getName().c_str());
            
    }

    return da;
}

int MuscleFirstOrderActivationDynamicModel::getArgumentSize() const
{
    return 2; // excitation and activation
}

int MuscleFirstOrderActivationDynamicModel::
    getMaxDerivativeOrder() const
{
    return 1;
}
