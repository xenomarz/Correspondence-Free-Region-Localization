// Web Modules Imports
import * as THREE from '../../web_modules/three.js';
import * as OBJLoaders from '../../web_modules/three/examples/jsm/loaders/OBJLoader.js';

// Components Imports
import { AutoquadsMeshProvider } from '../autoquads-mesh-provider/autoquads-mesh-provider.js';
import * as EnumsExports from '../../redux/enums.js';
import * as HelpersExports from '../../redux/helpers.js';

export class AutoquadsShapeMeshProvider extends AutoquadsMeshProvider {
    constructor(engine, meshColor) {
        super(engine, meshColor);
        this._bufferedFaces = this._engine.getShapeBufferedFaces();
        this._bufferedFacesCount = this._bufferedFaces.length;
    }

    // getFaces() {
    //     return this._engine.getDomainFaces();
    // }

    // getVertices() {
    //     return this._engine.getDomainVertices();
    // }

    // getFacesCount() {
    //     return this._engine.getDomainFacesCount();
    // }

    // getVerticesCount() {
    //     return this._engine.getDomainVerticesCount();
    // }       

    getBufferedVertices() {
        return this._engine.getShapeBufferedVertices();
    }
}