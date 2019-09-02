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
                caption="Model Options">
                <vaadin-button
                    theme="contrast primary"
                    @click="${this._loadModel}">
                    <span>Load Model...</span>
                </vaadin-button>
                <vaadin-checkbox
                    @change="${this._wireframeVisibilityChanged}"
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
                    label="Lambda">
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
                    label="Seamless Weight">
                </side-bar-parameter-input>
                <side-bar-parameter-input
                    id="position-weight"
                    value="${this.positionWeight}"
                    min="0"
                    max="10000"
                    step="1"
                    label="Position Weight">
                </side-bar-parameter-input>
            </side-bar-collapsable-section>
            <side-bar-collapsable-section
                caption="Solver">
                <paper-toggle-button
                    class="solver"
                    checked="${ReducerExports.isSolverOn(this._solverState)}"
                    disabled
                    @change="${this._solverStateChanged}">
                    <span>${ReducerExports.solverStateText(this._solverState)}</span>
                </paper-toggle-button>
            </side-bar-collapsable-section>
            <side-bar-collapsable-section
                caption="Interaction">
                <side-bar-color-picker
                    caption="Highlighted Face Color"
                    color="${this._highlightedFaceColor}"
                    @color-changed="${this._highlightedFaceColorChanged}">
                </side-bar-color-picker>
                <side-bar-color-picker
                    caption="Dragged Face Color"
                    color="${this._draggedFaceColor}"
                    @color-changed="${this._draggedFaceColorChanged}">
                </side-bar-color-picker>
                <side-bar-color-picker
                    caption="Fixed Face Color"
                    color="${this._fixedFaceColor}"
                    @color-changed="${this._fixedFaceColorChanged}">
                </side-bar-color-picker>
                <side-bar-color-picker
                    caption="Vertex Energy Color"
                    color="${this._vertexEnergyColor}"
                    @color-changed="${this._vertexEnergyColorChanged}">
                </side-bar-color-picker>
                <vaadin-select
                    label="Vertex Energy Type"
                    value="${this._vertexEnergyType}"
                    @change=${this._vertexEnergyTypeChanged}>
                    <template>
                        <vaadin-list-box>
                            <vaadin-item
                                value="${ReducerExports.EnergyType.SEPERATION}">
                                <span>${ReducerExports.energyTypeText(ReducerExports.EnergyType.SEPERATION)}</span>
                            </vaadin-item>
                            <vaadin-item
                                value="${ReducerExports.EnergyType.SEAMLESS}">
                                <span>${ReducerExports.energyTypeText(ReducerExports.EnergyType.SEAMLESS)}</span>
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
                    @change="${this._splitOrientationChanged}">
                    <template>
                    <vaadin-list-box>
                        <vaadin-item
                            value="${ReducerExports.SplitOrientation.HORIZONTAL}">
                            <span>${ReducerExports.splitOrientationText(ReducerExports.SplitOrientation.HORIZONTAL)}</span>
                        </vaadin-item>
                        <vaadin-item
                            value="${ReducerExports.SplitOrientation.VERTICAL}">
                            <span>${ReducerExports.splitOrientationText(ReducerExports.SplitOrientation.VERTICAL)}</span>
                        </vaadin-item>
                    </vaadin-list-box>
                    </template>
                </vaadin-dropdown-menu>
                <side-bar-color-picker
                    caption="Model Viewport Color"
                    color="${this._modelViewportColor}"
                    @color-changed="${this._modelViewportColorChanged}">
                </side-bar-color-picker>
                <side-bar-color-picker
                    caption="Soup Viewport Color"
                    color="${this._soupViewportColor}"
                    @color-changed="${this._soupViewportColorChanged}">
                </side-bar-color-picker>
                <side-bar-color-picker
                    caption="Model Color"
                    color="${this._modelColor}"
                    @color-changed="${this._modelColorChanged}">
                </side-bar-color-picker>
                <side-bar-color-picker
                    caption="Soup Color"
                    color="${this._soupColor}"
                    @color-changed="${this._soupColorChanged}">
                </side-bar-color-picker>
                <vaadin-checkbox
                    ?checked=${ReducerExports.isVisible(this._modelViewVisibility)}
                    @change="${this._modelViewVisibilityChanged}">
                    <span>Show Model View</span>
                </vaadin-checkbox>
                <vaadin-checkbox
                    ?checked=${ReducerExports.isVisible(this._soupViewVisibility)}
                    @change="${this._soupViewVisibilityChanged}">
                    <span>Show Soup View</span>
                </vaadin-checkbox>
                <vaadin-checkbox
                    ?checked=${ReducerExports.isVisible(this._unitGridVisibility)}
                    @change="${this._unitGridVisibilityChanged}">
                    <span>Show Unit Grid</span>
                </vaadin-checkbox>
                <vaadin-checkbox
                    ?checked=${ReducerExports.isVisible(this._optimizationDataMonitorVisibility)}
                    @change="${this._optimizationDataMonitorVisibilityChanged}">
                    <span>Show Optimization Data Monitor</span>
                </vaadin-checkbox>
            </side-bar-collapsable-section>
            <side-bar-collapsable-section
                caption="Grid">
                <side-bar-color-picker
                    color="${this._gridHorizontalColor}"
                    caption="Horizontal Color"
                    @color-changed="${this._gridHorizontalColorChanged}">
                </side-bar-color-picker>
                <side-bar-color-picker
                    color="${this._gridVerticalColor}"
                    caption="Vertical Color"
                    @color-changed="${this._gridVerticalColorChanged}"></side-bar-color-picker>
                <side-bar-color-picker
                    color="${this._gridBackgroundColor1}"
                    caption="Background Color 1"
                    @color-changed="${this._gridBackgroundColor1Changed}">
                </side-bar-color-picker>
                <side-bar-color-picker
                    caption="Background Color 2"
                    color="${this._gridBackgroundColor2}"
                    @color-changed="${this._gridBackgroundColor2Changed}">
                </side-bar-color-picker>
                <vaadin-number-field
                    label="Grid Unit Size (2^exp)"
                    value="${this._gridSize}"
                    min="0"
                    step="1"
                    always-float-label
                    @change="${this._gridSizeChanged}">
                </vaadin-number-field>
                <vaadin-number-field
                    label="Grid Texture Size (2^exp)"
                    value="${this._gridTextureSize}"
                    min="0"
                    step="1"
                    always-float-label
                    @change="${this._gridTextureSizeChanged}">
                </vaadin-number-field>
                <vaadin-number-field
                    label="Grid Line Width"
                    value="${this._gridLineWidth}"
                    min="0"
                    step="1"
                    always-float-label
                    @change="${this._gridLineWidthChanged}">
                </vaadin-number-field>
            </side-bar-collapsable-section>
        `;
    }

    static get properties() {
        return {
            lambda: {
                type: Number,
                attribute: 'lambda'
            },
            delta: {
                type: Number,
                attribute: 'delta'
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
            showOptimizationDataMonitor: {
                type: Boolean,
                attribute: 'show-optimization-data-monitor'
            },
            solverOn: {
                type: Boolean,
                attribute: 'solver-on'
            },
            editingTool: {
                type: String,
                attribute: 'editing-tool'
            }
        };
    }

    stateChanged(state) {
        this._splitOrientation = state.splitOrientation;
        this._modelViewportColor = state.modelViewportColor;
        this._soupViewportColor = state.soupViewportColor;
        this._modelColor = state.modelColor;
        this._soupColor = state.soupColor;
        this._wireframeVisibility = state.wireframeVisibility;
        this._modelViewVisibility = state.modelViewVisibility;
        this._soupViewVisibility = state.soupViewVisibility;
        this._delta = state.delta;
        this._lambda = state.lambda;
        this._seamlessWeight = state.seamlessWeight;
        this._positionWeight = state.positionWeight;
        this._gridHorizontalColor = state.gridHorizontalColor;
        this._gridVerticalColor = state.gridVerticalColor;
        this._gridBackgroundColor1 = state.gridBackgroundColor1;
        this._gridBackgroundColor2 = state.gridBackgroundColor2;
        this._highlightedFaceColor = state.highlightedFaceColor;
        this._draggedFaceColor = state.draggedFaceColor;
        this._fixedFaceColor = state.fixedFaceColor;
        this._vertexEnergyColor = state.vertexEnergyColor;
        this._vertexEnergyType = state.vertexEnergyType;
        this._gridSize = state.gridSize;
        this._gridTextureSize = state.gridTextureSize;
        this._gridLineWidth = state.gridLineWidth;
        this._unitGridVisibility = state.unitGridVisibility;
        this._soupViewGridTextureVisibility = state.soupViewGridTextureVisibility;
        this._optimizationDataMonitorVisibility = state.optimizationDataMonitorVisibility;
        this._solverState = state.solverState;
        this._modelFilename = state.modelFilename;
    }

    constructor() {
        super();
        // this.solverText = "Off";
        // this.lambda = 0.1;
        // this.delta = 0.9;
        // this.seamlessWeight = 0;
        // this.positionWeight = 0;
        // this.showOptimizationDataMonitor = true; 
        // this.solverOn = false;
        // this.editingTool = 'camera';
        // this._highlightedFaceColor = 'rgb(255,0,0)';
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

    _getSolverText() {
        if (this.solver) {
            return 'On';
        }

        return 'Off';
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

        store.dispatch(ActionsExports.changeModelFilename(files[0])); 
    }

    _reloadModule() {
        PubSub.publish('reload-module');
    }

    _splitOrientationChanged(e) {
        store.dispatch(ActionsExports.changeSplitOrientation(e.srcElement.value));    
    }

    _modelViewportColorChanged(e) {
        store.dispatch(ActionsExports.changeModelViewportColor(e.detail.color));
    }

    _soupViewportColorChanged(e) {
        store.dispatch(ActionsExports.changeSoupViewportColor(e.detail.color));
    }

    _modelColorChanged(e) {
        store.dispatch(ActionsExports.changeModelColor(e.detail.color));
    }

    _soupColorChanged(e) {
        store.dispatch(ActionsExports.changeSoupColor(e.detail.color));
    }

    _highlightedFaceColorChanged(e) {
        store.dispatch(ActionsExports.changeHighlightedFaceColor(e.detail.color));
    }

    _draggedFaceColorChanged(e) {
        store.dispatch(ActionsExports.changeDraggedFaceColor(e.detail.color));
    }

    _fixedFaceColorChanged(e) {
        store.dispatch(ActionsExports.changeFixedFaceColor(e.detail.color));
    }

    _vertexEnergyColorChanged(e) {
        store.dispatch(ActionsExports.changeVertexEnergyColor(e.detail.color));
    }

    _vertexEnergyTypeChanged(e) {
        store.dispatch(ActionsExports.changeVertexEnergyType(e.srcElement.value));
    }

    _wireframeVisibilityChanged(e) {
        store.dispatch(ActionsExports.changeWireframeVisiblity(ReducerExports.visibilityFromBool(e.srcElement.checked)));
    }

    _modelViewVisibilityChanged(e) {
        store.dispatch(ActionsExports.changeModelViewVisiblity(ReducerExports.visibilityFromBool(e.srcElement.checked)));        
    }

    _soupViewVisiblityChanged(e) {
        store.dispatch(ActionsExports.changeSoupViewVisiblity(ReducerExports.visibilityFromBool(e.srcElement.checked))); 
    }

    _unitGridVisiblityChanged(e) {
        store.dispatch(ActionsExports.changeUnitGridVisiblity(ReducerExports.visibilityFromBool(e.srcElement.checked))); 
    }

    _optimizationDataMonitorVisibilityChanged(e) {
        store.dispatch(ActionsExports.changeOptimizationDataMonitorVisibility(ReducerExports.visibilityFromBool(e.srcElement.checked))); 
    }

    _deltaChanged(e) {
        store.dispatch(ActionsExports.changeDelta(e.srcElement.value)); 
    }

    _lambdaChanged(e) {
        store.dispatch(ActionsExports.changeLambda(e.srcElement.value)); 
    }

    _seamlessWeightChanged(e) {
        store.dispatch(ActionsExports.changeSeamlessWeight(e.srcElement.value)); 
    }

    _positionWeightChanged(e) {
        store.dispatch(ActionsExports.changePositionWeight(e.srcElement.value)); 
    }

    _solverStateChanged(e) {
        store.dispatch(ActionsExports.changeSolverState(ReducerExports.solverStateFromBool(e.srcElement.checked))); 
    }

    _gridHorizontalColorChanged(e) {
        store.dispatch(ActionsExports.changeGridHorizontalColor(e.detail.color));
    }

    _gridVerticalColorChanged(e) {
        store.dispatch(ActionsExports.changeGridVerticalColor(e.detail.color));
    }

    _gridBackgroundColor1Changed(e) {
        store.dispatch(ActionsExports.changeBackgroundColor1(e.detail.color));
    }

    _gridBackgroundColor2Changed(e) {
        store.dispatch(ActionsExports.changeBackgroundColor2(e.detail.color));
    }

    _gridSizeChanged(e) {
        store.dispatch(ActionsExports.changeGridSize(e.srcElement.value));
    }

    _gridTextureSizeChanged(e) {
        store.dispatch(ActionsExports.changeGridTextureSize(e.srcElement.value));
    }

    _gridLineWidthChanged(e) {
        store.dispatch(ActionsExports.changeGridLineWidth(e.srcElement.value));
    }
}

customElements.define('autoquads-side-bar', AutoquadsSideBar);