import { LitElement, html, css } from '../../web_modules/lit-element.js';
import '../../web_modules/@polymer/iron-icons.js';
import '../../web_modules/@polymer/iron-collapse.js';
import '../../web_modules/@vaadin/vaadin-checkbox.js';
import chroma from '../../web_modules/chroma-js.js';
export class AutoquadsSideBarObjectiveFunctionVisualProperty extends LitElement {
    static get styles() {
        return [css`
            :host {
                display: flex;
                flex-direction: row;
                flex-shrink: 0;
                justify-items: center;
                align-items: center;
                margin-top: 5px;
            }

            input {
                flex-shrink: 0;
            }
        `];
    }

    render() {
        return html`
            <vaadin-checkbox
                ?checked=${this.selected}
                @change="${this._selectedInputChanged}">
                <span>${this.propertyName + ' (' + this.objectiveFunctionName + ')'}</span>
            </vaadin-checkbox>       
            <input type="color" value="${chroma(this._color).hex()}" @change="${this._colorInputChanged}" />
        `;
    }

    static get properties() {
        return {
            caption: {
                type: String,
                attribute: 'caption'
            },
            color: {
                type: String,
                attribute: 'color',
                reflect: true
            },
            selected: {
                type: Boolean,
                attribute: 'selected',
                reflect: true
            },
            objectiveFunctionId: {
                type: String,
                attribute: 'objective-function-id'
            },
            propertyId: {
                type: String,
                attribute: 'property-id'
            },
            objectiveFunctionName: {
                type: String,
                attribute: 'objective-function-name'
            },
            propertyName: {
                type: String,
                attribute: 'property-name'
            }            
        }
    }

    constructor() {
        super();
        this._color = 'rgb(0,0,0)';
        this._selected = false;
    }

    set selected(value) {
        const oldValue = this._selected;
        this._selected = value;
        let selectedChangedEvent = new CustomEvent('selected-changed', { 
            detail: { 
                objectiveFunctionId: this.objectiveFunctionId,
                propertyId: this.propertyId,
                selected: value,
            },
            bubbles: true, 
            composed: true 
        });
        this.dispatchEvent(selectedChangedEvent);
        this.requestUpdate('selected', oldValue);
    }

    get selected() {
        return this._selected;
    }    

    set color(value) {
        const oldValue = this._color;
        this._color = value;
        let colorChangedEvent = new CustomEvent('color-changed', { 
            detail: {
                objectiveFunctionId: this.objectiveFunctionId,
                propertyId: this.propertyId,
                color: value,
            },
            bubbles: true, 
            composed: true 
        });
        this.dispatchEvent(colorChangedEvent);
        this.requestUpdate('color', oldValue);
    }

    get color() {
        return this._color;
    }
    
    _selectedInputChanged(e) {
        this.selected = e.srcElement.checked;
    }

    _colorInputChanged(e) {
        this.color = chroma(e.srcElement.value).css();
    }        
}

customElements.define('autoquads-side-bar-objective-function-visual-property', AutoquadsSideBarObjectiveFunctionVisualProperty);