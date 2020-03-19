import { MeshProvider } from '../mesh-provider/mesh-provider.js';
import * as THREE from '../../web_modules/three.js';
import * as EnumsExports from '../../redux/enums.js';
import * as HelpersExports from '../../redux/helpers.js';

export class AutoquadsMeshProvider extends MeshProvider {
    constructor(
        engine, 
        meshColor) {
        // Call base constructor
        super();

        // Set backend C++ engine reference
        this._engine = engine;

        // Set faces
        // this._bufferedFaces = this._engine.getDomainBufferedFaces();
        // this._bufferedFacesCount = this._bufferedFaces.length;

        this.meshColor = meshColor;
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
        
        return bufferedColors;
    }

    getBufferedVertexColors() {
        return this.getBufferedColors(this._bufferedFacesCount, 1, EnumsExports.PropertyEffectType.VERTEX_COLOR, this._associatedView, this._meshColor);
    }

    getBufferedEdgeColors() {
        return this.getBufferedColors(this._edgeCount, 2, EnumsExports.PropertyEffectType.EDGE_COLOR, this._associatedView, new THREE.Color(0.5, 0.5, 0.5));
    }

    getBufferedUvs() {
        return [];
    }

    set meshColor(value) {
        this._meshColor = new THREE.Color(value);
    }

    get meshColor() {
        return this._meshColor;
    }
}