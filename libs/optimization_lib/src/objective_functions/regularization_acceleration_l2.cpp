#include <objective_functions/regularization_acceleration_l2.h>
#include <limits>


#include <igl/doublearea.h>

//#include <chrono>

using namespace std;
using namespace Eigen;

RegularizationAccelerationL2::RegularizationAccelerationL2()
{
    name = "Regularization: Acceleration L2";
}
void RegularizationAccelerationL2::init()
{
    if (numV == 0 || timeSteps == 0)
        throw "AccelerationL2 must define members numV and timeSteps before init()!";
    AX.resize(numV, timeSteps - 2);
    AY.resize(numV, timeSteps - 2);
    prepare_hessian();
    w = 0.0001;
}
void RegularizationAccelerationL2::updateX(const VectorXd& X)
{
    auto XX = Map<const MatrixXd>(X.data(), numV, timeSteps * 2); //XX = (x1,x2,x3,...,y1,y2,y3)

    AX = XX.middleCols(0, timeSteps - 2) - 2 * XX.middleCols(1, timeSteps - 2) + XX.middleCols(2, timeSteps - 2);
    AY = XX.middleCols(0 + timeSteps, timeSteps - 2) - 2 * XX.middleCols(1 + timeSteps, timeSteps - 2) + XX.middleCols(2 + timeSteps, timeSteps - 2);
}

double RegularizationAccelerationL2::value()
{
	return 0.5*(AX.cwiseAbs2().sum() + AY.cwiseAbs2().sum());
}
void RegularizationAccelerationL2::gradient(VectorXd& g)
{
    auto gx = Map<MatrixXd>(g.data(), numV, timeSteps);
    auto gy = Map<MatrixXd>(g.data() + numV * timeSteps, numV, timeSteps);
    gx.col(0) = AX.col(0);
    gy.col(0) = AY.col(0);  
    
    gx.col(1) = -2 * AX.col(0) + AX.col(1);
    gy.col(1) = -2 * AY.col(0) + AY.col(1);

    gx.middleCols(2, timeSteps - 4) = AX.middleCols(0, timeSteps - 4) - 2 * AX.middleCols(1, timeSteps - 4) + AX.middleCols(2, timeSteps - 4);
    gy.middleCols(2, timeSteps - 4) = AY.middleCols(0, timeSteps - 4) - 2 * AY.middleCols(1, timeSteps - 4) + AY.middleCols(2, timeSteps - 4);

    gx.col(timeSteps - 2) = AX.col(timeSteps - 4) -2 * AX.col(timeSteps - 3);
    gy.col(timeSteps - 2) = AY.col(timeSteps - 4) -2 * AY.col(timeSteps - 3);
    
    gx.col(timeSteps - 1) = AX.col(timeSteps - 3);
    gy.col(timeSteps - 1) = AY.col(timeSteps - 3);
}

void RegularizationAccelerationL2::hessian()
{
     //nothing to do here. Hessian is constant
}


void RegularizationAccelerationL2::prepare_hessian()
{
    II.clear();
    JJ.clear();
    SS.clear();
    //iterate over vertices at time steps. each energy term is (x_1-2 x_2+x_3)
    auto PushTriplet = [&](int i, int j, int s) { if (i > j) swap(i, j); II.push_back(i); JJ.push_back(j); SS.push_back(s); };
    int yStart = numV * timeSteps;
    for (int i = 0; i < numV; i++)
    {
        for (int j = 0; j < timeSteps-2; j++)
        {
            int I = i + j * numV;
            //SS=[1,-2,1;-2,4,-2;1,-2,1];
            PushTriplet(I, I, 1);               PushTriplet(I + numV, I + numV, 4);         PushTriplet(I + 2 * numV, I + 2 * numV, 1);
            PushTriplet(I, I + numV, -2);        PushTriplet(I + numV, I + 2 * numV, -2);
            PushTriplet(I, I + 2 * numV, 1);

            I += yStart;
            PushTriplet(I, I, 1);               PushTriplet(I + numV, I + numV, 4);         PushTriplet(I + 2 * numV, I + 2 * numV, 1);
            PushTriplet(I, I + numV, -2);        PushTriplet(I + numV, I + 2 * numV, -2);
            PushTriplet(I, I + 2 * numV, 1);
        }
    }
}