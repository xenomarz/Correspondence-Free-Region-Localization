import { LitElement, html, css } from '../../web_modules/lit-element.js';
export class SideBar extends LitElement {
    static get styles() {
        return [css`
            :host {
                padding: 10px;
                display: flex;
                flex-direction: column;
                width: 250px;
                align-items: stretch;
                background-color: #d8d8d8;
                font-size: 16px;
            }
        `];
    }
}

customElements.define('side-bar', SideBar);