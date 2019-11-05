import { MeshProvider } from '../mesh-provider/mesh-provider.js';
import * as THREE from '../../web_modules/three.js';
import * as EnumsExports from '../../redux/enums.js';
import * as HelpersExports from '../../redux/helpers.js';

export class AutoquadsMeshProvider extends MeshProvider {
    constructor(engine, meshColor, objectiveFunctionsProperties) {
        super();
        this._engine = engine;
        this._bufferedFaces = this._engine.getDomainBufferedFaces();
        this._bufferedFacesCount = this._bufferedFaces.length;
        this.meshColor = meshColor;
        this.objectiveFunctionsProperties = objectiveFunctionsProperties;
    }

    getBufferedColors() {
        let componenetsCount = 3 * this._bufferedFacesCount;
        let bufferedVertexColors = new Float32Array(componenetsCount);

        for (let i = 0; i < this._bufferedFacesCount; i++) {
            let baseIndex = 3 * i;
            for(let j = 0; j < 3; j++) {
                bufferedVertexColors[baseIndex + j] = 1;
            }
        }

        for(let objectiveFunctionProperty of this._objectiveFunctionsProperties) {
            if(HelpersExports.isVisible(objectiveFunctionProperty.visibility)) {
                let vector = this._engine.getObjectiveFunctionProperty(objectiveFunctionProperty.objectiveFunctionId, objectiveFunctionProperty.propertyId);
                let color = new THREE.Color(objectiveFunctionProperty.color);
                let colorArray = color.toArray();
                for (let i = 0; i < this._bufferedFacesCount; i++) {
                    let baseIndex = 3 * i;
                    let factor = Math.min(vector[this._bufferedFaces[i]] * 100, 1);
                    for(let j = 0; j < 3; j++) { 
                        bufferedVertexColors[baseIndex + j] = 1 * (1 - factor) + colorArray[j] * (factor);
                    }
                }
            }
        }
        
        return bufferedVertexColors;
    }

    getBufferedUvs() {
        let bufferedUvs = this._engine.getImageBufferedUvs();
        let interval = this._engine.getObjectiveFunctionProperty("Singularity", "interval");
        for(let i = 0; i < bufferedUvs.length; i++)
        {
            bufferedUvs[i] = bufferedUvs[i] / interval;
        }

        return bufferedUvs;
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