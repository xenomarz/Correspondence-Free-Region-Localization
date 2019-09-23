// Web Modules Imports
import { LitElement, html, css } from '../../web_modules/lit-element.js';
import '../../web_modules/@vaadin/vaadin-button.js';
import '../../web_modules/@vaadin/vaadin-split-layout.js';
import { PubSub } from '../../web_modules/pubsub-js.js';
import * as THREE from '../../web_modules/three.js';
import { connect } from '../../web_modules/pwa-helpers.js';

// Components Imports
import '../mesh-view/mesh-view.js';
import '../autoquads-side-bar/autoquads-side-bar.js';
import { MeshProvider } from '../mesh-provider/mesh-provider.js';
import { AutoquadsModelMeshProvider } from '../autoquads-model-mesh-provider/autoquads-model-mesh-provider.js';
import { AutoquadsSoupMeshProvider } from '../autoquads-soup-mesh-provider/autoquads-soup-mesh-provider.js';
import * as ReducerExports from '../../redux/reducer.js';
import * as ActionsExports from '../../redux/actions.js';
import * as EnumsExports from '../../redux/enums.js';
import * as HelpersExports from '../../redux/helpers.js';
import { store } from '../../redux/store.js';

export class AutoquadsView extends connect(store)(LitElement) {
    static get styles() {
        return [css`
            :host {
                display: flex;
                width: 100%;
                height: 100%;
            }

            .outer-split {
                flex: 1;
            }

            .inner-split {
                width: 100%;
                height: 100%;
            }

            /* https://stackoverflow.com/questions/41800823/hide-polymer-element */
            /* [hidden] {
                display: none;
            } */
        `];
    }

    render() {
        return html`  
            <vaadin-split-layout class="outer-split">
                <autoquads-side-bar></autoquads-side-bar>
                <vaadin-split-layout orientation="horizontal" class="inner-split">
                    <mesh-view 
                        id="model-mesh-view"
                        use-lights
                        enable-mesh-rotation
                        enable-vertex-selection
                        caption="Mesh View"
                        grid-horizontal-color="${this.gridHorizontalColor}"
                        grid-vertical-color="${this.gridVerticalColor}"
                        grid-background-color1="${this.gridBackgroundColor1}"
                        grid-background-color2="${this.gridBackgroundColor2}"
                        grid-size="${this.gridSize}"
                        grid-texture-size="${this.gridTextureSize}"
                        grid-line-width="${this.gridLineWidth}"
                        show-wireframe="${this.showWireframe}"
                        background-color="${this.modelViewportColor}"
                        mesh-color="${this.modelColor}"
                        .meshProvider="${this._modelMeshProvider}"
                        mesh-interaction="${this.meshInteraction}"
                        highlighted-face-color="${this.highlightedFaceColor}"
                        dragged-face-color="${this.draggedFaceColor}"
                        selected-face-color="${this.fixedFaceColor}"
                        show-grid-texture>
                    </mesh-view>
                    <mesh-view 
                        id="soup-mesh-view"
                        enable-face-dragging caption="Soup View"
                        show-grid="${this.showUnitGrid}"
                        grid-horizontal-color="${this.gridHorizontalColor}"
                        grid-vertical-color="${this.gridVerticalColor}"
                        grid-background-color1="${this.gridBackgroundColor1}"
                        grid-background-color2="${this.gridBackgroundColor2}"
                        grid-size="${this.gridSize}"
                        grid-texture-size="${this.gridTextureSize}"
                        grid-line-width="${this.gridLineWidth}"
                        show-wireframe="${this.showWireframe}"
                        background-color="${this.soupViewportColor}"
                        mesh-color="${this.soupColor}"
                        .meshProvider="${this._soupMeshProvider}"
                        mesh-interaction="${this.meshInteraction}"
                        highlighted-face-color="${this.highlightedFaceColor}"
                        dragged-face-color="${this.draggedFaceColor}"
                        selected-face-color="${this.fixedFaceColor}"
                        show-debug-data="${this.showOptimizationDataMonitor}"
                        show-grid-texture="${this.showGridTextureInSoupView}">
                    </mesh-view>
                </vaadin-split-layout>
            </vaadin-split-layout>
        `;
    }

    /**
     * Properties static declaration
     */

