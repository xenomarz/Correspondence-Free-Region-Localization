#include <objective_functions/objective_function.h>

void ObjectiveFunction::FDGradient(const VectorXd& X, VectorXd& g)
{
    VectorXd Xd = X;
    updateX(Xd);
    double dX = 1e-7; //10e-6;

    double f_P, f_M;
    //this is a very slow method that evaluates the gradient of the objective function through FD...
    for (int i = 0; i < g.size(); i++) {
        double tmpVal = X(i);
        Xd(i) = tmpVal + dX;
        updateX(Xd);
        f_P = value();

        Xd(i) = tmpVal - dX;
        updateX(Xd);
        f_M = value();

        //now reset the ith param value
        Xd(i) = tmpVal;
        g(i) = (f_P - f_M) / (2 * dX);
    }
    //now restore the parameter set and make sure to tidy up a bit...
    updateX(X);
}

void ObjectiveFunction::FDHessian(const VectorXd& X)
{
    VectorXd Xd = X;
    updateX(Xd);
    VectorXd g(X.size()), gp(X.size()), gm(X.size()), Hi(X.size());
    double dX = 10e-6; //10e-6;
    II.clear(); JJ.clear(); SS.clear();
    for (int i = 0; i < X.size(); i++) {
        double tmpVal = X(i);
        Xd(i) = tmpVal + dX;
        updateX(Xd);
        gradient(gp);

        Xd(i) = tmpVal - dX;
        updateX(Xd);
        gradient(gm);
        //now reset the ith param value
        Xd(i) = tmpVal;
        //and compute the row of the hessian
        Hi = (gp - gm) / (2 * dX);
        //each vector is a column vector of the hessian, so copy it in place...
        for (int j = i; j < X.size(); j++)
        {
            if (abs(Hi[j]) > 1e-8)
            {
                II.push_back(i);
                JJ.push_back(j);
                SS.push_back(Hi[j]);
            }
        }
    }
    init();
}

bool ObjectiveFunction::checkGradient(const VectorXd& X)
{
    double tol = 1e-4;
    double eps = 1e-10;

    VectorXd FD_gradient(X.size());
    VectorXd Analytic_gradient(X.size());

    updateX(X);
    gradient(Analytic_gradient);
    FDGradient(X, FD_gradient);

    cout << name << ": g.norm() = " << Analytic_gradient.norm() << "(analytic) , " << FD_gradient.norm() << "(FD)" << endl;
    for (int i = 0; i < Analytic_gradient.size(); i++) {
        double absErr = abs(FD_gradient[i] - Analytic_gradient[i]);
        double relError = 2 * absErr / (eps + Analytic_gradient[i] + FD_gradient[i]);
        if (relError > tol && absErr > 1e-6) {
            printf("Mismatch element %d: Analytic val: %lf, FD val: %lf. Error: %lf(%lf%%)\n", i, Analytic_gradient(i), FD_gradient(i), absErr, relError * 100);
            return false;
        }
    }

    return true;
}

bool ObjectiveFunction::checkHessian(const VectorXd& X)
{
    double tol = 1e-4;
    double eps = 1e-10;


    SparseMatrix<double> FDH(X.size(), X.size());
    SparseMatrix<double> H(X.size(), X.size());
    vector<Triplet<double>> Ht;

    FDHessian(X);
    for (int i = 0; i < II.size(); i++)
        Ht.push_back(Triplet<double>(II[i], JJ[i], SS[i]));
    FDH.setFromTriplets(Ht.begin(), Ht.end());
    
    Ht.clear();
    
    updateX(X);
    prepare_hessian();
    hessian();
    for (int i = 0; i < II.size(); i++)
        Ht.push_back(Triplet<double>(II[i], JJ[i], SS[i]));
    H.setFromTriplets(Ht.begin(), Ht.end());

    cout << name << ": testing hessians...\n";
    for (int i = 0; i < X.size(); i++) {
        for (int j = 0; j < X.size(); j++) {
            double absErr = abs(FDH.coeff(i, j) - H.coeff(i, j));
            double relError = 2 * absErr / (eps + FDH.coeff(i, j) + H.coeff(i, j));
            if (relError > tol && absErr > 1e-6) {
                printf("Mismatch element %d,%d: Analytic val: %lf, FD val: %lf. Error: %lf(%lf%%)\n", i, j, H.coeff(i, j), FDH.coeff(i, j), absErr, relError * 100);
                return false;
            }
        }
    }

    return true;
}
