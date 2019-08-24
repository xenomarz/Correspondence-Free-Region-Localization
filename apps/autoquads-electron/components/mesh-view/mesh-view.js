// Web Modules Imports
import { LitElement, html, css } from '../../web_modules/lit-element.js';
import { Machine, interpret } from '../../web_modules/xstate.js';
import { PubSub } from '../../web_modules/pubsub-js.js';
import * as THREE from '../../web_modules/three.js';
import OrbitControls from '../../web_modules/three-orbit-controls.js';

// Components Imports
import { MeshProvider } from '../../components/mesh-provider/mesh-provider.js';

export class MeshView extends LitElement {
    static get styles() {
        return [css`
            :host {
                width: 100%;
                height: 100%;
            }

            canvas {
                display: block;
            }

            .container {
                overflow: hidden;
                width: 100%;
                height: 100%;
                position: relative;
            }

            .canvas-info {
                display: flex;
                flex-direction: column;
                margin: 0px;
                padding: 0px;
                position: absolute;
                top: 20px;
                left: 20px;
                color: white;
                text-shadow: -1px -1px 0 #000, 1px -1px 0 #000, -1px 1px 0 #000, 1px 1px 0 #000;
                pointer-events: none;
                cursor: pointer;
            }

            .canvas-caption {
                font-size: 30px;
            }

            .debug-data {
                display: flex;
                flex-direction: row;
                font-family: Consolas;
                font-size: 16px;

                font-weight: bold;
            }

            .debug-data-labels {
                display: flex;
                flex-direction: column;
                margin-right: 15px;
            }

            .debug-data-values {
                display: flex;
                flex-direction: column;
            }

            /* https://stackoverflow.com/questions/41800823/hide-polymer-element */
            [hidden] {
                display: none;
            }
        `];
    }

    render(){
        return html`
            <div class="container">
                <div class="canvas-info">
                    <div class="canvas-caption">
                        <span>${this.caption}</span>
                    </div>
                    <!-- <div hidden="[[!showDebugData]]" class="debug-data">
                        <div class="debug-data-labels">
                            <template is="dom-repeat" items="[[_debugData]]">
                                <div>[[item.label]]</div>
                            </template>
                        </div>
                        <div class="debug-data-values">
                            <template is="dom-repeat" items="[[_debugData]]">
                                <div>[[item.value]]</div>
                            </template>
                        </div>
                    </div> -->
                </div>
            </div>
        `;
    }

    static get properties() { 
        return {
            id: {
                type: String,
                attribute: 'id'
            },
            meshProvider: {
                type: Object,
                attribute: 'mesh-provider'
            },
            caption: {
                type: String,
                attribute: 'caption'
            },
            backgroundColor: {
                type: String,
                attribute: 'background-color'
            },
            meshColor: {
                type: String,
                attribute: 'mesh-color'
            },
            useGridTexture: {
                type: Boolean,
                attribute: 'use-grid-texture'
            },
            showWireframe: {
                type: Boolean,
                attribute: 'show-wireframe'
            },
            showGrid: {
                type: Boolean,
                attribute: 'show-grid'
            },
            syncFaceSelection: {
                type: Boolean,
                attribute: 'sync-face-selection'
            },
            syncVertexSelection: {
                type: Boolean,
                attribute: 'sync-vertex-selection'
            },
            enableVertexSelection: {
                type: Boolean,
                attribute: 'enable-vertex-selection'
            },
            enableMeshRotation: {
                type: Boolean,
                attribute: 'enable-mesh-rotation'
            },
            enableFaceDragging: {
                type: Boolean,
                attribute: 'enable-face-dragging'
            },
            gridHorizontalColor: {
                type: String,
                attribute: 'grid-horizontal-color'
            },
            gridVerticalColor: {
                type: String,
                attribute: 'grid-vertical-color'
            },
            gridBackgroundColor1: {
                type: String,
                attribute: 'grid-background-color1'
            },
            gridBackgroundColor2: {
                type: String,
                attribute: 'grid-background-color2'
            },
            gridSize: {
                type: Number,
                attribute: 'grid-size'
            },
            gridTextureSize: {
                type: Number,
                attribute: 'grid-texture-size'
            },
            gridLineWidth: {
                type: Number,
                attribute: 'grid-line-width'
            },
            highlightedFaceColor: {
                type: String,
                attribute: 'highlighted-face-color'
            },
            draggedFaceColor: {
                type: String,
                attribute: 'dragged-face-color'
            },
            selectedFaceColor: {
                type: String,
                attribute: 'selected-face-color'
            },
            showDebugData: {
                type: Boolean,
                attribute: 'show-debug-data'
            },
            showUnitGrid: {
                type: Boolean,
                attribute: 'show-unit-grid'
            },
            showGridTexture: {
                type: Boolean,
                attribute: 'show-grid-texture'
            },
            useLights: {
                type: Boolean,
                attribute: 'use-lights'
            }
        };
    }

