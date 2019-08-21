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
            // meshProvider: {
            //     type: Object,
            //     attribute: 'mesh-provider'
            // },
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

    firstUpdated() {
        // super.connectedCallback();
        // this._renderer.domElement.onmousedown = this._mouseDownHandler.bind(this);
        // this._renderer.domElement.onmousemove = this._mouseMoveHandler.bind(this);
        // this._renderer.domElement.onmouseleave = this._mouseLeaveHandler.bind(this);
        // this._renderer.domElement.onmouseenter = this._mouseEnterHandler.bind(this);
        // this._renderer.domElement.onmouseup = this._mouseUpHandler.bind(this);
        // this._renderer.domElement.onclick = this._mouseClickHandler.bind(this);
        // this._renderer.domElement.oncontextmenu = this._contextMenuHandler.bind(this);
        // window.addEventListener('keydown', this._keydown.bind(this));
        // window.addEventListener('keyup', this._keyup.bind(this));

        window.setTimeout(() => {
            this.shadowRoot.querySelector('.container').appendChild(this._renderer.domElement);
            this._resizeScene();
            this._renderer.render(this._scene, this._camera);
        }, 0);


        
        // this.stats = new Stats();
        // this.stats.showPanel(1);
        // this.stats.dom.style.position = 'absolute';
        // this.stats.dom.style.top = '';
        // this.stats.dom.style.bottom = '0px';
        // this.root.querySelector('.container').appendChild(this.stats.dom);
    }

    /**
     * Properties
     */

    set backgroundColor(value) {
        const oldValue = this._backgroundColor;
        this._backgroundColor = value;
        this._renderer.setClearColor(new THREE.Color(value), 1.0);
        // this._renderer.render(this._scene, this._camera);
        this.requestUpdate('backgroundColor', oldValue);
    }

    get backgroundColor() {
        return this._backgroundColor;
    }

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

    _connectScene() {
    //   this._renderer.domElement.onmousedown = this._mouseDownHandler.bind(this);
    //   this._renderer.domElement.onmousemove = this._mouseMoveHandler.bind(this);
    //   this._renderer.domElement.onmouseleave = this._mouseLeaveHandler.bind(this);
    //   this._renderer.domElement.onmouseenter = this._mouseEnterHandler.bind(this);
    //   this._renderer.domElement.onmouseup = this._mouseUpHandler.bind(this);
    //   this._renderer.domElement.onclick = this._mouseClickHandler.bind(this);
    //   this._renderer.domElement.oncontextmenu = this._contextMenuHandler.bind(this);
    //   window.addEventListener('keydown', this._keydown.bind(this));
    //   window.addEventListener('keyup', this._keyup.bind(this));

    //   this.root.querySelector('.container').appendChild(this._renderer.domElement);
    //   this.stats = new Stats();
    //   this.stats.showPanel(1);
    //   this.stats.dom.style.position = 'absolute';
    //   this.stats.dom.style.top = '';
    //   this.stats.dom.style.bottom = '0px';
    //   this.root.querySelector('.container').appendChild(this.stats.dom);
    }

    _resizeScene() {
        this._camera.aspect = this.offsetWidth / this.offsetHeight;
        this._camera.updateProjectionMatrix();
        this._renderer.setSize(this.offsetWidth, this.offsetHeight);
    }







    // ready() {

    //   super.ready();
    // }

    // _updateDebugData() {
    //   while (this._debugData.length !== 0) {
    //     this.splice('_debugData', 0, 1);
    //   }

    //   let currentDebugData = this.meshProvider.debugData;
    //   if (currentDebugData) {
    //     for (let i = 0; i < currentDebugData.length; i++) {
    //       this.push('_debugData', currentDebugData[i]);
    //     }
    //   }
    // }

    // _showWireframeChanged(showWireframe) {
    //   if (this._meshWireframe) {
    //     this._meshWireframe.visible = showWireframe;
    //   }
    // }

    // _showGridChanged(showGrid) {
    //   if (this._grid) {
    //     this._grid.visible = showGrid;
    //   }
    // }

    // _showGridTextureChanged(showGridTexture) {
    //   if (showGridTexture) {
    //     this._mesh.material.map = this._gridTexture;
    //   } else {
    //     this._mesh.material.map = null;;
    //   }

    //   this._mesh.material.needsUpdate = true;
    // }

    // _syncFaceSelectionChanged() {

    // }

    // _syncVertexSelectionChanged() {

    // }

    // _enableVertexSelectionChanged() {

    // }

    // _enableMeshRotationChanged() {

    // }

    // _getBufferedUvs(bufferedVertices) {
    //   let bufferedUvs = [];
    //   let bufferedVerticesLength = bufferedVertices.length;
    //   for (let i = 0; i < bufferedVerticesLength; i++) {
    //     if (i % 3 !== 2) {
    //       bufferedUvs.push(bufferedVertices[i]);
    //     }
    //   }

    //   return bufferedUvs;
    // }

    // _getBufferedColors(verticesLength) {
    //   let THREE = require('three');
    //   let bufferedColors = [];

    //   for (let i = 0; i < verticesLength; i++) {
    //     if (this._vertexColors[i]) {
    //       bufferedColors.push(this._vertexColors[i].r);
    //       bufferedColors.push(this._vertexColors[i].g);
    //       bufferedColors.push(this._vertexColors[i].b);
    //     } else {
    //       let color = new THREE.Color(this.meshColor);
    //       bufferedColors.push(color.r);
    //       bufferedColors.push(color.g);
    //       bufferedColors.push(color.b);
    //     }
    //   }

    //   return bufferedColors;
    // }

    // _reinitializeMeshTexture() {
    //   let THREE = require('three');
    //   this._initializeGridTexture(this.gridTextureSize, this.gridSize, this.gridLineWidth, new THREE.Color(this.gridBackgroundColor1), new THREE.Color(this.gridBackgroundColor2), new THREE.Color(this.gridHorizontalColor), new THREE.Color(this.gridVerticalColor), new THREE.Color(0, 1, 0));

    //   if (this.showGridTexture) {
    //     this._mesh.material.map = this._gridTexture;
    //   }
    //   this._mesh.material.needsUpdate = true;
    // }

    // _initializeLights() {
    //   let THREE = require('three');
    //   let ambientLight = new THREE.AmbientLight(0xffffff, 0.2);
    //   this._light1 = new THREE.PointLight(0xffffff, 0.8, 0);

    //   if (this.useLights) {
    //     this._scene.add(ambientLight);
    //     this._scene.add(this._light1);
    //   }
    // }

    // _initializeMesh() {
    //   let THREE = require('three');
    //   let geometry = new THREE.BufferGeometry();
    //   let bufferedMeshVertices = this.meshProvider.bufferedMeshVertices;
    //   let bufferedMeshUvs = this._getBufferedUvs(bufferedMeshVertices);
    //   let color = new THREE.Color(this.meshColor);
    //   let bufferedMeshColors = this._getBufferedColors(bufferedMeshVertices.length / 3);

    //   geometry.addAttribute('position', new THREE.BufferAttribute(new Float32Array(bufferedMeshVertices), 3));
    //   geometry.addAttribute('uv', new THREE.BufferAttribute(new Float32Array(bufferedMeshUvs), 2));
    //   geometry.addAttribute('color', new THREE.BufferAttribute(new Float32Array(bufferedMeshColors), 3, true));

    //   let material;
    //   if (this.useLights) {
    //     material = new THREE.MeshPhongMaterial({
    //       vertexColors: THREE.VertexColors
    //     });
    //   } else {
    //     material = new THREE.MeshBasicMaterial({
    //       vertexColors: THREE.VertexColors
    //     });
    //   }

    //   material.color.setHex(color.getHex());
    //   if (this.showGridTexture) {
    //     material.map = this._gridTexture;
    //   }


    //   geometry.computeVertexNormals();

    //   this._mesh = new THREE.Mesh(geometry, material);
    //   this._scene.add(this._mesh);
    // }

    // _initializeMeshWireframe() {
    //   let THREE = require('three');
    //   let material = new THREE.MeshBasicMaterial({
    //     color: 0x000000,
    //     wireframe: true
    //   });
    //   this._meshWireframe = new this.THREE.Mesh(this._mesh.geometry, material);
    //   this._meshWireframe.renderOrder = 1;
    //   this._scene.add(this._meshWireframe);
    // }

    // _getRGBAString(color) {
    //   let r = Math.floor(color.r * 255);
    //   let g = Math.floor(color.g * 255);
    //   let b = Math.floor(color.b * 255);
    //   return "rgba(" + r + "," + g + "," + b + "," + 1 + ")";
    // }

    // _getGridCanvas(textureSize, gridSize, lineWidth, backgroundColor1, backgroundColor2, horizontalColor, verticalColor, pointColor) {
    //   let THREE = require('three');
    //   lineWidth = 2 * lineWidth + 1;
    //   gridSize = Math.pow(2, gridSize);
    //   textureSize = Math.pow(2, textureSize);

    //   let canvas = document.createElement("canvas");
    //   let context = canvas.getContext("2d");
    //   canvas.width = textureSize;
    //   canvas.height = textureSize;

    //   let setColor = (x, y, color) => {
    //     context.fillStyle = this._getRGBAString(color);
    //     context.fillRect(x, y, 1, 1);
    //   }

    //   let paintHorizontalLine = (y) => {
    //     for (let x = 0; x < textureSize; x++) {
    //       setColor(x, y, horizontalColor);
    //     }
    //   }

    //   let paintVerticalLine = (x) => {
    //     for (let y = 0; y < textureSize; y++) {
    //       setColor(x, y, verticalColor);
    //     }
    //   }

    //   // context.fillStyle = this._getRGBAString(backgroundColor);
    //   // context.fillRect(0, 0, textureSize, textureSize);

    //   let delta = textureSize / gridSize;
    //   let half = textureSize / 2;
    //   // for (let x = 0; x <= textureSize; x++) {
    //   //   for (let y = 0; y <= textureSize; y++) {
    //   //     if (x <= half && y < half) {
    //   //       setColor(x, y, new THREE.Color("rgb(101, 133, 255)"));
    //   //     } else if (x > half && y < half) {
    //   //       setColor(x, y, new THREE.Color("rgb(255, 253, 184)"));
    //   //     } else if (x <= half && y >= half) {
    //   //       setColor(x, y, new THREE.Color("rgb(255, 253, 184)"));
    //   //     } else if (x > half && y >= half) {
    //   //       setColor(x, y, new THREE.Color("rgb(101, 133, 255)"));
    //   //     }
    //   //   }
    //   // }


    //   for (let i = 0; i < delta; i++) {
    //     for (let j = 0; j < delta; j++) {

    //       let xStart = i * gridSize;
    //       let yStart = j * gridSize;

    //       let xEnd = (i + 1) * gridSize;
    //       let yEnd = (j + 1) * gridSize;

    //       if (i % 2 === 0 && j % 2 === 0) {

    //         for (let x = xStart; x <= xEnd; x++) {
    //           for (let y = yStart; y <= yEnd; y++) {
    //             setColor(x, y, new THREE.Color(backgroundColor1));
    //           }
    //         }

    //       } else if (i % 2 === 1 && j % 2 === 0) {

    //         for (let x = xStart; x <= xEnd; x++) {
    //           for (let y = yStart; y <= yEnd; y++) {
    //             setColor(x, y, new THREE.Color(backgroundColor2));
    //           }
    //         }

    //       } else if (i % 2 === 0 && j % 2 === 1) {

    //         for (let x = xStart; x <= xEnd; x++) {
    //           for (let y = yStart; y <= yEnd; y++) {
    //             setColor(x, y, new THREE.Color(backgroundColor2));
    //           }
    //         }

    //       } else if (i % 2 === 1 && j % 2 === 1) {

    //         for (let x = xStart; x <= xEnd; x++) {
    //           for (let y = yStart; y <= yEnd; y++) {
    //             setColor(x, y, new THREE.Color(backgroundColor1));
    //           }
    //         }

    //       }
    //     }
    //   }




    //   let halfLineWidth = Math.floor(lineWidth / 2);
    //   for (let i = 0; i <= delta; i++) {
    //     if (i == 0) {
    //       for (let y = 0; y < (halfLineWidth + 1); y++) {
    //         paintHorizontalLine(y);
    //       }
    //     } else if (i == delta) {
    //       for (let y = textureSize - halfLineWidth; y < textureSize; y++) {
    //         paintHorizontalLine(y);
    //       }
    //     } else {
    //       let xCenter = i * gridSize;
    //       for (let y = xCenter - halfLineWidth; y < xCenter + halfLineWidth + 1; y++) {
    //         paintHorizontalLine(y);
    //       }
    //     }
    //   }

    //   for (let i = 0; i <= delta; i++) {
    //     if (i == 0) {
    //       for (let x = 0; x < (halfLineWidth + 1); x++) {
    //         paintVerticalLine(x);
    //       }
    //     } else if (i == delta) {
    //       for (let x = textureSize - halfLineWidth; x < textureSize; x++) {
    //         paintVerticalLine(x);
    //       }
    //     } else {
    //       let xCenter = i * gridSize;
    //       for (let x = xCenter - halfLineWidth; x < xCenter + halfLineWidth + 1; x++) {
    //         paintVerticalLine(x);
    //       }
    //     }
    //   }

    //   return canvas;
    // }

    // _initializeGridTexture(textureSize, gridSize, lineWidth, backgroundColor1, backgroundColor2, horizontalColor, verticalColor, pointColor) {
    //   let THREE = require('three');
    //   let canvas = this._getGridCanvas(textureSize, gridSize, lineWidth, backgroundColor1, backgroundColor2, horizontalColor, verticalColor, pointColor);
    //   this._gridTexture = new THREE.CanvasTexture(canvas);
    //   this._gridTexture.wrapS = THREE.RepeatWrapping;
    //   this._gridTexture.wrapT = THREE.RepeatWrapping;
    //   this._gridTexture.needsUpdate = true
    // }

    // _initializeTextures() {
    //   let THREE = require('three');
    //   this._initializeGridTexture(this.gridTextureSize, this.gridSize, this.gridLineWidth, new THREE.Color(this.gridBackgroundColor1), new THREE.Color(this.gridBackgroundColor2), new THREE.Color(this.gridHorizontalColor), new THREE.Color(this.gridVerticalColor), new THREE.Color(0, 1, 0));
    // }

    // _initializeHoveredVertexPoint() {
    //   let THREE = require('three');
    //   let geometry = new THREE.BufferGeometry();
    //   geometry.addAttribute('position', new THREE.BufferAttribute(new Float32Array(3), 3));
    //   let colors = new Float32Array(3);
    //   geometry.addAttribute('customColor', new THREE.BufferAttribute(colors, 3));

    //   let fs = require('fs');
    //   let vertexshader = fs.readFileSync(__dirname + '/shaders/vertexshader.glsl', 'utf8');
    //   let fragmentshader = fs.readFileSync(__dirname + '/shaders/fragmentshader.glsl', 'utf8');

    //   let material = new THREE.ShaderMaterial({
    //     uniforms: {
    //       color: {
    //         value: new THREE.Color(0x000000)
    //       },
    //       textureSelected: {
    //         value: new THREE.TextureLoader().load("./textures/selected_16_16.png")
    //       },
    //       textureUnselected: {
    //         value: new THREE.TextureLoader().load("./textures/unselected_16_16.png")
    //       }
    //     },
    //     vertexShader: vertexshader,
    //     fragmentShader: fragmentshader,
    //     alphaTest: 0.9,
    //     depthTest: false
    //   });

    //   this._hoveredVertexPoint = new THREE.Points(geometry, material);
    //   this._scene.add(this._hoveredVertexPoint);
    // }

    // _initializeVertexSelectionGroups() {
    //   let THREE = require('three');
    //   let vertices = this.meshProvider.vertices;
    //   for (let i = 0; i < vertices.length; i++) {
    //     let currentVertex = vertices[i];
    //     this._selectedVerticesPoints[i] = {
    //       vertexId: i,
    //       vertex: new THREE.Vector3(currentVertex.x, currentVertex.y, currentVertex.z)
    //     }
    //   }
    // }

    // _initializePointcloud() {
    //   let THREE = require('three');
    //   let geometry = new THREE.BufferGeometry();
    //   geometry.addAttribute('position', new THREE.BufferAttribute(new Float32Array(this.meshProvider.bufferedVertices),
    //     3));

    //   let vertices = this.meshProvider.vertices;
    //   let sizes = new Float32Array(vertices.length);
    //   let selected = new Float32Array(vertices.length);

    //   for (let i = 0; i < vertices.length; i++) {
    //     sizes[i] = this._vertexSize;
    //     selected[i] = this._selectedVerticesPoints[i] !== undefined;
    //   }

    //   geometry.addAttribute('size', new THREE.BufferAttribute(sizes, 1));
    //   geometry.addAttribute('selected', new THREE.BufferAttribute(selected, 1));

    //   let fs = require('fs');
    //   let vertexshader = fs.readFileSync(__dirname + '/shaders/vertexshader.glsl', 'utf8');
    //   let fragmentshader = fs.readFileSync(__dirname + '/shaders/fragmentshader.glsl', 'utf8');

    //   let material = new THREE.ShaderMaterial({
    //     uniforms: {
    //       color: {
    //         value: new THREE.Color(0xff0000)
    //       },
    //       textureSelected: {
    //         value: new THREE.TextureLoader().load("./textures/selected_16_16.png")
    //       },
    //       textureUnselected: {
    //         value: new THREE.TextureLoader().load("./textures/unselected_16_16.png")
    //       }
    //     },
    //     vertexShader: vertexshader,
    //     fragmentShader: fragmentshader,
    //     alphaTest: 0.5,
    //     depthTest: false
    //   });

    //   this._pointcloud = new THREE.Points(geometry, material);
    //   this._scene.add(this._pointcloud);
    //   this._pointcloud.visible = false;
    // }

    // _initializeGrid() {
    //   let THREE = require('three');
    //   let size = 500;
    //   let divisions = 500;
    //   this._grid = new THREE.GridHelper(size, divisions);
    //   this._grid.geometry.rotateX(Math.PI / 2);
    //   this._grid.visible = this.showGrid;
    //   this._scene.add(this._grid);
    // }

    // _meshProviderChanged(meshProvider) {
    //   this._clearSceneSubtree(this._scene);
    //   this._initializeVertexSelectionGroups();
    //   this._initializeTextures();
    //   this._initializeLights();
    //   this._initializeMesh();
    //   this._initializeMeshWireframe();
    //   this._initializeHoveredVertexPoint();
    //   this._initializePointcloud();
    //   this._initializeGrid();
    // }

    // _backgroundColorChanged(color) {
    //   let THREE = require('three');
    //   this._renderer.setClearColor(new THREE.Color(color), 1.0);
    // }

    // _meshColorChanged(color) {
    //   let THREE = require('three');
    //   this._mesh.material.color.setHex(new THREE.Color(color).getHex());
    // }

    // _clearSceneSubtree(object) {
    //   while (object.children.length > 0) {
    //     this._clearSceneSubtree(object.children[0])
    //     object.remove(object.children[0]);
    //   }

    //   if (object.geometry) {
    //     object.geometry.dispose()
    //   }

    //   if (object.material) {
    //     object.material.dispose()
    //   }

    //   if (object.texture) {
    //     object.texture.dispose()
    //   }
    // }

    // _initializeStateMachine() {
    //   this._fsm = new StateMachine({
    //     init: 'idle',
    //     transitions: [{
    //         name: 'begin-vertex-selection',
    //         from: 'idle',
    //         to: 'vertex-selection'
    //       },
    //       {
    //         name: 'begin-vertex-selection',
    //         from: 'vertex-selection',
    //         to: 'vertex-selection'
    //       },
    //       {
    //         name: 'begin-face-selection',
    //         from: 'idle',
    //         to: 'face-selection'
    //       },
    //       {
    //         name: 'begin-face-selection',
    //         from: 'face-selection',
    //         to: 'face-selection'
    //       },
    //       {
    //         name: 'begin-face-dragging',
    //         from: 'idle',
    //         to: 'face-dragging'
    //       },
    //       {
    //         name: 'end-vertex-selection',
    //         from: 'vertex-selection',
    //         to: 'idle'
    //       },
    //       {
    //         name: 'end-face-selection',
    //         from: 'face-selection',
    //         to: 'idle'
    //       },
    //       {
    //         name: 'end-face-dragging',
    //         from: 'face-dragging',
    //         to: 'idle'
    //       },
    //       {
    //         name: 'begin-mesh-rotation',
    //         from: 'idle',
    //         to: 'mesh-rotation'
    //       },
    //       {
    //         name: 'begin-mesh-rotation',
    //         from: 'mesh-rotation',
    //         to: 'mesh-rotation'
    //       },
    //       {
    //         name: 'end-mesh-rotation',
    //         from: 'mesh-rotation',
    //         to: 'idle'
    //       },
    //     ],
    //     methods: {
    //       onBeginVertexSelection: () => {
    //         this._pointcloud.visible = true;
    //       },
    //       onEndVertexSelection: () => {
    //         this._pointcloud.visible = false;
    //       },
    //       onBeginMeshRotation: () => {
    //         this._controls.enableRotate = true;
    //       },
    //       onEndMeshRotation: () => {
    //         this._controls.enableRotate = false;
    //       },
    //       onBeginFaceDragging: () => {
    //         let THREE = require('three');
    //         this._setDraggedFace(this._faceIntersection.face);
    //         require('pubsub-js').publish('mesh-view-face-dragging-begin', {
    //           face: this._draggedFace
    //         });
    //         this._controls.enablePan = false;
    //       },
    //       onEndFaceDragging: () => {
    //         let THREE = require('three');
    //         require('pubsub-js').publish('mesh-view-face-dragging-end', {
    //           face: this._draggedFace
    //         });
    //         this._resetDraggedFace();
    //         this._controls.enablePan = true;
    //       }
    //     }
    //   });
    // }

    // _initialize() {
    //   let THREE = require('three');
    //   this._initializeStateMachine();
    //   this._createCamera();
    //   this._createRenderer();
    //   this._createMaterials();
    //   this._createScene();
    //   this._createOrbitControl();
    //   this._createRaycaster();
    //   this._faceIntersection = null;
    //   this._previousFaceIntersection = null;
    //   this._selectedFaces = {};
    //   this._selectedVerticesPoints = {};
    //   this._draggedFace = null;
    //   this._highlightedFace = null;
    //   this._hoveredVertexSize = 20;
    //   this._vertexSize = 15;
    //   this._vertexColors = [];
    //   // this._highlightedFaceColor = new THREE.Color('rgb(200,100,0)');
    //   // this._selectedFaceColor = new THREE.Color('rgb(0,255,0)');
    //   // this._draggedFaceColor = new THREE.Color('rgb(0,0,255)');
    //   this._mouseInCanvas = false;
    // }

    // _createCamera() {
    //   let THREE = require('three');
    //   this._camera = new THREE.PerspectiveCamera(45, 0, 0.1, 10000);
    //   this._camera.position.x = 0;
    //   this._camera.position.y = 0;
    //   this._camera.position.z = 3;
    // }

    // _createRenderer() {
    //   let THREE = require('three');
    //   this._renderer = new THREE.WebGLRenderer({
    //     antialias: true
    //   });
    //   this._renderer.setPixelRatio(window.devicePixelRatio);
    // }

    // _createMaterials() {
    //   let THREE = require('three');
    //   this._sphereMaterial = new THREE.MeshBasicMaterial({
    //     color: 0xff0000
    //   });
    // }

    // _createRaycaster() {
    //   let THREE = require('three');
    //   this._raycaster = new THREE.Raycaster();
    //   this._raycaster.params.Points.threshold = 0.05;
    // }

    // _createScene() {
    //   let THREE = require('three');
    //   this._scene = new THREE.Scene();
    // }

    // _createOrbitControl() {
    //   let THREE = require('three');
    //   let OrbitControls = require('three-orbit-controls')(THREE);
    //   this._controls = new OrbitControls(this._camera, this._renderer.domElement);
    //   this._controls.enableDamping = true;
    //   this._controls.dampingFactor = 0.25;
    //   this._controls.screenSpacePanning = false;
    //   this._controls.enableKeys = false;
    //   this._controls.minDistance = 0;
    //   this._controls.maxDistance = 1000;
    //   this._controls.maxPolarAngle = 2 * Math.PI;
    //   this._controls.enableRotate = false;
    // }

    // _resizeScene() {
    //   let THREE = require('three');
    //   this._camera.aspect = this.offsetWidth / (this.offsetHeight);
    //   this._camera.updateProjectionMatrix();
    //   this._renderer.setSize(this.offsetWidth, this.offsetHeight);
    // }

    // _resetAttributeArray(buffer, attributeArray) {
    //   let bufferLength = buffer.length;
    //   for (let i = 0; i < bufferLength; i++) {
    //     attributeArray[i] = buffer[i];
    //   }
    // }

    // _colorFaces() {
    //   for (let faceId in this._selectedFaces) {
    //     this._colorFace(this._selectedFaces[faceId], this._selectedFaceColor);
    //   }

    //   if (this._highlightedFace) {
    //     this._colorFace(this._highlightedFace, this._highlightedFaceColor);
    //   }

    //   if (this._draggedFace) {
    //     this._colorFace(this._draggedFace, this._draggedFaceColor);
    //   }
    // }

    // _colorVertices() {
    //   let bufferedMeshVertexColors = this.meshProvider.bufferedMeshVertexColors;
    //   for (let vertexId in bufferedMeshVertexColors) {
    //     this._colorVertex(vertexId, bufferedMeshVertexColors[vertexId]);
    //   }
    // }

    // _updateVertexColors() {
    //   this._vertexColors = [];
    //   this._colorVertices();
    //   this._colorFaces();
    // }

    // _renderScene() {
    //   this._light1.position.copy(this._camera.position);

    //   this.stats.begin();
    //   let THREE = require('three');

    //   if (this._fsm.state !== 'face-dragging' && this._mouseInCanvas) {
    //     this._faceIntersection = this._getMeshIntersection();
    //     this._vertexIntersection = this._getVerticesIntersection();

    //     if (this._faceIntersection) {
    //       this._setHighlightedFace(this._faceIntersection.face);
    //       require('pubsub-js').publish('mesh-view-face-highlighted', {
    //         face: this._faceIntersection.face,
    //         meshViewId: this.id
    //       });
    //     } else {
    //       this._resetHighlightedFace();
    //       require('pubsub-js').publish('mesh-view-face-unhighlighted', {
    //         meshViewId: this.id
    //       });
    //     }
    //   }

    //   let bufferedMeshVertices = this.meshProvider.bufferedMeshVertices;

    //   this._resetAttributeArray(bufferedMeshVertices, this._mesh.geometry.attributes.position.array);
    //   this._resetAttributeArray(this.meshProvider.bufferedVertices, this._pointcloud.geometry.attributes.position.array);
    //   this._resetAttributeArray(this.meshProvider.bufferedMeshUvs, this._mesh.geometry.attributes.uv.array);

    //   this._updateVertexColors();
    //   this._resetAttributeArray(this._getBufferedColors(bufferedMeshVertices.length / 3), this._mesh.geometry.attributes.color.array);

    //   this._mesh.geometry.applyMatrix(this._mesh.matrix);
    //   this._pointcloud.geometry.applyMatrix(this._pointcloud.matrix);

    //   if (this._fsm.state === 'vertex-selection') {
    //     if (this._vertexIntersection) {
    //       if (this._previousVertexIntersectionIndex !== null) {
    //         this._pointcloud.geometry.attributes.size.array[this._previousVertexIntersectionIndex] = this._vertexSize;
    //       }
    //       this._pointcloud.geometry.attributes.size.array[this._vertexIntersection.vertexId] = this._hoveredVertexSize;
    //       this._previousVertexIntersectionIndex = this._vertexIntersection.vertexId;
    //       this._pointcloud.geometry.attributes.size.needsUpdate = true;
    //     } else {
    //       if (this._previousVertexIntersectionIndex !== null) {
    //         this._pointcloud.geometry.attributes.size.array[this._previousVertexIntersectionIndex] = this._vertexSize;
    //         this._pointcloud.geometry.attributes.size.needsUpdate = true;
    //         this._previousVertexIntersectionIndex = null;
    //       }
    //     }
    //   }

    //   this._mesh.geometry.attributes.position.needsUpdate = true;
    //   this._mesh.geometry.attributes.uv.needsUpdate = true;
    //   this._mesh.geometry.attributes.color.needsUpdate = true;
    //   this._pointcloud.geometry.attributes.position.needsUpdate = true;
    //   this._renderer.render(this._scene, this._camera);
    //   this.stats.end();
    //   this.scheduledAnimationFrameId = requestAnimationFrame(() => this._renderScene());

    //   this._updateDebugData();
    // }

    // _connectScene() {
    //   this._renderer.domElement.onmousedown = this._mouseDownHandler.bind(this);
    //   this._renderer.domElement.onmousemove = this._mouseMoveHandler.bind(this);
    //   this._renderer.domElement.onmouseleave = this._mouseLeaveHandler.bind(this);
    //   this._renderer.domElement.onmouseenter = this._mouseEnterHandler.bind(this);
    //   this._renderer.domElement.onmouseup = this._mouseUpHandler.bind(this);
    //   this._renderer.domElement.onclick = this._mouseClickHandler.bind(this);
    //   this._renderer.domElement.oncontextmenu = this._contextMenuHandler.bind(this);
    //   window.addEventListener('keydown', this._keydown.bind(this));
    //   window.addEventListener('keyup', this._keyup.bind(this));

    //   this.root.querySelector('.container').appendChild(this._renderer.domElement);
    //   this.stats = new Stats();
    //   this.stats.showPanel(1);
    //   this.stats.dom.style.position = 'absolute';
    //   this.stats.dom.style.top = '';
    //   this.stats.dom.style.bottom = '0px';
    //   this.root.querySelector('.container').appendChild(this.stats.dom);
    // }

    // _mouseDownHandler(e) {
    //   if (e.button === 0) {
    //     if (this._fsm.state === 'idle' && this.enableFaceDragging) {
    //       if (this._faceIntersection) {
    //         require('pubsub-js').publish('mesh-view-face-down', {
    //           face: this._faceIntersection.face
    //         });
    //       }
    //     }
    //   }
    // }

    // _setRaycasterWithPixel(x, y) {
    //   let THREE = require('three');
    //   let deviceMousePosition = new THREE.Vector2();
    //   let boundingClientRect = this._renderer.domElement.getBoundingClientRect();
    //   deviceMousePosition.x = (x / boundingClientRect.width) * 2 - 1;
    //   deviceMousePosition.y = -(y / boundingClientRect.height) * 2 + 1;
    //   this._raycaster.setFromCamera(deviceMousePosition, this._camera);
    // }

    // _getOffsetFromIntersection() {
    //   let THREE = require('three');
    //   let currentIntersectionPoint = new THREE.Vector3();
    //   let offset = new THREE.Vector3();
    //   if (this._raycaster.ray.intersectPlane(this._faceIntersection.plane, currentIntersectionPoint)) {
    //     offset.subVectors(currentIntersectionPoint, this._faceIntersection.point);
    //   }

    //   return offset;
    // }

    // _getMeshIntersection() {
    //   let THREE = require('three');
    //   let intersections = this._raycaster.intersectObject(this._mesh);
    //   if (intersections.length > 0) {
    //     let intersection = intersections[0];
    //     intersection.plane = new THREE.Plane();
    //     intersection.offset = new THREE.Vector3();
    //     intersection.plane.setFromNormalAndCoplanarPoint(this._camera.getWorldDirection(intersection.plane.normal),
    //       intersection.point);
    //     intersection.face.id = Math.round(intersection.faceIndex / 3);
    //     return intersection;
    //   }

    //   return null;
    // }

    // _getVerticesIntersection() {
    //   let THREE = require('three');
    //   let intersections = this._raycaster.intersectObject(this._pointcloud);
    //   if (intersections.length > 0) {
    //     let intersection = intersections[0];
    //     intersection.vertexId = intersection.index;
    //     return intersection;
    //   }

    //   return null;
    // }

    // _unselectDraggedFace() {
    //   if (this._selectedFaces[this._faceIntersection.face.id]) {
    //     this._unselectFace(this._faceIntersection.face.id);
    //     require('pubsub-js').publish('mesh-view-face-unselected', {
    //       face: this._faceIntersection.face,
    //       meshViewId: this.id
    //     });
    //   }
    // }

    // _updateDragOffset() {
    //   this._faceIntersection.offset = this._getOffsetFromIntersection();
    //   require('pubsub-js').publish('mesh-view-face-dragging', {
    //     face: this._faceIntersection.face,
    //     offset: this._faceIntersection.offset
    //   });
    // }

    // _mouseEnterHandler(e) {
    //   this._mouseInCanvas = true;
    // }

    // _mouseLeaveHandler(e) {
    //   switch (this._fsm.state) {
    //     case 'face-dragging':
    //       this._fsm.endFaceDragging();
    //       break;
    //   }

    //   this._mouseInCanvas = false;
    //   if (this._faceIntersection) {
    //     this._resetHighlightedFace();
    //     require('pubsub-js').publish('mesh-view-face-unhighlighted', {
    //       meshViewId: this.id
    //     });
    //   }

    //   this._faceIntersection = null;
    //   this._vertexIntersection = null;
    // }

    // _mouseMoveHandler(e) {
    //   event.preventDefault();
    //   let THREE = require('three');
    //   this._setRaycasterWithPixel(e.offsetX, e.offsetY);

    //   if (this._fsm.state === 'idle') {
    //     if (this._faceIntersection) {
    //       if (e.buttons === 1 && this.enableFaceDragging) {
    //         this._draggedFace = this._faceIntersection.face;
    //         this._fsm.beginFaceDragging();
    //       }
    //     }
    //   } else if (this._fsm.state === 'face-dragging') {
    //     this._updateDragOffset();
    //   }
    // }

    // _mouseUpHandler(e) {
    //   event.preventDefault();
    //   this._setRaycasterWithPixel(e.offsetX, e.offsetY);
    //   if (e.button === 0) {
    //     switch (this._fsm.state) {
    //       case 'face-dragging':
    //         this._fsm.endFaceDragging();
    //         break;
    //     }
    //   }
    // }

    // _colorVertex(vertex, color) {
    //   this._vertexColors[vertex] = color;
    // }

    // _uncolorVertex(face) {
    //   delete this._vertexColors[vertex];
    // }

    // _colorFace(face, color) {
    //   this._colorVertex(face.a, color);
    //   this._colorVertex(face.b, color);
    //   this._colorVertex(face.c, color);
    // }

    // _uncolorFace(face) {
    //   this._uncolorVertex(face.a);
    //   this._uncolorVertex(face.b);
    //   this._uncolorVertex(face.c);
    // }

    // _setDraggedFace(face) {
    //   this._draggedFace = face;
    // }

    // _resetDraggedFace(face) {
    //   this._draggedFace = null;
    // }

    // _setHighlightedFace(face) {
    //   this._highlightedFace = face;
    // }

    // _resetHighlightedFace(face) {
    //   this._highlightedFace = null;
    // }

    // _selectFace(face) {
    //   this._selectedFaces[face.id] = face;
    // }

    // _unselectFace(face) {
    //   delete this._selectedFaces[face.id];
    // }

    // _selectVertex(vertexId, vertex) {
    //   this._pointcloud.geometry.attributes.selected.array[vertexId] = 1;
    //   this._pointcloud.geometry.attributes.selected.needsUpdate = true;
    //   this._selectedVerticesPoints[vertexId] = {
    //     vertexId: vertexId,
    //     vertex: vertex
    //   }
    // }

    // _unselectVertex(vertexId) {
    //   this._pointcloud.geometry.attributes.selected.array[vertexId] = 0;
    //   this._pointcloud.geometry.attributes.selected.needsUpdate = true;
    //   delete this._selectedVerticesPoints[vertexId];
    // }

    // _mouseClickHandler(e) {
    //   if (this._fsm.state === 'vertex-selection') {
    //     if (this._vertexIntersection) {
    //       if (this._selectedVerticesPoints[this._vertexIntersection.vertexId]) {
    //         this._unselectVertex(this._vertexIntersection.vertexId);
    //         require('pubsub-js').publish('mesh-view-vertex-unselected', {
    //           vertexId: this._vertexIntersection.vertexId,
    //           meshViewId: this.id
    //         });
    //       } else {
    //         this._selectVertex(this._vertexIntersection.vertexId, this._vertexIntersection.vertex);
    //         require('pubsub-js').publish('mesh-view-vertex-selected', {
    //           vertexId: this._vertexIntersection.vertexId,
    //           meshViewId: this.id
    //         });
    //       }
    //     }
    //   } else if (this._fsm.state === 'face-selection') {
    //     if (this._faceIntersection) {
    //       if (this._selectedFaces[this._faceIntersection.face.id]) {
    //         this._unselectFace(this._faceIntersection.face);
    //         require('pubsub-js').publish('mesh-view-face-unselected', {
    //           face: this._faceIntersection.face,
    //           meshViewId: this.id
    //         });
    //       } else {
    //         this._selectFace(this._faceIntersection.face);
    //         require('pubsub-js').publish('mesh-view-face-selected', {
    //           face: this._faceIntersection.face,
    //           meshViewId: this.id
    //         });
    //       }
    //     }
    //   }
    // }

    // _contextMenuHandler(e) {
    //   if (this._fsm.state === 'face-dragging') {
    //     this._selectFace(this._faceIntersection.face);
    //     require('pubsub-js').publish('mesh-view-face-selected', {
    //       face: this._faceIntersection.face,
    //       meshViewId: this.id
    //     });
    //     setTimeout(0, () => {
    //       this._fsm.endFaceDragging();
    //     });
    //   }
    // }

    // _keydown(event) {
    //   if (event.keyCode === 16) {
    //     if (this.enableVertexSelection) {
    //       this._fsm.beginVertexSelection();
    //     }
    //   } else if (event.keyCode === 17) {
    //     if (this.enableMeshRotation) {
    //       this._fsm.onBeginMeshRotation();
    //     }
    //   } else if (event.keyCode === 18) {
    //     this._fsm.beginFaceSelection();
    //   }
    // }

    // _keyup(event) {
    //   if (event.keyCode === 16) {
    //     if (this.enableVertexSelection) {
    //       this._fsm.endVertexSelection();
    //     }
    //   } else if (event.keyCode === 17) {
    //     if (this.enableMeshRotation) {
    //       this._fsm.onEndMeshRotation();
    //     }
    //   } else if (event.keyCode === 18) {
    //     this._fsm.endFaceSelection();
    //   }
    // }

    // _gridHorizontalColorChanged(gridHorizontalColor) {
    //   this._reinitializeMeshTexture();
    // }

    // _gridVerticalColorChanged(gridVerticalColor) {
    //   this._reinitializeMeshTexture();
    // }

    // _gridBackgroundColor1Changed(gridBackgroundColor1) {
    //   this._reinitializeMeshTexture();
    // }

    // _gridBackgroundColor2Changed(gridBackgroundColor2) {
    //   this._reinitializeMeshTexture();
    // }

    // _gridSizeChanged(gridSize) {
    //     this._reinitializeMeshTexture();
    // }

    // _gridTextureSizeChanged(gridTextureSize) {
    //     this._reinitializeMeshTexture();
    // }

    // _gridLineWidthChanged(gridLineWidth) {
    //     this._reinitializeMeshTexture();
    // }

    // _highlightedColorChanged() {
    //     let THREE = require('three');
    //     this._highlightedFaceColor = new THREE.Color(this.highlightedFaceColor);
    // }

    // _draggedFaceColorChanged() {
    //     let THREE = require('three');
    //     this._draggedFaceColor = new THREE.Color(this.draggedFaceColor);
    // }

    // _selectedFaceColorChanged() {
    //     let THREE = require('three');
    //     this._selectedFaceColor = new THREE.Color(this.selectedFaceColor);
    // }    
}

customElements.define('mesh-view', MeshView);