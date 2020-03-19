// Web Modules Imports
import * as THREE from '../../web_modules/three.js';
import * as OBJLoaders from '../../web_modules/three/examples/jsm/loaders/OBJLoader.js';

// Components Imports
import { AutoquadsMeshProvider } from '../autoquads-mesh-provider/autoquads-mesh-provider.js';
import * as EnumsExports from '../../redux/enums.js';
import * as HelpersExports from '../../redux/helpers.js';

export class AutoquadsModelMeshProvider extends AutoquadsMeshProvider {
    constructor(engine, meshColor, type) {
        super(engine, meshColor);
        this._type = type;    
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
}