    constructor() {
        super();
        this._meshProvider = new MeshProvider();
        this._faceIntersection = null;
        this._previousFaceIntersection = null;
        this._selectedFaces = {};
        this._selectedVerticesPoints = {};
        this._draggedFace = null;
        this._highlightedFace = null;
        this._hoveredVertexSize = 20;
        this._vertexSize = 15;
        this._vertexColors = [];
        this._mouseInCanvas = false;
        this._debugData = [];
        this._needResize = false;
        this._initializeStateMachine();
        this._createCamera();
        this._createRenderer();
        this._createMaterials();
        this._createScene();
        this._createOrbitControl();
        this._createRaycaster();

        // this.meshProvider = new MeshProvider();
        // this.backgroundColor = 'rgb(0, 0, 0)';
        // this.meshColor = 'rgb(255, 255, 255)';
        // this.useGridTexture = false;
        // this.showWireframe = false;
        // this.showGrid = false;
        // this.syncFaceSelection = true;
        // this.syncVertexSelection = true;
        // this.enableVertexSelection = false;
        // this.enableMeshRotation = false;
        // this.enableFaceDragging = false;
        // this.gridHorizontalColor = 'rgb(0, 0, 255)';
        // this.gridVerticalColor = 'rgb(0, 0, 255)';
        // this.gridBackgroundColor1 = 'rgb(255, 255, 255)';
        // this.gridBackgroundColor2 = 'rgb(255, 255, 255)';
        // this.gridSize = 3;
        // this.gridTextureSize = 8;
        // this.gridLineWidth = 0;
        // this.useLights = false;
    }
    
    /**
     * Public/Interface Methods
     */

    firstUpdated() {
        // https://stackoverflow.com/questions/54512325/getting-width-height-in-a-slotted-lit-element-in-edge
        window.setTimeout(() => {
            let container = this.shadowRoot.querySelector('.container');
            container.appendChild(this._renderer.domElement);
            this.resizeObserver = new ResizeObserver(() => {
                this._needResize = true;
            });
            
            this.resizeObserver.observe(container);            
            this._resizeScene();
            this._renderScene();
        }, 0);
    }

    connectedCallback() {
        super.connectedCallback();
        this._mouseDownBoundHandler = this._mouseDownHandler.bind(this);
        this._mouseMoveBoundHandler = this._mouseMoveHandler.bind(this);
        this._mouseLeaveBoundHandler = this._mouseLeaveHandler.bind(this);
        this._mouseEnterBoundHandler = this._mouseEnterHandler.bind(this);
        this._mouseUpBoundHandler = this._mouseUpHandler.bind(this);
        this._mouseClickBoundHandler = this._mouseClickHandler.bind(this);
        this._contextMenuBoundHandler = this._contextMenuHandler.bind(this);
        this._keyDownBoundHandler = this._keyDownHandler.bind(this);
        this._keyUpBoundHandler = this._keyUpHandler.bind(this);

        this._renderer.domElement.addEventListener("mousedown", this._mouseDownBoundHandler);
        this._renderer.domElement.addEventListener("mousemove", this._mouseMoveBoundHandler);
        this._renderer.domElement.addEventListener("mouseleave", this._mouseLeaveBoundHandler);
        this._renderer.domElement.addEventListener("mouseenter", this._mouseEnterBoundHandler);
        this._renderer.domElement.addEventListener("mouseup", this._mouseUpBoundHandler);
        this._renderer.domElement.addEventListener("click", this._mouseClickBoundHandler);
        this._renderer.domElement.addEventListener("contextmenu", this._contextMenuBoundHandler);

        // this._renderer.domElement.onmousedown = this._mouseDownBoundHandler;
        // this._renderer.domElement.onmousemove = this._mouseMoveBoundHandler;
        // this._renderer.domElement.onmouseleave = this._mouseLeaveBoundHandler;
        // this._renderer.domElement.onmouseenter = this._mouseEnterBoundHandler;
        // this._renderer.domElement.onmouseup = this._mouseUpBoundHandler;
        // this._renderer.domElement.onclick = this._mouseClickBoundHandler;
        // this._renderer.domElement.oncontextmenu = this._contextMenuBoundHandler;
        window.addEventListener('keydown', this._keyDownBoundHandler);
        window.addEventListener('keyup', this._keyUpBoundHandler);
    }

