// Web Modules Imports
import { LitElement, html, css } from '../../web_modules/lit-element.js';
import { PubSub } from '../../web_modules/pubsub-js.js';
import '../../web_modules/@polymer/iron-icons.js';
import '../../web_modules/@polymer/iron-collapse.js';
import '../../web_modules/@polymer/paper-toggle-button.js';
import '../../web_modules/@vaadin/vaadin-button.js';
import '../../web_modules/@vaadin/vaadin-combo-box.js';
import '../../web_modules/@vaadin/vaadin-checkbox.js';
import '../../web_modules/@vaadin/vaadin-text-field/vaadin-number-field.js';
import '../../web_modules/@vaadin/vaadin-select.js';

// Components Imports
import { SideBar } from '../side-bar/side-bar.js';
import '../side-bar-collapsable-section/side-bar-collapsable-section.js';
import '../side-bar-parameter-input/side-bar-parameter-input.js';
import '../side-bar-color-picker/side-bar-color-picker.js';
import * as ReducerExports from '../../redux/reducer.js';
import * as ActionsExports from '../../redux/actions.js';
import * as EnumsExports from '../../redux/enums.js';
import * as HelpersExports from '../../redux/helpers.js';
import { store } from '../../redux/store.js';
import { P } from '../../web_modules/common/paper-ripple-behavior-a07b40e2.js';

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
                caption="Model Options">
                <vaadin-button
                    theme="contrast primary"
                    @click="${this._loadModel}">
                    <span>Load Model...</span>
                </vaadin-button>
                <vaadin-checkbox
                    ?checked=${HelpersExports.isVisible(this._wireframeVisibility)}
                    @change="${this._wireframeVisibilityInputChanged}"
                    checked>
                    <span>Show Wireframe</span>
                </vaadin-checkbox>   
            </side-bar-collapsable-section>
            <side-bar-collapsable-section
                caption="Energy Parameters">
                <side-bar-parameter-input
                    id="lambda"
                    increase-key="d"
                    decrease-key="a"
                    value="${this._lambda}"
                    min="0"
                    max="1"
                    step="0.01"
                    label="Lambda"
                    @value-changed="${this._lambdaInputChanged}">
                </side-bar-parameter-input>
                <side-bar-parameter-input
                    id="delta"
                    increase-key="w"
                    decrease-key="s"
                    value="${this._delta}"
                    min="0"
                    max="1"
                    step="2"
                    label="Delta"
                    @value-changed="${this._deltaInputChanged}"
                    is-exponential>
                </side-bar-parameter-input>
                <side-bar-parameter-input
                    id="seamless-weight"
                    increase-key="c"
                    decrease-key="x"
                    value="${this._seamlessWeight}"
                    min="0"
                    max="2"
                    step="0.01"
                    label="Seamless Weight"
                    @value-changed="${this._seamlessWeightInputChanged}">
                </side-bar-parameter-input>
                <side-bar-parameter-input
                    id="position-weight"
                    value="${this._positionWeight}"
                    min="0"
                    max="10000"
                    step="1"
                    label="Position Weight"
                    @value-changed="${this._positionWeightInputChanged}">
                </side-bar-parameter-input>
            </side-bar-collapsable-section>
            <side-bar-collapsable-section
                caption="Solver">
                <paper-toggle-button
                    class="solver"
                    checked="${HelpersExports.isSolverOn(this._solverState)}"
                    disabled
                    @change="${this._solverStateInputChanged}">
                    <span>${HelpersExports.solverStateText(this._solverState)}</span>
                </paper-toggle-button>
            </side-bar-collapsable-section>
            <side-bar-collapsable-section
                caption="Interaction">
                <side-bar-color-picker
                    caption="Highlighted Face Color"
                    color="${this._highlightedFaceColor}"
                    @color-changed="${this._highlightedFaceColorInputChanged}">
                </side-bar-color-picker>
                <side-bar-color-picker
                    caption="Dragged Face Color"
                    color="${this._draggedFaceColor}"
                    @color-changed="${this._draggedFaceColorInputChanged}">
                </side-bar-color-picker>
                <side-bar-color-picker
                    caption="Fixed Face Color"
                    color="${this._fixedFaceColor}"
                    @color-changed="${this._fixedFaceColorInputChanged}">
                </side-bar-color-picker>
                <side-bar-color-picker
                    caption="Vertex Energy Color"
                    color="${this._vertexEnergyColor}"
                    @color-changed="${this._vertexEnergyColorInputChanged}">
                </side-bar-color-picker>
                <vaadin-select
                    label="Vertex Energy Type"
                    value="${this._vertexEnergyType}"
                    @change=${this._vertexEnergyTypeInputChanged}>
                    <template>
                        <vaadin-list-box>
                            <vaadin-item
                                value="${EnumsExports.EnergyType.SEPERATION}">
                                <span>${HelpersExports.energyTypeText(EnumsExports.EnergyType.SEPERATION)}</span>
                            </vaadin-item>
                            <vaadin-item
                                value="${EnumsExports.EnergyType.SEAMLESS}">
                                <span>${HelpersExports.energyTypeText(EnumsExports.EnergyType.SEAMLESS)}</span>
                            </vaadin-item>
                        </vaadin-list-box>
                    </template>
                </vaadin-select>
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
                    caption="Model Viewport Color"
                    color="${this._modelViewportColor}"
                    @color-changed="${this._modelViewportColorInputChanged}">
                </side-bar-color-picker>
                <side-bar-color-picker
                    caption="Soup Viewport Color"
                    color="${this._soupViewportColor}"
                    @color-changed="${this._soupViewportColorInputChanged}">
                </side-bar-color-picker>
                <side-bar-color-picker
                    caption="Model Color"
                    color="${this._modelColor}"
                    @color-changed="${this._modelColorInputChanged}">
                </side-bar-color-picker>
                <side-bar-color-picker
                    caption="Soup Color"
                    color="${this._soupColor}"
                    @color-changed="${this._soupColorInputChanged}">
                </side-bar-color-picker>
                <vaadin-checkbox
                    ?checked=${HelpersExports.isVisible(this._modelViewVisibility)}
                    @change="${this._modelViewVisibilityCheckboxChanged}">
                    <span>Show Model View</span>
                </vaadin-checkbox>
                <vaadin-checkbox
                    ?checked=${HelpersExports.isVisible(this._soupViewVisibility)}
                    @change="${this._soupViewVisibilityInputChanged}">
                    <span>Show Soup View</span>
                </vaadin-checkbox>
                <vaadin-checkbox
                    ?checked=${HelpersExports.isVisible(this._unitGridVisibility)}
                    @change="${this._unitGridVisibilityInputChanged}">
                    <span>Show Unit Grid</span>
                </vaadin-checkbox>
                <vaadin-checkbox
                    ?checked=${HelpersExports.isVisible(this._optimizationDataMonitorVisibility)}
                    @change="${this._optimizationDataMonitorVisibilityInputChanged}">
                    <span>Show Optimization Data Monitor</span>
                </vaadin-checkbox>
            </side-bar-collapsable-section>
            <side-bar-collapsable-section
                caption="Grid">
                <side-bar-color-picker
                    color="${this._gridHorizontalColor}"
                    caption="Horizontal Color"
                    @color-changed="${this._gridHorizontalColorInputChanged}">
                </side-bar-color-picker>
                <side-bar-color-picker
                    color="${this._gridVerticalColor}"
                    caption="Vertical Color"
                    @color-changed="${this._gridVerticalColorInputChanged}"></side-bar-color-picker>
                <side-bar-color-picker
                    color="${this._gridBackgroundColor1}"
                    caption="Background Color 1"
                    @color-changed="${this._gridBackgroundColor1InputChanged}">
                </side-bar-color-picker>
                <side-bar-color-picker
                    caption="Background Color 2"
                    color="${this._gridBackgroundColor2}"
                    @color-changed="${this._gridBackgroundColor2InputChanged}">
                </side-bar-color-picker>
                <vaadin-number-field
                    label="Grid Unit Size (2^exp)"
                    value="${this._gridSize}"
                    min="0"
                    step="1"
                    always-float-label
                    @change="${this._gridSizeInputChanged}">
                </vaadin-number-field>
                <vaadin-number-field
                    label="Grid Texture Size (2^exp)"
                    value="${this._gridTextureSize}"
                    min="0"
                    step="1"
                    always-float-label
                    @change="${this._gridTextureSizeInputChanged}">
                </vaadin-number-field>
                <vaadin-number-field
                    label="Grid Line Width"
                    value="${this._gridLineWidth}"
                    min="0"
                    step="1"
                    always-float-label
                    @change="${this._gridLineWidthInputChanged}">
                </vaadin-number-field>
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
        this._modelColor = value;
    }

    get modelColor() {
        return this._modelColor;
    }

    set soupColor(value) {
        this._soupColor = value;
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
        this._vertexEnergyColor = value;
    }

    get vertexEnergyColor() {
        return this._vertexEnergyColor;
    }    

    set vertexEnergyType(value) {
        this._vertexEnergyType = value;
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
        this._solverState = value;
    }

    get solverState() {
        return this._solverState;
    } 

    set modelFilename(value) {
        this._modelFilename = value;
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
        this._moduleFilename = value;
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

     firstUpdated() {
        const { join } = require('path');
        let moduleFilename = join(appRoot, 'node-addon.node');
        store.dispatch(ActionsExports.loadModuleFile(moduleFilename)); 
     }

    connectedCallback() {
        super.connectedCallback();

        this.reloadModelSubscriptionToken = require('pubsub-js').subscribe('reload-model', (name, payload) => {
            let solverToggleButton = this.root.querySelector(".solver");
            solverToggleButton.disabled = false;
            document.addEventListener("keydown", this._documentKeyDown.bind(this));
        });

        this.addEventListener('side-bar-parameter-input-value-changed', this._sideBarParameterInputValueChanged);

        document.querySelectorAll("vaadin-button").forEach(function (item) {
            item.addEventListener('focus', function () {
                this.blur();
            })
        });

        document.querySelectorAll("vaadin-checkbox").forEach(function (item) {
            item.addEventListener('focus', function () {
                this.blur();
            })
        });        
    }
    
    disconnectedCallback() {
        // TODO: Remove event listeners
        super.disconnectedCallback();
    }

    _documentKeyDown(e) {
        if (e.which === 32) {
            e.stopPropagation();
            e.preventDefault();
            this.solver = !this.solver;
        } else if (e.which === 192) {
            e.stopPropagation();
            e.preventDefault();
            this.showOptimizationDataMonitor = !this.showOptimizationDataMonitor;
        }
    }

    _loadModel() {
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

        store.dispatch(ActionsExports.loadModelFile(files[0])); 
    }

    _splitOrientationInputChanged(e) {
        store.dispatch(ActionsExports.setSplitOrientation(e.srcElement.value));    
    }

    _modelViewportColorInputChanged(e) {
        store.dispatch(ActionsExports.setModelViewportColor(e.detail.color));
    }

    _soupViewportColorInputChanged(e) {
        store.dispatch(ActionsExports.setSoupViewportColor(e.detail.color));
    }

    _modelColorInputChanged(e) {
        store.dispatch(ActionsExports.setModelColor(e.detail.color));
    }

    _soupColorInputChanged(e) {
        store.dispatch(ActionsExports.setSoupColor(e.detail.color));
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

    _vertexEnergyColorInputChanged(e) {
        store.dispatch(ActionsExports.setVertexEnergyColor(e.detail.color));
    }

    _vertexEnergyTypeInputChanged(e) {
        store.dispatch(ActionsExports.setVertexEnergyType(e.srcElement.value));
    }

    _wireframeVisibilityInputChanged(e) {
        if(e.srcElement.checked) {
            store.dispatch(ActionsExports.showWireframe());
        } else {
            store.dispatch(ActionsExports.hideWireframe());
        }
    }

    _modelViewVisibilityInputChanged(e) {
        if(e.srcElement.checked) {
            store.dispatch(ActionsExports.showModelView());
        } else {
            store.dispatch(ActionsExports.hideModelView());
        }  
    }

    _soupViewVisiblityInputChanged(e) {
        if(e.srcElement.checked) {
            store.dispatch(ActionsExports.showSoupView());
        } else {
            store.dispatch(ActionsExports.hideSoupView());
        }
    }

    _unitGridVisiblityInputChanged(e) {
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

    _deltaInputChanged(e) {
        store.dispatch(ActionsExports.setDelta(e.srcElement.value)); 
    }

    _lambdaInputChanged(e) {
        store.dispatch(ActionsExports.setLambda(e.srcElement.value)); 
    }

    _seamlessWeightInputChanged(e) {
        store.dispatch(ActionsExports.setSeamlessWeight(e.srcElement.value)); 
    }

    _positionWeightInputChanged(e) {
        store.dispatch(ActionsExports.setPositionWeight(e.srcElement.value)); 
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
        store.dispatch(ActionsExports.setBackgroundColor1(e.detail.color));
    }

    _gridBackgroundColor2InputChanged(e) {
        store.dispatch(ActionsExports.setBackgroundColor2(e.detail.color));
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
}

customElements.define('autoquads-side-bar', AutoquadsSideBar);