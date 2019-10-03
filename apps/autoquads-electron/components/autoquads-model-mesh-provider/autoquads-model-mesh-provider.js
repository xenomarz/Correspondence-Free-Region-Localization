import { AutoquadsMeshProvider } from '../autoquads-mesh-provider/autoquads-mesh-provider.js';
export class AutoquadsModelMeshProvider extends AutoquadsMeshProvider {
    constructor(engine, vertexEnergyType, energyColor, meshColor) {
        super(engine, vertexEnergyType, energyColor, meshColor);
    }

    get vertices() {
        return this._engine.domainVertices;
    }

    get faces() {
        return this._engine.domainFaces;
    }

    get bufferedVertices() {
        return this._engine.domainBufferedVertices;
    }

    get bufferedMeshVertices() {
        return this._engine.domainBufferedMeshVertices;
    }
}