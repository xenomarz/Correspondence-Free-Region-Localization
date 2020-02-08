#include "objective_functions/ConstrainedObjectiveFunction.h"

void ConstrainedObjectiveFunction::init()
{
	lambda.resize(F.rows());
	TriangleMeshObjectiveFunction::init();
	init_aug_hessian();
}

void ConstrainedObjectiveFunction::AddElementToAugHessian(std::vector<int> ind)
{
	auto PushPair = [&](int i, int j) { if (i > j) std::swap(i, j); II_aug.push_back(i); JJ_aug.push_back(j); };
	for (int i = 0; i < ind.size(); i++)
		for (int j = 0; j <= i; j++)
			PushPair(ind[i], ind[j]);
}

double ConstrainedObjectiveFunction::lagrangianValue(const bool update)
{
	// f(x) - objective function value 
	// c(x) - constraint function vector 
	// lambda - lagrange multipliers 
	// Lagrangian = f(x) - lambda * c(x)
	double lagrangian = objectiveValue(update) - lambda.cwiseProduct(constrainedValue(update)).sum();
	if (update) {
		energy_value = lagrangian;
	}
	return lagrangian;
}

double ConstrainedObjectiveFunction::value(const bool update) {
	return lagrangianValue(update);
}

double ConstrainedObjectiveFunction::AugmentedValue(const bool update)
{
	double augmented = lagrangianValue(update) +
		(augmented_value_parameter / 2) * constrainedValue(update).cwiseAbs2().sum();

	return augmented;
}

void ConstrainedObjectiveFunction::gradient(Eigen::VectorXd& g, const bool update)
{
	lagrangianGradient(g, update);
}

void ConstrainedObjectiveFunction::init_aug_hessian() {
	II_aug.clear();
	JJ_aug.clear();
	auto PushPair = [&](int i, int j) { if (i > j) std::swap(i, j); II_aug.push_back(i); JJ_aug.push_back(j); };
	int n = V.rows();
	for (int i = 0; i < F.rows(); ++i)
		AddElementToAugHessian({ F(i, 0), F(i, 1), F(i, 2), F(i, 0) + n, F(i, 1) + n, F(i, 2) + n });

	// shift the diagonal of the hessian
	for (int i = 0; i < (2 * n); i++) {
		PushPair(i, i);
	}

	//we add the indexes of the last element in order to tell the solver the size of the matrix
	//PushPair(2 * n - 1, 2 * n - 1);

	SS_aug = std::vector<double>(II_aug.size(), 0.);
}

void ConstrainedObjectiveFunction::init_hessian()
{
	II.clear();
	JJ.clear();
	auto PushPair = [&](int i, int j) { if (i > j) std::swap(i, j); II.push_back(i); JJ.push_back(j); };
	int n = V.rows();
	for (int i = 0; i < F.rows(); ++i)
		AddElementToHessian(II, JJ, { F(i, 0), F(i, 1), F(i, 2), F(i, 0) + n, F(i, 1) + n, F(i, 2) + n });

	for (int i = 0; i < F.rows(); ++i)
	{
		PushPair(i + 2 * n, F(i, 0));
		PushPair(i + 2 * n, F(i, 1));
		PushPair(i + 2 * n, F(i, 2));
		PushPair(i + 2 * n, F(i, 0) + n);
		PushPair(i + 2 * n, F(i, 1) + n);
		PushPair(i + 2 * n, F(i, 2) + n);
	}
	//we add the indexes of the last element in order to tell the solver the size of the matrix
	PushPair(2 * n + F.rows() - 1, 2 * n + F.rows() - 1);

	SS = std::vector<double>(II.size(), 0.);
}

bool ConstrainedObjectiveFunction::update_variables(const Eigen::VectorXd& X)
{
	this->X = X;
	this->lambda = X.tail(F.rows());
	return TriangleMeshObjectiveFunction::update_variables(X.head(2 * V.rows()));
}

void ConstrainedObjectiveFunction::FDObjectiveGradient(const Eigen::VectorXd& X, Eigen::VectorXd& g)
{
	g.resize(2 * V.rows());
	Eigen::VectorXd Xd = X;
	updateX(Xd);
	double dX = 1e-9; //10e-6;

	double f_P, f_M;
	//this is a very slow method that evaluates the gradient of the objective function through FD...
	for (int i = 0; i < g.size(); i++) {
		double tmpVal = X(i);
		Xd(i) = tmpVal + dX;
		updateX(Xd);
		f_P = objectiveValue(false);

		Xd(i) = tmpVal - dX;
		updateX(Xd);
		f_M = objectiveValue(false);

		//now reset the ith param value
		Xd(i) = tmpVal;
		g(i) = (f_P - f_M) / (2 * dX);
	}
	//now restore the parameter set and make sure to tidy up a bit...
	updateX(X);
}

bool ConstrainedObjectiveFunction::checkObjectiveGradient(const Eigen::VectorXd& X)
{
	double tol = 1e-4;
	double eps = 1e-10;

	Eigen::VectorXd FD_gradient(X.size());
	Eigen::VectorXd Analytic_gradient(X.size());

	updateX(X);
	Analytic_gradient = objectiveGradient(false);
	FDObjectiveGradient(X, FD_gradient);

	std::cout << name << "(objective): g.norm() = " << Analytic_gradient.norm() << "(analytic) , " << FD_gradient.norm() << "(FD)" << std::endl;
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

bool ConstrainedObjectiveFunction::checkConstrainedJacobian(const Eigen::VectorXd& X)
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

void ConstrainedObjectiveFunction::FDConstrainedJacobian(const Eigen::VectorXd& X)
{
	Eigen::VectorXd Xd = X;
	updateX(Xd);
	Eigen::VectorXd gp(X.size()), gm(X.size()), Hi(X.size());
	double dX = 10e-9;
	II.clear(); JJ.clear(); SS.clear();
	for (int i = 0; i < X.size(); i++) {
		double tmpVal = X(i);
		Xd(i) = tmpVal + dX;
		updateX(Xd);
		gp = constrainedValue(false);

		Xd(i) = tmpVal - dX;
		updateX(Xd);
		gm = constrainedValue(false);

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