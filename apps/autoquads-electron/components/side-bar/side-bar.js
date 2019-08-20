import { LitElement, html, css } from '../../web_modules/lit-element.js';
import { store } from '../../redux/store.js';
import { connect } from '../../web_modules/pwa-helpers.js';

export class SideBar extends connect(store)(LitElement) {
    static get styles() {
        return [css`
            :host {
                padding: 10px;
                display: flex;
                flex-direction: column;
                width: 350px;
                align-items: stretch;
                background-color: #d8d8d8;
                font-size: 16px;
            }
        `];
    }
}

customElements.define('side-bar', SideBar);