import { AutoquadsMeshProvider } from '../autoquads-mesh-provider/autoquads-mesh-provider.js';
export class AutoquadsSoupMeshProvider extends AutoquadsMeshProvider {
    constructor(engine, vertexEnergyType, energyColor, meshColor) {
        super(engine, vertexEnergyType, energyColor, meshColor);
    }

    get vertices() {
        return this._engine.imageVertices;
    }

    get faces() {
        return this._engine.imageFaces;
    }

    get bufferedVertices() {
        return this._engine.imageBufferedVertices;
    }

    get bufferedMeshVertices() {
        return this._engine.imageBufferedMeshVertices;
    }

    get debugData() {
        return this._engine.objectiveFunctionsData;
    }
}