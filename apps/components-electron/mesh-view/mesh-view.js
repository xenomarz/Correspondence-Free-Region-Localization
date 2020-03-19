// Web Modules Imports
import { LitElement, html, css } from '../../web_modules/lit-element.js';
import { Machine, interpret } from '../../web_modules/xstate.js';
import { PubSub } from '../../web_modules/pubsub-js.js';
import chroma from '../../web_modules/chroma-js.js';
import * as THREE from '../../web_modules/three.js';
import * as Line2 from '../../web_modules/three/examples/jsm/lines/Line2.js';
import * as LineSegments2 from '../../web_modules/three/examples/jsm/lines/LineSegments2.js';
import * as LineMaterial from '../../web_modules/three/examples/jsm/lines/LineMaterial.js';
import * as LineGeometry from '../../web_modules/three/examples/jsm/lines/LineGeometry.js';
import * as LineSegmentsGeometry from '../../web_modules/three/examples/jsm/lines/LineSegmentsGeometry.js';
import OrbitControls from '../../web_modules/three-orbit-controls.js';

// Components Imports
import { 
    MeshProvider,
    BufferedPrimitiveType } from '../../components/mesh-provider/mesh-provider.js';

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
                flex-direction: column;
                font-family: Consolas;
                font-size: 16px;
                font-weight: bold;
            }

            .debug-data-item {
                display: flex;
                flex-direction: column;
                margin-top: 20px;
            }

            .debug-data-item-fields {
                display: flex;
                flex-direction: row;
                margin-left: 10px;
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
                    <!-- TODO: extract the debug-data markup into an external custom element -->
                    <!-- ${this.showDebugData ? html`
                        <div class="debug-data">
                            ${this.debugData.map(item => html`
                                <div class="debug-data-item">
                                    <div>${item.name}</div>
                                    <div class="debug-data-item-fields">
                                        <div class="debug-data-labels">
                                            ${Object.keys(item.data).map((key, index) => html`
                                                <div>${key}</div>
                                            `)}
                                        </div>
                                        <div class="debug-data-values">
                                            ${Object.keys(item.data).map((key, index) => html`
                                                <div>${item.data[key]}</div>
                                            `)}
                                        </div>
                                    </div>
                                </div>
                            `)}
                        </div>
                    `: ''} -->
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
            showFatWireframe: {
                type: Boolean,
                attribute: 'show-fat-wireframe'
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
            highlightedEdgeColor: {
                type: String,
                attribute: 'highlighted-edge-color'
            },
            draggedFaceColor: {
                type: String,
                attribute: 'dragged-face-color'
            },
            selectedFaceColor: {
                type: String,
                attribute: 'selected-face-color'
            },
            selectedEdgeColor: {
                type: String,
                attribute: 'selected-edge-color'
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
            },
            debugData: {
                type: Array
            }
        };
    }

    constructor() {
        super();
        this._meshProvider = new MeshProvider();
        this._faceIntersection = null;
        this._previousFaceIntersection = null;
        this._selectedFaces = {};
        this._selectedEdges = {};
        this._selectedVerticesPoints = {};
        this._draggedFace = null;
        this._highlightedFace = null;
        this._highlightedEdge = null;
        this._hoveredVertexSize = 20;
        this._vertexSize = 15;
        this._vertexColors = [];
        this._edgeColors = [];
        this._mouseInCanvas = false;
        this.debugData = [];
        this._needResize = false;
        this.showWireframe = false;
        this.showFatWireframe = false;
        this.showUnitGrid = false;
        this.showDebugData = false;
        this._initializeStateMachine();
        this._createCamera();
        this._createRenderer();
        this._createMaterials();
        this._createScene();
        this._createOrbitControl();
        // this._createRaycaster();
        // this._createGrid();
    }

    /**
     * Properties
     */
    set meshProvider(value) {
        const oldValue = this._meshProvider;
        if(oldValue !== value) {
            this._meshProvider = value;
            this._clearSceneSubtree(this._scene);
            // this._initializeTextures();
            this._initializeLights();
            this._initializeMesh();
            // this._initializeMeshEdges();
            this._initializeMeshWireframe();
            // this._initializePointcloud();
            // this._initializeGrid();
            this.requestUpdate('meshProvider', oldValue);
        }
    }

    get meshProvider() {
        return this._meshProvider;
    }

    set backgroundColor(value) {
        const oldValue = this._backgroundColor;
        if(oldValue !== value) {
            this._backgroundColor = new THREE.Color(value);
            this._renderer.setClearColor(this._backgroundColor, 1.0);
            this.requestUpdate('backgroundColor', oldValue);
        }
    }

    get backgroundColor() {
        return this._backgroundColor;
    }

    set highlightedFaceColor(value) {
        const oldValue = this._highlightedFaceColor;
        if(oldValue !== value) {
            this._highlightedFaceColor = new THREE.Color(value);
            this.requestUpdate('highlightedFaceColor', oldValue);
        }
    }

    get highlightedFaceColor() {
        return this._highlightedFaceColor;
    }

    set highlightedEdgeColor(value) {
        const oldValue = this._highlightedEdgeColor;
        if(oldValue !== value) {
            this._highlightedEdgeColor = new THREE.Color(value);
            this.requestUpdate('highlightedEdgeColor', oldValue);
        }
    }

    get highlightedEdgeColor() {
        return this._highlightedEdgeColor;
    }

    set draggedFaceColor(value) {
        const oldValue = this._draggedFaceColor;
        if(oldValue !== value) {
            this._draggedFaceColor = new THREE.Color(value);
            this.requestUpdate('draggedFaceColor', oldValue);
        }
    }

    get draggedFaceColor() {
        return this._draggedFaceColor;
    }

    set selectedFaceColor(value) {
        const oldValue = this._selectedFaceColor;
        if(oldValue !== value) {
            this._selectedFaceColor = new THREE.Color(value);
            this.requestUpdate('selectedFaceColor', oldValue);
        }
    }

    get selectedFaceColor() {
        return this._selectedFaceColor;
    }
    
    set selectedEdgeColor(value) {
        const oldValue = this._selectedEdgeColor;
        if(oldValue !== value) {
            this._selectedEdgeColor = new THREE.Color(value);
            this.requestUpdate('selectedEdgeColor', oldValue);
        }
    }

    get selectedEdgeColor() {
        return this._selectedEdgeColor;
    }      

    set gridHorizontalColor(value) {
        const oldValue = this._gridHorizontalColor;
        if(oldValue !== value) {
            this._gridHorizontalColor = new THREE.Color(value);
            this._reinitializeMeshTexture();
            this.requestUpdate('gridHorizontalColor', oldValue);
        }
    }

    get gridHorizontalColor() {
        return this._gridHorizontalColor;
    }

    set gridVerticalColor(value) {
        const oldValue = this._gridVerticalColor;
        if(oldValue !== value) {
            this._gridVerticalColor = new THREE.Color(value);
            this._reinitializeMeshTexture();
            this.requestUpdate('gridVerticalColor', oldValue);
        }
    }

    get gridVerticalColor() {
        return this._gridVerticalColor;
    } 

    set gridBackgroundColor1(value) {
        const oldValue = this._gridBackgroundColor1;
        if(oldValue !== value) {
            this._gridBackgroundColor1 = new THREE.Color(value);
            this._reinitializeMeshTexture();
            this.requestUpdate('gridBackgroundColor1', oldValue);
        }
    }

    get gridBackgroundColor1() {
        return this._gridBackgroundColor1;
    }

    set gridBackgroundColor2(value) {
        const oldValue = this._gridBackgroundColor2;
        if(oldValue !== value) {
            this._gridBackgroundColor2 = new THREE.Color(value);
            this._reinitializeMeshTexture();
            this.requestUpdate('gridBackgroundColor2', oldValue);
        }
    }

    get gridBackgroundColor2() {
        return this._gridBackgroundColor2;
    }

    set gridSize(value) {
        const oldValue = this._gridSize;
        if(oldValue !== value) {
            this._gridSize = value;
            this._reinitializeMeshTexture();
            this.requestUpdate('gridSize', oldValue);
        }
    }

    get gridSize() {
        return this._gridSize;
    }     

    set gridTextureSize(value) {
        const oldValue = this._gridTextureSize;
        if(oldValue !== value) {
            this._gridTextureSize = value;
            this._reinitializeMeshTexture();
            this.requestUpdate('gridTextureSize', oldValue);
        }
    }

    get gridTextureSize() {
        return this._gridTextureSize;
    }     

    set gridLineWidth(value) {
        const oldValue = this._gridLineWidth;
        if(oldValue !== value) {
            this._gridLineWidth = value;
            this._reinitializeMeshTexture();
            this.requestUpdate('gridLineWidth', oldValue);
        }
    }

    get gridLineWidth() {
        return this._gridLineWidth;
    } 

    set showWireframe(value) {
        const oldValue = this._showWireframe;
        if(oldValue !== value) {
            this._showWireframe = value;
            if(this._meshWireframe) {
                this._meshWireframe.visible = value;
            }
            this.requestUpdate('showWireframe', oldValue);
        }
    }

    get showWireframe() {
        return this._showWireframe;
    }

    set showFatWireframe(value) {
        const oldValue = this._showFatWireframe;
        if(oldValue !== value) {
            this._showFatWireframe = value;
            if(this._meshEdges) {
                this._meshEdges.visible = value;
            }
            if(this._meshEdgesHidden) {
                this._meshEdgesHidden.visible = value;
            }
            this.requestUpdate('showFatWireframe', oldValue);
        }
    }

    get showFatWireframe() {
        return this._showFatWireframe;
    }

    set showUnitGrid(value) {
        const oldValue = this._showUnitGrid;
        if(oldValue !== value) {
            this._showUnitGrid = value;
            if(this._grid) {
                this._grid.visible = value;
            }
            this.requestUpdate('showUnitGrid', oldValue);
        }
    }

    get showUnitGrid() {
        return this._showUnitGrid;
    }

    set showDebugData(value) {
        const oldValue = this._showDebugData;
        if(oldValue !== value) {
            this._showDebugData = value;
            this.requestUpdate('showDebugData', oldValue);
        }
    }

    get showDebugData() {
        return this._showDebugData;
    }         
    
    /**
     * Public methods
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
        
        this.meshViewSelectFaceSubscriptionToken = PubSub.subscribe('mesh-view-select-face', (name, payload) => {
            if (payload.meshViewId !== this.id) {
                this._selectFace(payload.face);
            }
        });

        this.meshViewUnselectFaceSubscriptionToken = PubSub.subscribe('mesh-view-unselect-face', (name, payload) => {
            if (payload.meshViewId !== this.id) {
                this._unselectFace(payload.face);
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
     * Private methods
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

        geometry.addAttribute('position', new THREE.BufferAttribute(this.meshProvider.getBufferedVertices(BufferedPrimitiveType.TRIANGLE), 3));
        // geometry.addAttribute('uv', new THREE.BufferAttribute(this.meshProvider.getBufferedUvs(), 2));
        geometry.addAttribute('color', new THREE.BufferAttribute(this.meshProvider.getBufferedVertexColors(), 3, true));

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

        material.color = this.meshProvider.meshColor;

        // if (this.showGridTexture) {
        //     material.map = this._gridTexture;
        // }

        geometry.computeVertexNormals();

        this._mesh = new THREE.Mesh(geometry, material);
        this._scene.add(this._mesh);
    }

    _initializeMeshEdges() {
        let lineSegmentsGeometry = new LineSegmentsGeometry.LineSegmentsGeometry();
        let bufferGeometry = new THREE.BufferGeometry();

        lineSegmentsGeometry.setPositions(this.meshProvider.getBufferedVertices(BufferedPrimitiveType.EDGE));
        lineSegmentsGeometry.setColors(this.meshProvider.getBufferedEdgeColors());

        bufferGeometry.addAttribute('position', new THREE.BufferAttribute(this.meshProvider.getBufferedVertices(BufferedPrimitiveType.EDGE), 3));

        let lineMaterial = new LineMaterial.LineMaterial({ 
            color: 0xffffff,
            vertexColors: THREE.VertexColors, 
            linewidth: 10
        });

        lineMaterial.resolution.set(window.innerWidth, window.innerHeight);

        // LineSegments2 supports drawing fat lines but do not support raycasting collision detection
        this._meshEdges = new LineSegments2.LineSegments2(lineSegmentsGeometry, lineMaterial);

        // LineSegments supports raycasting collision detection but do not support drawing fat lines
        this._meshEdgesHidden = new THREE.LineSegments(bufferGeometry, lineMaterial);

        // We use _meshEdges for rendering
        this._scene.add(this._meshEdges);

        // We use _meshEdgesHidden for collision detection
        this._scene.add(this._meshEdgesHidden);

        this._meshEdges.visible = this.showFatWireframe;
        this._meshEdgesHidden.visible = this.showFatWireframe;
    }    

    _initializeMeshWireframe() {
        let material = new THREE.MeshBasicMaterial({
            color: 0x000000,
            wireframe: true
        });
        
        this._meshWireframe = new THREE.Mesh(this._mesh.geometry, material);
        this._meshWireframe.renderOrder = 1;
        this._meshWireframe.visible = this.showWireframe;
        this._scene.add(this._meshWireframe);
    }

    _initializePointcloud() {
        let geometry = new THREE.BufferGeometry();
        geometry.addAttribute('position', new THREE.BufferAttribute(this.meshProvider.getBufferedVertices(BufferedPrimitiveType.VERTEX), 3));

        let verticesCount = this.meshProvider.getVerticesCount();
        let sizes = new Float32Array(verticesCount);
        let selected = new Float32Array(verticesCount);

        for (let i = 0; i < verticesCount; i++) {
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

    _initializeGrid() {
        let size = 500;
        let divisions = 500;
        this._grid = new THREE.GridHelper(size, divisions);
        this._grid.geometry.rotateX(Math.PI / 2);
        this._grid.visible = this.showUnitGrid;
        this._scene.add(this._grid);
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
                        BEGIN_ELEMENT_SELECTION: {
                            target: 'elementSelection',
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
                elementSelection: {
                    on: {
                        END_ELEMENT_SELECTION: {
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
     * Mouse & keyboard handlers
     */
    _mouseDownHandler(e) {
        // e.preventDefault();
        // if (e.button === 0) {
        //     if (this._interactionService.state.value === 'idle' && this.enableFaceDragging) {
        //         if (this._faceIntersection) {
        //             this._publishFaceMessage('mesh-view-face-down', this._faceIntersection.face);  
        //         }
        //     }
        // }
    }     

    _mouseEnterHandler(e) {
        // e.preventDefault();
        // this._mouseInCanvas = true;
    }

    _mouseLeaveHandler(e) {
        // e.preventDefault();
        // switch (this._interactionService.state.value) {
        //     case 'faceDragging':
        //         this._interactionService.send('END_FACE_DRAGGING');
        //         break;
        // }

        // this._mouseInCanvas = false;
        // if (this._faceIntersection) {
        //     this._resetHighlightedFace();
        //     this._publishFaceMessage('mesh-view-face-unhighlighted');  
        // }

        // this._faceIntersection = null;
        // this._vertexIntersection = null;
    }

    _mouseMoveHandler(e) {
        // e.preventDefault();
        // this._setRaycasterWithPixel(e.offsetX, e.offsetY);
        // if (this._interactionService.state.value === 'idle') {
        //     if (this._faceIntersection) {
        //         if (e.buttons === 1 && this.enableFaceDragging) {
        //             this._draggedFace = this._faceIntersection.face;
        //             this._interactionService.send('BEGIN_FACE_DRAGGING');
        //         }
        //     }
        // } else if (this._interactionService.state.value === 'faceDragging') {
        //     this._updateDragOffset();
        // }
    }

    _mouseUpHandler(e) {
        // e.preventDefault();
        // this._setRaycasterWithPixel(e.offsetX, e.offsetY);
        // if (e.button === 0) {
        //     switch (this._interactionService.state.value) {
        //         case 'faceDragging':
        //             this._interactionService.send('END_FACE_DRAGGING');
        //             break;
        //     }
        // }
    }     

    _mouseClickHandler(e) {
        // e.preventDefault();
        // if (this._interactionService.state.value === 'vertexSelection') {
        //     if (this._vertexIntersection) {
        //         if (this._selectedVerticesPoints[this._vertexIntersection.vertexId]) {
        //             this._unselectVertex(this._vertexIntersection.vertexId);
        //             PubSub.publish('mesh-view-vertex-unselected', {
        //                 vertexId: this._vertexIntersection.vertexId,
        //                 meshViewId: this.id
        //             });
        //         } else {
        //             this._selectVertex(this._vertexIntersection.vertexId, this._vertexIntersection.vertex);
        //             PubSub.publish('mesh-view-vertex-selected', {
        //                 vertexId: this._vertexIntersection.vertexId,
        //                 meshViewId: this.id
        //             });
        //         }
        //     }
        // } else if (this._interactionService.state.value === 'elementSelection') {
        //     if(this._edgeIntersection) {
        //         if (this._selectedEdges[this._edgeIntersection.edge.id]) {
        //             this._unselectEdge(this._edgeIntersection.edge);
        //             this._publishEdgeMessage('mesh-view-edge-unselected', this._edgeIntersection.edge);  
        //         } else {
        //             this._selectEdge(this._edgeIntersection.edge);
        //             this._publishEdgeMessage('mesh-view-edge-selected', this._edgeIntersection.edge);  
        //         }
        //     } else if (this._faceIntersection) {
        //         if (this._selectedFaces[this._faceIntersection.face.id]) {
        //             this._unselectFace(this._faceIntersection.face);
        //             this._publishFaceMessage('mesh-view-face-unselected', this._faceIntersection.face);  
        //         } else {
        //             this._selectFace(this._faceIntersection.face);
        //             this._publishFaceMessage('mesh-view-face-selected', this._faceIntersection.face);  
        //         }
        //     }
        // }
    }

    _contextMenuHandler(e) {
        // e.preventDefault();
        // if (this._interactionService.state.value === 'faceDragging') {
        //     this._selectFace(this._faceIntersection.face);
        //     this._publishFaceMessage('mesh-view-face-selected', this._faceIntersection.face);  
        //     setTimeout(0, () => {
        //         this._interactionService.send('END_FACE_DRAGGING');
        //     });
        // }
    }

    _keyDownHandler(e) {
        if (e.keyCode === 192) {
            if (this.enableVertexSelection) {
                this._interactionService.send('BEGIN_VERTEX_SELECTION');
            }
        } else if (e.keyCode === 16) {
            if (this.enableMeshRotation) {
                this._interactionService.send('BEGIN_MESH_ROTATION');
            }
        } else if (e.keyCode === 17) {
            this._interactionService.send('BEGIN_ELEMENT_SELECTION');
        }
    }

    _keyUpHandler(e) {
        if (e.keyCode === 192) {
            if (this.enableVertexSelection) {
                this._interactionService.send('END_VERTEX_SELECTION');
            }
        } else if (e.keyCode === 16) {
            if (this.enableMeshRotation) {
                this._interactionService.send('END_MESH_ROTATION');
            }
        } else if (e.keyCode === 17) {
            this._interactionService.send('END_ELEMENT_SELECTION');
        }
    }

    /**
     * Face, edge and vertex intersection
     */
    _getFaceIntersection() {
        let intersections = this._raycaster.intersectObject(this._mesh);

        if (intersections.length > 0) {
            let intersection = intersections[0];
            intersection.plane = new THREE.Plane();
            intersection.offset = new THREE.Vector3();
            intersection.plane.setFromNormalAndCoplanarPoint(this._camera.getWorldDirection(intersection.plane.normal), intersection.point);
            intersection.face.id = intersection.faceIndex;
            return intersection;
        }

        return null;
    }

    _getEdgeIntersections() {
        return this._raycaster.intersectObject(this._meshEdgesHidden);
    }

    _getVertexIntersection() {
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

        let movementX = this._faceIntersection.offset.x - this._faceIntersection.previousOffset.x;
        let movementY = this._faceIntersection.offset.y - this._faceIntersection.previousOffset.y;

        this._faceIntersection.movement = new THREE.Vector3(movementX, movementY, 0);
        // this._faceIntersection.movement.sub(this._faceIntersection.previousOffset);
        this._faceIntersection.previousOffset = this._faceIntersection.offset;

        console.log(this._faceIntersection.movement);

        this._publishFaceMessage('mesh-view-face-dragging', this._faceIntersection.face, {
            offset: this._faceIntersection.movement
        });  
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
        this._raycaster.linePrecision = 0.005;
    }

    _createGrid() {
        let size = 500;
        let divisions = 500;
        this._grid = new THREE.GridHelper(size, divisions);
        this._grid.geometry.rotateX(Math.PI / 2);
    }

    /**
     * Textures
     */
    _initializeGridTexture(textureSizeExp, gridSizeExp, lineWidthFactor, backgroundColor1, backgroundColor2, horizontalColor, verticalColor) {
        let gridSize = Math.pow(2, gridSizeExp);
        let textureSize = Math.pow(2, textureSizeExp);

        let canvas = document.createElement("canvas");
        let context = canvas.getContext("2d");
        canvas.width = textureSize;
        canvas.height = textureSize;

        let setColor = (x, y, color) => {
            context.fillStyle = chroma(color.r * 255, color.g * 255, color.b * 255).css();
            context.fillRect(x, y, 1, 1);
        }

        let paintHorizontalLine = (y) => {
            for (let x = 0; x < textureSize; x++) {
                setColor(x, y, horizontalColor);
            }
        }

        let paintVerticalLine = (x) => {
            for (let y = 0; y < textureSize; y++) {
                setColor(x, y, verticalColor);
            }
        }

        let paintRegion = (xStart, yStart, xEnd, yEnd, color) => {
            for (let x = xStart; x <= xEnd; x++) {
                for (let y = yStart; y <= yEnd; y++) {
                    setColor(x, y, color);
                }
            }
        }

        let delta = textureSize / gridSize;
        for (let i = 0; i < delta; i++) {
            for (let j = 0; j < delta; j++) {
                let xStart = i * gridSize;
                let yStart = j * gridSize;

                let xEnd = (i + 1) * gridSize;
                let yEnd = (j + 1) * gridSize;

                if (i % 2 === 0 && j % 2 === 0) {
                    paintRegion(xStart, yStart, xEnd, yEnd, backgroundColor1);
                } else if (i % 2 === 1 && j % 2 === 0) {
                    paintRegion(xStart, yStart, xEnd, yEnd, backgroundColor2);
                } else if (i % 2 === 0 && j % 2 === 1) {
                    paintRegion(xStart, yStart, xEnd, yEnd, backgroundColor2);
                } else if (i % 2 === 1 && j % 2 === 1) {
                    paintRegion(xStart, yStart, xEnd, yEnd, backgroundColor1);
                }
            }
        }

        for (let i = 0; i <= delta; i++) {
            if (i == 0) {
                for (let y = 0; y < (lineWidthFactor + 1); y++) {
                    paintHorizontalLine(y);
                }
            } else if (i == delta) {
                for (let y = textureSize - lineWidthFactor; y < textureSize; y++) {
                    paintHorizontalLine(y);
                }
            } else {
                let xCenter = i * gridSize;
                for (let y = xCenter - lineWidthFactor; y < xCenter + lineWidthFactor + 1; y++) {
                    paintHorizontalLine(y);
                }
            }
        }

        for (let i = 0; i <= delta; i++) {
            if (i == 0) {
                for (let x = 0; x < (lineWidthFactor + 1); x++) {
                    paintVerticalLine(x);
                }
            } else if (i == delta) {
                for (let x = textureSize - lineWidthFactor; x < textureSize; x++) {
                    paintVerticalLine(x);
                }
            } else {
                let xCenter = i * gridSize;
                for (let x = xCenter - lineWidthFactor; x < xCenter + lineWidthFactor + 1; x++) {
                    paintVerticalLine(x);
                }
            }
        }

        this._gridTexture = new THREE.CanvasTexture(canvas);
        this._gridTexture.wrapS = THREE.RepeatWrapping;
        this._gridTexture.wrapT = THREE.RepeatWrapping;
        this._gridTexture.needsUpdate = true;
    }

    _initializeTextures() {
        this._initializeGridTexture(this.gridTextureSize, this.gridSize, this.gridLineWidth, this.gridBackgroundColor1, this.gridBackgroundColor2, this.gridHorizontalColor, this.gridVerticalColor);
    }
    
    _reinitializeMeshTexture() {
        if(this._mesh) {
            this._initializeGridTexture(this.gridTextureSize, this.gridSize, this.gridLineWidth, this.gridBackgroundColor1, this.gridBackgroundColor2, this.gridHorizontalColor, this.gridVerticalColor);
            if (this.showGridTexture) {
                this._mesh.material.map = this._gridTexture;
            }
            this._mesh.material.needsUpdate = true;
        }
    }    

    /**
     * Face, edge and vertex manipulation
     */
    _colorVertex(elementColors, vertexId, color) {
        elementColors.push({
            baseIndex: 3 * vertexId,
            value0: color.r,
            value1: color.g,
            value2: color.b
        });
    }

    _uncolorVertex(elementColors, vertexId) {
        delete elementColors[vertexId];
    }

    _colorEdge(edge, color) {
        this._colorVertex(this._edgeColors, edge.a, color);
        this._colorVertex(this._edgeColors, edge.b, color);
    }

    _uncolorEdge(edge) {
        this._uncolorVertex(this._edgeColors, edge.a);
        this._uncolorVertex(this._edgeColors, edge.b);
    }

    _colorFace(face, color) {
        this._colorVertex(this._vertexColors, face.a, color);
        this._colorVertex(this._vertexColors, face.b, color);
        this._colorVertex(this._vertexColors, face.c, color);
    }

    _uncolorFace(face) {
        this._uncolorVertex(this._vertexColors, face.a);
        this._uncolorVertex(this._vertexColors, face.b);
        this._uncolorVertex(this._vertexColors, face.c);
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

    _setHighlightedEdge(edge) {
        this._highlightedEdge = edge;
    }

    _resetHighlightedEdge(edge) {
        this._highlightedEdge = null;
    }

    _selectFace(face) {
        this._selectedFaces[face.id] = face;
    }

    _unselectFace(face) {
        delete this._selectedFaces[face.id];
    }

    _selectEdge(edge) {
        this._selectedEdges[edge.id] = edge;
    }

    _unselectEdge(edge) {
        delete this._selectedEdges[edge.id];
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

    _colorEdges() {
        for (let edgeId in this._selectedEdges) {
            this._colorEdge(this._selectedEdges[edgeId], this._selectedEdgeColor);
        }

        if (this._highlightedEdge !== null) {
            this._colorEdge(this._highlightedEdge, this._highlightedEdgeColor);
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

    _updateEdgeColors() {
        this._edgeColors = [];
        this._colorEdges();
    }

    /**
     * Scene manipulation
     */
    _overrideAttributeArray(entriesArray, attributeArray) {
        let entriesArrayLength = entriesArray.length;
        for (let i = 0; i < entriesArrayLength; i++) {
            let baseIndex = entriesArray[i].baseIndex;
            attributeArray[baseIndex] = entriesArray[i].value0;
            attributeArray[baseIndex + 1] = entriesArray[i].value1;
            attributeArray[baseIndex + 2] = entriesArray[i].value2;
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
        /**
         * Update debug data
         */
        // this._updateDebugData();

        /**
         * Handle window reesize
         */
        if(this._needResize) {
            this._resizeScene();
            this._needResize = false;
        }

        /**
         * Place the light at the camera position
         */
        this._pointLight.position.copy(this._camera.position);

        /**
         * Handle intersections
         */
        // if (this._interactionService.state.value !== 'faceDragging' && this._mouseInCanvas) {
        //     this._faceIntersection = this._getFaceIntersection();
        //     this._edgeIntersection = null;
        //     this._edgeIntersections = this._getEdgeIntersections();
        //     this._vertexIntersection = this._getVertexIntersection();

        //     // Give priority to edge intersections
        //     if(this._edgeIntersections.length > 0) {
        //         let adjacencyList;
        //         if(this._faceIntersection) {
        //             let faceIndex = this._faceIntersection.faceIndex;
        //             adjacencyList = this.meshProvider.faceEdgeAdjacency[faceIndex];
        //         }
        //         else {
        //             let edgeIntersection = this._edgeIntersections[0];
        //             let intersectedEdgeIndex = edgeIntersection.index / 2;
        //             adjacencyList = [intersectedEdgeIndex];
        //         }
       
        //         if(adjacencyList) {
        //             for(let i = 0; i < this._edgeIntersections.length; i++) {
        //                 let edgeIntersection = this._edgeIntersections[i];
        //                 let intersectedEdgeIndex = edgeIntersection.index / 2;
        //                 let baseEntryIndex = 2 * intersectedEdgeIndex;
        //                 let intersectedEdge = {
        //                     id: intersectedEdgeIndex,
        //                     a: baseEntryIndex,
        //                     b: baseEntryIndex + 1
        //                 };

        //                 if(adjacencyList.includes(intersectedEdgeIndex)) {
        //                     this._resetHighlightedFace();
        //                     this._setHighlightedEdge(intersectedEdge);
        //                     this._edgeIntersection = {
        //                         edge: intersectedEdge
        //                     };
        //                     break;
        //                 }
        //             }
        //         }
        //     }
        //     else if (this._faceIntersection) {
        //         this._resetHighlightedEdge();
        //         this._setHighlightedFace(this._faceIntersection.face);
        //         this._publishFaceMessage('mesh-view-face-highlighted', this._faceIntersection.face);  
        //     } else {
        //         this._resetHighlightedFace();
        //         this._resetHighlightedEdge();
        //         this._publishFaceMessage('mesh-view-face-unhighlighted');        
        //     }
        // }

        /**
         * Update mesh attributes (position, uv, color)
         */
        this._mesh.geometry.attributes.position.array = this.meshProvider.getBufferedVertices(BufferedPrimitiveType.TRIANGLE);
        // this._mesh.geometry.attributes.uv.array = this.meshProvider.getBufferedUvs();
        let bufferedVertexColors = this.meshProvider.getBufferedVertexColors();
        this._updateVertexColors();
        this._overrideAttributeArray(this._vertexColors, bufferedVertexColors);
        this._mesh.geometry.attributes.color.array = bufferedVertexColors;

        /**
         * Update mesh edges attributes
         */
        // this._meshEdges.geometry.setPositions(this.meshProvider.getBufferedVertices(BufferedPrimitiveType.EDGE));
        // let bufferedEdgeColors = this.meshProvider.getBufferedEdgeColors();
        // this._updateEdgeColors();
        // this._overrideAttributeArray(this._edgeColors, bufferedEdgeColors);
        // this._meshEdges.geometry.setColors(bufferedEdgeColors);
        // this._meshEdgesHidden.geometry.attributes.position.array = this.meshProvider.getBufferedVertices(BufferedPrimitiveType.EDGE);

        /**
         * Apply transformations to underlying geometry
         */
        this._mesh.geometry.applyMatrix(this._mesh.matrix);
        // this._meshEdges.geometry.applyMatrix(this._mesh.matrix);
        // this._meshEdgesHidden.geometry.applyMatrix(this._mesh.matrix);
        // this._pointcloud.geometry.applyMatrix(this._pointcloud.matrix);

        /**
         * Set needsUpdate flag
         */
        this._mesh.geometry.attributes.position.needsUpdate = true;
        // this._mesh.geometry.attributes.uv.needsUpdate = true;
        this._mesh.geometry.attributes.color.needsUpdate = true;
        // this._meshEdges.geometry.attributes.position.needsUpdate = true;
        // this._meshEdges.geometry.attributes.instanceColorStart.needsUpdate = true;
        // this._meshEdges.geometry.attributes.instanceColorEnd.needsUpdate = true;
        // this._meshEdgesHidden.geometry.attributes.position.needsUpdate = true;
        // this._pointcloud.geometry.attributes.position.needsUpdate = true;
        
        /**
         * Remove additional scene objects
         */
        // if(this._additionalSceneObjects) {
        //     for(let i = 0; i < this._additionalSceneObjects.length; i++) {
        //         this._scene.remove(this._additionalSceneObjects[i]);
        //     }
        // }

        /**
         * Add additional scene objects
         */
        // this._additionalSceneObjects = this._meshProvider.getAdditionalSceneObjects();
        // for(let i = 0; i < this._additionalSceneObjects.length; i++) {
        //     this._scene.add(this._additionalSceneObjects[i]);
        // }        

        /**
         * Render scene and request another animation frame
         */
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
        this._faceIntersection.previousOffset = new THREE.Vector3(0, 0, 0);
        this._setDraggedFace(this._faceIntersection.face);
        this._publishFaceMessage('mesh-view-face-dragging-begin', this._draggedFace);        
        this._controls.enablePan = false;
    }

    endFaceDragging() {
        this._publishFaceMessage('mesh-view-face-dragging-end', this._draggedFace);
        this._resetDraggedFace();
        this._controls.enablePan = true;
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

    _publishEdgeMessage(message, edge, options) {
        let payload = {
            meshViewId: this.id,
            ...options
        }

        if(edge !== null) {
            payload = {
                ...payload,
                edge: edge,
                edgeSelected: Boolean(this._selectedEdges[edge.id])  
            }
        }

        PubSub.publish(message, payload);         
    }

    /**
     * Debug data
     */
    _updateDebugData() {
        this.debugData = this.meshProvider.debugData;
    }

}

customElements.define('mesh-view', MeshView);