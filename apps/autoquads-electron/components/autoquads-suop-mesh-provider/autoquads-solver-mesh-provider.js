class AutocutsSolverMeshProvider extends AutoquadsMeshProvider {
  constructor(autocutsEngine, vertexEnergy, energyColor, meshColor) {
    super(autocutsEngine, vertexEnergy, energyColor, meshColor);
  }

  get vertices() {
    return this._autocutsEngine.solverVertices;
  }

  get faces() {
    return this._autocutsEngine.solverFaces;
  }

  get bufferedVertices() {
    return this._autocutsEngine.solverBufferedVertices;
  }

  get bufferedMeshVertices() {
    return this._autocutsEngine.solverBufferedMeshVertices;
  }

  get debugData() {
    return [{
        label: "Delta",
        value: this._autocutsEngine.delta
      },
      {
        label: "Lambda",
        value: this._autocutsEngine.lambda
      },
      {
        label: "Seamless Weight",
        value: this._autocutsEngine.seamlessWeight
      },
      {
        label: "Position Weight",
        value: this._autocutsEngine.positionWeight
      },
      {
        label: "Energy",
        value: this._autocutsEngine.energy
      },
      {
        label: "Separation Energy",
        value: this._autocutsEngine.separationEnergy
      },
      {
        label: "Sym Dir Energy",
        value: this._autocutsEngine.symDirEnergy
      },
      {
        label: "Integer Energy",
        value: this._autocutsEngine.integerEnergy
      },
      {
        label: "Seamless Energy",
        value: this._autocutsEngine.seamlessEnergy
      },
      {
        label: "Position Energy",
        value: this._autocutsEngine.positionEnergy
      },
      {
        label: "Gradient Norm",
        value: this._autocutsEngine.gradientNorm
      },
      {
        label: "Descent Angle Offset",
        value: Math.acos(this._autocutsEngine.descentAngleOffset)
      }
    ]
  }
}