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
                    <span>Show Model Wireframe</span>
                </vaadin-checkbox>
                <vaadin-checkbox
                    ?checked=${HelpersExports.isVisible(this._wireframeVisibility)}
                    @change="${this._wireframeVisibilityInputChanged}"
                    checked>
                    <span>Show Soup Wireframe</span>
                </vaadin-checkbox>                   
            </side-bar-collapsable-section>
            <side-bar-collapsable-section
                caption="Numeric Properties">
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
                    max="10000"
                    step="1"
                    label="Seamless Weight"
                    @value-changed="${this._seamlessWeightInputChanged}">
                </side-bar-parameter-input>
                <side-bar-parameter-input
                    id="singularity-weight"
                    increase-key="k"
                    decrease-key="l"
                    value="${this._singularityWeight}"
                    min="0"
                    max="10000"
                    step="1"
                    label="Singularity Weight"
                    @value-changed="${this._singularityWeightInputChanged}">
                </side-bar-parameter-input>
                <side-bar-parameter-input
                    id="singularity-interval"
                    increase-key="i"
                    decrease-key="u"
                    value="${this._singularityInterval}"
                    min="0"
                    max="10000"
                    step="2"
                    label="Singularity Interval"
                    @value-changed="${this._singularityIntervalInputChanged}"
                    is-exponential>
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
                caption="Visual Properties">
                ${this.objectiveFunctionsProperties.map(item => html`
                    <autoquads-side-bar-objective-function-visual-property
                        objective-function-id=${item.objectiveFunctionId}
                        property-id=${item.propertyId}
                        objective-function-name=${item.objectiveFunctionName}
                        property-name=${item.propertyName}
                        color=${item.color}
                        ?selected=${item.selected}
                        @color-changed="${this._objectiveFunctionVisualPropertyColorInputChanged}"
                        @selected-changed="${this._objectiveFunctionVisualPropertyVisibilityInputChanged}">
                    </autoquads-side-bar-objective-function-visual-property>
                `)}
            </side-bar-collapsable-section>
            <side-bar-collapsable-section
                caption="Solver">
                <paper-toggle-button
                    class="solver"
                    ?checked="${HelpersExports.isSolverOn(this.solverState)}"
                    ?disabled="${!HelpersExports.isModelLoaded(this.modelState)}"
                    @change="${this._solverStateInputChanged}">
                    <span>${HelpersExports.solverStateText(this.solverState)}</span>
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
        this.requestUpdate('modelColor', oldValue);
    }

    get modelColor() {
        return this._modelColor;
    }

    set soupColor(value) {
        const oldValue = this._soupColor;
        this._soupColor = value;
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

    set seamlessWeight(value) {
        const oldValue = this._seamlessWeight;
        this._seamlessWeight = value;
        this.requestUpdate('seamlessWeight', oldValue);
    }

    get seamlessWeight() {
        return this._seamlessWeight;
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
        const oldValue = this._solverState;
        this._solverState = value;
        this.requestUpdate('solverState', oldValue);
    }

    get solverState() {
        return this._solverState;
    }

    set modelFilename(value) {
        const oldValue = this._modelFilename;
        this._modelFilename = value;
        this.requestUpdate('modelFilename', oldValue);
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

    _singularityWeightInputChanged(e) {
        store.dispatch(ActionsExports.setSingularityWeight(e.srcElement.value)); 
    }

    _singularityIntervalInputChanged(e) {
        store.dispatch(ActionsExports.setSingularityInterval(e.srcElement.value)); 
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

    _objectiveFunctionVisualPropertyColorInputChanged(e) {
        store.dispatch(ActionsExports.setObjectiveFunctionPropertyColor(e.detail.objectiveFunctionId, e.detail.propertyId, e.detail.color));
    }

    _objectiveFunctionVisualPropertyVisibilityInputChanged(e) {
        store.dispatch(ActionsExports.setObjectiveFunctionPropertyVisibility(e.detail.objectiveFunctionId, e.detail.propertyId, e.detail.selected ? EnumsExports.Visibility.VISIBLE : EnumsExports.Visibility.HIDDEN)); 
    }    
}

customElements.define('autoquads-side-bar', AutoquadsSideBar);