    disconnectedCallback() {
        this._renderer.domElement.removeEventListener("mousedown", this._mouseDownBoundHandler);
        this._renderer.domElement.removeEventListener("mousemove", this._mouseMoveBoundHandler);
        this._renderer.domElement.removeEventListener("mouseleave", this._mouseLeaveBoundHandler);
        this._renderer.domElement.removeEventListener("mouseenter", this._mouseEnterBoundHandler);
        this._renderer.domElement.removeEventListener("mouseup", this._mouseUpBoundHandler);
        this._renderer.domElement.removeEventListener("click", this._mouseClickBoundHandler);
        this._renderer.domElement.removeEventListener("contextmenu", this._contextMenuBoundHandler);
        window.removeEventListener('keydown', this._keyDownBoundHandler);
        window.removeEventListener('keyup', this._keyUpBoundHandler);
        super.disconnectedCallback();
    }

    /**
     * Properties
     */

    set backgroundColor(value) {
        const oldValue = this._backgroundColor;
        this._backgroundColor = value;
        this._renderer.setClearColor(new THREE.Color(value), 1.0);
        this.requestUpdate('backgroundColor', oldValue);
    }

    get backgroundColor() {
        return this._backgroundColor;
    }

    set meshProvider(value) {
        const oldValue = this._meshProvider;
        this._meshProvider = value;
        this._clearSceneSubtree(this._scene);
        this._initializeLights();
        this._initializeMesh();
        this._initializeMeshWireframe();
        this.requestUpdate('meshProvider', oldValue);
    }

    get meshProvider() {
        return this._meshProvider;
    }

    /**
     * Private Methods
     */

    /**
     * Mouse & Keyboard Handlers
     */

    _mouseDownHandler(e) {
        if (e.button === 0) {
            if (this._fsm.state === 'idle' && this.enableFaceDragging) {
                if (this._faceIntersection) {
                    require('pubsub-js').publish('mesh-view-face-down', {
                        face: this._faceIntersection.face
                    });
                }
            }
        }
    }     

    _mouseEnterHandler(e) {
        this._mouseInCanvas = true;
    }

    _mouseLeaveHandler(e) {
        switch (this._fsm.state) {
            case 'face-dragging':
                this._fsm.endFaceDragging();
                break;
        }

        this._mouseInCanvas = false;
        if (this._faceIntersection) {
            this._resetHighlightedFace();
            require('pubsub-js').publish('mesh-view-face-unhighlighted', {
                meshViewId: this.id
            });
        }

        this._faceIntersection = null;
        this._vertexIntersection = null;
    }

    _mouseMoveHandler(e) {
        event.preventDefault();
        this._setRaycasterWithPixel(e.offsetX, e.offsetY);
        if (this._fsm.state === 'idle') {
            if (this._faceIntersection) {
                if (e.buttons === 1 && this.enableFaceDragging) {
                    this._draggedFace = this._faceIntersection.face;
                    this._fsm.beginFaceDragging();
                }
            }
        } else if (this._fsm.state === 'face-dragging') {
            this._updateDragOffset();
        }
    }

    _mouseUpHandler(e) {
        event.preventDefault();
        this._setRaycasterWithPixel(e.offsetX, e.offsetY);
        if (e.button === 0) {
            switch (this._fsm.state) {
                case 'face-dragging':
                    this._fsm.endFaceDragging();
                    break;
            }
        }
    }     

