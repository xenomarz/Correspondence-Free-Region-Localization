#include <nan.h>
#include <cmath>
#include <igl/readOFF.h>
#include <igl/readOBJ.h>
#include <algorithm>
#include <string>
#include <iostream>
#include <memory>

#include "AutocutsUtils.h"
#include "SolverWrapper.h"

enum class FileType
{
    Obj,
    Off,
    Unknown
};

class AutocutsEngine : public Nan::ObjectWrap
{
  public:
    static NAN_MODULE_INIT(Init);

    static NAN_METHOD(New);
    static NAN_METHOD(LoadModel);
    static NAN_METHOD(StartSolver);
    static NAN_METHOD(StopSolver);
    static NAN_METHOD(SolverProgressed);

    static NAN_METHOD(SetMovingTriangleFaceId);
    static NAN_METHOD(UpdateMovingTrianglePosition);
    static NAN_METHOD(ResetMovingTriangleFaceId);

    static NAN_METHOD(AddTriangleToFixedPositionSet);
    static NAN_METHOD(RemoveTriangleFromFixedPositionSet);

    static NAN_METHOD(AddVertexToIntegerSet);
    static NAN_METHOD(RemoveVertexFromIntegerSet);

	static NAN_METHOD(GetSeamlessHistogram);

    static NAN_SETTER(DeltaSet);
    static NAN_GETTER(DeltaGet);

    static NAN_SETTER(LambdaSet);
    static NAN_GETTER(LambdaGet);

    static NAN_SETTER(IntegerWeightSet);
    static NAN_GETTER(IntegerWeightGet);

    static NAN_SETTER(IntegerSpacingSet);
    static NAN_GETTER(IntegerSpacingGet);

	static NAN_SETTER(SeamlessWeightSet);
	static NAN_GETTER(SeamlessWeightGet);

	static NAN_SETTER(PositionWeightSet);
	static NAN_GETTER(PositionWeightGet);

    static NAN_GETTER(ModelVerticesGet);
    static NAN_GETTER(ModelFacesGet);
    static NAN_GETTER(SolverVerticesGet);
    static NAN_GETTER(SolverFacesGet);
    static NAN_GETTER(ModelBufferedVerticesGet);
    static NAN_GETTER(SolverBufferedVerticesGet);
    static NAN_GETTER(ModelBufferedMeshVerticesGet);
    static NAN_GETTER(SolverBufferedMeshVerticesGet);

	static NAN_GETTER(GradientNormGet);
	static NAN_GETTER(DescentAngleOffsetGet);

	static NAN_GETTER(EnergyGet);
	static NAN_GETTER(SeparationEnergyGet);
	static NAN_GETTER(SymDirEnergyGet);
	static NAN_GETTER(IntegerEnergyGet);
	static NAN_GETTER(SeamlessEnergyGet);
	static NAN_GETTER(PositionEnergyGet);

	static NAN_GETTER(SeparationVertexEnergyGet);
	static NAN_GETTER(SeamlessVertexEnergyGet);

    AutocutsEngine();
    virtual ~AutocutsEngine();
    void LoadModel(std::string modelFilename);
    void StartSolver();
    void StopSolver();
    bool SolverProgressed();

    void SetDelta(double delta);
    double GetDelta();

    void SetLambda(double lambda);
    double GetLambda();

    void SetIntegerWeight(double int_weight);
    double GetIntegerWeight();

    void SetIntegerSpacing(double int_spacing);
    double GetIntegerSpacing();

	void SetSeamlessWeight(double seamlessWeight);
	double GetSeamlessWeight();

	void SetPositionWeight(double positionWeight);
	double GetPositionWeight();

	double GetGradientNorm();
	double GetDescentAngleOffset();

	double GetEnergy();
	double GetSeparationEnergy();
	double GetSymDirEnergy();
	double GetIntegerEnergy();
	double GetSeamlessEnergy();
	double GetPositionEnergy();

    const Nan::Persistent<v8::Array>& GetModelVerticesArray();
    const Nan::Persistent<v8::Array>& GetModelFacesArray();
    const Nan::Persistent<v8::Array>& GetSolverVerticesArray();
    const Nan::Persistent<v8::Array>& GetSolverFacesArray();

    const Nan::Persistent<v8::Array>& GetBufferedModelVertices();
    const Nan::Persistent<v8::Array>& GetBufferedSolverVertices();

    const Nan::Persistent<v8::Array>& GetBufferedModelMeshVertices();
    const Nan::Persistent<v8::Array>& GetBufferedSolverMeshVertices();

	const Nan::Persistent<v8::Array>& GetSeamlessHistogram(int buckets);


	const Nan::Persistent<v8::Array>& GetSeparationVertexEnergy();
	const Nan::Persistent<v8::Array>& GetSeamlessVertexEnergy();


    void AddTriangleToFixedPositionSet(int32_t faceId);
    void RemoveTriangleFromFixedPositionSet(int32_t faceId);

    void AddVertexToIntegerSet(int32_t vertexId);
    void RemoveVertexFromIntegerSet(int32_t vertexId);

    void SetMovingTriangleFaceId(int32_t faceId);
    void ResetMovingTriangleFaceId();
    void UpdateMovingTrianglePosition(const RVec2& offset);

  private:
    FileType GetFilenameExtension(std::string fileName);
    std::string ToLower(std::string data);
    void FindEdgeLenghtsForSeparation();
    int FindCorrespondingUvEdges(int v1, int v2);
    int FindCorrespondingPairIndex(int i1, int i2);

    Eigen::MatrixXd modelVerticesMatrix;
    Eigen::MatrixXi modelFacesMatrix;
    Eigen::MatrixX2d solverVerticesMatrix;
    Eigen::MatrixXi solverFacesMatrix;

    Nan::Persistent<v8::Array> modelVerticesArray;
    Nan::Persistent<v8::Array> modelFacesArray;
    Nan::Persistent<v8::Array> solverVerticesArray;
    Nan::Persistent<v8::Array> solverFacesArray;

    Nan::Persistent<v8::Array> bufferedModelVertices;
    Nan::Persistent<v8::Array> bufferedSolverVertices;

    Nan::Persistent<v8::Array> bufferedModelMeshVertices;
    Nan::Persistent<v8::Array> bufferedSolverMeshVertices;

	Nan::Persistent<v8::Array> seamlessHistogramArray;


	Nan::Persistent<v8::Array> separationVertexEnergyArray;
	Nan::Persistent<v8::Array> seamlessVertexEnergyArray;


    std::shared_ptr<SolverWrapper> solverWrapper;
    std::thread solverThread;

    Mat32 movingTriangleInitialPosition;
    Mat32 currentTriangleInitialPosition;
    int32_t movingTriangleFaceId;
	int32_t constrainedTriangleFaceId;
    bool setEdgeLenghtsToAverage = false;

    // The UV vertex pair(s) mapped from the 3d mesh vertex pair ev1-ev2
    vector<pair<int, int>> uvEdges;
};