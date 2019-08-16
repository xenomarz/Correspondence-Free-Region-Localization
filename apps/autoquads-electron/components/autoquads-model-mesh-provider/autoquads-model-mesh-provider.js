class AutocutsModelMeshProvider extends AutoquadsMeshProvider {
  constructor(autocutsEngine, vertexEnergy, energyColor, meshColor) {
    super(autocutsEngine, vertexEnergy, energyColor, meshColor);
  }

  get vertices() {
    return this._autocutsEngine.modelVertices;
  }

  get faces() {
    return this._autocutsEngine.modelFaces;
  }

  get bufferedVertices() {
    return this._autocutsEngine.modelBufferedVertices;
  }

  get bufferedMeshVertices() {
    return this._autocutsEngine.modelBufferedMeshVertices;
  }

  get debugData() {
    return [];
  }
}