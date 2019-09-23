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
        window.addEventListener('keydown', this._keyDownBoundHandler);
        window.addEventListener('keyup', this._keyUpBoundHandler);

        this._meshViewHighlightFaceSubscriptionToken = PubSub.subscribe('mesh-view-highlight-face', (name, payload) => {
            if (payload.meshViewId !== this.id) {
                this._setHighlightedFace(payload.face);
                this._externalFaceHighlight = true;
            }
        });

        this._meshViewUnhighlightFaceSubscriptionToken = PubSub.subscribe('mesh-view-unhighlight-face', (name, payload) => {
            if (payload.meshViewId !== this.id) {
                this._resetHighlightedFace();
                this._externalFaceHighlight = false;
            }
        });        
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

    set meshProvider(value) {
        const oldValue = this._meshProvider;
        this._meshProvider = value;
        this._clearSceneSubtree(this._scene);
        this._initializeLights();
        this._initializeMesh();
        this._initializeMeshWireframe();
        this._initializePointcloud();
        this.requestUpdate('meshProvider', oldValue);
    }

    get meshProvider() {
        return this._meshProvider;
    }

    set backgroundColor(value) {
        const oldValue = this._backgroundColor;
        this._backgroundColor = new THREE.Color(value);
        this._renderer.setClearColor(this._backgroundColor, 1.0);
        this.requestUpdate('backgroundColor', oldValue);
    }

    get backgroundColor() {
        return this._backgroundColor;
    }

    set highlightedFaceColor(value) {
        const oldValue = this._highlightedFaceColor;
        this._highlightedFaceColor = new THREE.Color(value);
        this.requestUpdate('highlightedFaceColor', oldValue);
    }

    get highlightedFaceColor() {
        return this._highlightedFaceColor;
    }

    set draggedFaceColor(value) {
        const oldValue = this._draggedFaceColor;
        this._draggedFaceColor = new THREE.Color(value);
        this.requestUpdate('draggedFaceColor', oldValue);
    }

    get draggedFaceColor() {
        return this._draggedFaceColor;
    }

    set selectedFaceColor(value) {
        const oldValue = this._selectedFaceColor;
        this._selectedFaceColor = new THREE.Color(value);
        this.requestUpdate('selectedFaceColor', oldValue);
    }

    get selectedFaceColor() {
        return this._selectedFaceColor;
    }      

    /**
     * Private Methods
     */

    /**
     * Initialization
     */

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

    _initializePointcloud() {
        let geometry = new THREE.BufferGeometry();
        geometry.addAttribute('position', new THREE.BufferAttribute(new Float32Array(this.meshProvider.bufferedVertices), 3));

        let vertices = this.meshProvider.vertices;
        let sizes = new Float32Array(vertices.length);
        let selected = new Float32Array(vertices.length);

        for (let i = 0; i < vertices.length; i++) {
            sizes[i] = this._vertexSize;
            selected[i] = this._selectedVerticesPoints[i] !== undefined;
        }

        geometry.addAttribute('size', new THREE.BufferAttribute(sizes, 1));
        geometry.addAttribute('selected', new THREE.BufferAttribute(selected, 1));

        const fs = require('fs-extra');
        const { join } = require('path');
        let vertexshader = fs.readFileSync(join(this.meshProvider.shadersFolderAbsolutePath, 'vertexshader.glsl'), 'utf8');
        let fragmentshader = fs.readFileSync(join(this.meshProvider.shadersFolderAbsolutePath, 'fragmentshader.glsl'), 'utf8');

        let material = new THREE.ShaderMaterial({
            uniforms: {
                color: {
                    value: new THREE.Color(0xff0000)
                },
                textureSelected: {
                    value: new THREE.TextureLoader().load(join(this.meshProvider.texturesFolderRelativePath, 'selected_16_16.png'))
                },
                textureUnselected: {
                    value: new THREE.TextureLoader().load(join(this.meshProvider.texturesFolderRelativePath, 'unselected_16_16.png'))
                }
            },
            vertexShader: vertexshader,
            fragmentShader: fragmentshader,
            alphaTest: 0.5,
            depthTest: false
        });

        this._pointcloud = new THREE.Points(geometry, material);
        this._scene.add(this._pointcloud);
        this._pointcloud.visible = false;
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
                            actions: ['beginVertexSelection']
                        },
                        BEGIN_FACE_SELECTION: {
                            target: 'faceSelection',
                            actions: []
                        },
                        BEGIN_FACE_DRAGGING: {
                            target: 'faceDragging',
                            actions: ['beginFaceDragging']
                        },
                        BEGIN_MESH_ROTATION: {
                            target: 'meshRotation',
                            actions: ['beginMeshRotation']
                        }                            
                    }
                },
                vertexSelection: {
                    on: {
                        END_VERTEX_SELECTION: {
                            target: 'idle',
                            actions: ['endVertexSelection']
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
                faceDragging: {
                    on: {
                        END_FACE_DRAGGING: {
                            target: 'idle',
                            actions: ['endFaceDragging']
                        },                         
                    }
                },                
                meshRotation: {
                    on: {
                        END_MESH_ROTATION: {
                            target: 'idle',
                            actions: ['endMeshRotation']
                        },                         
                    }
                }                                      
            }
        },
        {
            actions: {
                beginVertexSelection: (context, event) => {
                    this.beginVertexSelection();
                },
                endVertexSelection: (context, event) => {
                    this.endVertexSelection();
                },
                beginMeshRotation: (context, event) => {
                    this.beginMeshRotation();
                },
                endMeshRotation: (context, event) => {
                    this.endMeshRotation();
                },
                beginFaceDragging: (context, event) => {
                    this.beginFaceDragging();
                },
                endFaceDragging: (context, event) => {
                    this.endFaceDragging();
                }                                    
            }
        });

        this._interactionService = interpret(this._interactionMachine).onTransition(state => {
            console.log(state.value);
        });

        this._interactionService.start();
    }    

    /**
     * Mouse & Keyboard Handlers
     */

    _mouseDownHandler(e) {
        if (e.button === 0) {
            if (this._interactionService.state.value === 'idle' && this.enableFaceDragging) {
                if (this._faceIntersection) {
                    this._publishFaceMessage('mesh-view-face-down', this._faceIntersection.face);  
                    // PubSub.publish('mesh-view-face-down', {
                    //     face: this._faceIntersection.face
                    // });
                }
            }
        }
    }     

    _mouseEnterHandler(e) {
        this._mouseInCanvas = true;
    }

    _mouseLeaveHandler(e) {
        switch (this._interactionService.state.value) {
            case 'faceDragging':
                    this._interactionService.send('END_FACE_DRAGGING');
                break;
        }

        this._mouseInCanvas = false;
        if (this._faceIntersection) {
            this._resetHighlightedFace();
            this._publishFaceMessage('mesh-view-face-unhighlighted');  
            // PubSub.publish('mesh-view-face-unhighlighted', {
            //     meshViewId: this.id
            // });
        }

        this._faceIntersection = null;
        this._vertexIntersection = null;
    }

    _mouseMoveHandler(e) {
        e.preventDefault();
        this._setRaycasterWithPixel(e.offsetX, e.offsetY);
        if (this._interactionService.state.value === 'idle') {
            if (this._faceIntersection) {
                if (e.buttons === 1 && this.enableFaceDragging) {
                    this._draggedFace = this._faceIntersection.face;
                    this._interactionService.send('BEGIN_FACE_DRAGGING');
                }
            }
        } else if (this._interactionService.state.value === 'faceDragging') {
            this._updateDragOffset();
        }
    }

    _mouseUpHandler(e) {
        e.preventDefault();
        this._setRaycasterWithPixel(e.offsetX, e.offsetY);
        if (e.button === 0) {
            switch (this._interactionService.state.value) {
                case 'faceDragging':
                    this._interactionService.send('END_FACE_DRAGGING');
                    break;
            }
        }
    }     

    _mouseClickHandler(e) {
        if (this._interactionService.state.value === 'vertexSelection') {
            if (this._vertexIntersection) {
                if (this._selectedVerticesPoints[this._vertexIntersection.vertexId]) {
                    this._unselectVertex(this._vertexIntersection.vertexId);
                    PubSub.publish('mesh-view-vertex-unselected', {
                        vertexId: this._vertexIntersection.vertexId,
                        meshViewId: this.id
                    });
                } else {
                    this._selectVertex(this._vertexIntersection.vertexId, this._vertexIntersection.vertex);
                    PubSub.publish('mesh-view-vertex-selected', {
                        vertexId: this._vertexIntersection.vertexId,
                        meshViewId: this.id
                    });
                }
            }
        } else if (this._interactionService.state.value === 'faceSelection') {
            if (this._faceIntersection) {
                if (this._selectedFaces[this._faceIntersection.face.id]) {
                    this._unselectFace(this._faceIntersection.face);
                    this._publishFaceMessage('mesh-view-face-unselected', this._faceIntersection.face);  
                    // PubSub.publish('mesh-view-face-unselected', {
                    //     face: this._faceIntersection.face,
                    //     meshViewId: this.id
                    // });
                } else {
                    this._selectFace(this._faceIntersection.face);
                    this._publishFaceMessage('mesh-view-face-selected', this._faceIntersection.face);  
                    // PubSub.publish('mesh-view-face-selected', {
                    //     face: this._faceIntersection.face,
                    //     meshViewId: this.id
                    // });
                }
            }
        }
    }

    _contextMenuHandler(e) {
        if (this._interactionService.state.value === 'faceDragging') {
            this._selectFace(this._faceIntersection.face);
            this._publishFaceMessage('mesh-view-face-selected', this._faceIntersection.face);  
            // PubSub.publish('mesh-view-face-selected', {
            //     face: this._faceIntersection.face,
            //     meshViewId: this.id
            // });
            setTimeout(0, () => {
                this._interactionService.send('END_FACE_DRAGGING');
            });
        }
    }

    _keyDownHandler(event) {
        if (event.keyCode === 16) {
            if (this.enableVertexSelection) {
                this._interactionService.send('BEGIN_VERTEX_SELECTION');
            }
        } else if (event.keyCode === 17) {
            if (this.enableMeshRotation) {
                this._interactionService.send('BEGIN_MESH_ROTATION');
            }
        } else if (event.keyCode === 18) {
            this._interactionService.send('BEGIN_FACE_SELECTION');
        }
    }

    _keyUpHandler(event) {
        if (event.keyCode === 16) {
            if (this.enableVertexSelection) {
                this._interactionService.send('END_VERTEX_SELECTION');
            }
        } else if (event.keyCode === 17) {
            if (this.enableMeshRotation) {
                this._interactionService.send('END_MESH_ROTATION');
            }
        } else if (event.keyCode === 18) {
            this._interactionService.send('END_FACE_SELECTION');
        }
    }

    /**
     * Face and vertex intersection
     */

    _getMeshIntersection() {
        let intersections = this._raycaster.intersectObject(this._mesh);
        if (intersections.length > 0) {
            let intersection = intersections[0];
            intersection.plane = new THREE.Plane();
            intersection.offset = new THREE.Vector3();
            intersection.plane.setFromNormalAndCoplanarPoint(this._camera.getWorldDirection(intersection.plane.normal),
            intersection.point);
            intersection.face.id = Math.round(intersection.faceIndex / 3);
            return intersection;
        }

        return null;
    }

    _getVerticesIntersection() {
        let intersections = this._raycaster.intersectObject(this._pointcloud);
        if (intersections.length > 0) {
            let intersection = intersections[0];
            intersection.vertexId = intersection.index;
            return intersection;
        }

        return null;
    }
    
    _getOffsetFromIntersection() {
        let currentIntersectionPoint = new THREE.Vector3();
        let offset = new THREE.Vector3();
        if (this._raycaster.ray.intersectPlane(this._faceIntersection.plane, currentIntersectionPoint)) {
            offset.subVectors(currentIntersectionPoint, this._faceIntersection.point);
        }

        return offset;
    }

    _updateDragOffset() {
        this._faceIntersection.offset = this._getOffsetFromIntersection();
        this._publishFaceMessage('mesh-view-face-dragging', this._faceIntersection.face, {
            offset: this._faceIntersection.offset
        });  
        // PubSub.publish('mesh-view-face-dragging', {
        //     face: this._faceIntersection.face,
        //     offset: this._faceIntersection.offset
        // });
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
        this._controls.enableRotate = false;
    }

    _createRaycaster() {
        this._raycaster = new THREE.Raycaster();
        this._raycaster.params.Points.threshold = 0.05;
    }

    /**
     * Face & Vertex manipulation
     */

    _colorVertex(vertex, color) {
        this._vertexColors[vertex] = color;
    }

    _uncolorVertex(face) {
        delete this._vertexColors[vertex];
    }

    _colorFace(face, color) {
        this._colorVertex(face.a, color);
        this._colorVertex(face.b, color);
        this._colorVertex(face.c, color);
    }

    _uncolorFace(face) {
        this._uncolorVertex(face.a);
        this._uncolorVertex(face.b);
        this._uncolorVertex(face.c);
    }

    _setDraggedFace(face) {
        this._draggedFace = face;
    }

    _resetDraggedFace(face) {
        this._draggedFace = null;
    }

    _setHighlightedFace(face) {
        this._highlightedFace = face;
    }

    _resetHighlightedFace(face) {
        this._highlightedFace = null;
    }

    _selectFace(face) {
        this._selectedFaces[face.id] = face;
    }

    _unselectFace(face) {
        delete this._selectedFaces[face.id];
    }

    _selectVertex(vertexId, vertex) {
        this._pointcloud.geometry.attributes.selected.array[vertexId] = 1;
        this._pointcloud.geometry.attributes.selected.needsUpdate = true;
        this._selectedVerticesPoints[vertexId] = {
            vertexId: vertexId,
            vertex: vertex
        }
    }

    _unselectVertex(vertexId) {
        this._pointcloud.geometry.attributes.selected.array[vertexId] = 0;
        this._pointcloud.geometry.attributes.selected.needsUpdate = true;
        delete this._selectedVerticesPoints[vertexId];
    }

    _colorVertex(vertex, color) {
        this._vertexColors[vertex] = color;
    }

    _uncolorVertex(face) {
        delete this._vertexColors[vertex];
    }

    _colorFace(face, color) {
        this._colorVertex(face.a, color);
        this._colorVertex(face.b, color);
        this._colorVertex(face.c, color);
    }

    _uncolorFace(face) {
        this._uncolorVertex(face.a);
        this._uncolorVertex(face.b);
        this._uncolorVertex(face.c);
    }

    _colorFaces() {
        for (let faceId in this._selectedFaces) {
            this._colorFace(this._selectedFaces[faceId], this._selectedFaceColor);
        }

        if (this._highlightedFace) {
            this._colorFace(this._highlightedFace, this._highlightedFaceColor);
        }

        if (this._draggedFace) {
            this._colorFace(this._draggedFace, this._draggedFaceColor);
        }
    }

    _colorVertices() {
        let bufferedMeshVertexColors = this.meshProvider.bufferedMeshVertexColors;
        for (let vertexId in bufferedMeshVertexColors) {
            this._colorVertex(vertexId, bufferedMeshVertexColors[vertexId]);
        }
    }
    
    _updateVertexColors() {
        this._vertexColors = [];
        // this._colorVertices();
        this._colorFaces();
    }

    /**
     * Scene manipulation
     */

    _resetAttributeArray(buffer, attributeArray) {
        let bufferLength = buffer.length;
        for (let i = 0; i < bufferLength; i++) {
            attributeArray[i] = buffer[i];
        }
    }

    _resizeScene() {
        this._camera.aspect = this.offsetWidth / this.offsetHeight;
        this._camera.updateProjectionMatrix();
        this._renderer.setSize(this.offsetWidth, this.offsetHeight);
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

        if (this._interactionService.state.value !== 'faceDragging' && this._mouseInCanvas) {
            this._faceIntersection = this._getMeshIntersection();
            this._vertexIntersection = this._getVerticesIntersection();

            if (this._faceIntersection) {
                this._setHighlightedFace(this._faceIntersection.face);
                this._publishFaceMessage('mesh-view-face-highlighted', this._faceIntersection.face);  
                // PubSub.publish('mesh-view-face-highlighted', {
                //     face: this._faceIntersection.face,
                //     meshViewId: this.id
                // });
            } else {
                this._resetHighlightedFace();
                this._publishFaceMessage('mesh-view-face-unhighlighted');        
                // PubSub.publish('mesh-view-face-unhighlighted', {
                //     meshViewId: this.id
                // });
            }
        }

        let bufferedMeshVertices = this.meshProvider.bufferedMeshVertices;

        this._updateVertexColors();
        this._resetAttributeArray(this._getBufferedColors(bufferedMeshVertices.length / 3), this._mesh.geometry.attributes.color.array);

        this._mesh.geometry.attributes.position.needsUpdate = true;
        this._mesh.geometry.attributes.uv.needsUpdate = true;
        this._mesh.geometry.attributes.color.needsUpdate = true;
        this._pointcloud.geometry.attributes.position.needsUpdate = true;        
        this._renderer.render(this._scene, this._camera);
        this.scheduledAnimationFrameId = requestAnimationFrame(() => this._renderScene());
    }

    _setRaycasterWithPixel(x, y) {
        let deviceMousePosition = new THREE.Vector2();
        let boundingClientRect = this._renderer.domElement.getBoundingClientRect();
        deviceMousePosition.x = (x / boundingClientRect.width) * 2 - 1;
        deviceMousePosition.y = -(y / boundingClientRect.height) * 2 + 1;
        this._raycaster.setFromCamera(deviceMousePosition, this._camera);
    }    

    /**
     * State machine actions
     */

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

    beginFaceDragging() {
        this._setDraggedFace(this._faceIntersection.face);
        this._publishFaceMessage('mesh-view-face-dragging-begin', this._draggedFace);        
        // PubSub.publish('mesh-view-face-dragging-begin', {
        //     face: this._draggedFace
        // });
        this._controls.enablePan = false;
    }

    endFaceDragging() {
        this._publishFaceMessage('mesh-view-face-dragging-end', this._draggedFace);
        // PubSub.publish('mesh-view-face-dragging-end', {
        //     face: this._draggedFace
        // });
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

    /**
     * Message publication
     */

    _publishFaceMessage(message, face, options) {
        let payload = {
            meshViewId: this.id,
            ...options
        }

        if(face) {
            payload = {
                ...payload,
                face: face,
                faceSelected: Boolean(this._selectedFaces[face.id])  
            }
        }

        PubSub.publish(message, payload);         
    }
}

customElements.define('mesh-view', MeshView);