    _mouseClickHandler(e) {
        if (this._fsm.state === 'vertex-selection') {
            if (this._vertexIntersection) {
                if (this._selectedVerticesPoints[this._vertexIntersection.vertexId]) {
                    this._unselectVertex(this._vertexIntersection.vertexId);
                    require('pubsub-js').publish('mesh-view-vertex-unselected', {
                        vertexId: this._vertexIntersection.vertexId,
                        meshViewId: this.id
                    });
                } else {
                    this._selectVertex(this._vertexIntersection.vertexId, this._vertexIntersection.vertex);
                    require('pubsub-js').publish('mesh-view-vertex-selected', {
                        vertexId: this._vertexIntersection.vertexId,
                        meshViewId: this.id
                    });
                }
            }
        } else if (this._fsm.state === 'face-selection') {
            if (this._faceIntersection) {
                if (this._selectedFaces[this._faceIntersection.face.id]) {
                    this._unselectFace(this._faceIntersection.face);
                    require('pubsub-js').publish('mesh-view-face-unselected', {
                        face: this._faceIntersection.face,
                        meshViewId: this.id
                    });
                } else {
                    this._selectFace(this._faceIntersection.face);
                    require('pubsub-js').publish('mesh-view-face-selected', {
                        face: this._faceIntersection.face,
                        meshViewId: this.id
                    });
                }
            }
        }
    }

    _contextMenuHandler(e) {
        if (this._fsm.state === 'face-dragging') {
            this._selectFace(this._faceIntersection.face);
            require('pubsub-js').publish('mesh-view-face-selected', {
                face: this._faceIntersection.face,
                meshViewId: this.id
            });
            setTimeout(0, () => {
                this._fsm.endFaceDragging();
            });
        }
    }

    _keyDownHandler(event) {
        if (event.keyCode === 16) {
            if (this.enableVertexSelection) {
                this._fsm.beginVertexSelection();
            }
        } else if (event.keyCode === 17) {
            if (this.enableMeshRotation) {
                this._fsm.onBeginMeshRotation();
            }
        } else if (event.keyCode === 18) {
            this._fsm.beginFaceSelection();
        }
    }

    _keyUpHandler(event) {
        if (event.keyCode === 16) {
            if (this.enableVertexSelection) {
                this._fsm.endVertexSelection();
            }
        } else if (event.keyCode === 17) {
            if (this.enableMeshRotation) {
                this._fsm.onEndMeshRotation();
            }
        } else if (event.keyCode === 18) {
            this._fsm.endFaceSelection();
        }
    }

    /**
     * Three.js objects creation
     */

    _createCamera() {
        this._camera = new THREE.PerspectiveCamera(45, 0, 0.1, 10000);
        this._camera.position.x = 0;
        this._camera.position.y = 0;
        this._camera.position.z = 3;
    }

    _createRenderer() {
        this._renderer = new THREE.WebGLRenderer({
            antialias: true
        });
        this._renderer.setPixelRatio(window.devicePixelRatio);
    }

    _createMaterials() {
        this._sphereMaterial = new THREE.MeshBasicMaterial({
            color: 0xff0000
        });
    }   

    _createScene() {
      this._scene = new THREE.Scene();
    }

    _createOrbitControl() {
        this._controls = new (OrbitControls(THREE))(this._camera, this._renderer.domElement);
        this._controls.enableDamping = true;
        this._controls.dampingFactor = 0.25;
        this._controls.screenSpacePanning = false;
        this._controls.enableKeys = false;
        this._controls.minDistance = 0;
        this._controls.maxDistance = 1000;
        this._controls.maxPolarAngle = 2 * Math.PI;
        this._controls.enableRotate = true;
    }

    _createRaycaster() {
        this._raycaster = new THREE.Raycaster();
        this._raycaster.params.Points.threshold = 0.05;
    }

    _resizeScene() {
        this._camera.aspect = this.offsetWidth / this.offsetHeight;
        this._camera.updateProjectionMatrix();
        this._renderer.setSize(this.offsetWidth, this.offsetHeight);
    }

    _initializeLights() {
        let ambientLight = new THREE.AmbientLight(0xffffff, 0.2);
        this._pointLight = new THREE.PointLight(0xffffff, 0.8, 0);

        if (this.useLights) {
            this._scene.add(ambientLight);
            this._scene.add(this._pointLight);
        }
    }    

    _initializeMesh() {
        let geometry = new THREE.BufferGeometry();
        let bufferedMeshVertices = this.meshProvider.bufferedMeshVertices;
        let bufferedMeshUvs = this._getBufferedUvs(bufferedMeshVertices);
        let color = new THREE.Color(this.meshColor);
        let bufferedMeshColors = this._getBufferedColors(bufferedMeshVertices.length / 3);

        geometry.addAttribute('position', new THREE.BufferAttribute(new Float32Array(bufferedMeshVertices), 3));
        geometry.addAttribute('uv', new THREE.BufferAttribute(new Float32Array(bufferedMeshUvs), 2));
        geometry.addAttribute('color', new THREE.BufferAttribute(new Float32Array(bufferedMeshColors), 3, true));

        let material;
        if (this.useLights) {
            material = new THREE.MeshPhongMaterial({
                vertexColors: THREE.VertexColors
            });
        } else {
            material = new THREE.MeshBasicMaterial({
                vertexColors: THREE.VertexColors
            });
        }

        material.color.setHex(color.getHex());
        if (this.showGridTexture) {
            material.map = this._gridTexture;
        }

        geometry.computeVertexNormals();

        this._mesh = new THREE.Mesh(geometry, material);
        this._scene.add(this._mesh);
    }

