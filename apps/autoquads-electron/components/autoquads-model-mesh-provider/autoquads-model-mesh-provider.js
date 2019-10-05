import { AutoquadsMeshProvider, BufferedPrimitiveType } from '../autoquads-mesh-provider/autoquads-mesh-provider.js';
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

    getBufferedVertices() {
        return this._engine.getDomainBufferedVertices(BufferedPrimitiveType.TRIANGLE);
    }

    getBufferedUvs() {
        return this._engine.getDomainBufferedUvs();
    }

    getBufferedColors() {
        return new Float32Array();
    }    
}