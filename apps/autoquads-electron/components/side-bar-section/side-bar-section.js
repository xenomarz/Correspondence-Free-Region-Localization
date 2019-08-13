import { LitElement, html, css } from '../../web_modules/lit-element.js';
export class SideBarSection extends LitElement {
    static get styles() {
        return [css`
            :host {
            display: flex;
            flex-direction: column;
            flex-shrink: 0;
            }
        `];
    }

    render() {
        return html`
            <h3>[[caption]]</h3>
            <slot></slot>
        `;
    }
}

customElements.define('side-bar-section', SideBarSection);