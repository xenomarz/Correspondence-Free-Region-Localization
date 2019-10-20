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
            },
            objectiveFunctionsProperties: {
                type: Object,
                attribute: 'objective-functions-properties'
            }
        };
    }

    /**
     * State handling
     */
    stateChanged(state) {
        let localState = {};
        for (let [key, value] of Object.entries(state)) {
            localState[key] = this[key];
        }

        for (let [key, value] of Object.entries(state)) {
            let currentState = store.getState();
            if((state[key] === currentState[key]) && (localState[key] !== state[key])) {
                this[key] = state[key];
            }
        }    
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
        const oldValue = this._splitOrientation;
        this._splitOrientation = value;
        this.requestUpdate('splitOrientation', oldValue);
    }

    get splitOrientation() {
        return this._splitOrientation;
    }
    
    set modelViewportColor(value) {
        const oldValue = this._modelViewportColor;
        this._modelViewportColor = value;
        this.requestUpdate('modelViewportColor', oldValue);
    }

    get modelViewportColor() {
        return this._modelViewportColor;
    }

    set soupViewportColor(value) {
        const oldValue = this._soupViewportColor;
        this._soupViewportColor = value;
        this.requestUpdate('soupViewportColor', oldValue);
    }

    get soupViewportColor() {
        return this._soupViewportColor;
    }

    set modelColor(value) {
        const oldValue = this._modelColor;
        this._modelColor = value;
        this._modelMeshProvider.meshColor = this._modelColor;
        this.requestUpdate('modelColor', oldValue);
    }

    get modelColor() {
        return this._modelColor;
    }

    set soupColor(value) {
        const oldValue = this._soupColor;
        this._soupColor = value;
        this._soupMeshProvider.meshColor = this._soupColor;
        this.requestUpdate('soupColor', oldValue);
    }

    get soupColor() {
        return this._soupColor;
    }

    set wireframeVisibility(value) {
        const oldValue = this._wireframeVisibility;
        this._wireframeVisibility = value;
        this.requestUpdate('wireframeVisibility', oldValue);
    }

    get wireframeVisibility() {
        return this._wireframeVisibility;        
    }

    set modelViewVisibility(value) {
        const oldValue = this._modelViewVisibility;
        this._modelViewVisibility = value;
        this.requestUpdate('modelViewVisibility', oldValue);
    }

    get modelViewVisibility() {
        return this._modelViewVisibility;
    }

    set soupViewVisibility(value) {
        const oldValue = this._soupViewVisibility;
        this._soupViewVisibility = value;
        this.requestUpdate('soupViewVisibility', oldValue);        
    }

    get soupViewVisibility() {
        return this._soupViewVisibility;
    }

    set delta(value) {
        if(HelpersExports.isModuleLoaded(this.moduleState)) {
            const oldValue = this._delta;
            this._delta = value;
            this._engine.setObjectiveFunctionProperty('Separation', 'delta', value);
            this.requestUpdate('delta', oldValue);
        }
    }

    get delta() {
        if(HelpersExports.isModuleLoaded(this.moduleState)) {
            return this._engine.getObjectiveFunctionProperty('Separation', 'delta');
        }
    }

    set lambda(value) {
        if(HelpersExports.isModuleLoaded(this.moduleState)) {
            const oldValue = this._lambda;
            this._lambda = value;
            this._engine.setObjectiveFunctionProperty('Separation', 'weight', value);
            this._engine.setObjectiveFunctionProperty('Symmetric Dirichlet', 'weight', 1 - value);
            this.requestUpdate('lambda', oldValue);
        }
    }

    get lambda() {
        if(HelpersExports.isModuleLoaded(this.moduleState)) {
            return this._engine.getObjectiveFunctionProperty('Separation', 'weight');
        }
    }

    set seamlessWeight(value) {
        const oldValue = this._seamlessWeight;
        this._seamlessWeight = value;
        this.requestUpdate('seamlessWeight', oldValue);
    }

    get seamlessWeight() {
        return this._seamlessWeight;
    }
    
    set positionWeight(value) {
        if(HelpersExports.isModuleLoaded(this.moduleState)) {
            const oldValue = this._positionWeight;
            this._positionWeight = value;
            this._engine.positionWeight = value;
            this.requestUpdate('positionWeight', oldValue);
        }
    }

    get positionWeight() {
        return this._positionWeight;
    }

    set gridHorizontalColor(value) {
        const oldValue = this._gridHorizontalColor;
        this._gridHorizontalColor = value;
        this.requestUpdate('gridHorizontalColor', oldValue);
    }

    get gridHorizontalColor() {
        return this._gridHorizontalColor;
    }

    set gridVerticalColor(value) {
        const oldValue = this._gridVerticalColor;
        this._gridVerticalColor = value;
        this.requestUpdate('gridVerticalColor', oldValue);
    }

    get gridVerticalColor() {
        return this._gridVerticalColor;
    } 

    set gridBackgroundColor1(value) {
        const oldValue = this._gridBackgroundColor1;
        this._gridBackgroundColor1 = value;
        this.requestUpdate('gridBackgroundColor1', oldValue);
    }

    get gridBackgroundColor1() {
        return this._gridBackgroundColor1;
    }

    set gridBackgroundColor2(value) {
        const oldValue = this._gridBackgroundColor2;
        this._gridBackgroundColor2 = value;
        this.requestUpdate('gridBackgroundColor2', oldValue);
    }

    get gridBackgroundColor2() {
        return this._gridBackgroundColor2;
    } 

    set highlightedFaceColor(value) {
        const oldValue = this._highlightedFaceColor;
        this._highlightedFaceColor = value;
        this.requestUpdate('highlightedFaceColor', oldValue);
    }

    get highlightedFaceColor() {
        return this._highlightedFaceColor;
    } 

    set draggedFaceColor(value) {
        const oldValue = this._draggedFaceColor;
        this._draggedFaceColor = value;
        this.requestUpdate('draggedFaceColor', oldValue);
    }

    get draggedFaceColor() {
        return this._draggedFaceColor;
    }  
    
    set fixedFaceColor(value) {
        const oldValue = this._fixedFaceColor;
        this._fixedFaceColor = value;
        this.requestUpdate('fixedFaceColor', oldValue);
    }

    get fixedFaceColor() {
        return this._fixedFaceColor;
    }
    
    set gridSize(value) {
        const oldValue = this._gridSize;
        this._gridSize = value;
        this.requestUpdate('gridSize', oldValue);
    }

    get gridSize() {
        return this._gridSize;
    }     

    set gridTextureSize(value) {
        const oldValue = this._gridTextureSize;
        this._gridTextureSize = value;
        this.requestUpdate('gridTextureSize', oldValue);
    }

    get gridTextureSize() {
        return this._gridTextureSize;
    }     

    set gridLineWidth(value) {
        const oldValue = this._gridLineWidth;
        this._gridLineWidth = value;
        this.requestUpdate('gridLineWidth', oldValue);
    }

    get gridLineWidth() {
        return this._gridLineWidth;
    } 
   
    set unitGridVisibility(value) {
        const oldValue = this._unitGridVisibility;
        this._unitGridVisibility = value;
        this.requestUpdate('unitGridVisibility', oldValue);
    }

    get unitGridVisibility() {
        return this._unitGridVisibility;
    } 
    
    set soupViewGridTextureVisibility(value) {
        const oldValue = this._soupViewGridTextureVisibility;
        this._soupViewGridTextureVisibility = value;
        this.requestUpdate('soupViewGridTextureVisibility', oldValue);
    }

    get soupViewGridTextureVisibility() {
        return this._soupViewGridTextureVisibility;
    } 

    set optimizationDataMonitorVisibility(value) {
        const oldValue = this._optimizationDataMonitorVisibility;
        this._optimizationDataMonitorVisibility = value;
        this.requestUpdate('optimizationDataMonitorVisibility', oldValue);
    }

    get optimizationDataMonitorVisibility() {
        return this._optimizationDataMonitorVisibility;
    } 

    set solverState(value) {
        if(HelpersExports.isModuleLoaded(this.moduleState)) {
            const oldValue = this._solverState;
            this._solverState = value;
            switch(this._solverState) {
                case EnumsExports.SolverState.ON:
                    this._engine.resumeSolver();
                    break;
                case EnumsExports.SolverState.OFF:
                    this._engine.pauseSolver();
                    break;                
            }
            this.requestUpdate('solverState', oldValue);
        }
    }

    get solverState() {
        return this._solverState;
    }

    set modelFilename(value) {
        if(value) {
            const oldValue = this._modelFilename;
            this._modelFilename = value;
            this._loadModel(this._modelFilename);
            this.requestUpdate('modelFilename', oldValue);
        }
    }

    get modelFilename() {
        return this._modelFilename;
    }

    set modelState(value) {
        const oldValue = this._modelState;
        this._modelState = value;
        this.requestUpdate('modelState', oldValue);
    }

    get modelState() {
        return this._modelState;
    }

    set moduleFilename(value) {
        if(value) {
            const oldValue = this._moduleFilename; 
            this._moduleFilename = value;
            this._loadModule(this._moduleFilename);
            this.requestUpdate('moduleFilename', oldValue);
        }
    }    

    get moduleFilename() {
        return this._moduleFilename;
    }

    set moduleState(value) {
        const oldValue = this._moduleState;
        this._moduleState = value;
        this.requestUpdate('moduleState', oldValue);
    }

    get moduleState() {
        return this._moduleState;
    }

    set objectiveFunctionsProperties(value) {
        const oldValue = this._objectiveFunctionsProperties;
        this._objectiveFunctionsProperties = value;
        this._modelMeshProvider.objectiveFunctionsProperties = value;
        this._soupMeshProvider.objectiveFunctionsProperties = value;
        this.requestUpdate('objectiveFunctionsProperties', oldValue);
    }

    get objectiveFunctionsProperties() {
        return this._objectiveFunctionsProperties;
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
            this._engine.updateConstrainedFacePosition(payload.face.id, payload.offset.x, payload.offset.y);
        });

        this._meshViewFaceDraggingEndSubscriptionToken = PubSub.subscribe('mesh-view-face-dragging-end', (name, payload) => {
            if(!payload.faceSelected) {
                this._engine.unconstrainFacePosition(payload.face.id);
            }
            else {
                this._engine.reconstrainFacePosition(payload.face.id);
            }
            PubSub.publish('mesh-view-reset-dragged-face', payload);
        });

        this._meshViewFaceSelectedSubscriptionToken = PubSub.subscribe('mesh-view-face-selected', (name, payload) => {
            this._engine.constrainFacePosition(payload.face.id);
            PubSub.publish('mesh-view-select-face', payload);
        });

        this._meshViewFaceUnselectedSubscriptionToken = PubSub.subscribe('mesh-view-face-unselected', (name, payload) => {
            this._engine.unconstrainFacePosition(payload.face.id);
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
                this._modelMeshProvider = new AutoquadsModelMeshProvider(this._engine, this.modelColor, this.objectiveFunctionsProperties);
                this._soupMeshProvider = new AutoquadsSoupMeshProvider(this._engine, this.soupColor, this.objectiveFunctionsProperties);  
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