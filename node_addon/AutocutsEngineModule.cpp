#include "AutocutsEngine.h"

NAN_METHOD(AutocutsEngine::New)
{
    if (!info.IsConstructCall())
    {
        return Nan::ThrowError(Nan::New("constructor called without new keyword").ToLocalChecked());
    }

    AutocutsEngine *autocuts_engine = new AutocutsEngine();
    autocuts_engine->Wrap(info.Holder());
}

NAN_METHOD(AutocutsEngine::LoadModel)
{
    // `Unwrap` refer C++ object from JS Object
    auto autocutsEngine = Nan::ObjectWrap::Unwrap<AutocutsEngine>(info.Holder());

    // Validate input arguments
    if (!info[0]->IsString())
    {
        Nan::ThrowTypeError("Argument should be a string");
        return;
    }

    // Fetch model's path parameter
    v8::String::Utf8Value modelFilePathV8(info[0]->ToString());
    auto modelFilePath = std::string(*modelFilePathV8);

    // Load model file
    autocutsEngine->LoadModel(modelFilePath);

}

NAN_METHOD(AutocutsEngine::StartSolver)
{
    // `Unwrap` refer C++ object from JS Object
    auto autocutsEngine = Nan::ObjectWrap::Unwrap<AutocutsEngine>(info.Holder());

    // Start solver
    autocutsEngine->StartSolver();
}

NAN_METHOD(AutocutsEngine::StopSolver)
{
    // `Unwrap` refer C++ object from JS Object
    auto autocutsEngine = Nan::ObjectWrap::Unwrap<AutocutsEngine>(info.Holder());

    // Stop solver
    autocutsEngine->StopSolver();
}

NAN_METHOD(AutocutsEngine::SolverProgressed)
{
    // `Unwrap` refer C++ object from JS Object
    auto autocutsEngine = Nan::ObjectWrap::Unwrap<AutocutsEngine>(info.Holder());

    // Return solver's progress state
    bool solverProgressed = autocutsEngine->SolverProgressed();
    info.GetReturnValue().Set(Nan::New<v8::Boolean>(solverProgressed));
}

NAN_GETTER(AutocutsEngine::DeltaGet)
{
    // `Unwrap` refer C++ object from JS Object
    auto autocutsEngine = Nan::ObjectWrap::Unwrap<AutocutsEngine>(info.Holder());

    // Return delta
    double delta = autocutsEngine->GetDelta();
    info.GetReturnValue().Set(Nan::New<v8::Number>(delta));
}

NAN_SETTER(AutocutsEngine::DeltaSet)
{
    // `Unwrap` refer C++ object from JS Object
    auto autocutsEngine = Nan::ObjectWrap::Unwrap<AutocutsEngine>(info.Holder());

    // Set delta
    auto delta = Nan::To<v8::Number>(value).ToLocalChecked();
    autocutsEngine->SetDelta(delta->Value());
}

NAN_GETTER(AutocutsEngine::LambdaGet)
{
    // `Unwrap` refer C++ object from JS Object
    auto autocutsEngine = Nan::ObjectWrap::Unwrap<AutocutsEngine>(info.Holder());

    // Return delta
    double lambda = autocutsEngine->GetLambda();
    info.GetReturnValue().Set(Nan::New<v8::Number>(lambda));
}

NAN_SETTER(AutocutsEngine::LambdaSet)
{
    // `Unwrap` refer C++ object from JS Object
    auto autocutsEngine = Nan::ObjectWrap::Unwrap<AutocutsEngine>(info.Holder());

    // Set delta
    auto lambda = Nan::To<v8::Number>(value).ToLocalChecked();
    autocutsEngine->SetLambda(lambda->Value());
}

NAN_GETTER(AutocutsEngine::IntegerWeightGet)
{
    // `Unwrap` refer C++ object from JS Object
    auto autocutsEngine = Nan::ObjectWrap::Unwrap<AutocutsEngine>(info.Holder());

    // Return int weight
    double intWeight = autocutsEngine->GetIntegerWeight();
    info.GetReturnValue().Set(Nan::New<v8::Number>(intWeight));
}

NAN_SETTER(AutocutsEngine::IntegerWeightSet)
{
    // `Unwrap` refer C++ object from JS Object
    auto autocutsEngine = Nan::ObjectWrap::Unwrap<AutocutsEngine>(info.Holder());

    // Set int weight
    auto intWeight = Nan::To<v8::Number>(value).ToLocalChecked();
    autocutsEngine->SetIntegerWeight(intWeight->Value());
}

