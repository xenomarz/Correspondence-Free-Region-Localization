import { LitElement, html, css } from '../../web_modules/lit-element.js';
import { SideBar } from '../side-bar/side-bar.js';
import '../side-bar-collapsable-section/side-bar-collapsable-section.js';
import '../side-bar-parameter-input/side-bar-parameter-input.js';
import '../side-bar-color-picker/side-bar-color-picker.js';
import '../../web_modules/@polymer/iron-icons.js';
import '../../web_modules/@polymer/iron-collapse.js';
import '../../web_modules/@polymer/paper-toggle-button.js';
import '../../web_modules/@vaadin/vaadin-button.js';
import '../../web_modules/@vaadin/vaadin-combo-box.js';
import '../../web_modules/@vaadin/vaadin-checkbox.js';
import '../../web_modules/@vaadin/vaadin-select.js';
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
                <vaadin-button theme="contrast primary" @click="${this.handleClick}">
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
                <paper-toggle-button class="solver" checked="{{solver}}" disabled>${this.solverText}</paper-toggle-button>
            </side-bar-collapsable-section>

            <side-bar-collapsable-section caption="Interaction" caption-size="3">
                <side-bar-color-picker on-color-changed="_highlightedFaceColorChanged" color="#ff00d0" caption="Highlighted Face Color"></side-bar-color-picker>
                <side-bar-color-picker on-color-changed="_draggedFaceColorChanged" color="#0000ff" caption="Dragged Face Color"></side-bar-color-picker>
                <side-bar-color-picker on-color-changed="_fixedFaceColorChanged" color="#00ff00" caption="Fixed Face Color"></side-bar-color-picker>
                <side-bar-color-picker on-color-changed="_vertexEnergyColorChanged" color="#ff0000" caption="Vertex Energy Color"></side-bar-color-picker>
                <vaadin-select label="Vertex Energy" value="Seperation">
                    <template>
                        <vaadin-list-box>
                            <vaadin-item>Seperation</vaadin-item>
                            <vaadin-item>Seamless</vaadin-item>
                        </vaadin-list-box>
                    </template>
                </vaadin-select>
            </side-bar-collapsable-section>

            <!-- <side-bar-collapsable-section caption="Viewports" caption-size="3">
                <vaadin-dropdown-menu label="Split Orientation" value="horizontal" on-value-changed="_splitOrientationChanged">
                    <template>
                    <vaadin-list-box>
                        <vaadin-item value="vertical">Vertical</vaadin-item>
                        <vaadin-item value="horizontal">Horizontal</vaadin-item>
                    </vaadin-list-box>
                    </template>
                </vaadin-dropdown-menu>
                <side-bar-color-picker on-color-changed="_modelViewportColorChanged" color="#757575" caption="Model Viewport Color"></side-bar-color-picker>
                <side-bar-color-picker on-color-changed="_solverViewportColorChanged" color="#757575" caption="Solver Viewport Color"></side-bar-color-picker>
                <side-bar-color-picker on-color-changed="_modelColorChanged" color="#ffffff" caption="Model Color"></side-bar-color-picker>
                <side-bar-color-picker on-color-changed="_solverColorChanged" color="#ffffff" caption="Solver Color"></side-bar-color-picker>
                <vaadin-checkbox on-checked-changed="_showMeshViewChanged" checked>Show Mesh View</vaadin-checkbox>
                <vaadin-checkbox on-checked-changed="_showSolverViewChanged" checked>Show Solver View</vaadin-checkbox>
                <vaadin-checkbox on-checked-changed="_showInfographicsChanged">Show Infographics</vaadin-checkbox>
                <vaadin-checkbox on-checked-changed="_showUnitGridChanged">Show Unit Grid</vaadin-checkbox>
                <vaadin-checkbox on-checked-changed="_showGridTextureInSuopViewChanged">Show Grid Texture In Soup View</vaadin-checkbox>
                <vaadin-checkbox on-checked-changed="_showOptimizationDataMonitorChanged" checked="{{showOptimizationDataMonitor}}">Show Optimization Data Monitor</vaadin-checkbox>
            </side-bar-collapsable-section> -->
        `;
    }

    constructor() {
        super();
        this.solverText = "Off";
    }
    
    handleClick() {
        alert("hello");
    }
}

customElements.define('autoquads-side-bar', AutoquadsSideBar);