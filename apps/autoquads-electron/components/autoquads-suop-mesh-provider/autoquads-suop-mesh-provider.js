import { AutoquadsMeshProvider } from '../autoquads-mesh-provider/autoquads-mesh-provider.js';
export class AutoquadsSuopMeshProvider extends AutoquadsMeshProvider {
  constructor(engine, vertexEnergyType, energyColor, meshColor) {
    super(engine, vertexEnergyType, energyColor, meshColor);
  }

  get vertices() {
    return this._engine.suopVertices;
  }

  get faces() {
    return this._engine.suopFaces;
  }

  get bufferedVertices() {
    return this._engine.suopBufferedVertices;
  }

  get bufferedMeshVertices() {
    return this._engine.suopBufferedMeshVertices;
  }

  get debugData() {
    return [{
        label: "Delta",
        value: this._engine.delta
      },
      {
        label: "Lambda",
        value: this._engine.lambda
      },
      {
        label: "Seamless Weight",
        value: this._engine.seamlessWeight
      },
      {
        label: "Position Weight",
        value: this._engine.positionWeight
      },
      {
        label: "Energy",
        value: this._engine.energy
      },
      {
        label: "Separation Energy",
        value: this._engine.separationEnergy
      },
      {
        label: "Sym Dir Energy",
        value: this._engine.symDirEnergy
      },
      {
        label: "Integer Energy",
        value: this._engine.integerEnergy
      },
      {
        label: "Seamless Energy",
        value: this._engine.seamlessEnergy
      },
      {
        label: "Position Energy",
        value: this._engine.positionEnergy
      },
      {
        label: "Gradient Norm",
        value: this._engine.gradientNorm
      },
      {
        label: "Descent Angle Offset",
        value: Math.acos(this._engine.descentAngleOffset)
      }
    ]
  }
}