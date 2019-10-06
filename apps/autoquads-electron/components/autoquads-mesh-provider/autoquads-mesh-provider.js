import { MeshProvider } from '../mesh-provider/mesh-provider.js';
import * as THREE from '../../web_modules/three.js';

export class AutoquadsMeshProvider extends MeshProvider {
    constructor(engine, vertexEnergyType, energyColor, meshColor) {
        super();
        this._engine = engine;
        this.vertexEnergyType = vertexEnergyType;
        this.energyColor = energyColor;
        this.meshColor = meshColor;
    }

    getBufferedColors() {
        let domainFacesCount = this._engine.getDomainFacesCount();
        let verticesCount = 3 * domainFacesCount;
        let componenetsCount = 3 * verticesCount;
        let bufferedVertexColors = new Float32Array(componenetsCount);

        for (let i = 0; i < verticesCount; i++) {
            let baseIndex = 3 * i;
            bufferedVertexColors[baseIndex] = 1;
            bufferedVertexColors[baseIndex + 1] = 1;
            bufferedVertexColors[baseIndex + 2] = 1;
        }

        return bufferedVertexColors;
    }

    set energyColor(value) {
        this._energyColor = new THREE.Color(value);
    }

    get energyColor() {
        return this._energyColor;
    }

    set meshColor(value) {
        this._meshColor = new THREE.Color(value);
    }

    get meshColor() {
        return this._meshColor;
    }

    set vertexEnergyType(value) {
        this._vertexEnergyType = value;
    }

    get vertexEnergyType() {
        return this._vertexEnergyType;
    }
}