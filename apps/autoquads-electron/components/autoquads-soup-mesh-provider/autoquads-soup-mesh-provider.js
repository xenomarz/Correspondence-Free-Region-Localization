import { AutoquadsMeshProvider } from '../autoquads-mesh-provider/autoquads-mesh-provider.js';
export class AutoquadsSoupMeshProvider extends AutoquadsMeshProvider {
    constructor(engine, meshColor, objectiveFunctionsProperties) {
        super(engine, meshColor, objectiveFunctionsProperties);
    }

    getFaces() {
        return this._engine.getImageFaces();
    }

    getVertices() {
        return this._engine.getImageVertices();
    }

    getFacesCount() {
        return this._engine.getImageFacesCount();
    }

    getVerticesCount() {
        return this._engine.getImageVerticesCount();
    }    

    getBufferedVertices(primitiveType) {
        return this._engine.getImageBufferedVertices(primitiveType);
    }

    getBufferedUvs() {
        return this._engine.getImageBufferedUvs();
    }

    get debugData() {
        return this._engine.objectiveFunctionsData;
    }
}