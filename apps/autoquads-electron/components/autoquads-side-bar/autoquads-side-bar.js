// Web Modules Imports
import { LitElement, html, css } from '../../web_modules/lit-element.js';
import { PubSub } from '../../web_modules/pubsub-js.js';
import '../../web_modules/@polymer/iron-icons.js';
import '../../web_modules/@polymer/iron-collapse.js';
import '../../web_modules/@polymer/paper-toggle-button.js';
import '../../web_modules/@vaadin/vaadin-button.js';
import '../../web_modules/@vaadin/vaadin-combo-box.js';
import '../../web_modules/@vaadin/vaadin-checkbox.js';
import '../../web_modules/@vaadin/vaadin-radio-button.js';
import '../../web_modules/@vaadin/vaadin-text-field/vaadin-number-field.js';
import '../../web_modules/@vaadin/vaadin-select.js';

// Components Imports
import { SideBar } from '../side-bar/side-bar.js';
import '../side-bar-collapsable-section/side-bar-collapsable-section.js';
import '../side-bar-parameter-input/side-bar-parameter-input.js';
import '../side-bar-color-picker/side-bar-color-picker.js';
import '../autoquads-side-bar-objective-function-visual-property/autoquads-side-bar-objective-function-visual-property.js';
import * as ReducerExports from '../../redux/reducer.js';
import * as ActionsExports from '../../redux/actions.js';
import * as EnumsExports from '../../redux/enums.js';
import * as HelpersExports from '../../redux/helpers.js';
import { store } from '../../redux/store.js';

export class AutoquadsSideBar extends SideBar {
    static get styles() {
        return [
            ...super.styles,
            css`
                :host {
                    --paper-menu-button: {
                        padding: 0px;
                    }

                    --paper-icon-button: {
                        padding: 4px;
                        margin-right: 8px;
                    }

                    --lumo-space-m: 0px;
                }

                .viewport-color-picker-container {
                    display: flex;
                    flex-direction: row;
                    align-items: flex-end;
                }

                .parameter-icon {
                    cursor: pointer;
                }

                side-bar-parameter-input {
                    width: 100%;
                }

                vaadin-checkbox {
                    display: block;
                }
            `
        ];
    }

