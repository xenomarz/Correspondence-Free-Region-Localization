// Web Modules Imports
import { LitElement, html, css } from '../../web_modules/lit-element.js';
import { PubSub } from '../../web_modules/pubsub-js.js';
import '../../web_modules/@polymer/iron-icons.js';
import '../../web_modules/@polymer/iron-collapse.js';
import '../../web_modules/@polymer/paper-toggle-button.js';
import '../../web_modules/@vaadin/vaadin-button.js';
import '../../web_modules/@vaadin/vaadin-combo-box.js';
import '../../web_modules/@vaadin/vaadin-checkbox.js';
import '../../web_modules/@vaadin/vaadin-select.js';

// Components Imports
import { SideBar } from '../side-bar/side-bar.js';
import '../side-bar-collapsable-section/side-bar-collapsable-section.js';
import '../side-bar-parameter-input/side-bar-parameter-input.js';
import '../side-bar-color-picker/side-bar-color-picker.js';

import { 
    SplitOrientation,
    Visibility,
    EnergyType,
    SolverState,
    isVisible
} from '../../redux/reducer.js';

import * as Actions from '../../redux/actions.js';
import { store } from '../../redux/store.js';
import { connect } from '../../web_modules/pwa-helpers.js';

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
            <side-bar-collapsable-section caption="Model Options">
                <vaadin-button theme="contrast primary" @click="${this._loadModel}">
                    <span>Load Model...</span>
                </vaadin-button>
                <vaadin-number-field label="Number" value="1" min="1" max="10" step="0.0002" has-controls>
                </vaadin-number-field>            
            </side-bar-collapsable-section>

            <side-bar-collapsable-section caption="Energy Parameters">
                <side-bar-parameter-input id="lambda" increase-key="d" decrease-key="a" value="0.5" min="0" max="1" step="0.01" label="Lambda"></side-bar-parameter-input>
                <!-- <side-bar-parameter-input id="delta" is-exponential increase-key="w" decrease-key="s" value="0.5" min="0" max="1" step="2" label="Delta"></side-bar-parameter-input>
                <side-bar-parameter-input id="integer-weight" value="0" min="0" max="1" step="0.0001" label="Integer Weight"></side-bar-parameter-input>
                <side-bar-parameter-input id="integer-spacing" value="0" min="0" max="100" step="0.1" label="Integer Spacing"></side-bar-parameter-input>
                <side-bar-parameter-input id="seamless-weight" increase-key="c" decrease-key="x" value="0" min="0" max="2" step="0.01" label="Seamless Weight"></side-bar-parameter-input>
                <side-bar-parameter-input id="position-weight" value="100" min="0" max="10000" step="1" label="Position Weight"></side-bar-parameter-input> -->
            </side-bar-collapsable-section>

            <side-bar-collapsable-section caption="Solver">
                <paper-toggle-button class="solver" checked="${this.solverOn}" disabled>${this.solverText}</paper-toggle-button>
            </side-bar-collapsable-section>

            <side-bar-collapsable-section caption="Interaction">
                <side-bar-color-picker @color-changed="${this._highlightedFaceColorChanged}" color="#ff00d0" caption="Highlighted Face Color"></side-bar-color-picker>
                <side-bar-color-picker @color-changed="${this._draggedFaceColorChanged}" color="#0000ff" caption="Dragged Face Color"></side-bar-color-picker>
                <side-bar-color-picker @color-changed="${this._fixedFaceColorChanged}" color="#00ff00" caption="Fixed Face Color"></side-bar-color-picker>
                <side-bar-color-picker @color-changed="${this._vertexEnergyColorChanged}" color="#ff0000" caption="Vertex Energy Color"></side-bar-color-picker>
                <vaadin-select label="Vertex Energy" value="Seperation">
                    <template>
                        <vaadin-list-box>
                            <vaadin-item value="seperation">Seperation</vaadin-item>
                            <vaadin-item value="seamless">Seamless</vaadin-item>
                        </vaadin-list-box>
                    </template>
                </vaadin-select>
            </side-bar-collapsable-section>

            <side-bar-collapsable-section caption="Viewports" caption-size="3">
                <vaadin-dropdown-menu label="Split Orientation" value="${this._splitOrientation}" on-value-changed="_splitOrientationChanged">
                    <template>
                    <vaadin-list-box>
                        <vaadin-item value="vertical">Vertical</vaadin-item>
                        <vaadin-item value="horizontal">Horizontal</vaadin-item>
                    </vaadin-list-box>
                    </template>
                </vaadin-dropdown-menu>
                <side-bar-color-picker @color-changed="${this._modelViewportColorChanged}" color="${this._modelViewportColor}" caption="Model Viewport Color"></side-bar-color-picker>
                <side-bar-color-picker @color-changed="${this._suopViewportColorChanged}" color="${this._suopViewportColor}" caption="Suop Viewport Color"></side-bar-color-picker>
                <side-bar-color-picker @color-changed="${this._modelColorChanged}" color="${this._modelColor}" caption="Model Color"></side-bar-color-picker>
                <side-bar-color-picker @color-changed="${this._suopColorChanged}" color="${this._suopColor}" caption="Suop Color"></side-bar-color-picker>
                <vaadin-checkbox @change="_modelViewVisibilityChanged" ?checked=${isVisible(this._modelViewVisibility)}>Show Model View</vaadin-checkbox>
                <vaadin-checkbox @change="_suopViewVisibilityChanged" ?checked=${isVisible(this._suoplViewVisibility)}>Show Suop View</vaadin-checkbox>
                <vaadin-checkbox @change="_unitGridVisibilityChanged" ?checked=${isVisible(this._unitGridVisibility)}>Show Unit Grid</vaadin-checkbox>
                <vaadin-checkbox @change="_optimizationDataMonitorVisibilityChanged" ?checked=${isVisible(this._optimizationDataMonitorVisibility)}>Show Optimization Data Monitor</vaadin-checkbox>
            </side-bar-collapsable-section>

            <!-- <side-bar-collapsable-section caption="Grid">
                <side-bar-color-picker on-color-changed="_gridHorizontalColorInputChanged" color="#000000" caption="Horizontal Color"></side-bar-color-picker>
                <side-bar-color-picker on-color-changed="_gridVerticalColorInputChanged" color="#000000" caption="Vertical Color"></side-bar-color-picker>
                <side-bar-color-picker on-color-changed="_gridBackgroundColor1InputChanged" color="#6585ff" caption="Background Color 1"></side-bar-color-picker>
                <side-bar-color-picker on-color-changed="_gridBackgroundColor2InputChanged" color="#fffdb8" caption="Background Color 2"></side-bar-color-picker>
                <paper-input on-change="_gridSizeInputChanged" value="3" always-float-label label="Grid Unit Size (2^exp)" type="number" min="0" step="1"></paper-input>
                <paper-input on-change="_gridTextureSizeInputChanged" value="8" always-float-label label="Grid Texture Size (2^exp)" type="number" min="0" step="1"></paper-input>
                <paper-input on-change="_gridLineWidthInputChanged" value="0" always-float-label label="Grid Line Width" type="number" min="0" step="1"></paper-input>
            </side-bar-collapsable-section> -->
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
        this._suopViewportColor = state.suopViewportColor;
        this._modelColor = state.modelColor;
        this._suopColor = state.suopColor;
        this._modelViewVisibility = state.modelViewVisibility;
        this._suopViewVisibility = state.suopViewVisibility;
        this._unitGridVisibility = state.suopViewVisibility;
        this._suopViewGridTextureVisibility  = state.suopViewGridTextureVisibility;
        this._optimizationDataMonitorVisibility = state.optimizationDataMonitorVisibility;
    }   

    constructor() {
        super();
        this.solverText = "Off";
        this.lambda = 0.1;
        this.delta = 0.9;
        this.seamlessWeight = 0;
        this.positionWeight = 0;
        this.showOptimizationDataMonitor = true; 
        this.solverOn = false;
        this.editingTool = 'camera';       
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
        let files = dialog.showOpenDialog({
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

        PubSub.publish('reload-model', {
            modelFilename: files[0]
        });
    }

    _reloadModule() {
        PubSub.publish('reload-module');
    }

    _splitOrientationChanged(e) {
        this.dispatch('changeSplitOrientation', e.detail.value);
        PubSub.publish('mesh-view.resize');
    }

    _modelViewportColorChanged(e) {
        // this.dispatch('changeModelViewportColor', e.detail.value);
        store.dispatch(Actions.changeModelViewportColor(e.detail.color));
    }

    _suopViewportColorChanged(e) {
        // this.dispatch('changeSolverViewportColor', e.detail.value);
    }

    _modelColorChanged(e) {
        // this.dispatch('changeModelColor', e.detail.value);
    }

    _solverColorChanged(e) {
        this.dispatch('changeSolverColor', e.detail.value);
    }

    _highlightedFaceColorChanged(e) {
        this.dispatch('changeHighlightedFaceColor', e.detail.value);
    }

    _draggedFaceColorChanged(e) {
        this.dispatch('changeDraggedFaceColor', e.detail.value);
    }

    _fixedFaceColorChanged(e) {
        this.dispatch('changeFixedFaceColor', e.detail.value);
    }

    _vertexEnergyColorChanged(e) {
        this.dispatch('changeVertexEnergyColor', e.detail.value);
    }

    _vertexEnergyChanged(e) {
        this.dispatch('changeVertexEnergy', e.detail.value);
    }

    _showWireframeChanged(e) {
        this.dispatch('changeShowWireframe', e.detail.value);
    }

    _showMeshViewChanged(e) {
        this.dispatch('changeShowMeshView', e.detail.value);
        require('pubsub-js').publish('mesh-view.resize');
    }

    _showSolverViewChanged(e) {
        this.dispatch('changeShowSolverView', e.detail.value);
        require('pubsub-js').publish('mesh-view.resize');
    }

    _showInfographicsChanged(e) {
        this.dispatch('changeShowInfographics', e.detail.value);
        require('pubsub-js').publish('mesh-view.resize');
    }

    _showUnitGridChanged(e) {
        this.dispatch('changeShowUnitGrid', e.detail.value);
    }

    _showGridTextureInSuopViewChanged(e) {
        this.dispatch('changeShowGridTextureInSuopView', e.detail.value);
    }

    _showOptimizationDataMonitorChanged(e) {
        // this.dispatch('changeShowOptimizationDataMonitor', e.detail.value);
    }

    _deltaChanged(delta) {
        this.dispatch('changeDelta', delta);
    }

    _lambdaChanged(lambda) {
        this.dispatch('changeLambda', lambda);
    }

    _integerWeightChanged(integerWeight) {
        this.dispatch('changeIntegerWeight', integerWeight);
    }

    _integerSpacingChanged(integerSpacing) {
        this.dispatch('changeIntegerSpacing', integerSpacing);
    }

    _seamlessWeightChanged(seamlessWeight) {
        this.dispatch('changeSeamlessWeight', seamlessWeight);
    }

    _positionWeightChanged(positionWeight) {
        this.dispatch('changePositionWeight', positionWeight);
    }

    _solverChanged(solver) {
        this.dispatch('changeSolver', solver);
    }

    _editingToolChanged(editingTool) {
        this.dispatch('changeEditingTool', editingTool);
    }

    _gridHorizontalColorChanged(gridHorizontalColor) {
        this.dispatch('changeGridHorizontalColor', gridHorizontalColor);
    }

    _gridVerticalColorChanged(gridVerticalColor) {
        this.dispatch('changeGridVerticalColor', gridVerticalColor);
    }

    _gridBackgroundColor1Changed(gridBackgroundColor1) {
        this.dispatch('changeGridBackgroundColor1', gridBackgroundColor1);
    }

    _gridBackgroundColor2Changed(gridBackgroundColor2) {
        this.dispatch('changeGridBackgroundColor2', gridBackgroundColor2);
    }

    _gridSizeChanged(gridSize) {
        this.dispatch('changeGridSize', gridSize);
    }

    _gridTextureSizeChanged(gridTextureSize) {
        this.dispatch('changeGridTextureSize', gridTextureSize);
    }

    _gridLineWidthChanged(gridLineWidthSize) {
        this.dispatch('changeGridLineWidth', gridLineWidthSize);
    }

    _gridHorizontalColorInputChanged(e) {
        this.gridHorizontalColor = e.detail.value;
    }

    _gridVerticalColorInputChanged(e) {
        this.gridVerticalColor = e.detail.value;
    }

    _gridBackgroundColor1InputChanged(e) {
        this.gridBackgroundColor1 = e.detail.value;
    }

    _gridBackgroundColor2InputChanged(e) {
        this.gridBackgroundColor2 = e.detail.value;
    }

    _gridSizeInputChanged(e) {
        this.gridSize = parseFloat(e.srcElement.value);
    }

    _gridTextureSizeInputChanged(e) {
        this.gridTextureSize = parseFloat(e.srcElement.value);
    }

    _gridLineWidthInputChanged(e) {
        this.gridLineWidth = parseFloat(e.srcElement.value);
    }

    _showOptimizationDataMonitorChanged() {
        this.dispatch('changeShowOptimizationDataMonitor', this.showOptimizationDataMonitor);
    }

    _sideBarParameterInputValueChanged(e) {
        switch (e.detail.id) {
            case "lambda":
                this.lambda = e.detail.value;
                break;
            case "delta":
                this.delta = e.detail.value;
                break;
            case "integer-weight":
                this.integerWeight = e.detail.value;
                break;
            case "integer-spacing":
                this.integerSpacing = e.detail.value;
                break;
            case "seamless-weight":
                this.seamlessWeight = e.detail.value;
                break;
            case "position-weight":
                this.positionWeight = e.detail.value;
                break;
        }
    }
}

customElements.define('autoquads-side-bar', AutoquadsSideBar);