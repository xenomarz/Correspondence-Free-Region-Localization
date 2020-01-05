// Web Modules Imports
import * as THREE from '../../web_modules/three.js';
import * as OBJLoaders from '../../web_modules/three/examples/jsm/loaders/OBJLoader.js';

// Components Imports
import { AutoquadsMeshProvider } from '../autoquads-mesh-provider/autoquads-mesh-provider.js';
import * as EnumsExports from '../../redux/enums.js';
import * as HelpersExports from '../../redux/helpers.js';

export class AutoquadsModelMeshProvider extends AutoquadsMeshProvider {
    constructor(engine, meshColor, objectiveFunctionsProperties) {
        super(engine, meshColor, objectiveFunctionsProperties, EnumsExports.View.MODEL);
        var objLoader = new OBJLoaders.OBJLoader();
        objLoader.load(
            // model path
            'models/obj/iso_sphere.obj',
            
            // called when resource is loaded
            (function(object) {
                console.log("iso_sphere.obj model loaded");
                this._isoSphere = object;
            }).bind(this),

            // called when loading is in progresses
            function(xhr) {

            },

            // called when loading has errors
            function(error) {
                console.log("Couldn't load iso_sphere.obj model");
            }
        );      
    }

    getFaces() {
        return this._engine.getDomainFaces();
    }

    getVertices() {
        return this._engine.getDomainVertices();
    }

    getFacesCount() {
        return this._engine.getDomainFacesCount();
    }

    getVerticesCount() {
        return this._engine.getDomainVerticesCount();
    }       

    getBufferedVertices(primitiveType) {
        return this._engine.getDomainBufferedVertices(primitiveType);
    }

    getAdditionalSceneObjects() {
        let additionalSceneObjects = [];
        let domainVertices = this._engine.getDomainVertices();
        for(let objectiveFunctionProperty of this._objectiveFunctionsProperties) {
            if((objectiveFunctionProperty.objectiveFunctionId === 'Singular Points') && 
            HelpersExports.isVisible(objectiveFunctionProperty.visibility) &&
            ((objectiveFunctionProperty.propertyId === 'positive_angular_defect_singularities_indices') || (objectiveFunctionProperty.propertyId === 'negative_angular_defect_singularities_indices'))) {
                let vector = this._engine.getObjectiveFunctionProperty(objectiveFunctionProperty.objectiveFunctionId, objectiveFunctionProperty.propertyId);
                for(let i = 0; i < vector.length; i++) {
                    let currentDomainVertex = domainVertices[vector[i]];
                    let isoSphereClone = this._isoSphere.clone();
                    isoSphereClone.scale.set(0.0025, 0.0025, 0.0025);
                    isoSphereClone.translateX(currentDomainVertex.x);
                    isoSphereClone.translateY(currentDomainVertex.y);
                    isoSphereClone.translateZ(currentDomainVertex.z);
                    isoSphereClone.traverse( function( child ) {
                        if(child instanceof THREE.Mesh) {
                            child.material = new THREE.MeshBasicMaterial({
                                color: new THREE.Color(objectiveFunctionProperty.color)
                            });
                        }
                    });

                    additionalSceneObjects.push(isoSphereClone);
                }
            }
        }

        return additionalSceneObjects;
    }    
}