NAN_GETTER(AutocutsEngine::SeamlessWeightGet)
{
	// `Unwrap` refer C++ object from JS Object
	auto autocutsEngine = Nan::ObjectWrap::Unwrap<AutocutsEngine>(info.Holder());

	// Return seamless weight
	double seamlessWeight = autocutsEngine->GetSeamlessWeight();
	info.GetReturnValue().Set(Nan::New<v8::Number>(seamlessWeight));
}

NAN_SETTER(AutocutsEngine::SeamlessWeightSet)
{
	// `Unwrap` refer C++ object from JS Object
	auto autocutsEngine = Nan::ObjectWrap::Unwrap<AutocutsEngine>(info.Holder());

	// Set seamless weight
	auto seamlessWeight = Nan::To<v8::Number>(value).ToLocalChecked();
	autocutsEngine->SetSeamlessWeight(seamlessWeight->Value());
}

NAN_GETTER(AutocutsEngine::PositionWeightGet)
{
	// `Unwrap` refer C++ object from JS Object
	auto autocutsEngine = Nan::ObjectWrap::Unwrap<AutocutsEngine>(info.Holder());

	// Return position weight
	double positionWeight = autocutsEngine->GetPositionWeight();
	info.GetReturnValue().Set(Nan::New<v8::Number>(positionWeight));
}

NAN_SETTER(AutocutsEngine::PositionWeightSet)
{
	// `Unwrap` refer C++ object from JS Object
	auto autocutsEngine = Nan::ObjectWrap::Unwrap<AutocutsEngine>(info.Holder());

	// Set position weight
	auto positionWeight = Nan::To<v8::Number>(value).ToLocalChecked();
	autocutsEngine->SetPositionWeight(positionWeight->Value());
}

NAN_GETTER(AutocutsEngine::IntegerSpacingGet)
{
    // `Unwrap` refer C++ object from JS Object
    auto autocutsEngine = Nan::ObjectWrap::Unwrap<AutocutsEngine>(info.Holder());

    // Return int spacing
    double intSpacing = autocutsEngine->GetIntegerSpacing();
    info.GetReturnValue().Set(Nan::New<v8::Number>(intSpacing));
}

NAN_SETTER(AutocutsEngine::IntegerSpacingSet)
{
    // `Unwrap` refer C++ object from JS Object
    auto autocutsEngine = Nan::ObjectWrap::Unwrap<AutocutsEngine>(info.Holder());

    // Set int spacing
    auto intSpacing = Nan::To<v8::Number>(value).ToLocalChecked();
    autocutsEngine->SetIntegerSpacing(intSpacing->Value());
}

NAN_GETTER(AutocutsEngine::ModelVerticesGet)
{
    // `Unwrap` refer C++ object from JS Object
    auto autocutsEngine = Nan::ObjectWrap::Unwrap<AutocutsEngine>(info.Holder());
    v8::Local<v8::Array> modelVerticesArray = Nan::New(autocutsEngine->GetModelVerticesArray());
    info.GetReturnValue().Set(modelVerticesArray);
}

NAN_GETTER(AutocutsEngine::ModelFacesGet)
{
    // `Unwrap` refer C++ object from JS Object
    auto autocutsEngine = Nan::ObjectWrap::Unwrap<AutocutsEngine>(info.Holder());
    v8::Local<v8::Array> modelFacesArray = Nan::New(autocutsEngine->GetModelFacesArray());
    info.GetReturnValue().Set(modelFacesArray);
}

NAN_GETTER(AutocutsEngine::SolverVerticesGet)
{
    // `Unwrap` refer C++ object from JS Object
    auto autocutsEngine = Nan::ObjectWrap::Unwrap<AutocutsEngine>(info.Holder());
    v8::Local<v8::Array> solverVerticesArray = Nan::New(autocutsEngine->GetSolverVerticesArray());
    info.GetReturnValue().Set(solverVerticesArray);
}

NAN_GETTER(AutocutsEngine::SolverFacesGet)
{
    // `Unwrap` refer C++ object from JS Object
    auto autocutsEngine = Nan::ObjectWrap::Unwrap<AutocutsEngine>(info.Holder());
    v8::Local<v8::Array> solverFacesArray = Nan::New(autocutsEngine->GetSolverFacesArray());
    info.GetReturnValue().Set(solverFacesArray);
}

