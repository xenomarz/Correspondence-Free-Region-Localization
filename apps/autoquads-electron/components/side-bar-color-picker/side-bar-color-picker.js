import { LitElement, html, css } from '../../web_modules/lit-element.js';
import '../../web_modules/@polymer/iron-icons.js';
import '../../web_modules/@polymer/iron-collapse.js';
export class SideBarColorPicker extends LitElement {
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

            .caption-container {
                margin-left: 10px;
            }

            #color {
                flex-shrink: 0;
            }
        `];
    }

    render() {
        return html`
            <input type="color" id="color" value="${this.color}" />
            <div class="caption-container">
                <span>${this.caption}</span>
            </div> 
        `;
    }

    static get properties() {
        return {
            color: {
                type: String,
                reflect: true
            },
            caption: {
                type: String
            }
        }
    } 
}

customElements.define('side-bar-color-picker', SideBarColorPicker);