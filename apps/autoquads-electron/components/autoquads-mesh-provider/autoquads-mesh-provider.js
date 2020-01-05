import { MeshProvider } from '../mesh-provider/mesh-provider.js';
import * as THREE from '../../web_modules/three.js';
import * as EnumsExports from '../../redux/enums.js';
import * as HelpersExports from '../../redux/helpers.js';

export class AutoquadsMeshProvider extends MeshProvider {
    constructor(
        engine, 
        meshColor, 
        objectiveFunctionsProperties, 
        associatedView) {
        // Call base constructor
        super();

        // Set backend C++ engine reference
        this._engine = engine;

        // Set associated view
        this._associatedView = associatedView;

        // Set faces
        this._bufferedFaces = this._engine.getDomainBufferedFaces();
        this._bufferedFacesCount = this._bufferedFaces.length;

        // Set edge count
        switch(this._associatedView) {
            case EnumsExports.View.MODEL:
                this._edgeCount = this._engine.getDomainEdgesCount();
                break;
            case EnumsExports.View.SOUP:
                this._edgeCount = this._engine.getImageEdgesCount();
                break;                
        }

        this.meshColor = meshColor;
        this.objectiveFunctionsProperties = objectiveFunctionsProperties;
    }

    getBufferedColors2(elementCount, propertyEffectType, associatedView) {
        let componenetsCount = 6 * elementCount;
        let bufferedColors = new Float32Array(componenetsCount);

        for (let i = 0; i < elementCount; i++) {
            let baseIndex = 6 * i;
            bufferedColors[baseIndex + 0] = 1;
            bufferedColors[baseIndex + 1] = 0;
            bufferedColors[baseIndex + 2] = 0;
            bufferedColors[baseIndex + 3] = 1;
            bufferedColors[baseIndex + 4] = 0;
            bufferedColors[baseIndex + 5] = 0;
        }

        // for(let objectiveFunctionProperty of this._objectiveFunctionsProperties) {
        //     if(HelpersExports.isVisible(objectiveFunctionProperty.visibility) && 
        //         (objectiveFunctionProperty.propertyEffectType === propertyEffectType) &&
        //         (objectiveFunctionProperty.associatedView & associatedView !== 0)) {
        //         let vector = this._engine.getObjectiveFunctionProperty(objectiveFunctionProperty.objectiveFunctionId, objectiveFunctionProperty.propertyId);
        //         let color = new THREE.Color(objectiveFunctionProperty.color);
        //         let colorArray = color.toArray();
        //         for (let i = 0; i < elementCount; i++) {
        //             let baseIndex = 3 * i;
        //             let factor = Math.min(vector[i] * 100, 1);
        //             for(let j = 0; j < 3; j++) { 
        //                 bufferedColors[baseIndex + j] = 1 * (1 - factor) + colorArray[j] * (factor);
        //             }
        //         }
        //     }
        // }
        
        return bufferedColors;
    }

    getBufferedColors(elementCount, colorComponenetsPerElement, propertyEffectType, associatedView) {
        let componenetsCount = colorComponenetsPerElement * elementCount;
        let bufferedColors = new Float32Array(componenetsCount);

        for (let i = 0; i < elementCount; i++) {
            let baseIndex = colorComponenetsPerElement * i;
            for(let j = 0; j < colorComponenetsPerElement; j++) {
                bufferedColors[baseIndex + j] = 1;
            }
        }

        for(let objectiveFunctionProperty of this._objectiveFunctionsProperties) {
            if(HelpersExports.isVisible(objectiveFunctionProperty.visibility) && 
                (objectiveFunctionProperty.propertyEffectType === propertyEffectType) &&
                (objectiveFunctionProperty.associatedView & associatedView !== 0)) {
                let vector = this._engine.getObjectiveFunctionProperty(objectiveFunctionProperty.objectiveFunctionId, objectiveFunctionProperty.propertyId);
                let color = new THREE.Color(objectiveFunctionProperty.color);
                let colorArray = color.toArray();
                for (let i = 0; i < elementCount; i++) {
                    let baseIndex = colorComponenetsPerElement * i;
                    let factor = Math.min(vector[i] * 100, 1);
                    for(let j = 0; j < colorComponenetsPerElement; j++) { 
                        bufferedColors[baseIndex + j] = 1 * (1 - factor) + colorArray[j % 3] * (factor);
                    }
                }
            }
        }
        
        return bufferedColors;
    }

    getBufferedVertexColors() {
        return this.getBufferedColors(this._bufferedFacesCount, 3, EnumsExports.PropertyEffectType.VERTEX_COLOR, this._associatedView);
    }

    getBufferedEdgeColors() {
        return this.getBufferedColors(this._edgeCount, 6, EnumsExports.PropertyEffectType.EDGE_COLOR, this._associatedView);
    }

    getBufferedUvs() {
        let bufferedUvs = this._engine.getImageBufferedUvs();
        let interval = this._engine.getObjectiveFunctionProperty("Singular Points", "interval");
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