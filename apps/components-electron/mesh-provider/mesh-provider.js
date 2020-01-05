export const BufferedPrimitiveType = {
    VERTEX: 0,
    EDGE: 1,
    TRIANGLE: 2
};

export class MeshProvider {
    constructor() {

    }

    getFaces() {
        return [];
    }

    getVertices() {
        return [];
    }

    getFacesCount() {
        return 0;
    }

    getVerticesCount() {
        return 0;
    }    

    getBufferedVertices(primitiveType) {
        return new Float32Array();
    }

    getBufferedUvs() {
        return new Float32Array();
    }

    getBufferedVertexColors() {
        return new Float32Array();
    }

    getBufferedEdgeColors() {
        return new Float32Array();
    }

    getAdditionalSceneObjects() {
        return [];
    }

    get shadersFolderName() {
        return 'shaders';
    }

    get shadersFolderAbsolutePath() {
        const { join } = require('path');
        return join(appRoot, this.shadersFolderName);
    }

    get shadersFolderRelativePath() {
        const { join } = require('path');
        return join('./', this.shadersFolderName);
    }

    get texturesFolderName() {
        return 'textures';
    }    

    get texturesFolderAbsolutePath() {
        const { join } = require('path');
        return join(appRoot, this.texturesFolderName);
    }

    get texturesFolderRelativePath() {
        const { join } = require('path');
        return join('./', this.texturesFolderName);
    }
    
    get debugData() {
        return [];
    }    
}