NAN_GETTER(AutocutsEngine::ModelBufferedVerticesGet)
{
    // `Unwrap` refer C++ object from JS Object
    auto autocutsEngine = Nan::ObjectWrap::Unwrap<AutocutsEngine>(info.Holder());
    v8::Local<v8::Array> bufferedModelVertices = Nan::New(autocutsEngine->GetBufferedModelVertices());
    info.GetReturnValue().Set(bufferedModelVertices);
}

NAN_GETTER(AutocutsEngine::SolverBufferedVerticesGet)
{
    // `Unwrap` refer C++ object from JS Object
    auto autocutsEngine = Nan::ObjectWrap::Unwrap<AutocutsEngine>(info.Holder());
    v8::Local<v8::Array> bufferedSolverVertices = Nan::New(autocutsEngine->GetBufferedSolverVertices());
    info.GetReturnValue().Set(bufferedSolverVertices);
}

NAN_GETTER(AutocutsEngine::ModelBufferedMeshVerticesGet)
{
    // `Unwrap` refer C++ object from JS Object
    auto autocutsEngine = Nan::ObjectWrap::Unwrap<AutocutsEngine>(info.Holder());
    v8::Local<v8::Array> bufferedModelMeshVertices = Nan::New(autocutsEngine->GetBufferedModelMeshVertices());
    info.GetReturnValue().Set(bufferedModelMeshVertices);
}

NAN_GETTER(AutocutsEngine::SolverBufferedMeshVerticesGet)
{
    // `Unwrap` refer C++ object from JS Object
    auto autocutsEngine = Nan::ObjectWrap::Unwrap<AutocutsEngine>(info.Holder());
    v8::Local<v8::Array> bufferedSolverMeshVertices = Nan::New(autocutsEngine->GetBufferedSolverMeshVertices());
    info.GetReturnValue().Set(bufferedSolverMeshVertices);
}

NAN_METHOD(AutocutsEngine::AddTriangleToFixedPositionSet)
{
    auto autocutsEngine = Nan::ObjectWrap::Unwrap<AutocutsEngine>(info.Holder());

    // Validate input arguments
    if (!info[0]->IsInt32())
    {
        Nan::ThrowTypeError("Argument should be an integer");
        return;
    }

    auto faceId = Nan::To<v8::Number>(info[0]).ToLocalChecked();
    autocutsEngine->AddTriangleToFixedPositionSet(faceId->Int32Value());
}

NAN_METHOD(AutocutsEngine::RemoveTriangleFromFixedPositionSet)
{
    auto autocutsEngine = Nan::ObjectWrap::Unwrap<AutocutsEngine>(info.Holder());

    // Validate input arguments
    if (!info[0]->IsInt32())
    {
        Nan::ThrowTypeError("Argument should be an integer");
        return;
    }

    auto faceId = Nan::To<v8::Number>(info[0]).ToLocalChecked();
    autocutsEngine->RemoveTriangleFromFixedPositionSet(faceId->Int32Value());
}

NAN_METHOD(AutocutsEngine::AddVertexToIntegerSet)
{
    auto autocutsEngine = Nan::ObjectWrap::Unwrap<AutocutsEngine>(info.Holder());

    // Validate input arguments
    if (!info[0]->IsInt32())
    {
        Nan::ThrowTypeError("Argument should be an integer");
        return;
    }

    auto vertexId = Nan::To<v8::Number>(info[0]).ToLocalChecked();
    autocutsEngine->AddVertexToIntegerSet(vertexId->Int32Value());
}

NAN_METHOD(AutocutsEngine::RemoveVertexFromIntegerSet)
{
    auto autocutsEngine = Nan::ObjectWrap::Unwrap<AutocutsEngine>(info.Holder());

    // Validate input arguments
    if (!info[0]->IsInt32())
    {
        Nan::ThrowTypeError("Argument should be an integer");
        return;
    }

    auto vertexId = Nan::To<v8::Number>(info[0]).ToLocalChecked();
    autocutsEngine->RemoveVertexFromIntegerSet(vertexId->Int32Value());
}


