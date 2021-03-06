// Components Imports
import { AutoquadsMeshProvider } from '../autoquads-mesh-provider/autoquads-mesh-provider.js';
import * as EnumsExports from '../../redux/enums.js';

export class AutoquadsSoupMeshProvider extends AutoquadsMeshProvider {
    constructor(engine, meshColor, objectiveFunctionsProperties) {
        super(engine, meshColor, objectiveFunctionsProperties, EnumsExports.View.SOUP);
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

    get debugData() {
        return this._engine.objectiveFunctionsData;
    }
}