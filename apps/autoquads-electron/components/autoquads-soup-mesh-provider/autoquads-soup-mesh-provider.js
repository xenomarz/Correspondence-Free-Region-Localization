import { AutoquadsMeshProvider, BufferedPrimitiveType } from '../autoquads-mesh-provider/autoquads-mesh-provider.js';
export class AutoquadsSoupMeshProvider extends AutoquadsMeshProvider {
    constructor(engine, vertexEnergyType, energyColor, meshColor) {
        super(engine, vertexEnergyType, energyColor, meshColor);
    }

    getFaces() {
        return this._engine.getImageFaces();
    }

    getVertices() {
        return this._engine.getImageVertices();
    }

    getBufferedVertices() {
        return this._engine.getImageBufferedVertices(BufferedPrimitiveType.TRIANGLE);
    }

    getBufferedUvs() {
        return this._engine.getImageBufferedUvs();
    }

    get debugData() {
        return this._engine.objectiveFunctionsData;
    }
}