NAN_METHOD(AutocutsEngine::SetMovingTriangleFaceId)
{
    auto autocutsEngine = Nan::ObjectWrap::Unwrap<AutocutsEngine>(info.Holder());

    // Validate input arguments
    if (!info[0]->IsInt32())
    {
        Nan::ThrowTypeError("Argument should be an integer");
        return;
    }

    auto faceId = Nan::To<v8::Number>(info[0]).ToLocalChecked();
    autocutsEngine->SetMovingTriangleFaceId(faceId->Int32Value());
}

NAN_METHOD(AutocutsEngine::UpdateMovingTrianglePosition)
{
    auto autocutsEngine = Nan::ObjectWrap::Unwrap<AutocutsEngine>(info.Holder());

    // Validate input arguments
    if (!info[0]->IsNumber())
    {
        Nan::ThrowTypeError("First argument should be a Number type");
        return;
    }

    if (!info[1]->IsNumber())
    {
        Nan::ThrowTypeError("Second argument should be a Number type");
        return;
    }

    auto offsetX = Nan::To<v8::Number>(info[0]).ToLocalChecked();
    auto offsetY = Nan::To<v8::Number>(info[1]).ToLocalChecked();
    autocutsEngine->UpdateMovingTrianglePosition(RVec2(offsetX->NumberValue(), offsetY->NumberValue()));
}

NAN_METHOD(AutocutsEngine::ResetMovingTriangleFaceId)
{
    auto autocutsEngine = Nan::ObjectWrap::Unwrap<AutocutsEngine>(info.Holder());
    autocutsEngine->ResetMovingTriangleFaceId();
}

NAN_METHOD(AutocutsEngine::GetSeamlessHistogram)
{
	auto autocutsEngine = Nan::ObjectWrap::Unwrap<AutocutsEngine>(info.Holder());

	// Validate input arguments
	if (!info[0]->IsInt32())
	{
		Nan::ThrowTypeError("Argument should be an integer");
		return;
	}

	auto buckets = Nan::To<v8::Number>(info[0]).ToLocalChecked();
	v8::Local<v8::Array> seamlessHistogramArray = Nan::New(autocutsEngine->GetSeamlessHistogram(buckets->Int32Value()));
	info.GetReturnValue().Set(seamlessHistogramArray);
}

NAN_GETTER(AutocutsEngine::GradientNormGet)
{
    // `Unwrap` refer C++ object from JS Object
    auto autocutsEngine = Nan::ObjectWrap::Unwrap<AutocutsEngine>(info.Holder());

    // Return gradient norm
    double gradientNorm = autocutsEngine->GetGradientNorm();
    info.GetReturnValue().Set(Nan::New<v8::Number>(gradientNorm));
}

NAN_GETTER(AutocutsEngine::DescentAngleOffsetGet)
{
	// `Unwrap` refer C++ object from JS Object
	auto autocutsEngine = Nan::ObjectWrap::Unwrap<AutocutsEngine>(info.Holder());

	// Return descent angle offset
	double descentAngleOffset = autocutsEngine->GetDescentAngleOffset();
	info.GetReturnValue().Set(Nan::New<v8::Number>(descentAngleOffset));
}

NAN_GETTER(AutocutsEngine::EnergyGet)
{
	// `Unwrap` refer C++ object from JS Object
	auto autocutsEngine = Nan::ObjectWrap::Unwrap<AutocutsEngine>(info.Holder());

	// Return energy
	double energy = autocutsEngine->GetEnergy();
	info.GetReturnValue().Set(Nan::New<v8::Number>(energy));
}

NAN_GETTER(AutocutsEngine::SeparationEnergyGet)
{
	auto autocutsEngine = Nan::ObjectWrap::Unwrap<AutocutsEngine>(info.Holder());
	double separationEnergy = autocutsEngine->GetSeparationEnergy();
	info.GetReturnValue().Set(Nan::New<v8::Number>(separationEnergy));
}

NAN_GETTER(AutocutsEngine::SymDirEnergyGet)
{
	auto autocutsEngine = Nan::ObjectWrap::Unwrap<AutocutsEngine>(info.Holder());
	double symDirEnergy = autocutsEngine->GetSymDirEnergy();
	info.GetReturnValue().Set(Nan::New<v8::Number>(symDirEnergy));
}

