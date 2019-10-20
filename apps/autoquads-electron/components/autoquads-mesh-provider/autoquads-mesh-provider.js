import { MeshProvider } from '../mesh-provider/mesh-provider.js';
import * as THREE from '../../web_modules/three.js';

export class AutoquadsMeshProvider extends MeshProvider {
    constructor(engine, meshColor, objectiveFunctionsProperties) {
        super();
        this._engine = engine;
        this.meshColor = meshColor;
        this.objectiveFunctionsProperties = objectiveFunctionsProperties;
    }

    getBufferedColors() {
        let y =5;

        for(let objectiveFunctionProperty of this._objectiveFunctionsProperties) {
            let property = this._engine.getObjectiveFunctionProperty(objectiveFunctionProperty.objectiveFunctionId, objectiveFunctionProperty.propertyId);
        }
        
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

    set meshColor(value) {
        this._meshColor = new THREE.Color(value);
    }

    get meshColor() {
        return this._meshColor;
    }

    set objectiveFunctionsProperties(value) {
        this._objectiveFunctionsProperties = value;
    }

    get objectiveFunctionsProperties() {
        return this._objectiveFunctionsProperties;
    }    
}