    _initializeMeshWireframe() {
        let material = new THREE.MeshBasicMaterial({
            color: 0x000000,
            wireframe: true
        });
        
        this._meshWireframe = new THREE.Mesh(this._mesh.geometry, material);
        this._meshWireframe.renderOrder = 1;
        this._scene.add(this._meshWireframe);
    }

    _initializeStateMachine() {
        this._interactionMachine = Machine({
            id: 'interaction',
            initial: 'idle',
            states: {
                idle: {
                    on: {
                        BEGIN_VERTEX_SELECTION: {
                            target: 'vertexSelection',
                            actions: []
                        },
                        BEGIN_FACE_SELECTION: {
                            target: 'faceSelection',
                            actions: []
                        },
                        BEGIN_FACE_DRAGGING: {
                            target: 'faceDragging',
                            actions: []
                        },
                        BEGIN_MESH_ROTATION: {
                            target: 'meshRotation',
                            actions: []
                        }                            
                    }
                },
                vertexSelection: {
                    on: {
                        END_VERTEX_SELECTION: {
                            target: 'idle',
                            actions: []
                        },                         
                    }
                },
                faceSelection: {
                    on: {
                        END_FACE_SELECTION: {
                            target: 'idle',
                            actions: []
                        },                         
                    }
                },
                meshRotation: {
                    on: {
                        END_MESH_ROTATION: {
                            target: 'idle',
                            actions: []
                        },                         
                    }
                }                                      
            }
        });
    }

    beginVertexSelection() {
        this._pointcloud.visible = true;
    }

    endVertexSelection() {
        this._pointcloud.visible = false;
    }

    beginMeshRotation() {
        this._controls.enableRotate = true;
    }

    endMeshRotation() {
        this._controls.enableRotate = false;
    }

    onBeginFaceDragging() {
        this._setDraggedFace(this._faceIntersection.face);
        require('pubsub-js').publish('mesh-view-face-dragging-begin', {
            face: this._draggedFace
        });
        this._controls.enablePan = false;
    }

    onEndFaceDragging() {
        require('pubsub-js').publish('mesh-view-face-dragging-end', {
            face: this._draggedFace
        });
        this._resetDraggedFace();
        this._controls.enablePan = true;
    }    
    
    _getBufferedUvs(bufferedVertices) {
        let bufferedUvs = [];
        let bufferedVerticesLength = bufferedVertices.length;
        for (let i = 0; i < bufferedVerticesLength; i++) {
            if (i % 3 !== 2) {
                bufferedUvs.push(bufferedVertices[i]);
            }
        }

        return bufferedUvs;
    }

    _getBufferedColors(verticesLength) {
        let bufferedColors = [];
        for (let i = 0; i < verticesLength; i++) {
            if (this._vertexColors[i]) {
                bufferedColors.push(this._vertexColors[i].r);
                bufferedColors.push(this._vertexColors[i].g);
                bufferedColors.push(this._vertexColors[i].b);
            } else {
                let color = new THREE.Color(this.meshColor);
                bufferedColors.push(color.r);
                bufferedColors.push(color.g);
                bufferedColors.push(color.b);
            }
        }

        return bufferedColors;
    }

    _clearSceneSubtree(object) {
        while (object.children.length > 0) {
            this._clearSceneSubtree(object.children[0])
            object.remove(object.children[0]);
        }

        if (object.geometry) {
            object.geometry.dispose()
        }

        if (object.material) {
            object.material.dispose()
        }

        if (object.texture) {
            object.texture.dispose()
        }
    }

    _renderScene() {
        if(this._needResize) {
            this._resizeScene();
            this._needResize = false;
        }
        this._pointLight.position.copy(this._camera.position);
        this._renderer.render(this._scene, this._camera);
        this.scheduledAnimationFrameId = requestAnimationFrame(() => this._renderScene());
    }
}

customElements.define('mesh-view', MeshView);