NAN_GETTER(AutocutsEngine::IntegerEnergyGet)
{
	auto autocutsEngine = Nan::ObjectWrap::Unwrap<AutocutsEngine>(info.Holder());
	double integerEnergy = autocutsEngine->GetIntegerEnergy();
	info.GetReturnValue().Set(Nan::New<v8::Number>(integerEnergy));
}

NAN_GETTER(AutocutsEngine::SeamlessEnergyGet)
{
	auto autocutsEngine = Nan::ObjectWrap::Unwrap<AutocutsEngine>(info.Holder());
	double seamlessEnergy = autocutsEngine->GetSeamlessEnergy();
	info.GetReturnValue().Set(Nan::New<v8::Number>(seamlessEnergy));
}

NAN_GETTER(AutocutsEngine::PositionEnergyGet)
{
	auto autocutsEngine = Nan::ObjectWrap::Unwrap<AutocutsEngine>(info.Holder());
	double positionEnergy = autocutsEngine->GetPositionEnergy();
	info.GetReturnValue().Set(Nan::New<v8::Number>(positionEnergy));
}

NAN_GETTER(AutocutsEngine::SeparationVertexEnergyGet)
{
	auto autocutsEngine = Nan::ObjectWrap::Unwrap<AutocutsEngine>(info.Holder());
	v8::Local<v8::Array> separationVertexEnergy = Nan::New(autocutsEngine->GetSeparationVertexEnergy());
	info.GetReturnValue().Set(separationVertexEnergy);
}

NAN_GETTER(AutocutsEngine::SeamlessVertexEnergyGet)
{
	auto autocutsEngine = Nan::ObjectWrap::Unwrap<AutocutsEngine>(info.Holder());
	v8::Local<v8::Array> seamlessVertexEnergy = Nan::New(autocutsEngine->GetSeamlessVertexEnergy());
	info.GetReturnValue().Set(seamlessVertexEnergy);
}

