import { LitElement, html, css } from '../../web_modules/lit-element.js';
import '../../web_modules/@polymer/iron-icons.js';
import '../../web_modules/@polymer/iron-collapse.js';
export class SideBarCollapsableSection extends LitElement {
    static get styles() {
        return [css`
            :host {
                display: flex;
                flex-direction: column;
                flex-shrink: 0;
            }

            .section-header-container {
                display: flex;
                flex-direction: row;
                align-items: center;
                cursor: pointer;
            }

            .section-header {
                font-weight: bold;
            }

            .section-collapse {
                display: flex;
                flex-direction: column;
            }
                     
            #collapse {
                margin-left: 15px;
            }
        `];
    }

    render(){
        return html`
            <div id="header" @click="${this._collapseStateChanged}" class="section-header-container">
                <iron-icon icon="${this.icon}"></iron-icon>
                <div class="section-header">${this.caption}</div>
            </div>
            <iron-collapse id="collapse" class="section-collapse">
                <slot></slot>
            </iron-collapse>
        `;
    }

    constructor() {
        super();
        this._expanded = false;
        this.icon = this.resolveIcon();
    }

    static get properties() { 
        return {
            caption: { 
                type: String,
                attribute: 'caption' 
            },
            expanded: { 
                type: Boolean,
                attribute: 'expanded',
                reflect: true
            }
        };
    }

    set expanded(value) {
        const oldValue = this.myProp;
        let element = this.shadowRoot.querySelector('#collapse');
        if(value == true) {
            element.show();
        }
        else {
            element.hide();
        }
        this._expanded = value;
        this.requestUpdate('expanded', oldValue);
    }

    get expanded() {
        return this._expanded;
    }

    _collapseStateChanged() {
        this.expanded = !this.expanded;
        this.icon = this.resolveIcon();
    }

    resolveIcon() {
        if (this.expanded) {
            return "icons:remove";
        }

        return "icons:add";
    }
}

customElements.define('side-bar-collapsable-section', SideBarCollapsableSection);