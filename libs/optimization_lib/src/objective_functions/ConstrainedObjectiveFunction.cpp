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
		AddElementToHessian({ F(i, 0), F(i, 1), F(i, 2), F(i, 0) + n, F(i, 1) + n, F(i, 2) + n });

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
	lambda = X.tail(F.rows());
	/*std::cout << "X = " << X << std::endl;
	std::cout << "lambda = " << lambda << std::endl;
	std::cout << "X.head(2 * V.rows()) = " << X.head(2 * V.rows()) << std::endl;*/
	return TriangleMeshObjectiveFunction::update_variables(X.head(2 * V.rows()));
}