NAN_MODULE_INIT(AutocutsEngine::Init)
{
    auto cname = Nan::New("AutocutsEngine").ToLocalChecked();
    auto ctor = Nan::New<v8::FunctionTemplate>(AutocutsEngine::New);

    // Target for member functions
    auto ctorInst = ctor->InstanceTemplate();

    // As `ctor.name` in JS
    ctor->SetClassName(cname);

    // For ObjectWrap, it should set 1
    ctorInst->SetInternalFieldCount(1);

    // Add member functions
    Nan::SetPrototypeMethod(ctor, "loadModel", LoadModel);
    Nan::SetPrototypeMethod(ctor, "startSolver", StartSolver);
    Nan::SetPrototypeMethod(ctor, "stopSolver", StopSolver);
    Nan::SetPrototypeMethod(ctor, "solverProgressed", SolverProgressed);
    Nan::SetPrototypeMethod(ctor, "setMovingTriangleFaceId", SetMovingTriangleFaceId);
    Nan::SetPrototypeMethod(ctor, "updateMovingTrianglePosition", UpdateMovingTrianglePosition);
    Nan::SetPrototypeMethod(ctor, "resetMovingTriangleFaceId", ResetMovingTriangleFaceId);
    Nan::SetPrototypeMethod(ctor, "addTriangleToFixedPositionSet", AddTriangleToFixedPositionSet);
    Nan::SetPrototypeMethod(ctor, "removeTriangleFromFixedPositionSet", RemoveTriangleFromFixedPositionSet);
    Nan::SetPrototypeMethod(ctor, "addVertexToIntegerSet", AddVertexToIntegerSet);
    Nan::SetPrototypeMethod(ctor, "removeVertexFromIntegerSet", RemoveVertexFromIntegerSet);
	Nan::SetPrototypeMethod(ctor, "getSeamlessHistogram", GetSeamlessHistogram);

    // Add member accessors
    auto deltaProperty = Nan::New("delta").ToLocalChecked();
    Nan::SetAccessor(ctorInst, deltaProperty, DeltaGet, DeltaSet);

    auto lambdaProperty = Nan::New("lambda").ToLocalChecked();
    Nan::SetAccessor(ctorInst, lambdaProperty, LambdaGet, LambdaSet);

    auto integerWeightProperty = Nan::New("integerWeight").ToLocalChecked();
    Nan::SetAccessor(ctorInst, integerWeightProperty, IntegerWeightGet, IntegerWeightSet);

    auto integerSpacingProperty = Nan::New("integerSpacing").ToLocalChecked();
    Nan::SetAccessor(ctorInst, integerSpacingProperty, IntegerSpacingGet, IntegerSpacingSet);

	auto seamlessWeightProperty = Nan::New("seamlessWeight").ToLocalChecked();
	Nan::SetAccessor(ctorInst, seamlessWeightProperty, SeamlessWeightGet, SeamlessWeightSet);

	auto positionWeightProperty = Nan::New("positionWeight").ToLocalChecked();
	Nan::SetAccessor(ctorInst, positionWeightProperty, PositionWeightGet, PositionWeightSet);

    auto modelVerticesProperty = Nan::New("modelVertices").ToLocalChecked();
    Nan::SetAccessor(ctorInst, modelVerticesProperty, ModelVerticesGet);

    auto modelFacesProperty = Nan::New("modelFaces").ToLocalChecked();
    Nan::SetAccessor(ctorInst, modelFacesProperty, ModelFacesGet);

    auto solverVerticesProperty = Nan::New("solverVertices").ToLocalChecked();
    Nan::SetAccessor(ctorInst, solverVerticesProperty, SolverVerticesGet);

    auto solverFacesProperty = Nan::New("solverFaces").ToLocalChecked();
    Nan::SetAccessor(ctorInst, solverFacesProperty, SolverFacesGet);

    auto modelBufferedVerticesProperty = Nan::New("modelBufferedVertices").ToLocalChecked();
    Nan::SetAccessor(ctorInst, modelBufferedVerticesProperty, ModelBufferedVerticesGet);

    auto solverBufferedVerticesProperty = Nan::New("solverBufferedVertices").ToLocalChecked();
    Nan::SetAccessor(ctorInst, solverBufferedVerticesProperty, SolverBufferedVerticesGet);

    auto modelBufferedMeshVerticesProperty = Nan::New("modelBufferedMeshVertices").ToLocalChecked();
    Nan::SetAccessor(ctorInst, modelBufferedMeshVerticesProperty, ModelBufferedMeshVerticesGet);

    auto solverBufferedMeshVerticesProperty = Nan::New("solverBufferedMeshVertices").ToLocalChecked();
    Nan::SetAccessor(ctorInst, solverBufferedMeshVerticesProperty, SolverBufferedMeshVerticesGet);

	auto gradientNormProperty = Nan::New("gradientNorm").ToLocalChecked();
	Nan::SetAccessor(ctorInst, gradientNormProperty, GradientNormGet);

	auto descentAngleOffsetProperty = Nan::New("descentAngleOffset").ToLocalChecked();
	Nan::SetAccessor(ctorInst, descentAngleOffsetProperty, DescentAngleOffsetGet);

	auto energyProperty = Nan::New("energy").ToLocalChecked();
	Nan::SetAccessor(ctorInst, energyProperty, EnergyGet);

	auto separationEnergyProperty = Nan::New("separationEnergy").ToLocalChecked();
	Nan::SetAccessor(ctorInst, separationEnergyProperty, SeparationEnergyGet);

	auto symDirEnergyProperty = Nan::New("symDirEnergy").ToLocalChecked();
	Nan::SetAccessor(ctorInst, symDirEnergyProperty, SymDirEnergyGet);

	auto integerEnergyProperty = Nan::New("integerEnergy").ToLocalChecked();
	Nan::SetAccessor(ctorInst, integerEnergyProperty, IntegerEnergyGet);

	auto seamlessEnergyProperty = Nan::New("seamlessEnergy").ToLocalChecked();
	Nan::SetAccessor(ctorInst, seamlessEnergyProperty, SeamlessEnergyGet);

	auto positionEnergyProperty = Nan::New("positionEnergy").ToLocalChecked();
	Nan::SetAccessor(ctorInst, positionEnergyProperty, PositionEnergyGet);

	auto separationVertexEnergyProperty = Nan::New("separationVertexEnergy").ToLocalChecked();
	Nan::SetAccessor(ctorInst, separationVertexEnergyProperty, SeparationVertexEnergyGet);

	auto seamlessVertexEnergyProperty = Nan::New("seamlessVertexEnergy").ToLocalChecked();
	Nan::SetAccessor(ctorInst, seamlessVertexEnergyProperty, SeamlessVertexEnergyGet);

    Nan::Set(target, cname, Nan::GetFunction(ctor).ToLocalChecked());
}