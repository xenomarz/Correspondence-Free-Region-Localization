// Web Modules Imports
import * as THREE from '../../web_modules/three.js';
import * as OBJLoaders from '../../web_modules/three/examples/jsm/loaders/OBJLoader.js';

// Components Imports
import { AutoquadsMeshProvider } from '../autoquads-mesh-provider/autoquads-mesh-provider.js';
import * as EnumsExports from '../../redux/enums.js';
import * as HelpersExports from '../../redux/helpers.js';

export class AutoquadsShapeMeshProvider extends AutoquadsMeshProvider {
    constructor(engine, meshColor, sigmaThreshold) {
        super(engine, meshColor);
        this._bufferedFaces = this._engine.getShapeBufferedFaces();
        this._bufferedFacesCount = this._bufferedFaces.length;
        this._sigmaThreshold = sigmaThreshold;
    }      

    set sigmaThreshold(value) {
        this._sigmaThreshold = value;
    }

    getBufferedVertices() {
        return this._engine.getShapeBufferedVertices();
    }

    getBufferedColors(elementCount, colorsPerElement, propertyEffectType, associatedView, defaultColor) {
        let componentsPerColor = 3;
        let componentsPerElement = componentsPerColor * colorsPerElement;
        let componenetsCount = componentsPerElement * elementCount;
        let bufferedColors = new Float32Array(componenetsCount);

        let v = this._engine.getV();

        for (let i = 0; i < elementCount; i++) {
            let baseIndex = componentsPerElement * i;
            for(let j = 0; j < colorsPerElement; j++) {
                if(v[i] < this._sigmaThreshold)
                {
                    bufferedColors[baseIndex + componentsPerColor * j + 0] = 1;
                    bufferedColors[baseIndex + componentsPerColor * j + 1] = 1;
                    bufferedColors[baseIndex + componentsPerColor * j + 2] = 1;
                }
                else
                {
                    bufferedColors[baseIndex + componentsPerColor * j + 0] = defaultColor.r;
                    bufferedColors[baseIndex + componentsPerColor * j + 1] = defaultColor.g;
                    bufferedColors[baseIndex + componentsPerColor * j + 2] = defaultColor.b;
                }

            }
        }
        
        return bufferedColors;
    }

    get debugData() {
        let lambdaData = {}
        let lambda = this._engine.getLambda();
        if(lambda) {
            for(let i = 0; i < lambda.length; i++) {
                lambdaData["lambda" + i] = lambda[i];
            }
        }

        let muData = {}
        let mu = this._engine.getMu();
        if(mu) {
            for(let i = 0; i < mu.length; i++) {
                muData["mu" + i] = mu[i];
            }
        }

        return [
            {
                name: "Info",
                data: {
                    "Value": this._engine.getValue(),
                    "Iteration": this._engine.getIteration(),
                    "Line search iteration": this._engine.getLineSearchIteration(),
                    "Step size": this._engine.getStepSize(),
                    "Tau": this._engine.getTau()
                }
            },
            {
                name: "Lambda",
                data: lambdaData
            },
            {
                name: "Mu",
                data: muData
            }
        ];
    }
}