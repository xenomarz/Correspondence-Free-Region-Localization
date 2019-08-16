export class AutoquadsMeshProvider extends MeshProvider {
    constructor(autocutsEngine, vertexEnergy, energyColor, meshColor) {
        super();
        this._autocutsEngine = autocutsEngine;
        this._vertexEnergy = vertexEnergy;
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

    set vertexEnergy(value) {
        this._vertexEnergy = value;
    }

    get vertexEnergy() {
        return this._vertexEnergy;
    }

    get bufferedMeshUvs() {
        let _bufferedMeshUvs = new Array();
        let bufferedVertices = this._autocutsEngine.solverBufferedMeshVertices;
        let bufferedVerticesLength = bufferedVertices.length;
        let j = 0;
        for (let i = 0; i < bufferedVerticesLength; i++) {
            if (i % 3 !== 2) {
                _bufferedMeshUvs[j] = bufferedVertices[i];
                j++;
            }
        }

        return _bufferedMeshUvs;
    }

    get bufferedMeshVertexColors() {
        let THREE = require('three');
        let bufferedVertexColors = [];

        let vertexEnergyArray;
        let factor;
        if (this._vertexEnergy === 'separation') {
            factor = 1;
            vertexEnergyArray = this._autocutsEngine.separationVertexEnergy;
        } else if (this._vertexEnergy === 'seamless') {
            factor = 7000;
            vertexEnergyArray = this._autocutsEngine.seamlessVertexEnergy;
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