    static get properties() {
        return {
            splitOrientation: {
                type: String,
                attribute: 'split-orientation'
            },
            modelViewportColor: {
                type: String,
                attribute: 'model-viewport-color'
            },
            soupViewportColor: {
                type: String,
                attribute: 'soup-viewport-color'
            },
            modelColor: {
                type: String,
                attribute: 'model-color'
            },
            soupColor: {
                type: String,
                attribute: 'soup-color'
            },
            wireframeVisibility: {
                type: String,
                attribute: 'wireframe-visibility'
            },
            modelViewVisibility: {
                type: String,
                attribute: 'model-view-visibility'
            },
            soupViewVisibility: {
                type: String,
                attribute: 'soup-view-visibility'
            },
            delta: {
                type: Number,
                attribute: 'delta'
            },
            lambda: {
                type: Number,
                attribute: 'lambda'
            },
            seamlessWeight: {
                type: Number,
                attribute: 'seamless-weight'
            },
            positionWeight: {
                type: Number,
                attribute: 'position-weight'
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
            highlightedFaceColor: {
                type: String,
                attribute: 'highlighted-face-color'
            },
            draggedFaceColor: {
                type: String,
                attribute: 'dragged-face-color'
            },
            fixedFaceColor: {
                type: String,
                attribute: 'fixed-face-color'
            },
            vertexEnergyColor: {
                type: String,
                attribute: 'vertex-energy-color'
            },
            vertexEnergyType: {
                type: String,
                attribute: 'vertex-energy-type'
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
            unitGridVisibility: {
                type: String,
                attribute: 'unit-grid-visibility'
            },
            soupViewGridTextureVisibility: {
                type: String,
                attribute: 'soup-view-grid-texture-visibility'
            },
            optimizationDataMonitorVisibility: {
                type: String,
                attribute: 'optimization-data-monitor-visibility'
            },  
            solverState: {
                type: String,
                attribute: 'solver-state'
            },  
            model: {
                type: Object,
                attribute: 'model'
            },  
            module: {
                type: Object,
                attribute: 'module'
            }
        };
    }

    /**
     * State handling
     */

    stateChanged(state) {
        this.splitOrientation = state.splitOrientation;
        this.modelViewportColor = state.modelViewportColor;
        this.soupViewportColor = state.soupViewportColor;
        this.modelColor = state.modelColor;
        this.soupColor = state.soupColor;
        this.wireframeVisibility = state.wireframeVisibility;
        this.modelViewVisibility = state.modelViewVisibility;
        this.soupViewVisibility = state.soupViewVisibility;
        this.delta = state.delta;
        this.lambda = state.lambda;
        this.seamlessWeight = state.seamlessWeight;
        this.positionWeight = state.positionWeight;
        this.gridHorizontalColor = state.gridHorizontalColor;
        this.gridVerticalColor = state.gridVerticalColor;
        this.gridBackgroundColor1 = state.gridBackgroundColor1;
        this.gridBackgroundColor2 = state.gridBackgroundColor2;
        this.highlightedFaceColor = state.highlightedFaceColor;
        this.draggedFaceColor = state.draggedFaceColor;
        this.fixedFaceColor = state.fixedFaceColor;
        this.vertexEnergyColor = state.vertexEnergyColor;
        this.vertexEnergyType = state.vertexEnergyType;
        this.gridSize = state.gridSize;
        this.gridTextureSize = state.gridTextureSize;
        this.gridLineWidth = state.gridLineWidth;
        this.unitGridVisibility = state.unitGridVisibility;
        this.soupViewGridTextureVisibility = state.soupViewGridTextureVisibility;
        this.optimizationDataMonitorVisibility = state.optimizationDataMonitorVisibility;
        this.solverState = state.solverState;
        this.modelFilename = state.modelFilename;
        this.moduleFilename = state.moduleFilename;
        this.modelState = state.modelState;
        this.moduleState = state.moduleState;   
    }

    /**
     * Constructor
     */

    constructor() {
        super();
        this._modelMeshProvider = new MeshProvider();
        this._soupMeshProvider = new MeshProvider();           
    }

    /**
     * Properties
     */

    set splitOrientation(value) {
        this._splitOrientation = value;
    }

    get splitOrientation() {
        return this._splitOrientation;
    }
    
    set modelViewportColor(value) {
        this._modelViewportColor = value;
    }

    get modelViewportColor() {
        return this._modelViewportColor;
    }

    set soupViewportColor(value) {
        this._soupViewportColor = value;
    }

    get soupViewportColor() {
        return this._soupViewportColor;
    }

    set modelColor(value) {
        const oldValue = this._modelColor;
        this._modelColor = value;
        
        if(this._modelMeshProvider) {
            this._modelMeshProvider.meshColor = this._modelColor;
        }

        this.requestUpdate('modelColor', oldValue);
    }

    get modelColor() {
        return this._modelColor;
    }

    set soupColor(value) {
        const oldValue = this._soupColor;
        this._soupColor = value;
        
        if(this._soupMeshProvider) {
            this._soupMeshProvider.meshColor = this._soupColor;
        }

        this.requestUpdate('soupColor', oldValue);
    }

    get soupColor() {
        return this._soupColor;
    }

    set wireframeVisibility(value) {
        this._wireframeVisibility = value;
    }

    get wireframeVisibility() {
        return this._wireframeVisibility;        
    }

    set modelViewVisibility(value) {
        this._modelViewVisibility = value;
    }

    get modelViewVisibility() {
        return this._modelViewVisibility;
    }

    set soupViewVisibility(value) {
        this._soupViewVisibility = value;
    }

    get soupViewVisibility() {
        return this._soupViewVisibility;
    }

    set delta(value) {
        this._delta = value;
    }

    get delta() {
        return this._delta;
    }

    set lambda(value) {
        this._lambda = value;
    }

    get lambda() {
        return this._lambda;
    }

    set seamlessWeight(value) {
        this._seamlessWeight = value;
    }

    get seamlessWeight() {
        return this._seamlessWeight;
    }
    
    set positionWeight(value) {
        this._positionWeight = value;
    }

    get positionWeight() {
        return this._positionWeight;
    }

    set gridHorizontalColor(value) {
        this._gridHorizontalColor = value;
    }

    get gridHorizontalColor() {
        return this._gridHorizontalColor;
    }

    set gridVerticalColor(value) {
        this._gridVerticalColor = value;
    }

    get gridVerticalColor() {
        return this._gridVerticalColor;
    } 

    set gridBackgroundColor1(value) {
        this._gridBackgroundColor1 = value;
    }

    get gridBackgroundColor1() {
        return this._gridBackgroundColor1;
    }

    set gridBackgroundColor2(value) {
        this._gridBackgroundColor2 = value;
    }

    get gridBackgroundColor2() {
        return this._gridBackgroundColor2;
    } 

    set highlightedFaceColor(value) {
        this._highlightedFaceColor = value;
    }

    get highlightedFaceColor() {
        return this._highlightedFaceColor;
    } 

    set draggedFaceColor(value) {
        this._draggedFaceColor = value;
    }

    get draggedFaceColor() {
        return this._draggedFaceColor;
    }  
    
    set fixedFaceColor(value) {
        this._fixedFaceColor = value;
    }

    get fixedFaceColor() {
        return this._fixedFaceColor;
    }
    
    set vertexEnergyColor(value) {
        const oldValue = this._vertexEnergyColor;
        this._vertexEnergyColor = value;
        
        if(this._modelMeshProvider) {
            this._modelMeshProvider.vertexEnergyColor = value;
        }

        if(this._soupMeshProvider) {
            this._soupMeshProvider.vertexEnergyColor = value;
        }

        this.requestUpdate('vertexEnergyColor', oldValue);
    }

    get vertexEnergyColor() {
        return this._vertexEnergyColor;
    } 

    set vertexEnergyType(value) {
        const oldValue = this._vertexEnergyType;
        this._vertexEnergyType = value;
        
        if(this._modelMeshProvider) {
            this._modelMeshProvider.vertexEnergyType = value;
        }

        if(this._soupMeshProvider) {
            this._soupMeshProvider.vertexEnergyType = value;
        }

        this.requestUpdate('vertexEnergyType', oldValue);
    }

    get vertexEnergyType() {
        return this._vertexEnergyType;
    }
    
    set gridSize(value) {
        this._gridSize = value;
    }

    get gridSize() {
        return this._gridSize;
    }     

    set gridTextureSize(value) {
        this._gridTextureSize = value;
    }

    get gridTextureSize() {
        return this._gridTextureSize;
    }     

    set gridLineWidth(value) {
        this._gridLineWidth = value;
    }

    get gridLineWidth() {
        return this._gridLineWidth;
    } 
   
    set unitGridVisibility(value) {
        this._unitGridVisibility = value;
    }

    get unitGridVisibility() {
        return this._unitGridVisibility;
    } 
    
    set soupViewGridTextureVisibility(value) {
        this._soupViewGridTextureVisibility = value;
    }

    get soupViewGridTextureVisibility() {
        return this._soupViewGridTextureVisibility;
    } 

    set optimizationDataMonitorVisibility(value) {
        this._optimizationDataMonitorVisibility = value;
    }

    get optimizationDataMonitorVisibility() {
        return this._optimizationDataMonitorVisibility;
    } 

    set solverState(value) {
        const oldValue = this._solverState;
        this._solverState = value;
        if(HelpersExports.isModuleLoaded(this.moduleState)) {
            switch(this._solverState) {
                case ReducerExports.SolverState.ON:
                    this._engine.resumeSolver();
                    break;
                case ReducerExports.SolverState.OFF:
                    this._engine.pauseSolver();
                    break;                
            }
        }
        this.requestUpdate('solverState', oldValue);
    }

    get solverState() {
        return this._solverState;
    }

    set modelFilename(value) {
        const oldValue = this._modelFilename;
        if(value && value !== oldValue) {
            this._modelFilename = value;
            this._loadModel(this._modelFilename);
            this.requestUpdate('modelFilename', oldValue);
        }
    }

    get modelFilename() {
        return this._modelFilename;
    }

    set modelState(value) {
        this._modelState = value;
    }

    get modelState() {
        return this._modelState;
    }

    set moduleFilename(value) {
        const oldValue = this._moduleFilename; 
        if(value && value !== oldValue) {
            this._moduleFilename = value;
            this._loadModule(this._moduleFilename);
            this.requestUpdate('moduleFilename', oldValue);
        }
    }    

    get moduleFilename() {
        return this._moduleFilename;
    }

    set moduleState(value) {
        this._moduleState = value;
    }

    get moduleState() {
        return this._moduleState;
    } 

    /**
     * Element life-cycle callbacks
     */

    connectedCallback() {
        super.connectedCallback();

        this._meshViewFaceHighlightedSubscriptionToken = PubSub.subscribe('mesh-view-face-highlighted', (name, payload) => {
            PubSub.publish('mesh-view-highlight-face', payload);
        });

        this._meshViewFaceUnhighlightedSubscriptionToken = PubSub.subscribe('mesh-view-face-unhighlighted', (name, payload) => {
            PubSub.publish('mesh-view-unhighlight-face', payload);
        });

        this._meshViewFaceDraggingBeginSubscriptionToken = PubSub.subscribe('mesh-view-face-dragging-begin', (name, payload) => {
            if(!payload.faceSelected) {
                this._engine.constrainFacePosition(payload.face.id);
            }
            PubSub.publish('mesh-view-set-dragged-face', payload);
        });

        this._meshViewFaceDraggingSubscriptionToken = PubSub.subscribe('mesh-view-face-dragging', (name, payload) => {
            this._engine.updateConstrainedFacePosition(payload.offset.x, payload.offset.y);
        });

        this._meshViewFaceDraggingEndSubscriptionToken = PubSub.subscribe('mesh-view-face-dragging-end', (name, payload) => {
            if(!payload.faceSelected) {
                this._engine.unconstrainFacePosition();
            }
            PubSub.publish('mesh-view-reset-dragged-face', payload);
        });

        this._meshViewFaceSelectedSubscriptionToken = PubSub.subscribe('mesh-view-face-selected', (name, payload) => {
            this._engine.constrainFacePosition(payload.face.id);
            PubSub.publish('mesh-view-select-face', payload);
        });

        this._meshViewFaceUnselectedSubscriptionToken = PubSub.subscribe('mesh-view-face-unselected', (name, payload) => {
            this._engine.unconstrainFacePosition();
            PubSub.publish('mesh-view-unselect-face', payload);
        });        
    }
    
    disconnectedCallback() {
        // TODO: Remove event listeners
        super.disconnectedCallback();
    }

    /**
     * Private Methods
     */

    _loadModel(modelFilename) {
        if(HelpersExports.isModuleLoaded(this.moduleState)) {
            store.dispatch(ActionsExports.setModelState(EnumsExports.LoadState.LOADING));
            try {
                this._engine.loadModel(modelFilename);
                this._modelMeshProvider = new AutoquadsModelMeshProvider(this._engine, this.vertexEnergyType, this.vertexEnergyColor, this.modelColor);
                this._soupMeshProvider = new AutoquadsSoupMeshProvider(this._engine, this.vertexEnergyType, this.vertexEnergyColor, this.soupColor);  
                console.log("Model loaded: " + modelFilename);
                store.dispatch(ActionsExports.setModelState(EnumsExports.LoadState.LOADED));
            }
            catch(e) {
                this._modelMeshProvider = new MeshProvider();
                this._soupMeshProvider = new MeshProvider();   
                alert("Couldn't load model file");
                console.error("Failed to load model: " + modelFilename);
                store.dispatch(ActionsExports.setModelState(EnumsExports.LoadState.UNLOADED));
            }
        }
    }

    _loadModule(moduleFilename) {
        store.dispatch(ActionsExports.setModuleState(EnumsExports.LoadState.LOADING));
        try {
            let RDSModule = require(moduleFilename);
            this._engine = new RDSModule.Engine();
            PubSub.publish('module-loaded');
            console.log("Node module loaded: " + moduleFilename);
            store.dispatch(ActionsExports.setModuleState(EnumsExports.LoadState.LOADED));
        }
        catch(e) {
            alert("Couldn't load node module");
            console.error("Failed to node module: " + moduleFilename);
            store.dispatch(ActionsExports.setModuleState(EnumsExports.LoadState.UNLOADED));
        }
        finally {
            this._modelMeshProvider = new MeshProvider();
            this._soupMeshProvider = new MeshProvider();
        }
    }

    _reloadModule() {
        this._loadModule();
        this._loadModel(this._modelFilename);
    }
}

customElements.define('autoquads-view', AutoquadsView);