    render() {
        return html`
            <side-bar-collapsable-section
                caption="Shape Options">
                <vaadin-button
                    theme="contrast primary"
                    @click="${this._loadShape}">
                    <span>Load Shape...</span>
                </vaadin-button>
                <vaadin-button
                    theme="contrast primary"
                    @click="${this._loadPartial}">
                    <span>Load Partial...</span>
                </vaadin-button>
                <vaadin-checkbox
                    ?checked=${HelpersExports.isVisible(this._shapeWireframeVisibility)}
                    @change="${this._shapeWireframeVisibilityInputChanged}"
                    checked>
                    <span>Show Shape Wireframe</span>
                </vaadin-checkbox>
                <vaadin-checkbox
                    ?checked=${HelpersExports.isVisible(this._partialWireframeVisibility)}
                    @change="${this._partialWireframeVisibilityInputChanged}"
                    checked>
                    <span>Show Partial Wireframe</span>
                </vaadin-checkbox>            
            </side-bar-collapsable-section>         
            <side-bar-collapsable-section
                caption="Viewports">
                <vaadin-dropdown-menu
                    label="Split Orientation"
                    value="${this._splitOrientation}"
                    @change="${this._splitOrientationInputChanged}">
                    <template>
                    <vaadin-list-box>
                        <vaadin-item
                            value="${EnumsExports.SplitOrientation.HORIZONTAL}">
                            <span>${HelpersExports.splitOrientationText(EnumsExports.SplitOrientation.HORIZONTAL)}</span>
                        </vaadin-item>
                        <vaadin-item
                            value="${EnumsExports.SplitOrientation.VERTICAL}">
                            <span>${HelpersExports.splitOrientationText(EnumsExports.SplitOrientation.VERTICAL)}</span>
                        </vaadin-item>
                    </vaadin-list-box>
                    </template>
                </vaadin-dropdown-menu>
                <side-bar-color-picker
                    caption="Shape Viewport Color"
                    color="${this._shapeViewportColor}"
                    @color-changed="${this._shapeViewportColorInputChanged}">
                </side-bar-color-picker>
                <side-bar-color-picker
                    caption="Partial Viewport Color"
                    color="${this._partialViewportColor}"
                    @color-changed="${this._partialViewportColorInputChanged}">
                </side-bar-color-picker>
                <side-bar-color-picker
                    caption="Shape Color"
                    color="${this._shapeColor}"
                    @color-changed="${this._shapeColorInputChanged}">
                </side-bar-color-picker>
                <side-bar-color-picker
                    caption="Partial Color"
                    color="${this._partialColor}"
                    @color-changed="${this._partialColorInputChanged}">
                </side-bar-color-picker>
                <vaadin-checkbox
                    ?checked=${HelpersExports.isVisible(this._shapeViewVisibility)}
                    @change="${this._shapeViewVisibilityInputChanged}">
                    <span>Show Shape View</span>
                </vaadin-checkbox>
                <vaadin-checkbox
                    ?checked=${HelpersExports.isVisible(this._partialViewVisibility)}
                    @change="${this._partialViewVisibilityInputChanged}">
                    <span>Show Partial View</span>
                </vaadin-checkbox>
            </side-bar-collapsable-section>
            <side-bar-collapsable-section
                caption="Solver">
                <paper-toggle-button
                    class="solver"
                    ?checked="${HelpersExports.isSolverOn(this.solverState)}"
                    ?disabled="${!HelpersExports.isShapeLoaded(this.shapeState) || !HelpersExports.isPartialLoaded(this.partialState)}"
                    @change="${this._solverStateInputChanged}">
                    <span>${HelpersExports.solverStateText(this.solverState)}</span>
                </paper-toggle-button>
            </side-bar-collapsable-section>
            <side-bar-collapsable-section
                caption="Numeric Properties">
                <side-bar-parameter-input
                    id="threshold"
                    increase-key="a"
                    decrease-key="s"
                    value="${this._sigmaThreshold}"
                    min="0"
                    max="1"
                    step="0.0001"
                    label="Sigma Threshold"
                    @value-changed="${this._sigmaThresholdInputChanged}">
                </side-bar-parameter-input>
            </side-bar-collapsable-section>
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
            selectedEdgeSeamlessAngleWeight: {
                type: Number,
                attribute: 'selected-edge-seamless-angle-weight'
            },
            selectedEdgeSeamlessLengthWeight: {
                type: Number,
                attribute: 'selected-edge-seamless-length-weight'
            },
            singularityWeight: {
                type: Number,
                attribute: 'singularity-weight'
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
        this.requestUpdate('shapeColor', oldValue);
    }

    get shapeColor() {
        return this._shapeColor;
    }

    set partialColor(value) {
        const oldValue = this._partialColor;
        this._partialColor = value;
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

    set shapeFatWireframeVisibility(value) {
        const oldValue = this._shapeFatWireframeVisibility;
        this._shapeFatWireframeVisibility = value;
        this.requestUpdate('shapeFatWireframeVisibility', oldValue);
    }

    get shapeFatWireframeVisibility() {
        return this._shapeFatWireframeVisibility;        
    }

    set partialFatWireframeVisibility(value) {
        const oldValue = this._partialFatWireframeVisibility;
        this._partialFatWireframeVisibility = value;
        this.requestUpdate('partialFatWireframeVisibility', oldValue);
    }

    get partialFatWireframeVisibility() {
        return this._partialFatWireframeVisibility;        
    }  

    set shapeViewVisibility(value) {
        const oldValue = this._shapeViewVisibility;
        this._shapeViewVisibility = value;
        this.requestUpdate('shapeViewVisibility', oldValue);
    }

    get shapeViewVisibility() {
        return this._shapeViewVisibility;
    }

    set partialViewVisibility(value) {
        const oldValue = this._partialViewVisibility;
        this._partialViewVisibility = value;
        this.requestUpdate('partialViewVisibility', oldValue);        
    }

    get partialViewVisibility() {
        return this._partialViewVisibility;
    }

    set autocutsWeight(value) {
        const oldValue = this._autocutsWeight;
        this._autocutsWeight = value;
        this.requestUpdate('autocutsWeight', oldValue);
    }

    get autocutsWeight() {
        return this._autocutsWeight;
    }    

    set delta(value) {
        const oldValue = this._delta;
        this._delta = value;
        this.requestUpdate('delta', oldValue);
    }

    get delta() {
        return this._delta;
    }

    set lambda(value) {
        const oldValue = this._lambda;
        this._lambda = value;
        this.requestUpdate('lambda', oldValue);
    }

    get lambda() {
        return this._lambda;
    }

    set zeta(value) {
        const oldValue = this._zeta;
        this._zeta = value;
        this.requestUpdate('zeta', oldValue);
    }

    get zeta() {
        return this._zeta;
    }    

    set seamlessWeight(value) {
        const oldValue = this._seamlessWeight;
        this._seamlessWeight = value;
        this.requestUpdate('seamlessWeight', oldValue);
    }

    get seamlessWeight() {
        return this._seamlessWeight;
    }

    set selectedEdgeSeamlessAngleWeight(value) {
        const oldValue = this._selectedEdgeSeamlessAngleWeight;
        this._selectedEdgeSeamlessAngleWeight = value;
        this.requestUpdate('selectedEdgeSeamlessAngleWeight', oldValue);
    }

    get selectedEdgeSeamlessAngleWeight() {
        return this._selectedEdgeSeamlessAngleWeight;
    }

    set selectedEdgeSeamlessLengthWeight(value) {
        const oldValue = this._selectedEdgeSeamlessLengthWeight;
        this._selectedEdgeSeamlessLengthWeight = value;
        this.requestUpdate('selectedEdgeSeamlessLengthWeight', oldValue);
    }

    get selectedEdgeSeamlessLengthWeight() {
        return this._selectedEdgeSeamlessLengthWeight;
    }

    set singularityWeight(value) {
        const oldValue = this._singularityWeight;
        this._singularityWeight = value;
        this.requestUpdate('singularityWeight', oldValue);
    }

    get singularityWeight() {
        return this._singularityWeight;
    }

    set singularityInterval(value) {
        const oldValue = this._singularityInterval;
        this._singularityInterval = value;
        this.requestUpdate('singularityInterval', oldValue);
    }

    get singularityInterval() {
        return this._singularityInterval;
    }   
    
    set positionWeight(value) {
        const oldValue = this._positionWeight;
        this._positionWeight = value;
        this.requestUpdate('positionWeight', oldValue);
    }

    get positionWeight() {
        return this._positionWeight;
    }

    set sigmaThreshold(value) {
        const oldValue = this._sigmaThreshold;
        this._sigmaThreshold = value;
        this.requestUpdate('sigmaThreshold', oldValue);
    }

    get sigmaThreshold() {
        return this._sigmaThreshold;
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
        const oldValue = this._solverState;
        this._solverState = value;
        this.requestUpdate('solverState', oldValue);
    }

    get solverState() {
        return this._solverState;
    }

    set shapeFilename(value) {
        const oldValue = this._shapeFilename;
        this._shapeFilename = value;
        this.requestUpdate('shapeFilename', oldValue);
    }

    get shapeFilename() {
        return this._shapeFilename;
    }

    set partialFilename(value) {
        const oldValue = this._partialFilename;
        this._partialFilename = value;
        this.requestUpdate('partialFilename', oldValue);
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

    set moduleFilename(value) {
        const oldValue = this._moduleFilename; 
        this._moduleFilename = value;
        this.requestUpdate('moduleFilename', oldValue);
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

    set objectivePropertiesVisualData(value) {
        const oldValue = this._objectivePropertiesVisualData;
        this._objectivePropertiesVisualData = value;
        this.requestUpdate('objectivePropertiesVisualData', oldValue);
    }

    get objectivePropertiesVisualData() {
        return this._objectivePropertiesVisualData;
    }

    set algorithmType(value) {
        const oldValue = this._algorithmType;
        this._algorithmType = value;
        this.requestUpdate('algorithmType', oldValue);
    }

    get algorithmType() {
        return this._algorithmType;
    }

    /**
     * Element life-cycle callbacks
     */

     firstUpdated() {
        // TODO: call removeEventListener for the 'focus' events
        // https://stackoverflow.com/questions/22280139/prevent-space-button-from-triggering-any-other-button-click-in-jquery
        this.shadowRoot.querySelectorAll('vaadin-button').forEach(function (item) {
            item.addEventListener('focus', function () {
                this.blur();
            })
        });

        this.shadowRoot.querySelectorAll('vaadin-checkbox').forEach(function (item) {
            item.addEventListener('focus', function () {
                this.blur();
            })
        });  
     }

    connectedCallback() {
        super.connectedCallback();

        this._keyDownBoundHandler = this._keyDownHandler.bind(this);
        window.addEventListener('keydown', this._keyDownBoundHandler);
    }
    
    disconnectedCallback() {
        window.removeEventListener('keydown', this._keyDownBoundHandler);
        super.disconnectedCallback();
    }

    _keyDownHandler(e) {
        switch(e.which) {
            case 32:
                switch(this.solverState) {
                    case EnumsExports.SolverState.OFF:
                        store.dispatch(ActionsExports.resumeSolver());
                        break;
                    case EnumsExports.SolverState.ON:
                        store.dispatch(ActionsExports.pauseSolver());
                        break;
                }
                break;
        }
    }

    _loadShape() {
        const { dialog } = require('electron').remote;
        let files = dialog.showOpenDialogSync({
            properties: ['openFile'],
            filters: [
                {
                    name: '3D Model Files',
                    extensions: ['obj', 'off']
                },
                {
                    name: 'All Files',
                    extensions: ['*']
                }
            ]
        });

        store.dispatch(ActionsExports.loadShapeFile(files[0])); 
    }

    _loadPartial() {
        const { dialog } = require('electron').remote;
        let files = dialog.showOpenDialogSync({
            properties: ['openFile'],
            filters: [
                {
                    name: '3D Model Files',
                    extensions: ['obj', 'off']
                },
                {
                    name: 'All Files',
                    extensions: ['*']
                }
            ]
        });

        store.dispatch(ActionsExports.loadPartialFile(files[0])); 
    }

    _splitOrientationInputChanged(e) {
        store.dispatch(ActionsExports.setSplitOrientation(e.srcElement.value));    
    }

    _shapeViewportColorInputChanged(e) {
        store.dispatch(ActionsExports.setShapeViewportColor(e.detail.color));
    }

    _partialViewportColorInputChanged(e) {
        store.dispatch(ActionsExports.setPartialViewportColor(e.detail.color));
    }

    _shapeColorInputChanged(e) {
        store.dispatch(ActionsExports.setShapeColor(e.detail.color));
    }

    _partialColorInputChanged(e) {
        store.dispatch(ActionsExports.setPartialColor(e.detail.color));
    }

    _highlightedFaceColorInputChanged(e) {
        store.dispatch(ActionsExports.setHighlightedFaceColor(e.detail.color));
    }

    _draggedFaceColorInputChanged(e) {
        store.dispatch(ActionsExports.setDraggedFaceColor(e.detail.color));
    }

    _fixedFaceColorInputChanged(e) {
        store.dispatch(ActionsExports.setFixedFaceColor(e.detail.color));
    }

    _highlightedEdgeColorInputChanged(e) {
        store.dispatch(ActionsExports.setHighlightedEdgeColor(e.detail.color));
    }

    _editedEdgeColorInputChanged(e) {
        store.dispatch(ActionsExports.setEditedEdgeColor(e.detail.color));
    }

    _autoquadsEnabledInputChanged(e) {
        if(e.srcElement.checked) {
            store.dispatch(ActionsExports.setAlgorithmType(EnumsExports.AlgorithmType.AUTOQUADS));
        } else {
            store.dispatch(ActionsExports.setAlgorithmType(EnumsExports.AlgorithmType.AUTOCUTS));
        }
    }

    _shapeWireframeVisibilityInputChanged(e) {
        if(e.srcElement.checked) {
            store.dispatch(ActionsExports.showShapeWireframe());
        } else {
            store.dispatch(ActionsExports.hideShapeWireframe());
        }
    }

    _partialWireframeVisibilityInputChanged(e) {
        if(e.srcElement.checked) {
            store.dispatch(ActionsExports.showPartialWireframe());
        } else {
            store.dispatch(ActionsExports.hidePartialWireframe());
        }
    }

    _shapeFatWireframeVisibilityInputChanged(e) {
        if(e.srcElement.checked) {
            store.dispatch(ActionsExports.showShapeFatWireframe());
        } else {
            store.dispatch(ActionsExports.hideShapeFatWireframe());
        }
    }

    _partialFatWireframeVisibilityInputChanged(e) {
        if(e.srcElement.checked) {
            store.dispatch(ActionsExports.showPartialFatWireframe());
        } else {
            store.dispatch(ActionsExports.hidePartialFatWireframe());
        }
    }   

    _shapeViewVisibilityInputChanged(e) {
        if(e.srcElement.checked) {
            store.dispatch(ActionsExports.showShapeView());
        } else {
            store.dispatch(ActionsExports.hideShapeView());
        }  
    }

    _partialViewVisibilityInputChanged(e) {
        if(e.srcElement.checked) {
            store.dispatch(ActionsExports.showPartialView());
        } else {
            store.dispatch(ActionsExports.hidePartialView());
        }
    }

    _unitGridVisibilityInputChanged(e) {
        if(e.srcElement.checked) {
            store.dispatch(ActionsExports.showUnitGrid());
        } else {
            store.dispatch(ActionsExports.hideUnitGrid());
        }
    }

    _optimizationDataMonitorVisibilityInputChanged(e) {
        if(e.srcElement.checked) {
            store.dispatch(ActionsExports.showOptimizationDataMonitor());
        } else {
            store.dispatch(ActionsExports.hideOptimizationDataMonitor());
        }
    }

    _autocutsWeightInputChanged(e) {
        store.dispatch(ActionsExports.setAutocutsWeight(e.srcElement.value)); 
    }

    _deltaInputChanged(e) {
        store.dispatch(ActionsExports.setDelta(e.srcElement.value)); 
    }

    _lambdaInputChanged(e) {
        store.dispatch(ActionsExports.setLambda(e.srcElement.value)); 
    }

    _zetaInputChanged(e) {
        store.dispatch(ActionsExports.setZeta(e.srcElement.value)); 
    }

    _seamlessWeightInputChanged(e) {
        store.dispatch(ActionsExports.setSeamlessWeight(e.srcElement.value)); 
    }

    _selectedEdgeSeamlessAngleWeightInputChanged(e) {
        store.dispatch(ActionsExports.setSelectedEdgeSeamlessAngleWeight(e.srcElement.value)); 
    }

    _selectedEdgeSeamlessLengthWeightInputChanged(e) {
        store.dispatch(ActionsExports.setSelectedEdgeSeamlessLengthWeight(e.srcElement.value)); 
    }

    _singularityWeightInputChanged(e) {
        store.dispatch(ActionsExports.setSingularityWeight(e.srcElement.value)); 
    }

    _singularityIntervalInputChanged(e) {
        store.dispatch(ActionsExports.setSingularityInterval(e.srcElement.value)); 
    }   

    _positionWeightInputChanged(e) {
        store.dispatch(ActionsExports.setPositionWeight(e.srcElement.value)); 
    }

    _sigmaThresholdInputChanged(e) {
        store.dispatch(ActionsExports.setSigmaThreshold(e.srcElement.value)); 
    }

    _solverStateInputChanged(e) {
        if(e.srcElement.checked) {
            store.dispatch(ActionsExports.resumeSolver());
        } else {
            store.dispatch(ActionsExports.pauseSolver());
        }
    }

    _gridHorizontalColorInputChanged(e) {
        store.dispatch(ActionsExports.setGridHorizontalColor(e.detail.color));
    }

    _gridVerticalColorInputChanged(e) {
        store.dispatch(ActionsExports.setGridVerticalColor(e.detail.color));
    }

    _gridBackgroundColor1InputChanged(e) {
        store.dispatch(ActionsExports.setGridBackgroundColor1(e.detail.color));
    }

    _gridBackgroundColor2InputChanged(e) {
        store.dispatch(ActionsExports.setGridBackgroundColor2(e.detail.color));
    }

    _gridSizeInputChanged(e) {
        store.dispatch(ActionsExports.setGridSize(e.srcElement.value));
    }

    _gridTextureSizeInputChanged(e) {
        store.dispatch(ActionsExports.setGridTextureSize(e.srcElement.value));
    }

    _gridLineWidthInputChanged(e) {
        store.dispatch(ActionsExports.setGridLineWidth(e.srcElement.value));
    }

    _objectiveFunctionVisualPropertyColorInputChanged(e) {
        store.dispatch(ActionsExports.setObjectiveFunctionPropertyColor(e.detail.objectiveFunctionId, e.detail.propertyId, e.detail.color));
    }

    _objectiveFunctionVisualPropertyVisibilityInputChanged(e) {
        store.dispatch(ActionsExports.setObjectiveFunctionPropertyVisibility(e.detail.objectiveFunctionId, e.detail.propertyId, e.detail.selected ? EnumsExports.Visibility.VISIBLE : EnumsExports.Visibility.HIDDEN)); 
    }    
}

customElements.define('autoquads-side-bar', AutoquadsSideBar);