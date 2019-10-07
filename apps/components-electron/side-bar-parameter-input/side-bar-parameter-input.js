import { LitElement, html, css } from '../../web_modules/lit-element.js';
import '../../web_modules/@polymer/iron-icons.js';
import '../../web_modules/@polymer/iron-collapse.js';
import '../../web_modules/@vaadin/vaadin-text-field/vaadin-number-field.js';
import '../../web_modules/@vaadin/vaadin-checkbox.js';
import '../../web_modules/@polymer/paper-slider.js';
import '../side-bar-collapsable-section/side-bar-collapsable-section.js';
export class SideBarParameterInput extends LitElement {
    static get styles() {
        return [css`
            :host {
                display: flex;
                flex-direction: column;
                flex-shrink: 0;
                background: rgba(198, 200, 204, 0.5);
                border-radius: 5px;
                margin-bottom: 10px;
            }

            /* HACK: check why vaadin-number-field has 'width: 8em' property on its host element */
            vaadin-number-field {
                width: 90%;
            } 

            paper-slider {
                width: 100%;
            }

            paper-slider {
                --paper-slider-height: 3px;
            }

            vaadin-text-field::part(value) {
                text-transform: uppercase;
            }

            .parameter-input {
                padding-left: 10px;
                padding-right: 10px;
            }

            .slider-parameter-input {
                padding-left: 10px;
                padding-right: 10px;
            }
        `];
    }

    render() {
        return html`
            <vaadin-number-field id="valueInput" class="parameter-input" @input="${this._handleNumberFieldChange}" @change="${this._handleNumberFieldChange}" value="${this.value}" always-float-label label="${this.label}" min="${this.min}" max="${this.max}" step="${this.step}" has-controls always-float-label></vaadin-number-field>
            ${!this.isExponential?html`<paper-slider id="valueSlider" @immediate-value-change="${this._handleSliderChange}" value="${this.value}" min="${this.min}" max="${this.max}" step="${Number.EPSILON}"></paper-slider>`: ''}
            <side-bar-collapsable-section caption="Options">
                <vaadin-checkbox on-checked-changed="${this._isExponentialChanged}" checked="${this.isExponential}" disabled>Exponential</vaadin-checkbox>
                <vaadin-number-field value="${this.min}" @change="${this._minInputChanged}" class="slider-parameter-input" label="Min" step="null" always-float-label></vaadin-number-field>
                <vaadin-number-field value="${this.max}" @change="${this._maxInputChanged}" class="slider-parameter-input" label="Max" step="null" always-float-label></vaadin-number-field>
                <vaadin-number-field type="number" value="${this.step}" @change="${this._stepInputChanged}" class="slider-parameter-input" label="Step" step="null" always-float-label></vaadin-number-field>
                <vaadin-text-field value="${this.increaseKey}" @change="${this._increaseKeyChanged}" @keydown="${this._keySelectionKeyDown}" class="slider-parameter-input" label="Increase Key" minlength="1" maxlength="1" always-float-label></vaadin-text-field>
                <vaadin-text-field value="${this.decreaseKey}" @change="${this._decreaseKeyChanged}" @keydown="${this._keySelectionKeyDown}" class="slider-parameter-input" label="Decrease Key" minlength="1" maxlength="1" always-float-label></vaadin-text-field>
            </side-bar-collapsable-section>
        `;
    }

    static get properties() {
        return {
            label: {
                type: String
            },
            min: {
                type: Number
            },
            max: {
                type: Number
            },
            step: {
                type: Number
            },
            value: {
                type: Number
            },
            id: {
                type: String
            },
            isExponential: {
                type: Boolean,
                attribute: 'is-exponential'
            },
            increaseKey: {
                type: String,
                attribute: 'increase-key'
            },
            decreaseKey: {
                type: String,
                attribute: 'decrease-key'                
            }
        };
    }

    constructor() {
        super();
        this.isExponential = false;
        this.value = 0;
        this.min = 0;
        this.max = 1;
        this.step = 0.1;
    }

    set value(value) {
        const oldValue = this._value;
        if(value !== oldValue) {
            this._value = value;
            let valueChangedEvent = new CustomEvent('value-changed', { 
                detail: { 
                    value: value
                },
                bubbles: true, 
                composed: true 
            });
            this.dispatchEvent(valueChangedEvent);
            this.requestUpdate('value', oldValue);
        }
    }

    get value() {
        return this._value;
    }

    connectedCallback() {
        super.connectedCallback();
        document.addEventListener("keydown", this._handleDocumentKeyDown.bind(this));
    }
    
    disconnectedCallback() {
        // TODO: Remove event listeners
        super.disconnectedCallback();
    }

    _handleDocumentKeyDown(e) {
        if (e.key.toUpperCase() === this.increaseKey.toUpperCase()) {
            if (this.isExponential) {
                this.value = Math.min(parseFloat(this.value) * parseFloat(this.step), parseFloat(this.max));
            } else {
                this.value = Math.min(parseFloat(this.value) + parseFloat(this.step), parseFloat(this.max));
            }
        } else if (e.key.toUpperCase() === this.decreaseKey.toUpperCase()) {
            if (this.isExponential) {
                this.value = Math.max(parseFloat(this.value) / parseFloat(this.step), parseFloat(this.min));
            } else {
                this.value = Math.max(parseFloat(this.value) - parseFloat(this.step), parseFloat(this.min));
            }
        }
    }

    _handleNumberFieldChange(e) {
        this.value = parseFloat(e.srcElement.value);
    }

    _handleSliderChange(e) {
        this.value = parseFloat(e.srcElement.immediateValue);
    }

    _minInputChanged(e) {
        this.min = parseFloat(e.srcElement.value);
    }

    _maxInputChanged(e) {
        this.max = parseFloat(e.srcElement.value);
    }

    _stepInputChanged(e) {
        this.step = parseFloat(e.srcElement.value);
    }    

    _increaseKeyChanged(e) {
        this.increaseKey = e.srcElement.value;
    }

    _decreaseKeyChanged(e) {
        this.decreaseKey = e.srcElement.value;
    }    
}

customElements.define('side-bar-parameter-input', SideBarParameterInput);