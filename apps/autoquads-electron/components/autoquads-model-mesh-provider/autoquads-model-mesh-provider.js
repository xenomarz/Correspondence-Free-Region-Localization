import { AutoquadsMeshProvider } from '../autoquads-mesh-provider/autoquads-mesh-provider.js';
export class AutoquadsModelMeshProvider extends AutoquadsMeshProvider {
    constructor(engine, vertexEnergyType, energyColor, meshColor) {
        super(engine, vertexEnergyType, energyColor, meshColor);
    }

    getFaces() {
        return this._engine.getDomainFaces();
    }

    getVertices() {
        return this._engine.getDomainVertices();
    }

    getFacesCount() {
        return this._engine.getDomainFacesCount();
    }

    getVerticesCount() {
        return this._engine.getDomainVerticesCount();
    }       

    getBufferedVertices(primitiveType) {
        return this._engine.getDomainBufferedVertices(primitiveType);
    }

    getBufferedUvs() {
        return this._engine.getDomainBufferedUvs();
    }  
}