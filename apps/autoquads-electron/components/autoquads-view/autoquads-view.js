// Web Modules Imports
import { LitElement, html, css } from '../../web_modules/lit-element.js';
import { classMap } from '../../web_modules/lit-html/directives/class-map.js';
import '../../web_modules/@vaadin/vaadin-button.js';
import '../../web_modules/@vaadin/vaadin-split-layout.js';
import '../../web_modules/@vaadin/vaadin-dialog.js';
import { PubSub } from '../../web_modules/pubsub-js.js';
import * as THREE from '../../web_modules/three.js';
import { connect } from '../../web_modules/pwa-helpers.js';

// Components Imports
import '../mesh-view/mesh-view.js';
import '../autoquads-side-bar/autoquads-side-bar.js';
import { MeshProvider } from '../mesh-provider/mesh-provider.js';
import { AutoquadsShapeMeshProvider } from '../autoquads-shape-mesh-provider/autoquads-shape-mesh-provider.js';
import { AutoquadsPartialMeshProvider } from '../autoquads-partial-mesh-provider/autoquads-partial-mesh-provider.js';
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

            .hidden {
                display: none;
            }
        `];
    }

    render() {
        return html`  
            <vaadin-split-layout class="outer-split">
                <autoquads-side-bar></autoquads-side-bar>
                <vaadin-split-layout orientation="horizontal" class="inner-split">
                    <mesh-view 
                        id="shape-mesh-view"
                        class="${classMap(this._shapeMeshViewClasses)}"
                        use-lights
                        enable-mesh-rotation
                        enable-vertex-selection
                        caption="Shape View"
                        grid-horizontal-color="${this.gridHorizontalColor}"
                        grid-vertical-color="${this.gridVerticalColor}"
                        grid-background-color1="${this.gridBackgroundColor1}"
                        grid-background-color2="${this.gridBackgroundColor2}"
                        grid-size="${this.gridSize}"
                        grid-texture-size="${this.gridTextureSize}"
                        grid-line-width="${this.gridLineWidth}"
                        ?show-wireframe="${HelpersExports.isVisible(this.shapeWireframeVisibility)}"
                        ?show-fat-wireframe="${HelpersExports.isVisible(this.shapeFatWireframeVisibility)}"
                        background-color="${this.shapeViewportColor}"
                        mesh-color="${this.shapeColor}"
                        .meshProvider="${this._shapeMeshProvider}"
                        mesh-interaction="${this.meshInteraction}"
                        highlighted-face-color="${this.highlightedFaceColor}"
                        highlighted-edge-color="${this.highlightedEdgeColor}"
                        dragged-face-color="${this.draggedFaceColor}"
                        selected-face-color="${this.fixedFaceColor}"
                        selected-edge-color="${this.editedEdgeColor}"
                        ?show-debug-data="${HelpersExports.isVisible(this.optimizationDataMonitorVisibility)}"
                        show-grid-texture>
                    </mesh-view>
                    <mesh-view 
                        id="partial-mesh-view"
                        class="${classMap(this._partialMeshViewClasses)}"
                        use-lights
                        enable-mesh-rotation
                        enable-face-dragging caption="Partial View"
                        ?show-unit-grid="${HelpersExports.isVisible(this.unitGridVisibility)}"
                        grid-horizontal-color="${this.gridHorizontalColor}"
                        grid-vertical-color="${this.gridVerticalColor}"
                        grid-background-color1="${this.gridBackgroundColor1}"
                        grid-background-color2="${this.gridBackgroundColor2}"
                        grid-size="${this.gridSize}"
                        grid-texture-size="${this.gridTextureSize}"
                        grid-line-width="${this.gridLineWidth}"
                        ?show-wireframe="${HelpersExports.isVisible(this.partialWireframeVisibility)}"
                        ?show-fat-wireframe="${HelpersExports.isVisible(this.partialFatWireframeVisibility)}"
                        background-color="${this.partialViewportColor}"
                        mesh-color="${this.partialColor}"
                        .meshProvider="${this._partialMeshProvider}"
                        mesh-interaction="${this.meshInteraction}"
                        highlighted-face-color="${this.highlightedFaceColor}"
                        highlighted-edge-color="${this.highlightedEdgeColor}"
                        dragged-face-color="${this.draggedFaceColor}"
                        selected-face-color="${this.fixedFaceColor}"
                        selected-edge-color="${this.editedEdgeColor}"
                        show-grid-texture="${this.showGridTextureInPartialView}">
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
            shapeViewportColor: {
                type: String,
                attribute: 'shape-viewport-color'
            },
            partialViewportColor: {
                type: String,
                attribute: 'partial-viewport-color'
            },
            shapeColor: {
                type: String,
                attribute: 'shape-color'
            },
            partialColor: {
                type: String,
                attribute: 'partial-color'
            },
            shapeWireframeVisibility: {
                type: String,
                attribute: 'shape-wireframe-visibility'
            },
            partialWireframeVisibility: {
                type: String,
                attribute: 'partial-wireframe-visibility'
            },
            shapeFatWireframeVisibility: {
                type: String,
                attribute: 'shape-fat-wireframe-visibility'
            },
            partialFatWireframeVisibility: {
                type: String,
                attribute: 'partial-fat-wireframe-visibility'
            },            
            shapeViewVisibility: {
                type: String,
                attribute: 'shape-view-visibility'
            },
            partialViewVisibility: {
                type: String,
                attribute: 'partial-view-visibility'
            },
            autocutsWeight: {
                type: Number,
                attribute: 'autocuts-weight'
            },            
            delta: {
                type: Number,
                attribute: 'delta'
            },
            lambda: {
                type: Number,
                attribute: 'lambda'
            },
            zeta: {
                type: Number,
                attribute: 'zeta'
            },            
            seamlessWeight: {
                type: Number,
                attribute: 'seamless-weight'
            },
            seamlessWeight: {
                type: Number,
                attribute: 'seamless-weight'
            },
            selectedEdgeSeamlessAngleWeight: {
                type: Number,
                attribute: 'selected-edge-seamless-angle-weight'
            },
            selectedEdgeSeamlessLengthWeight: {
                type: Number,
                attribute: 'selected-edge-seamless-length-weight'
            },
            singularityInterval: {
                type: Number,
                attribute: 'singularity-interval'
            },
            positionWeight: {
                type: Number,
                attribute: 'position-weight'
            },
            sigmaThreshold: {
                type: Number,
                attribute: 'sigma-threshold'
            },
            initialStepSize: {
                type: Number,
                attribute: 'initial-step-size'
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
            highlightedEdgeColor: {
                type: String,
                attribute: 'highlighted-edge-color'
            },
            draggedFaceColor: {
                type: String,
                attribute: 'dragged-face-color'
            },
            fixedFaceColor: {
                type: String,
                attribute: 'fixed-face-color'
            },
            editedEdgeColor: {
                type: String,
                attribute: 'edited-edge-color'
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
            partialViewGridTextureVisibility: {
                type: String,
                attribute: 'partial-view-grid-texture-visibility'
            },
            optimizationDataMonitorVisibility: {
                type: String,
                attribute: 'optimization-data-monitor-visibility'
            },  
            solverState: {
                type: String,
                attribute: 'solver-state'
            },  
            module: {
                type: Object,
                attribute: 'module'
            },
            objectiveFunctionsProperties: {
                type: Object,
                attribute: 'objective-functions-properties'
            },
            algorithmType: {
                type: String,
                attribute: 'algorithm-type'
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
        this._shapeMeshProvider = new MeshProvider();
        this._partialMeshProvider = new MeshProvider();
        this._shapeMeshViewClasses = { hidden: false };
        this._partialMeshViewClasses = { hidden: false };
        this._selectedEdge = null;      
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
    
    set shapeViewportColor(value) {
        const oldValue = this._shapeViewportColor;
        this._shapeViewportColor = value;
        this.requestUpdate('shapeViewportColor', oldValue);
    }

    get shapeViewportColor() {
        return this._shapeViewportColor;
    }

    set partialViewportColor(value) {
        const oldValue = this._partialViewportColor;
        this._partialViewportColor = value;
        this.requestUpdate('partialViewportColor', oldValue);
    }

    get partialViewportColor() {
        return this._partialViewportColor;
    }

    set shapeColor(value) {
        const oldValue = this._shapeColor;
        this._shapeColor = value;
        this._shapeMeshProvider.meshColor = this._shapeColor;
        this.requestUpdate('shapeColor', oldValue);
    }

    get shapeColor() {
        return this._shapeColor;
    }

    set partialColor(value) {
        const oldValue = this._partialColor;
        this._partialColor = value;
        this._partialMeshProvider.meshColor = this._partialColor;
        this.requestUpdate('partialColor', oldValue);
    }

    get partialColor() {
        return this._partialColor;
    }

    set shapeWireframeVisibility(value) {
        const oldValue = this._shapeWireframeVisibility;
        this._shapeWireframeVisibility = value;
        this.requestUpdate('shapeWireframeVisibility', oldValue);
    }

    get shapeWireframeVisibility() {
        return this._shapeWireframeVisibility;        
    }

    set partialWireframeVisibility(value) {
        const oldValue = this._partialWireframeVisibility;
        this._partialWireframeVisibility = value;
        this.requestUpdate('partialWireframeVisibility', oldValue);
    }

    get partialWireframeVisibility() {
        return this._partialWireframeVisibility;        
    }

    set optimizationDataMonitorVisibility(value) {
        const oldValue = this._optimizationDataMonitorVisibility;
        this._optimizationDataMonitorVisibility = value;
        this.requestUpdate('optimizationDataMonitorVisibility', oldValue);
    }

    get optimizationDataMonitorVisibility() {
        return this._optimizationDataMonitorVisibility;        
    }

    set shapeViewVisibility(value) {
        const oldValue = this._shapeViewVisibility;
        this._shapeViewVisibility = value;
        this._shapeMeshViewClasses = {
            hidden: !HelpersExports.isVisible(value)
        }
        this.requestUpdate('shapeViewVisibility', oldValue);
    }

    get shapeViewVisibility() {
        return this._shapeViewVisibility;
    }

    set partialViewVisibility(value) {
        const oldValue = this._partialViewVisibility;
        this._partialViewVisibility = value;
        this._partialMeshViewClasses = {
            hidden: !HelpersExports.isVisible(value)
        }
        this.requestUpdate('partialViewVisibility', oldValue);        
    }

    get partialViewVisibility() {
        return this._partialViewVisibility;
    }

    set autocutsWeight(value) {
        // if(HelpersExports.isModuleLoaded(this.moduleState)) {
        //     const oldValue = this._autocutsWeight;
        //     this._autocutsWeight = value;
        //     this._engine.setObjectiveFunctionProperty('Separation', 'weight', '', this.autocutsWeight * this.lambda);
        //     this._engine.setObjectiveFunctionProperty('Symmetric Dirichlet', 'weight', '', this.autocutsWeight * (1 - this.lambda));
        //     this.requestUpdate('autocutsWeight', oldValue);
        // }
    }

    get autocutsWeight() {
        return this._autocutsWeight;
    }   

    set delta(value) {
        // if(HelpersExports.isModuleLoaded(this.moduleState)) {
        //     const oldValue = this._delta;
        //     this._delta = value;
        //     this._engine.setObjectiveFunctionProperty('Separation', 'delta', '', value);
        //     this.requestUpdate('delta', oldValue);
        // }
    }

    get delta() {
        // if(HelpersExports.isModuleLoaded(this.moduleState)) {
        //     return this._engine.getObjectiveFunctionProperty('Separation', 'delta', 'none', '');
        // }
    }

    set lambda(value) {
        // if(HelpersExports.isModuleLoaded(this.moduleState)) {
        //     const oldValue = this._lambda;
        //     this._lambda = value;
        //     this._engine.setObjectiveFunctionProperty('Separation', 'weight', '', this.autocutsWeight * value);
        //     this._engine.setObjectiveFunctionProperty('Symmetric Dirichlet', 'weight', '', this.autocutsWeight * (1 - value));
        //     this.requestUpdate('lambda', oldValue);
        // }
    }

    get lambda() {
        // if(HelpersExports.isModuleLoaded(this.moduleState)) {
        //     return this._engine.getObjectiveFunctionProperty('Separation', 'weight', 'none', '');
        // }
    }

    set zeta(value) {
        // if(HelpersExports.isModuleLoaded(this.moduleState)) {
        //     const oldValue = this._zeta;
        //     this._zeta = value;
        //     this._engine.setObjectiveFunctionProperty('Seamless', 'zeta', '', value);
        //     this.requestUpdate('zeta', oldValue);
        // }
    }

    get zeta() {
        // if(HelpersExports.isModuleLoaded(this.moduleState)) {
        //     return this._engine.getObjectiveFunctionProperty('Seamless', 'zeta', 'none', '');
        // }
    }    

    set seamlessWeight(value) {
        // if(HelpersExports.isModuleLoaded(this.moduleState)) {
        //     const oldValue = this._seamlessWeight;
        //     this._seamlessWeight = value;
        //     this._engine.setObjectiveFunctionProperty('Seamless', 'weight', '', value);
        //     this.requestUpdate('seamlessWeight', oldValue);
        // }
    }

    get seamlessWeight() {
        // if(HelpersExports.isModuleLoaded(this.moduleState)) {
        //     return this._engine.getObjectiveFunctionProperty('Seamless', 'weight', 'none', '');
        // }
    }

    set selectedEdgeSeamlessAngleWeight(value) {
        // if(HelpersExports.isModuleLoaded(this.moduleState) && this._selectedEdge !== null) {
        //     const oldValue = this._selectedEdgeSeamlessAngleWeight;
        //     this._selectedEdgeSeamlessAngleWeight = value;
        //     this._engine.setObjectiveFunctionProperty('Seamless', 'edge_angle_weight', this._selectedEdge.id, value);
        //     this.requestUpdate('selectedEdgeSeamlessAngleWeight', oldValue);
        // }
    }

    get selectedEdgeSeamlessAngleWeight() {
        // if(HelpersExports.isModuleLoaded(this.moduleState) && this._selectedEdge !== null) {
        //     return this._engine.getObjectiveFunctionProperty('Seamless', 'edge_angle_weight', 'none', this._selectedEdge.id);
        // }
    }

    set selectedEdgeSeamlessLengthWeight(value) {
        // if(HelpersExports.isModuleLoaded(this.moduleState) && this._selectedEdge !== null) {
        //     const oldValue = this._selectedEdgeSeamlessLengthWeight;
        //     this._selectedEdgeSeamlessLengthWeight = value;
        //     this._engine.setObjectiveFunctionProperty('Seamless', 'edge_length_weight', this._selectedEdge.id, value);
        //     this.requestUpdate('selectedEdgeSeamlessLengthWeight', oldValue);
        // }
    }

    get selectedEdgeSeamlessLengthWeight() {
        // if(HelpersExports.isModuleLoaded(this.moduleState) && this._selectedEdge !== null) {
        //     return this._engine.getObjectiveFunctionProperty('Seamless', 'edge_length_weight', 'none', this._selectedEdge.id);
        // }
    }

    set singularityWeight(value) {
        // if(HelpersExports.isModuleLoaded(this.moduleState)) {
        //     const oldValue = this._singularityWeight;
        //     this._singularityWeight = value;
        //     this._engine.setObjectiveFunctionProperty('Singular Points Position', 'weight', '', value / 2);
        //     this.requestUpdate('singularityWeight', oldValue);
        // }
    }

    get singularityWeight() {
        // if(HelpersExports.isModuleLoaded(this.moduleState)) {
        //     return this._engine.getObjectiveFunctionProperty('Singular Points Position', 'weight', 'none', '');
        // }
    }

    set singularityInterval(value) {
        // if(HelpersExports.isModuleLoaded(this.moduleState)) {
        //     const oldValue = this._singularityInterval;
        //     this._singularityInterval = value;
        //     this._engine.setObjectiveFunctionProperty('Singular Points Position', 'interval', '', value);
        //     this._engine.setObjectiveFunctionProperty('Seamless', 'translation_interval', '', value);
        //     this.requestUpdate('singularityInterval', oldValue);
        // }
    }

    get singularityInterval() {
        // if(HelpersExports.isModuleLoaded(this.moduleState)) {
        //     return this._engine.getObjectiveFunctionProperty('Singular Points Position', 'interval', 'none', '');
        // }
    }

    set positionWeight(value) {
        // if(HelpersExports.isModuleLoaded(this.moduleState)) {
        //     const oldValue = this._positionWeight;
        //     this._positionWeight = value;
        //     this._engine.positionWeight = value;
        //     this.requestUpdate('positionWeight', oldValue);
        // }
    }

    get positionWeight() {
        // return this._positionWeight;
    }

    set sigmaThreshold(value) {
        const oldValue = this._sigmaThreshold;
        this._sigmaThreshold = value;
        this._shapeMeshProvider.sigmaThreshold = value;
        this.requestUpdate('sigmaThreshold', oldValue);
    }

    get sigmaThreshold() {
        return this._sigmaThreshold;
    }

    set initialStepSize(value) {
        if(HelpersExports.isModuleLoaded(this.moduleState)) {
            const oldValue = this._initialStepSize;
            this._initialStepSize = value;
            this._engine.setInitialStepSize(value);
            this.requestUpdate('initialStepSize', oldValue);
        }
    }

    get initialStepSize() {
        return this._initialStepSize;
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

    set highlightedEdgeColor(value) {
        const oldValue = this._highlightedEdgeColor;
        this._highlightedEdgeColor = value;
        this.requestUpdate('highlightedEdgeColor', oldValue);
    }

    get highlightedEdgeColor() {
        return this._highlightedEdgeColor;
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

    set editedEdgeColor(value) {
        const oldValue = this._editedEdgeColor;
        this._editedEdgeColor = value;
        this.requestUpdate('editedEdgeColor', oldValue);
    }

    get editedEdgeColor() {
        return this._editedEdgeColor;
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
    
    set partialViewGridTextureVisibility(value) {
        const oldValue = this._partialViewGridTextureVisibility;
        this._partialViewGridTextureVisibility = value;
        this.requestUpdate('partialViewGridTextureVisibility', oldValue);
    }

    get partialViewGridTextureVisibility() {
        return this._partialViewGridTextureVisibility;
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

            console.log("Lambda:");
            console.log(this._engine.getLambda());

            console.log("Mu:");
            console.log(this._engine.getMu());

            this.requestUpdate('solverState', oldValue);
        }
    }

    get solverState() {
        return this._solverState;
    }

    set shapeFilename(value) {
        if(value) {
            const oldValue = this._shapeFilename;
            this._shapeFilename = value;
            this._loadShape(this._shapeFilename);
            this.requestUpdate('shapeFilename', oldValue);
        }
    }

    get shapeFilename() {
        return this._shapeFilename;
    }

    set partialFilename(value) {
        if(value) {
            const oldValue = this._partialFilename;
            this._partialFilename = value;
            this._loadPartial(this._partialFilename);
            this.requestUpdate('partialFilename', oldValue);
        }
    }

    get partialFilename() {
        return this._partialFilename;
    }

    set shapeState(value) {
        const oldValue = this._shapeState;
        this._shapeState = value;
        this.requestUpdate('shapeState', oldValue);
    }

    get shapeState() {
        return this._shapeState;
    }

    set partialState(value) {
        const oldValue = this._partialState;
        this._partialState = value;
        this.requestUpdate('partialState', oldValue);
    }

    get partialState() {
        return this._partialState;
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

    /**
     * Element life-cycle callbacks
     */
    connectedCallback() {
        super.connectedCallback();
    }
    
    disconnectedCallback() {
        // TODO: Remove event listeners
        super.disconnectedCallback();
    }

    /**
     * Private Methods
     */

    _loadShape(shapeFilename) {
        if(HelpersExports.isModuleLoaded(this.moduleState)) {
            store.dispatch(ActionsExports.setShapeState(EnumsExports.LoadState.LOADING));
            try {
                this._engine.loadShape(shapeFilename);
                this._shapeMeshProvider = new AutoquadsShapeMeshProvider(this._engine, this.shapeColor, this.sigmaThreshold); 
                console.log("Shape loaded: " + shapeFilename);
                store.dispatch(ActionsExports.setShapeState(EnumsExports.LoadState.LOADED));
            }
            catch(e) {
                this._shapeMeshProvider = new MeshProvider();   
                alert("Couldn't load shape file");
                console.error("Failed to load shape: " + shapeFilename);
                store.dispatch(ActionsExports.setShapeState(EnumsExports.LoadState.UNLOADED));
            }
        }
    }

    _loadPartial(partialFilename) {
        if(HelpersExports.isModuleLoaded(this.moduleState)) {
            store.dispatch(ActionsExports.setPartialState(EnumsExports.LoadState.LOADING));
            try {
                this._engine.loadPartial(partialFilename);
                this._partialMeshProvider = new AutoquadsPartialMeshProvider(this._engine, this.partialColor);
                console.log("Partial loaded: " + partialFilename);
                store.dispatch(ActionsExports.setPartialState(EnumsExports.LoadState.LOADED));
            }
            catch(e) {
                this._partialMeshProvider = new MeshProvider();
                alert("Couldn't load partial file");
                console.error("Failed to load partial: " + partialFilename);
                store.dispatch(ActionsExports.setPartialState(EnumsExports.LoadState.UNLOADED));
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
            this._shapeMeshProvider = new MeshProvider();
            this._partialMeshProvider = new MeshProvider();
        }
    }

    _reloadModule() {
        this._loadModule();
        this._loadShape(this._shapeFilename);
        this._loadPartial(this._partialFilename);
    }
}

customElements.define('autoquads-view', AutoquadsView);