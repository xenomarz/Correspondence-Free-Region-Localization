import { LitElement, html, css } from '../../web_modules/lit-element.js';
import '../autoquads-side-bar/autoquads-side-bar.js';
import '../../web_modules/@vaadin/vaadin-button.js';
import '../../web_modules/@vaadin/vaadin-split-layout.js';
export class AutoquadsView extends LitElement {
    static get styles() {
        return [css`
            :host {
                display: flex;
                width: 100%;
                height: 100%;
            }

            .outer-split {
                flex: 1;
            }

            .inner-split {
                width: 100%;
                height: 100%;
            }

            /* https://stackoverflow.com/questions/41800823/hide-polymer-element */
            [hidden] {
                display: none;
            }
        `];
    }

    render() {
    return html`  
        <vaadin-split-layout class="outer-split">
            <autoquads-side-bar></autoquads-side-bar>
            <vaadin-split-layout orientation="horizontal" class="inner-split">
                <vaadin-split-layout orientation="horizontal" class="inner-split">
                </vaadin-split-layout>
            </vaadin-split-layout>
        </vaadin-split-layout>
    `;
    }
}

customElements.define('autoquads-view', AutoquadsView);