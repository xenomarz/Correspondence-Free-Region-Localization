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
                this._propertyModifier = 'domain';
                break;
            case EnumsExports.View.SOUP:
                this._edgeCount = this._engine.getImageEdgesCount();
                this._propertyModifier = 'image';
                break;                
        }

        this.meshColor = meshColor;
        this.objectiveFunctionsProperties = objectiveFunctionsProperties;
    }

    getBufferedColors(elementCount, colorsPerElement, propertyEffectType, associatedView, defaultColor) {
        let componentsPerColor = 3;
        let componentsPerElement = componentsPerColor * colorsPerElement;
        let componenetsCount = componentsPerElement * elementCount;
        let bufferedColors = new Float32Array(componenetsCount);

        for (let i = 0; i < elementCount; i++) {
            let baseIndex = componentsPerElement * i;
            for(let j = 0; j < colorsPerElement; j++) {
                bufferedColors[baseIndex + componentsPerColor * j + 0] = defaultColor.r;
                bufferedColors[baseIndex + componentsPerColor * j + 1] = defaultColor.g;
                bufferedColors[baseIndex + componentsPerColor * j + 2] = defaultColor.b;
            }
        }

        for(let objectiveFunctionProperty of this._objectiveFunctionsProperties) {
            if(HelpersExports.isVisible(objectiveFunctionProperty.visibility) && 
                (objectiveFunctionProperty.propertyEffectType === propertyEffectType) &&
                ((objectiveFunctionProperty.associatedView & associatedView) !== 0)) {
                let vector = this._engine.getObjectiveFunctionProperty(objectiveFunctionProperty.objectiveFunctionId, objectiveFunctionProperty.propertyId, 'none');
                let color = new THREE.Color(objectiveFunctionProperty.color);
                let colorArray = color.toArray();
                let defaultColorArray = defaultColor.toArray();
                for (let i = 0; i < elementCount; i++) {
                    let baseIndex = componentsPerElement * i;
                    let factor = Math.min(vector[i] * objectiveFunctionProperty.weight, 1);
                    for(let j = 0; j < componentsPerElement; j++) { 
                        bufferedColors[baseIndex + j] = defaultColorArray[j % 3] * (1 - factor) + colorArray[j % 3] * (factor);
                    }
                }
            }
        }
        
        return bufferedColors;
    }

    getBufferedVertexColors() {
        return this.getBufferedColors(this._bufferedFacesCount, 1, EnumsExports.PropertyEffectType.VERTEX_COLOR, this._associatedView, new THREE.Color(1, 1, 1));
    }

    getBufferedEdgeColors() {
        return this.getBufferedColors(this._edgeCount, 2, EnumsExports.PropertyEffectType.EDGE_COLOR, this._associatedView, new THREE.Color(0.5, 0.5, 0.5));
    }

    getBufferedUvs() {
        let bufferedUvs = this._engine.getImageBufferedUvs();
        let interval = this._engine.getObjectiveFunctionProperty('Singular Points', 'interval', 'none');
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