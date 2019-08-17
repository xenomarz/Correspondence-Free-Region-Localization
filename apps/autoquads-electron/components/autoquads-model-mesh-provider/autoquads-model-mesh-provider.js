import { AutoquadsMeshProvider } from '../autoquads-mesh-provider/autoquads-mesh-provider.js';
export class AutoquadsModelMeshProvider extends AutoquadsMeshProvider {
  constructor(engine, vertexEnergyType, energyColor, meshColor) {
    super(engine, vertexEnergyType, energyColor, meshColor);
  }

  get vertices() {
    return this._engine.modelVertices;
  }

  get faces() {
    return this._engine.modelFaces;
  }

  get bufferedVertices() {
    return this._engine.modelBufferedVertices;
  }

  get bufferedMeshVertices() {
    return this._engine.modelBufferedMeshVertices;
  }

  get debugData() {
    return [];
  }
}