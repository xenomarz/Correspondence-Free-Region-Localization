export class MeshProvider {
    constructor() {

    }

    get vertices() {
        return [];
    }

    get faces() {
        return [];
    }

    get bufferedVertices() {
        return [];
    }

    get bufferedMeshVertices() {
        return [];
    }

    get bufferedMeshUvs() {
        return [];
    }

    get bufferedMeshVertexColors() {
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