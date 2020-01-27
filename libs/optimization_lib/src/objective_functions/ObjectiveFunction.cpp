#include "objective_functions/ObjectiveFunction.h"
#include "objective_functions/LagrangianLscmStArea.h"

void ObjectiveFunction::FDGradient(const Eigen::VectorXd& X, Eigen::VectorXd& g)
{
	Eigen::VectorXd Xd = X;
    updateX(Xd);
    double dX = 1e-9; //10e-6;

    double f_P, f_M;
    //this is a very slow method that evaluates the gradient of the objective function through FD...
    for (int i = 0; i < g.size(); i++) {
        double tmpVal = X(i);
        Xd(i) = tmpVal + dX;
        updateX(Xd);
        f_P = value(false);

        Xd(i) = tmpVal - dX;
        updateX(Xd);
        f_M = value(false);

        //now reset the ith param value
        Xd(i) = tmpVal;
        g(i) = (f_P - f_M) / (2 * dX);
    }
    //now restore the parameter set and make sure to tidy up a bit...
    updateX(X);
}

void ObjectiveFunction::FDHessian(const Eigen::VectorXd& X)
{
	Eigen::VectorXd Xd = X;
    updateX(Xd);
	Eigen::VectorXd g(X.size()), gp(X.size()), gm(X.size()), Hi(X.size());
    double dX = 10e-9;
    II.clear(); JJ.clear(); SS.clear();
    for (int i = 0; i < X.size(); i++) {
        double tmpVal = X(i);
        Xd(i) = tmpVal + dX;
        updateX(Xd);
        gradient(gp,false);

        Xd(i) = tmpVal - dX;
        updateX(Xd);
        gradient(gm,false);
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
}

bool ObjectiveFunction::checkGradient(const Eigen::VectorXd& X)
{
	//////////////////////////////////////////
	checkAugGradient(X);
	//////////////////////////////////////////
    
	double tol = 1e-4;
    double eps = 1e-10;

	Eigen::VectorXd FD_gradient(X.size());
	Eigen::VectorXd Analytic_gradient(X.size());

    updateX(X);
    gradient(Analytic_gradient,false);
    FDGradient(X, FD_gradient);

	std::cout << name << ": g.norm() = " << Analytic_gradient.norm() << "(analytic) , " << FD_gradient.norm() << "(FD)" << std::endl;
	//cout << "analyt = " << endl << Analytic_gradient.transpose() << endl;
	//cout << "FD = " << endl << FD_gradient.transpose() << endl;

	for (int i = 0; i < Analytic_gradient.size(); i++) {
        double absErr = abs(FD_gradient[i] - Analytic_gradient[i]);
        double relError = 2 * absErr / (eps + Analytic_gradient[i] + FD_gradient[i]);
        if (relError > tol && absErr > 1e-6) {
            printf("Mismatch element %d: Analytic val: %lf, FD val: %lf. Error: %lf(%lf%%)\n", i, Analytic_gradient(i), FD_gradient(i), absErr, relError * 100);
            //return false;
        }
    }

    return true;
}

bool ObjectiveFunction::checkHessian(const Eigen::VectorXd& X)
{
	/////////////////////////////////////////////////////
	checkAugHessian(X);
	/////////////////////////////////////////////////////

	double tol = 1e-4;
	double eps = 1e-10;

	Eigen::SparseMatrix<double>  FDH(X.size(), X.size());
	Eigen::SparseMatrix<double> H(X.size(), X.size());
	std::vector<Eigen::Triplet<double>> Ht;

	FDHessian(X);
	for (int i = 0; i < II.size(); i++)
		Ht.push_back(Eigen::Triplet<double>(II[i], JJ[i], SS[i]));
	FDH.setFromTriplets(Ht.begin(), Ht.end());
	
	Ht.clear();
	init();
	updateX(X);
	
	hessian();
	
	for (int i = 0; i < II.size(); i++)
		Ht.push_back(Eigen::Triplet<double>(II[i], JJ[i], SS[i]));
	H.setFromTriplets(Ht.begin(), Ht.end());

	//std::cout << "FDH.selfadjointView<Upper>()" << endl;
	//std::cout << FDH.selfadjointView<Upper>() << endl;
	//std::cout << "H.selfadjointView<Upper>()" << endl;
	//std::cout << H.selfadjointView<Upper>() << endl;

	std::cout << name << ": testing hessians...\n";
	for (int i = 0; i < X.size(); i++) {
		for (int j = 0; j < X.size(); j++) {
			double absErr = std::abs(FDH.coeff(i, j) - H.coeff(i, j));
			double relError = 2 * absErr / (eps + FDH.coeff(i, j) + H.coeff(i, j));
			if (relError > tol && absErr > 1e-6) {
				printf("Mismatch element %d,%d: Analytic val: %lf, FD val: %lf. Error: %lf(%lf%%)\n", i, j, H.coeff(i, j), FDH.coeff(i, j), absErr, relError * 100);
				return false;
			}
		}
	}
	return true;
}

void ObjectiveFunction::init_mesh(const Eigen::MatrixXd& V, const Eigen::MatrixX3i& F) {
	Eigen::MatrixX3d V3d(V.rows(), 3);
	if (V.cols() == 2) {
		V3d.leftCols(2) = V;
		V3d.col(2).setZero();
	}
	else if (V.cols() == 3) {
		V3d = V;
	}
	this->V = V3d;
	this->F = F;
}

bool ObjectiveFunction::checkAugGradient(const Eigen::VectorXd& X)
{
	LagrangianLscmStArea* constrained_function = dynamic_cast<LagrangianLscmStArea*>(this);
	if (constrained_function == NULL) {
		std::cout << name << " is not a contrained function" << std::endl;
		return false;
	}

	double tol = 1e-4;
	double eps = 1e-10;

	Eigen::VectorXd FD_gradient(2 * V.rows());
	Eigen::VectorXd Analytic_gradient(2 * V.rows());

	updateX(X);
	constrained_function->AuglagrangGradWRTX(Analytic_gradient, false);
	FDAugGradient(X, FD_gradient);

	std::cout << name << "(AugmentedGradCheck): g.norm() = " << Analytic_gradient.norm() << "(analytic) , " << FD_gradient.norm() << "(FD)" << std::endl;
	//cout << "analyt = " << endl << Analytic_gradient.transpose() << endl;
	//cout << "FD = " << endl << FD_gradient.transpose() << endl;

	for (int i = 0; i < Analytic_gradient.size(); i++) {
		double absErr = abs(FD_gradient[i] - Analytic_gradient[i]);
		double relError = 2 * absErr / (eps + Analytic_gradient[i] + FD_gradient[i]);
		if (relError > tol && absErr > 1e-6) {
			printf("Mismatch element %d: Analytic val: %lf, FD val: %lf. Error: %lf(%lf%%)\n", i, Analytic_gradient(i), FD_gradient(i), absErr, relError * 100);
			//return false;
		}
	}

	return true;
}

void ObjectiveFunction::FDAugGradient(const Eigen::VectorXd& X, Eigen::VectorXd& g)
{
	LagrangianLscmStArea* constrained_function = dynamic_cast<LagrangianLscmStArea*>(this);
	if (constrained_function == NULL) {
		std::cout << name << " is not a contrained function" << std::endl;
		return;
	}

	g.resize(2 * V.rows());
	g.setZero();

	Eigen::VectorXd Xd = X;
	updateX(Xd);
	double dX = 1e-9; //10e-6;

	double f_P, f_M;
	//this is a very slow method that evaluates the gradient of the objective function through FD...
	for (int i = 0; i < (2 * V.rows()); i++) {
		double tmpVal = X(i);
		Xd(i) = tmpVal + dX;
		updateX(Xd);
		f_P = AugmentedValue(false);

		Xd(i) = tmpVal - dX;
		updateX(Xd);
		f_M = AugmentedValue(false);

		//now reset the ith param value
		Xd(i) = tmpVal;
		g(i) = (f_P - f_M) / (2 * dX);
	}
	//now restore the parameter set and make sure to tidy up a bit...
	updateX(X);
}

void ObjectiveFunction::FDAugHessian(const Eigen::VectorXd& X)
{
	LagrangianLscmStArea* c = dynamic_cast<LagrangianLscmStArea*>(this);
	if (c == NULL) {
		std::cout << name << " is not a contrained function" << std::endl;
		return;
	}

	Eigen::VectorXd Xd = X;
	updateX(Xd);
	Eigen::VectorXd g(2 * V.rows()), gp(2 * V.rows()), gm(2 * V.rows()), Hi(2 * V.rows());
	double dX = 10e-9;
	c->II_aug.clear(); c->JJ_aug.clear(); c->SS_aug.clear();
	for (int i = 0; i < (2 * V.rows()); i++) {
		double tmpVal = X(i);
		Xd(i) = tmpVal + dX;
		updateX(Xd);
		c->AuglagrangGradWRTX(gp, false);

		Xd(i) = tmpVal - dX;
		updateX(Xd);
		c->AuglagrangGradWRTX(gm, false);
		//now reset the ith param value
		Xd(i) = tmpVal;
		//and compute the row of the hessian
		Hi = (gp - gm) / (2 * dX);
		//each vector is a column vector of the hessian, so copy it in place...
		for (int j = i; j < (2 * V.rows()); j++)
		{
			if (abs(Hi[j]) > 1e-8)
			{
				c->II_aug.push_back(i);
				c->JJ_aug.push_back(j);
				c->SS_aug.push_back(Hi[j]);
			}
		}
	}
}

bool ObjectiveFunction::checkAugHessian(const Eigen::VectorXd& X)
{
	LagrangianLscmStArea* c = dynamic_cast<LagrangianLscmStArea*>(this);
	if (c == NULL) {
		std::cout << name << " is not a contrained function" << std::endl;
		return false;
	}

	double tol = 1e-4;
	double eps = 1e-10;

	Eigen::SparseMatrix<double>  FDH(2 * V.rows(), 2 * V.rows());
	Eigen::SparseMatrix<double> H(2 * V.rows(), 2 * V.rows());
	std::vector<Eigen::Triplet<double>> Ht;

	FDAugHessian(X);
	for (int i = 0; i < c->II_aug.size(); i++)
		Ht.push_back(Eigen::Triplet<double>(c->II_aug[i], c->JJ_aug[i], c->SS_aug[i]));
	FDH.setFromTriplets(Ht.begin(), Ht.end());

	Ht.clear();
	init();
	updateX(X);

	c->aughessian();

	for (int i = 0; i < c->II_aug.size(); i++)
		Ht.push_back(Eigen::Triplet<double>(c->II_aug[i], c->JJ_aug[i], c->SS_aug[i]));
	H.setFromTriplets(Ht.begin(), Ht.end());

	//std::cout << "FDH.selfadjointView<Upper>()" << endl;
	//std::cout << FDH.selfadjointView<Upper>() << endl;
	//std::cout << "H.selfadjointView<Upper>()" << endl;
	//std::cout << H.selfadjointView<Upper>() << endl;

	std::cout << name << ": testing hessians (augmented)...\n";
	for (int i = 0; i < (2 * V.rows()); i++) {
		for (int j = 0; j < (2 * V.rows()); j++) {
			double absErr = std::abs(FDH.coeff(i, j) - H.coeff(i, j));
			double relError = 2 * absErr / (eps + FDH.coeff(i, j) + H.coeff(i, j));
			if (relError > tol && absErr > 1e-6) {
				printf("Mismatch element %d,%d: Analytic val: %lf, FD val: %lf. Error: %lf(%lf%%)\n", i, j, H.coeff(i, j), FDH.coeff(i, j), absErr, relError * 100);
				return false;
			}
		}
	}
	return true;
}