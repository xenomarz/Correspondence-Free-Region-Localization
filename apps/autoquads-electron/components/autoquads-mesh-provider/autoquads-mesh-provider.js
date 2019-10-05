import { MeshProvider } from '../mesh-provider/mesh-provider.js';
import * as THREE from '../../web_modules/three.js';

export const BufferedPrimitiveType = {
    VERTEX: 0,
    TRIANGLE: 1
};

export class AutoquadsMeshProvider extends MeshProvider {
    constructor(engine, vertexEnergyType, energyColor, meshColor) {
        super();
        this._engine = engine;
        this._vertexEnergyType = vertexEnergyType;
        this._energyColor = energyColor;
        this._meshColor = meshColor;
    }

    set energyColor(value) {
        this._energyColor = value;
    }

    get energyColor() {
        return this._energyColor;
    }

    set meshColor(value) {
        this._meshColor = value;
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

    get bufferedMeshVertexColors() {
        let bufferedVertexColors = [];

        let vertexEnergyArray;
        let factor;
        if (this._vertexEnergyType === 'separation') {
            factor = 1;
            vertexEnergyArray = this._engine.separationVertexEnergy;
        } else if (this._vertexEnergyType === 'seamless') {
            factor = 7000;
            vertexEnergyArray = this._engine.seamlessVertexEnergy;
        }

        for (let i = 0; i < vertexEnergyArray.length; i++) {
            let energy = vertexEnergyArray[i] * factor;

            if (energy > 1) {
                energy = 1;
            }

            let red = energy * this._energyColor.r + (1 - energy) * this._meshColor.r;
            let green = energy * this._energyColor.g + (1 - energy) * this._meshColor.g;
            let blue = energy * this._energyColor.b + (1 - energy) * this._meshColor.b;

            bufferedVertexColors.push(new THREE.Color(red, green, blue));
        }

        return bufferedVertexColors;
    }
}